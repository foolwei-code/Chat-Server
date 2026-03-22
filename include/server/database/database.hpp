#pragma once
#include <mysql/mysql.h>
#include <string>
#include<muduo/base/Logging.h>
// 数据库配置信息
static std::string server{"127.0.0.1"};
static unsigned int port{3307};
static std::string user{"root"};
static std::string password{"123456abc"};
static std::string dbname{"chat"};
class MySQL {
public:
  // 初始化数据库连接
  MySQL();
  // 释放数据库连接资源
  ~MySQL();
  //进行连接
  bool connect();
  //更新操作
  bool update(std::string sql);
  // 查询操作
  MYSQL_RES *query(std::string sql);
  // 获取连接
  MYSQL* getConnection();
private:
  MYSQL* conn_;
};