#include "server.h"
#include <iostream>

int main() {
    try {
        asio::io_context io_context;
        ChatServer server(io_context, 9000);
        server.start();
        std::cout << "进入事件循环..." << std::endl;
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
