#include <functional>
#include <iostream>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include <string>
class ChatServer {
public:
  // 构造函数
  ChatServer(muduo::net::EventLoop *loop,               // 事件循环指针
             const muduo::net::InetAddress &listenAddr, // 服务器的ip和端口
             const std::string &nameArg);               // 服务器的名称
  // 开启服务器
  void start();
private:
  // 用户的创建连接和断开连接回调函数
  void onConnection(const muduo::net::TcpConnectionPtr &);
  // 用户的读写回调函数
  void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                 muduo::Timestamp);
  muduo::net::TcpServer server_;
  muduo::net::EventLoop *loop_;
};
// 构造函数
ChatServer::ChatServer(
    muduo::net::EventLoop *loop,               // 事件循环指针
    const muduo::net::InetAddress &listenAddr, // 服务器的ip和端口
    const std::string &nameArg)                // 服务器名称
    : server_(loop, listenAddr, nameArg), loop_(loop) {
  // 注册回调
  server_.setConnectionCallback(
      std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
  server_.setMessageCallback(
      std::bind(&ChatServer::onMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
  // 设置线程池中线程的数量
  server_.setThreadNum(4);
}
// 用户的创建连接和断开连接回调函数
void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
  if (conn->connected()) {
    std::cout << conn->peerAddress().toIpPort() << "->"
              << conn->localAddress().toIpPort() << "status:online"
              << std::endl;
  } else {
    std::cout << conn->peerAddress().toIpPort() << "->"
              << conn->localAddress().toIpPort() << " status:offline"
              << std::endl;
    conn->shutdown(); // close(fd)
    // loop_->quit();
  }
}
// 用户的读写回调函数
void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buffer, muduo::Timestamp time) {
  std::string buf = buffer->retrieveAllAsString();
  std::cout << "recv data:" << buf << " time: " << time.toString() << std::endl;
  conn->send(buf);
}
void ChatServer::start() {
  // 开启事件循环
  server_.start();
}
int main() {
  muduo::net::EventLoop loop;
  muduo::net::InetAddress addr{"127.0.0.1", 6000};
  ChatServer server{&loop, addr, "ChatServer"};
  server.start(); //listenfd epoll_ctl一>epoll
  loop.loop();  //epoll_wait以阻塞的方式等待 新用户的连接，已连接的用户的读写事件等
}
