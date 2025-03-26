#include "https_requester.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;

HTTPSRequester::HTTPSRequester(const string& host) : ioc_(), ctx_(ssl::context::sslv23_client), resolver_(ioc_), stream_(ioc_, ctx_), host_(host) {
    connect();
}

HTTPSRequester::~HTTPSRequester() {
    if (connected_)
        close();
}

void HTTPSRequester::connect() {
    boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> results;
    try {
        results = resolver_.resolve(host_, "443");
    } catch (const boost::system::system_error& e) {
        // cerr << "Boost System Error: " << e.what() << endl;
        cerr << "Error: " << e.code() << " (" << e.code().message() << ")" << endl;
        return;
    }

    beast::get_lowest_layer(stream_).connect(results);
    stream_.handshake(ssl::stream_base::client);
    connected_ = true;
}

string HTTPSRequester::get(const string& target) {
    if (!connected_)
        return "";
        
    return send_request(http::verb::get, target, "");
}

string HTTPSRequester::post(const string& target, const string& body) {
    if (!connected_)
        return "";
        
    return send_request(http::verb::post, target, body);
}

string HTTPSRequester::send_request(http::verb method, const string& target, const string& body) {
    http::request<http::string_body> req{method, target, 11};
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, "Boost.Beast");

    if (method == http::verb::post) {
        req.set(http::field::content_type, "application/x-www-form-urlencoded");
        req.body() = body;
        req.prepare_payload();
    }

    http::write(stream_, req);

    beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(stream_, buffer, res);

    return beast::buffers_to_string(res.body().data());
}

void HTTPSRequester::close() {
    beast::error_code ec;
    stream_.shutdown(ec);
}
