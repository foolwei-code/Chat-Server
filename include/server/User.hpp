#pragma once
#include <string>
//User表的ORM类
class User {
public:
  User(int id = 1, std::string name = "", std::string passWord = "",
       std::string status = "offline")
      : id_(id), name_(name), passWord_(passWord), status_(status) {}
  ~User() = default;
  //设置数据库属性
  void setId(int id) { id_ = id; }
  void setName(std::string name) { name_ = name; }
  void setPassWord(std::string passWord) { passWord_ = passWord; }
  void setStatus(std::string status) { status_ = status; }
  // 获取数据库属性
  int getId() { return id_; }
  std::string getName() { return name_; }
  std::string getPassWord() { return passWord_; }
  std::string getStatus(){return status_;}
private:
  int id_;
  std::string name_;
  std::string passWord_;
  std::string status_;
};