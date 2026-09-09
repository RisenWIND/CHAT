cmake_minimum_required(VERSION 3.10)
project(CHAT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 尝试寻找 Asio（standalone 或 Boost 的 Asio 都可以）
find_package(asio REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

# 服务器可执行文件
add_executable(chat_server
    src/server/server_main.cpp
)

target_link_libraries(chat_server
    asio::asio
)

# 客户端可执行文件
add_executable(chat_client
    src/client/client_main.cpp
)

target_link_libraries(chat_client
    asio::asio
)
