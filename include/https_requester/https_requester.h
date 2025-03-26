#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <string>

using namespace std;

class HTTPSRequester {
public:
    explicit HTTPSRequester(const string& host);
    ~HTTPSRequester();

    string get(const string& target);
    string post(const string& target, const string& body);
    operator bool() const { return connected_; }

private:
    boost::asio::io_context ioc_;
    boost::asio::ssl::context ctx_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    string host_;
    bool connected_ = false;

    void connect();
    string send_request(boost::beast::http::verb method, const string& target, const string& body);
    void close();
};

#include "https_requester.cpp"