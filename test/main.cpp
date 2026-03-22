#if 0
#include "json.hpp"
#include <iostream>
#include <string>
#include <vector>
#include<map>
using json = nlohmann::json;
// json示例1
std::string func1() {
  json js1;
  js1["message_type"] = 2;
  js1["from"] = "zhangsan";
  js1["to"] = "lisi";
  js1["message"] = "how do you do";
  std::cout << js1 << std::endl;
  return js1.dump();
}
// json示例2
void func2() {
  json js2;
  // 添加数组
  js2["array"] = {1, 2, 3, 4};
  // 添加json对象
  js2["employee"]["name"] = "zhangsan";
  js2["employee"]["id"] = 10;
  std::cout<<js2<<std::endl;
}
// json3示例3(与容器进行搭配)
std::string func3() {
  std::vector<int> vec{1, 2, 3, 4};
  json js3;
  js3["vector"] = vec;
  std::map<int, std::string> map;
  map.insert({1, "zhangsan"});
  map.insert({2, "lisi"});
  js3["map"] = map;
  std::cout << js3 << std::endl;
  return js3.dump();
}
int main() {
  std::cout << func1() << std::endl;
  func2();
  json js4 = json::parse(func3());
  for (auto &p : js4["vector"])
    std::cout << p << " ";
  std::cout<<std::endl;
}
#endif