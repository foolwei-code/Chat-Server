aux_source_directory(./ SRC_LIST)
aux_source_directory(./database SRC_LIST)
add_executable(ChatServer ${SRC_LIST})
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread )