#pragma once

#include "request.hpp"
#include "response.hpp"

#include <QTcpSocket>

class CtudcConn {
public:
	CtudcConn() = default;
	CtudcConn(const QString& hostName, quint16 port);
	void connectToHost(const QString& hostName, quint16 port);
	trek::net::Response send(const trek::net::Request& request);
private:
	QTcpSocket mSocket;
};

