#pragma once
#include <string>
#include <ctime>
#include <cstdint>
#include <cstring>

// 消息类型枚举
enum MessageType {
    MSG_LOGIN = 1,      // 登录请求
    MSG_LOGIN_RESPONSE = 2,  // 登录响应
    MSG_TEXT = 3,       // 文本消息
    MSG_HEARTBEAT = 4   // 心跳包
};

// 简单的消息结构
// 为了教学简化，我们直接用简单的二进制格式
struct Message {
    uint8_t type;           // 消息类型（1 字节）
    uint32_t length;        // 消息体长度（4 字节，大端）
    char data[4096];        // 消息体（最大 4KB）
    
    std::string serialize() const {
        std::string result;
        result.push_back(static_cast<char>(type));
        unsigned char len_bytes[4];
        len_bytes[0] = (length >> 24) & 0xFF;
        len_bytes[1] = (length >> 16) & 0xFF;
        len_bytes[2] = (length >> 8) & 0xFF;
        len_bytes[3] = (length) & 0xFF;
        for (int i = 0; i < 4; ++i) result.push_back(len_bytes[i]);
        result.append(data, length);
        return result;
    }

    // 从 raw 中尝试解析一个完整消息。返回已消费的字节数（0 表示不完整）
    static int deserialize(const std::string& raw, Message& msg) {
        if (raw.size() < 5) return 0;
        msg.type = static_cast<uint8_t>(raw[0]);
        uint32_t len = 0;
        len |= (static_cast<unsigned char>(raw[1]) << 24);
        len |= (static_cast<unsigned char>(raw[2]) << 16);
        len |= (static_cast<unsigned char>(raw[3]) << 8);
        len |= (static_cast<unsigned char>(raw[4]));
        msg.length = len;
        if (raw.size() < 5 + len) return 0;
        if (len > sizeof(msg.data)) return 0; // 防止溢出（简单保护）
        memcpy(msg.data, raw.data() + 5, len);
        return 5 + static_cast<int>(len);
    }
};
