# CHAT - 简易即时通讯教学项目

这是一个用于教学的简易 C++ 即时通讯示例（控制台客户端 + 服务器）。

目标：
- 用最小的代码演示服务器-客户端通信（基于 asio）
- 使用 CMake 构建
- 便于初学者逐步扩展为 GUI / 数据库 / 加密等功能

结构：

```
CHAT/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── .gitignore
├── src/
│   ├── common/
│   │   └── message.h
│   ├── server/
│   │   ├── server.h
│   │   └── server_main.cpp
│   └── client/
│       ├── client.h
│       └── client_main.cpp
```

说明：
- 本仓库使用 standalone Asio（头文件库）作为网络库。Windows 下推荐用 vcpkg 安装 asio 或使用 Boost.Asio。
- 教学案例以可读性为主，省略了大量生产环境中必须的错误处理和安全措施。
