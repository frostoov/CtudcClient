#include "ctudcconn.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>

#include <trek/common/timeprint.hpp>

#include <mutex>

using trek::net::Request;
using trek::net::Response;

namespace asio = boost::asio;

using std::runtime_error;
using std::string;
using std::vector;
using std::lock_guard;
using std::mutex;
using std::make_unique;

namespace asio = boost::asio;

class CtudcConn::impl {
public:
    impl(const string& addr, uint16_t port)
        : mSocket(mIoService) {
        connect(addr, port);
    }

    ~impl() {
        disconnect();
    }
    
    void connect(const string& addr, uint16_t port) {
        auto endpoint = asio::ip::tcp::endpoint(asio::ip::address::from_string(addr), port);
        mSocket.connect(endpoint);
    }
    
    void disconnect() {
        mSocket.cancel();
        mSocket.shutdown(mSocket.shutdown_both);
    }

    Response send(const Request& request) {
        auto str = string(request);
        auto length = uint64_t(str.size());
        auto buf = vector<char>();
        {
            std::lock_guard<std::mutex> lk(mMutex);
            asio::write(mSocket, asio::buffer(&length, sizeof(length)));
            asio::write(mSocket, asio::buffer(str));
            asio::read(mSocket, asio::buffer(&length, sizeof(length)));
            buf.resize(length);
            asio::read(mSocket, asio::buffer(buf));
        }
        auto response = Response({buf.begin(), buf.end()});
        std::cout << "send: " << string(request) << std::endl;
        std::cout << "recv: " << string(response) << std::endl;
        if(!response.status.empty())
            throw runtime_error(string("CtudcServer failed handle response") + response.status);
        return response;
    }
private:
    boost::asio::io_service mIoService;
    boost::asio::ip::tcp::endpoint mEndpoint;
    boost::asio::ip::tcp::socket mSocket;

    std::mutex mMutex;
};

CtudcConn::CtudcConn(const string& addr, uint16_t port)
    : mImpl(make_unique<impl>(addr, port)) { }

CtudcConn::~CtudcConn() { }

Response CtudcConn::send(const Request& request) {
    return mImpl->send(request);
}
