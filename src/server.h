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
    Session(std::shared_ptr<ba::ip::tcp::socket> socket, std::set<std::shared_ptr<Session>>& cl, std::shared_ptr<data_base::Database> data) :
        socket_(socket),
        cl_(cl),
        db_(data)
    {
        sb_ = std::make_shared<boost::asio::streambuf>();
    }

    void start_session()
    {
        session_mutex.lock();
        cl_.insert(shared_from_this());
        session_mutex.unlock();
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
                    std::getline(in, str_);
                    do_write();
                }
                else
                {
                    session_mutex.lock();
                    cl_.erase(shared_from_this());
                    session_mutex.unlock();
                }
            });
    }

    void do_write()
    {
        str_ = db_->query(std::move(str_));
        str_ += "\n";
        auto self(shared_from_this());
        socket_->async_write_some(boost::asio::buffer(str_),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
                else
                {
                    session_mutex.lock();
                    cl_.erase(shared_from_this());
                    session_mutex.unlock();
                }
            });
    }

    std::shared_ptr<ba::ip::tcp::socket> socket_;   
    std::set<std::shared_ptr<Session>>& cl_;        //ссылка на контейнер с сессиями
    std::shared_ptr<boost::asio::streambuf> sb_;    //указатель на буфер для ввода/вывода
    std::shared_ptr<data_base::Database> db_;       //указатель на объект БД
    std::string str_;                               //строка для хранения запроса от клиента и ответа от БД
};


class Server
{
public:
    Server(ba::io_service& io_service, const ba::ip::tcp::endpoint& endpoint)
        : service_(io_service),
        acceptor_(io_service, endpoint)
    {
        data_= std::make_shared<data_base::Database>();
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
