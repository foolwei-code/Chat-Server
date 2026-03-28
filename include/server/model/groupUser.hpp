#pragma once
#include "User.hpp"
#include <string>
// 群组用户，多了个role角色信息，从User类直接继承，复用User的其他信息
class GroupUser : public User {
public:
  void setRole(std::string role) { this->role_ = role; };
  std::string getRole() { return role_; }

private:
  std::string role_;
};