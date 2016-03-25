#pragma once

#include <trek/net/request.hpp>
#include <trek/net/response.hpp>

#include <mutex>
#include <thread>
#include <boost/asio/ip/tcp.hpp>

class CtudcConn {
	using Buffer = std::vector<char>;
	using Socket = boost::asio::ip::tcp::socket;
	using RecvCallback = std::function<void(const trek::net::Response& response)>;
public:
	CtudcConn();
	CtudcConn(const std::string& hostName, uint16_t port);
	~CtudcConn();
	void connect(const std::string& hostName, uint16_t port);
	void disconnect();
	void send(const trek::net::Request& request);
	void onRecv(const RecvCallback& callback);
protected:
	void run();
	void recv();
private:
	boost::asio::io_service mIoService;
	Socket mSocket;

	Buffer mBuffer;
	uint64_t mMsgSize;

	RecvCallback mOnRecv;

	std::thread mThread;
	std::mutex mMutex;
};
