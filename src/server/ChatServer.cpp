#include "ChatServer.hpp"
#include <functional>
#include "json.hpp"
#include <string>
#include "ChatService.hpp"
using json=nlohmann::json;
// 初始化聊天服务器对象
ChatServer::ChatServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr,
                       const std::string &nameArg)
    : server_(loop, listenAddr, nameArg), loop_(loop) {
  // 注册建立连接和断开的回调函数
  server_.setConnectionCallback(
      std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
  // 注册用户读写回调函数
  server_.setMessageCallback(
      std::bind(&ChatServer::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  // 设置线程数量
  server_.setThreadNum(4);
}
// 启动服务
void ChatServer::start() { server_.start(); }
// 设置用户建立连接与断开的回调函数
void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
  if (!conn->connected()) {
    //处理客户端关闭异常
    ChatService::getChatService()->clientCloseException(conn);
    //客户端关闭连接
    conn->shutdown();
  }
}
// 设置用户进行读写的回调函数
void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buffer, muduo::Timestamp time) {
  std::string buf{buffer->retrieveAllAsString()};
  // 数据的反序列化
  json js=json::parse(buf);
  // 将网络模块的代码与业务模块的代码进行解耦合
  auto handler = ChatService::getChatService()->getHandler(
      js["messageId"].get<EnMessageType>());
  //回调消息绑定好的事件处理器，来执行相应的业务处理
  handler(conn,js,time);
}