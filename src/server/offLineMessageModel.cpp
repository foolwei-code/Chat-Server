#include "offLineMessageModel.hpp"
#include "database.hpp"
#include <mysql/mysql.h>
// 存储用户的离线消息
void offLineMsModel::insert(int userId, std::string message) {
  // 1.组装sql语句
  char sql[1024]{};
  sprintf(sql, "insert into offlinemessage values(%d,'%s')", userId,
          message.c_str());
  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}
// 删除用户的离线消息
void offLineMsModel::remove(int userId) {
  // 1.组装sql语句
  char sql[1024]{};
  sprintf(sql, "delete from offlinemessage where userid=%d", userId);
  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}
// 查询用户的离线消息
std::vector<std::string> offLineMsModel::query(int userId) {
   // 1.组装sql语句
  char sql[1024]{};
  sprintf(sql, "select message from offlinemessage where userid=%d", userId);
  MySQL mysql;
  std::vector<std::string>vec;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
      // 将userid用户的所有离线消息放入vec中
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != nullptr) {
        vec.emplace_back(row[0]);
      }
      mysql_free_result(res);
    }
  }
  return vec;
}