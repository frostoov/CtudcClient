#pragma once

#include "request.hpp"
#include "response.hpp"

#include <mutex>
#include <boost/asio/ip/tcp.hpp>

class CtudcConn {
	using Socket = boost::asio::ip::tcp::socket;
public:
	CtudcConn();
	CtudcConn(const std::string& hostName, uint16_t port);
	void connectToHost(const std::string& hostName, uint16_t port);
	void disconnect();
	trek::net::Response send(const trek::net::Request& request);
private:
	boost::asio::io_service mIoService;
	Socket mSocket;
	std::mutex mMutex;
	std::vector<char> mBuffer;
};

