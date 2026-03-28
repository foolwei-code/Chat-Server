#pragma once
// 用单例设计模式实现聊天服务器业务类
#include "FriendModel.hpp"
#include "UserModel.hpp"
#include "json.hpp"
#include "public.hpp"
#include <functional>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <mutex>
#include <unordered_map>
#include "offLineMessageModel.hpp"
#include"groupModel.hpp"
using json = nlohmann::json;
// 表示处理消息的函数的类型
using MessageHandler = std::function<void(const muduo::net::TcpConnectionPtr &,
                                          json &, muduo::Timestamp)>;
class ChatService {
public:
  // 提供静态接口获取实例化对象
  static ChatService *getChatService();
  // 处理登录业务
  void login(const muduo::net::TcpConnectionPtr &, json &, muduo::Timestamp);
  // 处理注册业务
  void reg(const muduo::net::TcpConnectionPtr &, json &, muduo::Timestamp);
  // 删除拷贝函数
  ChatService(const ChatService &) = delete;
  ChatService &operator=(const ChatService &) = delete;
  // 获取消息对应的处理器
  MessageHandler getHandler(EnMessageType messageId);
  // 处理客户端异常退出
  void clientCloseException(const muduo::net::TcpConnectionPtr &);
  // 一对一聊天业务
  void oneChat(const muduo::net::TcpConnectionPtr &, json &, muduo::Timestamp);
  // 服务器异常，业务处理方法
  void reset();
  // 添加好友业务
  void addFriend(const muduo::net::TcpConnectionPtr &, json &,
                 muduo::Timestamp);
  // 创建群组业务
  void createGroup(const muduo::net::TcpConnectionPtr &, json &,
                   muduo::Timestamp);
  // 加入群组业务
  void addGroup(const muduo::net::TcpConnectionPtr&,json&,muduo::Timestamp);

  //群聊天业务
void groupChat(const muduo::net::TcpConnectionPtr&,json&,muduo::Timestamp);

private:
  //存储对应的消息Id和其事件处理器
  std::unordered_map<EnMessageType, MessageHandler> messageHandlerMap_;
  // 存储在线用户的通信连接
  std::unordered_map<int, muduo::net::TcpConnectionPtr> userConnectionMap_;
  // 定义互斥锁，保障userConnectionMap_的线程安全
  std::mutex connectionMutex_;
  // 将构造函数私有化
  ChatService();
  // 数据操作类对象
  UserModel userModel_;
  offLineMsModel offLineMesModel_;
  FriendModel friendModel_;
  GroupModel groupModel_;
};