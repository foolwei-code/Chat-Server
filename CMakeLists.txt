cmake_minimum_required(VERSION 3.10)
project(ChatServer)
#添加调试信息
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
#设置c++标准
set(CMAKE_CXX_STANDARD 20)
#设置最终可执行文件的所属目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#链接头文件所在路径
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCR_DIR}/include/client)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/include/server/database)
#链接的库文件所在的路径
link_directories(/usr/lib64/mysql/)
#加载子目录
add_subdirectory(src)
