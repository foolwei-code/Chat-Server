#include "UserModel.hpp"
#include "database.hpp"
#include <cstdio>
#include <mysql/mysql.h>
// User表的增加方法
bool UserModel::insert(User &user) {
  // 组装sql语句
  char sql[1024] = {};
  sprintf(sql, "insert into user(name,password,status)values('%s','%s','%s')",
          user.getName().c_str(), user.getPassWord().c_str(),
          user.getStatus().c_str());
  MySQL mysql;
  if (mysql.connect()) {
    if (mysql.update(sql)) {
      // 获取插入成功的用户数据生成的主键id
      user.setId(mysql_insert_id(mysql.getConnection()));
      return true;
    }
  }
  return false;
}
//根据用户号码查询用户信息
User UserModel::query(int id) {
  // 1.组装sql语句
  char sql[1024]{};
  sprintf(sql, "select * from user where id=%d", id);
  MySQL mysql;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr) {
      MYSQL_ROW row = mysql_fetch_row(res);
      if (row != nullptr) {
        User user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setPassWord(row[2]);
        user.setStatus(row[3]);
        mysql_free_result(res);
        return user;
      }
    }
  }
  return User();
}
bool UserModel::updateStatus(User user) {
  // 1.组装sql语句
  char sql[1024]{};
  sprintf(sql,"update user set status='%s'where id=%d",user.getStatus().c_str(),user.getId());
  MySQL mysql;
  if (mysql.connect()) {
    if (mysql.update(sql))
      return true;
  }
  return false;
}
 // 重置用户的状态信息
void UserModel::resetStatus() {
  char sql[1024]{};
  sprintf(sql, "update user set status='offline' where status='online'");
  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}