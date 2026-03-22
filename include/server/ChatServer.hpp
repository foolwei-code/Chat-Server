#pragma once
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include<string>
class ChatServer {
public:
  //初始化聊天服务器对象
  ChatServer(muduo::net::EventLoop *loop,
             const muduo::net::InetAddress &listenAddr,
             const std::string &nameArg);
  ~ChatServer() = default;
  // 启动服务
  void start();

private:
  //设置用户建立连接与断开的回调函数
  void onConnection(const muduo::net::TcpConnectionPtr &);
  // 设置用户进行读写的回调函数
  void onMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
  muduo::net::TcpServer server_;
  muduo::net::EventLoop* loop_;
};