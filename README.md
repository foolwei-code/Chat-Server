#  c++实现集群聊天服务器



## 	技术栈

1. Json 序列化和反序列化 
2. muduo 网络库开发 
3. nginx源码编译安装和环境部署 
4. nginx的tcp负载均衡器配置 
5. redis缓存服务器编程实践 
6. 基于发布-订阅的服务器中间件 redis 消息队列编程实践 
7. MySQL数据库编程 
8. CMake 构建编译环境 
9. Github 托管项目

## 项目需求

1. 客户端新用户注册 
2. 客户端用户登录 
3. 添加好友和添加群组 
4. 好友聊天 
5. 群组聊天 
6. nginx配置tcp负载均衡 
7. 集群聊天系统支持客户端跨服务器通信

## 项目目标

1. 掌握服务器的网络I/O模块，业务模块，数据模块分层的设计思想 
2. 掌握C++ muduo网络库的编程以及实现原理 
3. 掌握Json的编程应用 
4. 掌握nginx配置部署tcp负载均衡器的应用以及原理 
5. 掌握服务器中间件的应用场景和基于发布-订阅的redis编程实践以及应用原理 
6. 掌握CMake构建自动化编译环境 
7. 掌握Github管理项目

## 开发环境

1. ubuntu linux环境 
2. 安装Json开发库 ，参考博客：https://blog.csdn.net/qq_52201535/article/details/133581163
3. 安装boost + muduo网络库开发环境，参考博客： https://blog.csdn.net/m0_74795952/article/details/144631342
4. 安装redis环境 
5. 安装mysql数据库环境 
6. 安装CMake环境

配置远程开发环境  windows+vscode配置远程linux开发环境 (在widows下用vscode远程连接ubuntu)

参考博客：https://www.yuque.com/lianlianfengchen-cvvh2/qkmqzb/ro3mb752y21czzx5

##  json的介绍

### 1.json的定义

***JSON（JavaScript Object Notation）是一种**轻量级的数据交换格式**，它基于 JavaScript 的对象语法，但完全独立于编程语言 —— 几乎所有主流语言（C++、Python、Java、Go 等）都能解析和生成 JSON。简单来说，JSON 就是一种 “通用的数据格式”，可以把它理解成不同程序之间沟通的 “通用语言”：比如你的 C++ 服务器要给前端网页传数据，或者手机 App 要给后端接口发数据，用 JSON 来传递会非常方便。***

### 2.使用json的好处

**易读易写**：相比 XML、二进制格式，JSON 结构清晰，人类能直接看懂，编写和调试成本低；

**跨语言兼容**：不管是前端（JS）、后端（C++/Java/Python）、移动端（Android/iOS），都能轻松解析 / 生成 JSON，是跨平台数据交换的 “标配”；

**轻量高效**：相比 XML 省去了大量标签，数据体积更小，网络传输更快；

**解析效率高**：主流语言都有成熟的 JSON 解析库，解析速度快，且支持 “结构化解析”（能精准提取某个字段）；

**灵活性强**：支持嵌套（对象里嵌数组 / 对象），能表达复杂的数据结构，同时也能简化单值数据。

C++ 标准库本身没有内置 JSON 解析 / 生成功能，新手最推荐使用 **nlohmann/json** 库（单头文件，无需编译，直接引入即可）。

下载单头文件 `json.hpp`：https://github.com/nlohmann/json/releases

### 3.json序列化实例

#### 1.示例一

```c++
void func1()
{
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhangsan";
    js["to"] = "lisi";
    js["msg"] = "hello,what are you ding now?";
    std::string sendBuf_ = js.dump();
    std::cout << sendBuf_ << std::endl;
}
```

#### 2.示例二

```c++
void func2()
{
    json js2;
    //添加数组
    js2["id"] = {1, 2, 3, 4, 5};
   //添加对象
    js2["msg"]["zhangsan"] = "hello world";
    js2["msg"]["lisi"] = "haha";
    std::cout << js2 << std::endl;
}
```

#### 3.示例三

```c++
void func3()
{
    json js;
    std::vector<int> vec;
    vec.emplace_back(1);
    vec.emplace_back(2);
    vec.emplace_back(3);
    js["list"]=vec;
    std::map<int, std::string> map;
    map.insert({1,"黄山"});
    map.insert({2,"华山"});
    map.insert({3,"泰山"});
    js["path"] = map;
}
```

### 4.Json反序列化实例

```c++
std::string func3()
{
    json js;
    std::vector<int> vec;
    vec.emplace_back(1);
    vec.emplace_back(2);
    vec.emplace_back(3);
    js["list"]=vec;
    std::map<int, std::string> map;
    map.insert({1,"黄山"});
    map.insert({2,"华山"});
    map.insert({3,"泰山"});
    js["path"] = map;
    std::string sendBuf_ = js.dump();
    std::cout << sendBuf_ << std::endl;
    return sendBuf_;
}
int main()
{
    //func1();
    std::string recvBuf_ = func3();
    // json数据的反序列化
    json jsBuf_ = json::parse(recvBuf_);
    std::vector<int> vec = jsBuf_["list"];
    for (auto &p : vec)
      std::cout<<p<<std::endl;
}

```

## muduo网络库编程

#### 1.网络服务器编程常用的模型

|                    方案                     |          核心思想          |                        适用场景与特点                        |
| :-----------------------------------------: | :------------------------: | :----------------------------------------------------------: |
|              accept+send/recv               |        单进程服务器        |                          串行服务器                          |
|                 accept+fork                 | 为每个连接创建一个独立进程 |     适合并发连接数不大，计算任务工作量大于fork开销的场景     |
|               accept + thread               | 为每个连接创建一个独立线程 |    开销比方案2小，但高并发时会造成线程堆积过多，资源耗尽     |
|   muduo的网络设计（one loop per thread）    |    多Reactor多线程模型     | 一个main reactor负责accept新连接，然后通过round-robin轮询分发到sub reactor，连接的所有IO操作都在其所在的sub reactor线程中完成，能充分利用多核CPU |
| reactors in process（one loop per process） |    多Reactor多进程模型     | 以Nginx为代表，基于进程设计，多个Reactor作为IO进程和工作进程，通过accept锁解决惊群问题 |

### 2.muduo网络库的Reactor模型讲解

muduo采用的是one loop per thread的多Reactor模型，这是其高性能的核心：

- Main Reactor：
- 通常只有一个，由 base IO thread 负责。
- 主要职责是监听并 accept 新的客户端连接。
- Sub Reactor：
- 数量通常与CPU核心数相等，由 EventLoopThreadPool 线程池管理。
- Main Reactor接收到新连接后，会使用轮询（round-robin）的方式，将连接挂载到某个Sub Reactor上。
- 该连接上的所有后续IO操作（如读写数据），都将在这个Sub Reactor所属的线程中完成，避免了多线程切换的开销。
- 线程池（ThreadPool）：
- 对于那些耗费CPU的计算任务，不会阻塞在IO线程中，而是提交给独立的 ThreadPool 线程池去处理，保证了IO线程的高效运行。

### 3.muduo网络库服务器端代码实例

```c++
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <string>
#include <functional>
#include<iostream>
// 基于muduo网络库开发的服务器程序
// 1.组合TcpServer对象
// 2.创建EventLoop事件循环的指针
// 3.明确TcpServer构造函数需要什么参数,输出ChatServer的构造函数
// 4.在当前服务器当中设置用户连接创建和断开的回调与读写回调
//5.设置合适的线程数量，muduo库会自己分配IO线程和读写线程
class ChatServer {
public:
  ChatServer(muduo::net::EventLoop *loop,  //事件循环
             const muduo::net::InetAddress &listenAddr,  //IP+port
             const std::string &nameArg)   //服务器的名称
      : server_(loop, listenAddr, nameArg), loop_(loop) {
    // 给服务器注册用户连接的创建和断开回调
    server_.setConnectionCallback(std::bind(&ChatServer::onConnection,this,std::placeholders::_1));
    // 给服务器注册用户读写事件的回调
    server_.setMessageCallback(
        std::bind(&ChatServer::onMessage, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
    // 设置服务器端的线程的数量,1个IO线程，3个读写线程
    server_.setThreadNum(4);
  }
  //开启事件循环
  void start() {
    server_.start();
  }
  private:
    //专门处理用户的连接和断开epoll lis
    void onConnection(const muduo::net::TcpConnectionPtr &conn) {
      if (conn->connected()) {
        std::cout << conn->peerAddress().toIpPort() << "->"
                  << conn->localAddress().toIpPort()
                  << "status:online"
                  <<std::endl;
      }
      else {
        std::cout << conn->peerAddress().toIpPort() << "->"
                  << conn->localAddress().toIpPort() << " status:offline"
                  << std::endl;
        conn->shutdown(); // close(fd)
        //loop_->quit();
      }
    }
    void onMessage(const muduo::net::TcpConnectionPtr &conn,//表示连接
                   muduo::net::Buffer *buffer,//缓冲区
                   muduo::Timestamp time) {   //接收到数据的事件信息
      std::string buf = buffer->retrieveAllAsString();
      std::cout << "recv data:" << buf << " time: " << time.toString()
                << std::endl;
      conn->send(buf);
    }
  muduo::net::TcpServer server_;
  muduo::net::EventLoop *loop_; // epoll
};

int main() {
  muduo::net::EventLoop loop;
  muduo::net::InetAddress addr{"127.0.0.1", 6000};
  ChatServer server{&loop, addr, "ChatServer"};
  server.start(); //listenfd epoll_ctl一>epoll
  loop.loop();  //epoll_wait以阻塞的方式等待 新用户的连接，已连接的用户的读写事件等
}
```

## cmake简介

* 在vscode上下载cmake与cmaketools插件
* 关掉cmaketools自动编译选项：[VScode使用配置技巧：取消cmake-tools的频繁的自动配置和编译_vscode取消自动编译-CSDN博客](https://blog.csdn.net/qq_35914805/article/details/135532395)
* cmake详细细节请看我的另一篇博客cmake核心讲解

## 数据库设计

* User表

| 字段名称 |         字段类型         |   字段说明   |            约束            |
| :------: | :----------------------: | :----------: | :------------------------: |
|    id    |           INT            |    用户id    | primary key auto_increment |
|   name   |       VARCHAR(50)        |    用户名    |      not null unique       |
| password |       VARCHAR(50)        |   用户密码   |          not null          |
|  status  | ENUM('online','offline') | 当前登录状态 |     default 'offline'      |

* Friend表

| 字段说明 | 字段类型 | 字段说明 |       约束        |
| :------: | :------: | :------: | :---------------: |
|  userid  |   INT    |  用户id  | NOT NULL 联合主键 |
|  friend  |   INT    |  好友id  | NOT NULL 联合主键 |

* AllGroup表

| 字段名称  |  字段类型   |  字段说明  |            约束            |
| :-------: | :---------: | :--------: | :------------------------: |
|    id     |     INT     |    组id    | PRIMARY KEY AUTO_INCREMENT |
| groupname | VARCHAR(50) |   组名称   |          NOT NULL          |
| groupdesc | VARCHAR(20) | 组功能描述 |        DEFAULT ' '         |

* GroupUser表

| 字段名称  |         字段类型         | 字段说明 |       约束       |
| :-------: | :----------------------: | :------: | :--------------: |
|  groupid  |           INT            |   组id   |   PRIMARY KEY    |
|  userid   |           INT            |  组员id  |     NOT NULL     |
| grouprole | ENUM('creator','normal') | 组内角色 | DEFAULT 'normal' |

* OfflineMessage表

| 字段名称 |   字段类型   |         字段说明         |    约束     |
| :------: | :----------: | :----------------------: | :---------: |
|  userid  |     INT      |          用户id          | PRIMARY KEY |
| message  | VRACHAR(500) | 离线消息(存储Json字符串) |  NOT NULL   |


