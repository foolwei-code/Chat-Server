#include "ChatServer.hpp"
#include <muduo/net/InetAddress.h>
#include <signal.h>
#include"ChatService.hpp"
//处理服务器ctrl+c 结束后，重置user的状态信息
void resetHandler(int) {
  ChatService::getChatService()->reset();
  exit(0);
}
int main() {
  signal(SIGINT,resetHandler);
  muduo::net::EventLoop loop;
  muduo::net::InetAddress addr{"127.0.0.1", 6000};
  ChatServer server{&loop, addr, "ChatServer"};
  server.start();
  loop.loop();
}