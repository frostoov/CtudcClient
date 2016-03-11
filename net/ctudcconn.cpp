#include "ctudcconn.hpp"

using trek::net::Request;
using trek::net::Response;

using std::runtime_error;
using std::string;

CtudcConn::CtudcConn(const QString& hostName, quint16 port) {
	connectToHost(hostName, port);
}

void CtudcConn::connectToHost(const QString& hostName, quint16 port) {
	mSocket.connectToHost(hostName, port);
	if(!mSocket.waitForConnected(5000))
		throw std::runtime_error("CtudcConn::connectToHost timeout");
}

Response CtudcConn::send(const Request& request) {
	std::cout << "send: " << std::string(request) << std::endl;
	auto rawRequest = std::string(request);
	if( mSocket.write(rawRequest.data(), rawRequest.size()) == -1)
		throw runtime_error("CtudcClient::send failed send request");
	if( !mSocket.waitForReadyRead() )
		throw runtime_error("CtudcClient::send response timeout");
	auto response = Response(mSocket.readAll().toStdString());
	std::cout << "recv: " << std::string(response) << std::endl;
	if(!response.status)
		throw runtime_error(response.outputs.at(0).get<string>());
	if(response.object != request.object || response.method != request.method)
		throw runtime_error("CtudcClient::send invlide response");
	return response;
}
