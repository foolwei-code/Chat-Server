#pragma once
#include <string>
#include<vector>
//提供离线消息表的操作接口方法
class offLineMsModel {
public:
  // 存储用户的离线消息
  void insert(int userId, std::string message);
  // 删除用户的离线消息
  void remove(int userId);
  // 查询用户的离线消息
  std::vector<std::string> query(int userId);
private:
  
};