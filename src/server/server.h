#pragma once
#include <asio.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>
#include "../common/message.h"

using asio::ip::tcp;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    using pointer = std::shared_ptr<ClientSession>;
    static pointer create(asio::io_context& io_context, int client_id) {
        return pointer(new ClientSession(io_context, client_id));
    }

    tcp::socket& socket() { return socket_; }

    void start() {
        std::cout << "[客户端 " << client_id_ << "] 已连接，等待数据..." << std::endl;
        async_read();
    }

    void send_message(const Message& msg) {
        std::string serialized = msg.serialize();
        asio::async_write(
            socket_,
            asio::buffer(serialized),
            [this, serialized](const asio::error_code& ec, std::size_t bytes_written) {
                if (!ec) {
                    std::cout << "[客户端 " << client_id_ << "] 发送 " << bytes_written << " 字节" << std::endl;
                } else {
                    std::cerr << "[客户端 " << client_id_ << "] 发送失败: " << ec.message() << std::endl;
                }
            }
        );
    }

    int get_id() const { return client_id_; }

private:
    ClientSession(asio::io_context& io_context, int id)
        : socket_(io_context), client_id_(id) {}

    void async_read() {
        auto buffer = std::make_shared<std::vector<char>>(1024);
        socket_.async_read_some(
            asio::buffer(*buffer),
            [this, buffer](const asio::error_code& ec, std::size_t bytes_read) {
                if (!ec && bytes_read > 0) {
                    std::string raw(buffer->data(), bytes_read);
                    std::cout << "[客户端 " << client_id_ << "] 收到 " << bytes_read << " 字节" << std::endl;
                    Message msg;
                    int msg_size = Message::deserialize(raw, msg);
                    if (msg_size > 0) handle_message(msg);
                    async_read();
                } else {
                    std::cerr << "[客户端 " << client_id_ << "] 连接断开: " << ec.message() << std::endl;
                }
            }
        );
    }

    void handle_message(const Message& msg) {
        std::cout << "[客户端 " << client_id_ << "] 收到消息类型: " << (int)msg.type << std::endl;
        switch (msg.type) {
            case MSG_LOGIN: {
                std::string username(msg.data, msg.length);
                std::cout << "[客户端 " << client_id_ << "] 登录用户名: " << username << std::endl;
                Message response;
                response.type = MSG_LOGIN_RESPONSE;
                std::string response_text = "LOGIN_OK";
                response.length = static_cast<uint32_t>(response_text.length());
                memcpy(response.data, response_text.c_str(), response.length);
                send_message(response);
                break;
            }
            case MSG_TEXT: {
                std::string text(msg.data, msg.length);
                std::cout << "[客户端 " << client_id_ << "] 说: " << text << std::endl;
                break;
            }
            case MSG_HEARTBEAT: {
                Message response;
                response.type = MSG_HEARTBEAT;
                response.length = 0;
                send_message(response);
                break;
            }
            default:
                std::cerr << "[客户端 " << client_id_ << "] 未知消息类型" << std::endl;
        }
    }

    tcp::socket socket_;
    int client_id_;
    std::vector<char> buffer_;
};

class ChatServer {
public:
    ChatServer(asio::io_context& io_context, int port)
        : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        std::cout << "服务器启动，监听端口 " << port << std::endl;
    }

    void start() { accept_connection(); }

private:
    void accept_connection() {
        auto new_session = ClientSession::create(io_context_, ++client_counter_);
        acceptor_.async_accept(
            new_session->socket(),
            [this, new_session](const asio::error_code& ec) {
                if (!ec) {
                    std::cout << "新连接: 客户端 " << new_session->get_id() << std::endl;
                    sessions_[new_session->get_id()] = new_session;
                    new_session->start();
                } else {
                    std::cerr << "接受连接失败: " << ec.message() << std::endl;
                }
                accept_connection();
            }
        );
    }

    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::map<int, ClientSession::pointer> sessions_;
    int client_counter_ = 0;
};
