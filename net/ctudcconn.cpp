#include "ctudcconn.hpp"

#include <chrono>
#include <cassert>

#include <QIODevice>

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <trek/common/timeprint.hpp>

using trek::net::Request;
using trek::net::Response;

using boost::asio::async_read;
using boost::asio::async_write;

using std::runtime_error;
using std::string;
using std::lock_guard;
using std::mutex;
using std::string;
using std::chrono::system_clock;

namespace asio = boost::asio;


CtudcConn::CtudcConn()
    : mSocket(mIoService) {

}

CtudcConn::CtudcConn(const string& hostName, uint16_t port)
    : mSocket(mIoService) {
    connect(hostName, port);
}

CtudcConn::~CtudcConn() {
    disconnect();
}

void CtudcConn::connect(const string& hostName, uint16_t port) {
    auto addr = asio::ip::address::from_string(hostName);
    asio::ip::tcp::endpoint ep(addr, port);
    mSocket.connect(ep);
    run();
}

void CtudcConn::disconnect() {
    mSocket.cancel();
    mIoService.stop();
    if(mThread.joinable())
        mThread.join();
}

void CtudcConn::run() {
    recv();
    mIoService.reset();
    mThread = std::thread([this] {
        mIoService.run();
    });
}

void CtudcConn::recv() {
    async_read(mSocket, asio::buffer(&mMsgSize, sizeof(mMsgSize)), [this](auto & errCode, auto l) {
        if(errCode || l != sizeof(mMsgSize)) {
            std::cout << "Failed recv msg: " << errCode << ". restart app please" << std::endl;
            return;
        }
        mBuffer.resize(mMsgSize);
        async_read(mSocket, asio::buffer(mBuffer), [this] (auto & errCode, auto l) {
            if(errCode || mBuffer.size() != l) {
                std::cout << "Failed recv msg: " << errCode << ". restart app please" << std::endl;
                return;
            }
            auto msg = std::string(mBuffer.data(), mBuffer.size());
            std::cout << "Recv: " << msg << std::endl;
            mOnRecv(Response(msg));
            recv();
        });
    });
}

void CtudcConn::send(const Request& request) {
    auto rawRequest = string(request);
    uint64_t length = rawRequest.size();
    std::ostringstream stream;
    stream.write(reinterpret_cast<char*>(&length), sizeof(length));
    stream << rawRequest;
    async_write(mSocket, asio::buffer(stream.str()), [this, rawRequest](auto & errCode, auto) {
        if(errCode) {
            std::cout << "Failed send msg: " << errCode << ". restart app please" << std::endl;
        }
        //TODO
    });
}

void CtudcConn::onRecv(const RecvCallback& callback) {
    mOnRecv = callback;
}
