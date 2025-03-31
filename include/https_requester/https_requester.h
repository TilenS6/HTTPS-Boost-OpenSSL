#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/copy.hpp>
#include <string>
#include <sstream>

using namespace std;

class HTTPSRequester {
public:
    explicit HTTPSRequester(const string& host);
    ~HTTPSRequester();

    string request(const string& target, const string& type = "GET", const string& header_key_vals = "", const string& body = "");
    // string post(const string& target, const string& body);
    operator bool() const { return connected_; }

private:
    boost::asio::io_context ioc_;
    boost::asio::ssl::context ctx_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    string host_;
    bool connected_ = false;

    void connect();
    string send_request(boost::beast::http::verb method, const string& target, const string& body = "", const string& = "");
    void close();
};

string decompressGZip(const string& compressedData);

#include "encodings.cpp"
#include "https_requester.cpp"