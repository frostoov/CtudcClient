#include "ctudcconn.hpp"

#include <chrono>
#include <cassert>

#include <QIODevice>

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <trek/common/timeprint.hpp>

using trek::net::Request;
using trek::net::Response;

using std::runtime_error;
using std::string;
using std::lock_guard;
using std::mutex;
using std::string;
using std::chrono::system_clock;

namespace asio = boost::asio;


CtudcConn::CtudcConn()
	: mSocket(mIoService) { }

CtudcConn::CtudcConn(const string& hostName, uint16_t port)
	: mSocket(mIoService) {
	connectToHost(hostName, port);
}

void CtudcConn::connectToHost(const string& hostName, uint16_t port) {
	auto addr = asio::ip::address::from_string(hostName);
	asio::ip::tcp::endpoint ep(addr, port);
	mSocket.connect(ep);
}

void CtudcConn::disconnect() {
	mSocket.shutdown(asio::socket_base::shutdown_both);
	mSocket.close();
}

Response CtudcConn::send(const Request& request) {
	auto rawRequest = std::string(request);
	uint64_t length = rawRequest.size();
	lock_guard<mutex> lock(mMutex);

	asio::write(mSocket, asio::buffer(&length, sizeof(length)));
	asio::write(mSocket, asio::buffer(rawRequest.data(), rawRequest.size()));
	std::cout << system_clock::now() << " Send: " << rawRequest << std::endl;
	asio::read(mSocket, asio::buffer(&length, sizeof(length)));
	mBuffer.resize(length);
	asio::read(mSocket, asio::buffer(mBuffer));
	auto rawResponse = string(mBuffer.data(), mBuffer.size());
	std::cout << system_clock::now() << " Recv: " << rawResponse << std::endl;
	auto response = Response(rawResponse);

	if(!response.status)
		throw runtime_error(response.outputs.at(0).get<string>());
	if(response.object != request.object || response.method != request.method)
		throw runtime_error("CtudcClient::send invlide response");
	return response;
}
