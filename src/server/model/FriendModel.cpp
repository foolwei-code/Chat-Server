#include "FriendModel.hpp"
#include "database.hpp"
#include <cstdio>
#include <mysql/mysql.h>
// 添加好友关系
void FriendModel::insert(int userId, int friendId) {
  char sql[1024]{};
  sprintf(sql, "insert into friend values(%d,%d)", userId, friendId);
  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}
// 返回用户好友列表
std::vector<User> FriendModel::query(int userId) {
  char sql[1024]{};
  sprintf(sql,
          "select a.id,a.name,a.status from user a join friend b on "
          "b.frinedid=a.userid where b.userid=%d",
          userId);
  std::vector<User> vec;
  MySQL mysql;
  if (mysql.connect()) {
    MYSQL_RES *res=mysql.query(sql);
    if (res != nullptr) {
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != nullptr) {
        User user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setStatus(row[2]);
        vec.emplace_back(user);
      }
      mysql_free_result(res);
    }
  }
  return vec;
}