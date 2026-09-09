#pragma once
#include <asio.hpp>
#include <iostream>
#include <thread>
#include "../common/message.h"

using asio::ip::tcp;

class ChatClient {
public:
    ChatClient(const std::string& host, int port)
        : host_(host), port_(port), socket_(io_context_) {}

    bool connect() {
        try {
            tcp::resolver resolver(io_context_);
            auto results = resolver.resolve(host_, std::to_string(port_));
            asio::connect(socket_, results);
            std::cout << "连接到服务器成功" << std::endl;
            // 在另一个线程中运行 io_context（用于异步接收）
            io_thread_ = std::thread([this]() { io_context_.run(); });
            return true;
        } catch (std::exception& e) {
            std::cerr << "连接失败: " << e.what() << std::endl;
            return false;
        }
    }

    void disconnect() {
        if (socket_.is_open()) socket_.close();
        io_context_.stop();
        if (io_thread_.joinable()) io_thread_.join();
    }

    void send_message(const Message& msg) {
        std::string serialized = msg.serialize();
        try {
            asio::write(socket_, asio::buffer(serialized));
            std::cout << "消息已发送" << std::endl;
        } catch (std::exception& e) {
            std::cerr << "发送失败: " << e.what() << std::endl;
        }
    }

    void start_receive() { async_read(); }

private:
    void async_read() {
        auto buffer = std::make_shared<std::vector<char>>(1024);
        socket_.async_read_some(
            asio::buffer(*buffer),
            [this, buffer](const asio::error_code& ec, std::size_t bytes_read) {
                if (!ec && bytes_read > 0) {
                    std::string raw(buffer->data(), bytes_read);
                    Message msg;
                    int msg_size = Message::deserialize(raw, msg);
                    if (msg_size > 0) handle_message(msg);
                    async_read();
                } else {
                    std::cerr << "连接已断开: " << ec.message() << std::endl;
                }
            }
        );
    }

    void handle_message(const Message& msg) {
        switch (msg.type) {
            case MSG_LOGIN_RESPONSE: {
                std::string response(msg.data, msg.length);
                std::cout << "服务器: " << response << std::endl;
                break;
            }
            case MSG_TEXT: {
                std::string text(msg.data, msg.length);
                std::cout << "服务器: " << text << std::endl;
                break;
            }
            case MSG_HEARTBEAT: {
                std::cout << "收到心跳" << std::endl;
                break;
            }
            default:
                std::cout << "未知消息类型: " << (int)msg.type << std::endl;
        }
    }

    std::string host_;
    int port_;
    asio::io_context io_context_;
    tcp::socket socket_;
    std::thread io_thread_;
};
