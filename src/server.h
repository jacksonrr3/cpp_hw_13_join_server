#pragma once
#include <array>
#include <set>
#include <mutex>
#include <iostream>
#include <utility>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "data_base.h"

namespace ba = boost::asio;
std::mutex session_mutex;

class Session
    : public std::enable_shared_from_this<Session>
{
public:
    Session(std::shared_ptr<ba::ip::tcp::socket> socket, std::set<std::shared_ptr<Session>>& cl, std::shared_ptr<data_base::Database> data):
        socket_(socket),
        cl_(cl),
        db_(data)
    {
    }

    void start_session()
    {
        session_mutex.lock();
        cl_.insert(shared_from_this());
        session_mutex.unlock();
       // id_ = async::connect(bl_size_);
       // buff_ = std::make_shared<std::array<char, buff_size>>();
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        async_read_until(*socket_, *sb_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    std::istream in(sb_.get());
                    std::string line;
                    std::getline(in, line);
                    std::string res = db_->query(std::move(line));
                    do_write(std::move(res));
                //    async::receive(id_, buff_->data(), length);
                //    do_read();
                }
                else
                {
                 //   async::disconnect(id_);
                 //   session_mutex.lock();
                    cl_.erase(shared_from_this());
                 //   session_mutex.unlock();
                }
            });
    }

    void do_write(std::string&& str)
    {
        str += "\n";
        auto self(shared_from_this());
        socket_->async_write_some(boost::asio::buffer(str),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                   // std::string res = db_->query(std::move(*buff_));
                    do_read();
                    //    async::receive(id_, buff_->data(), length);
                    //    do_read();
                }
                else
                {
                    //   async::disconnect(id_);
                    //   session_mutex.lock();
                    cl_.erase(shared_from_this());
                    //   session_mutex.unlock();
                }
            });
    }

    std::shared_ptr<ba::ip::tcp::socket> socket_;
    std::set<std::shared_ptr<Session>>& cl_;
    std::shared_ptr<boost::asio::streambuf> sb_;
    std::shared_ptr<data_base::Database> db_;
};


class Server
{
public:
    Server(ba::io_service& io_service, const ba::ip::tcp::endpoint& endpoint)
        : service_(io_service),
        acceptor_(io_service, endpoint)
    {
        socket_ = std::make_shared<ba::ip::tcp::socket>(service_);
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(*socket_,
            [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    std::make_shared<Session>(socket_, clients_, data_)->start_session();
                }
                socket_ = std::make_shared<ba::ip::tcp::socket>(service_);
                do_accept();
            });
    }

    ba::io_service& service_;
    ba::ip::tcp::acceptor acceptor_;
    std::shared_ptr<ba::ip::tcp::socket> socket_;
    std::set<std::shared_ptr<Session>> clients_;
    std::shared_ptr<data_base::Database> data_;
};
