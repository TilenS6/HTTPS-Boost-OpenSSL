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
    asio::ip::basic_resolver_results<asio::ip::tcp> results;
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

string HTTPSRequester::request(const string& target, const string& type, const string& header_key_vals, const string& body) {

    if (!connected_)
        return "";

    http::verb v = http::string_to_verb(type);

    if (v == http::verb::unknown) {
        cerr << "Unknown HTTP verb: " << type << endl;
        return "";
    }
        
    return send_request(v, target, body, header_key_vals);
}

string HTTPSRequester::send_request(http::verb method, const string& target, const string& body, const string& header_key_vals) {
    http::request<http::string_body> req{method, target, 11};
    req.set(http::field::host, host_);
    req.set(http::field::user_agent, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36");
    req.set(http::field::accept, "*/*");
    req.set(http::field::accept_encoding, "gzip"); //, deflate, br"); // TODO: Add deflate and br if needed
    req.set(http::field::accept_language, "en-US,en;q=0.9");


    bool typing_key = true;
    string key = "", val = "";
    for (char c : header_key_vals) {
        if (c == '=' || c == ';') {
            typing_key = !typing_key;
            if (typing_key) {
                req.set(key, val);
                key = val = ""; // Reset key and value for the next pair
            }
            continue;
        }
        if (typing_key) {
            key += c;
        } else {
            val += c;
        }
    }
    if (!key.empty() && !val.empty()) {
        req.set(key, val); // Set the last key-value pair if any
    }

    if (method == http::verb::post) {
        req.set(http::field::content_type, "application/x-www-form-urlencoded");
        req.body() = body;
        req.prepare_payload();
    }

    http::write(stream_, req);

    beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(stream_, buffer, res);


    //cout << "Status: " << res.result_int() << " " << obsolete_reason(res.result()) << "\n";

    //cout << res << "\n";
    //cout << "######## " << res.chunked() << "########\n";

    if (res.chunked()) {
        string compressed_data = beast::buffers_to_string(res.body().data());
        return decompressGZip(compressed_data);
    }

    return beast::buffers_to_string(res.body().data());
}

void HTTPSRequester::close() {
    beast::error_code ec;
    stream_.shutdown(ec);
}
