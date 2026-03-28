#include "ChatService.hpp"
#include "offLineMessageModel.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <mutex>
#include <vector>
// 提供静态接口获取实例化对象
ChatService *ChatService::getChatService() {
  static ChatService instance;
  return &instance;
}
// 注册消息以及对应的回调操作
ChatService::ChatService() {
  messageHandlerMap_.insert(
      {EnMessageType::LOGIN_MSG,
       std::bind(&ChatService::login, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3)});
  messageHandlerMap_.insert(
      {EnMessageType::REG_MSG,
       std::bind(&ChatService::reg, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3)});
  messageHandlerMap_.insert(
      {EnMessageType::ONE_CHAT_MSG,
       std::bind(&ChatService::oneChat, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3)});
  messageHandlerMap_.insert(
      {EnMessageType::ADD_FRIEND_MSG,
       std::bind(&ChatService::addFriend, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3)});
}
// 处理登录业务
void ChatService::login(const muduo::net::TcpConnectionPtr &conn, json &js,
                        muduo::Timestamp time) {
  int id = js["id"].get<int>();
  std::string password = js["password"];
  User user = userModel_.query(id);
  if (user.getId() == id && user.getPassWord() == password) {
    if (user.getStatus() == "online") {
      // 该用户已经登录了，不允许重复登录
      json response;
      response["messageId"] = EnMessageType::LOGIN_MSG_ACK;
      response["error"] = 2;
      response["errorMesssage"] = "该账号已经登录，请重新输入账号";
      conn->send(response.dump());
      return;
    }
    // 登陆成功，记录用户的连接信息
    {
      std::lock_guard<std::mutex> lock{connectionMutex_};
      userConnectionMap_.insert({id, conn});
    }
    // 登录成功,更新用户状态信息status:offline->online
    user.setStatus("online");
    userModel_.updateStatus(user);
    json response;
    response["messageId"] = EnMessageType::LOGIN_MSG_ACK;
    response["error"] = 0;
    response["id"] = user.getId();
    response["name"] = user.getName();
    // 查询该用户是否有离线消息
    std::vector<std::string> vec(offLineMesModel_.query(id));
    if (!vec.empty()) {
      response["offlineMessage"] = vec;
      // 读取该用户的离线消息后，将该用户的所有离线消息删除
      offLineMesModel_.remove(id);
    }
    // 查询该用户的好友信息并返回
    std::vector<User> userVec(friendModel_.query(id));
    if (!userVec.empty()) {
      std::vector<std::string> vec2;
      for (auto &user : userVec) {
        json js;
        js["id"] = user.getId();
        js["name"] = user.getName();
        js["status"] = user.getStatus();
        vec.emplace_back(js.dump());
      }
      response["friend"] = vec2;
    }
    conn->send(response.dump());
  } else {
    // 该用户不存在或密码错误，登录失败
    json response;
    response["messageId"] = EnMessageType::LOGIN_MSG_ACK;
    response["error"] = 1;
    response["errorMesssage"] = "用户名或密码错误";
    conn->send(response.dump());
  }
}
// 处理注册业务 name password
void ChatService::reg(const muduo::net::TcpConnectionPtr &conn, json &js,
                      muduo::Timestamp time) {
  std::string name = js["name"];
  std::string password = js["password"];
  User user;
  user.setName(name);
  user.setPassWord(password);
  bool status = userModel_.insert(user);
  if (status) {
    // 注册成功
    json response;
    response["messageId"] = EnMessageType::REG_MSG_ACK;
    response["error"] = 0;
    response["id"] = user.getId();
    conn->send(response.dump());
  } else {
    json response;
    response["messageId"] = EnMessageType::REG_MSG_ACK;
    response["error"] = 1;
    conn->send(response.dump());
  }
}
// 获取消对应的处理器
MessageHandler ChatService::getHandler(EnMessageType messageId) {
  // 记录错误日志，messageId没有对应的事件处理回调
  auto it{messageHandlerMap_.find(messageId)};
  if (it == messageHandlerMap_.end()) {
    LOG_ERROR << "messageId:" << static_cast<int>(messageId)
              << "can not find handler!";
    // 返回一个默认的处理器
    return [=](const muduo::net::TcpConnectionPtr &, json &, muduo::Timestamp) {
      LOG_ERROR << "messageId:" << static_cast<int>(messageId)
                << "can not find handler!";
    };
  }
  return messageHandlerMap_[messageId];
}

// 处理客户端异常退出
void ChatService::clientCloseException(
    const muduo::net::TcpConnectionPtr &conn) {
  User user;
  {
    std::lock_guard<std::mutex> lcok{connectionMutex_};
    for (auto it = userConnectionMap_.begin(); it != userConnectionMap_.end();
         it++) {
      if (it->second == conn) {
        // 从map表删除用户的连接信息
        user.setId(it->first);
        userConnectionMap_.erase(it);
        break;
      }
    }
  }
  // 更新用户的状态信息
  if (user.getId() != -1) {
    user.setStatus("offline");
    userModel_.updateStatus(user);
  }
}
// 一对一聊天业务
void ChatService::oneChat(const muduo::net::TcpConnectionPtr &conn, json &js,
                          muduo::Timestamp time) {
  int toId = js["to"].get<int>();
  // 表示用户是否在线
  {
    std::lock_guard<std::mutex> lock{connectionMutex_};
    auto it{userConnectionMap_.find(toId)};
    if (it != userConnectionMap_.end()) {
      // toId在线，转发消息  //服务器主动推送消息给toId用户
      it->second->send(js.dump());
      return;
    }
  }
  // toId不在线，存储离线消息
  offLineMesModel_.insert(toId, js.dump().c_str());
}
// 服务器异常，业务处理方法
void ChatService::reset() { userModel_.resetStatus(); }

// 添加好友业务
void ChatService::addFriend(const muduo::net::TcpConnectionPtr &conn, json &js,
                            muduo::Timestamp time) {
  int userId = js["id"].get<int>();
  int friendId = js["friendId"].get<int>();
  // 存储好友信息
  friendModel_.insert(userId, friendId);
}

// 创建群组业务
void ChatService::createGroup(const muduo::net::TcpConnectionPtr &conn,
                              json &js, muduo::Timestamp time) {
  int userId = js["id"].get<int>();
  std::string name{js["groupname"]};
  std::string desc{js["groupdesc"]};

  // 存储新创建的群组信息
  Group group{-1, name, desc};
  if (groupModel_.createGroup(group)) {
    // 存储群组的创建人信息
    groupModel_.addGroup(userId, group.getId(), "creator");
  }
}

// 加入群组业务
void ChatService::addGroup(const muduo::net::TcpConnectionPtr &conn, json &js,
                           muduo::Timestamp time) {
  int userId = js["id"].get<int>();
  int groupId = js["groupid"].get<int>();
  groupModel_.addGroup(userId, groupId, "normal");
}

// 群聊天业务
void ChatService::groupChat(const muduo::net::TcpConnectionPtr &conn, json &js,
                            muduo::Timestamp time) {
  int userId = js["id"].get<int>();
  int groupId = js["groupid"].get<int>();
  std::vector<int> userIdVec = groupModel_.queryGroupUsers(userId, groupId);
  std::lock_guard<std::mutex> lock{connectionMutex_};
  for (auto &id : userIdVec) {
    auto it{userConnectionMap_.find(id)};
    if (it != userConnectionMap_.end()) {
      it->second->send(js.dump());
    } else {
      offLineMesModel_.insert(id, js.dump());
    }
  }
}
