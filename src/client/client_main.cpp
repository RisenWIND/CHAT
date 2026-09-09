#include "client.h"
#include <iostream>

int main() {
    ChatClient client("127.0.0.1", 9000);
    if (!client.connect()) return 1;
    client.start_receive();

    std::string input;
    std::cout << "\n命令:\n  login <username>  - 登录\n  send <message>    - 发送消息\n  quit              - 退出\n" << std::endl;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (input.empty()) continue;
        size_t pos = input.find(' ');
        std::string cmd = (pos == std::string::npos) ? input : input.substr(0, pos);
        std::string arg = (pos == std::string::npos) ? std::string() : input.substr(pos + 1);
        if (cmd == "login") {
            Message msg; msg.type = MSG_LOGIN; msg.length = static_cast<uint32_t>(arg.size()); memcpy(msg.data, arg.c_str(), msg.length); client.send_message(msg);
        } else if (cmd == "send") {
            Message msg; msg.type = MSG_TEXT; msg.length = static_cast<uint32_t>(arg.size()); memcpy(msg.data, arg.c_str(), msg.length); client.send_message(msg);
        } else if (cmd == "quit") {
            break;
        } else {
            std::cout << "未知命令" << std::endl;
        }
    }

    client.disconnect();
    return 0;
}
