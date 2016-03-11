#include "tdccontroller.hpp"

#include <stdexcept>

using std::shared_ptr;
using std::string;

TdcController::TdcController(shared_ptr<CtudcConn> conn)
	: mConn(conn) { }

void TdcController::open() {
	auto response = mConn->send({"tdc", __func__});
}

void TdcController::close() {
	auto response = mConn->send({"tdc", __func__});
}

bool TdcController::isOpen() {
	auto response = mConn->send({"tdc", __func__});
	return response.outputs.at(0).get<bool>();
}

void TdcController::clear() {
	auto response = mConn->send({"tdc", __func__});
}

void TdcController::reset() {
	auto response = mConn->send({"tdc", __func__});
}

uint16_t TdcController::stat() {
	auto response = mConn->send({"tdc", __func__});
	return response.outputs.at(0).get<uint16_t>();
}

uint16_t TdcController::ctrl() {
	auto response = mConn->send({"tdc", __func__});
	return response.outputs.at(0).get<uint16_t>();
}

Tdc::Mode TdcController::mode() {
	auto response = mConn->send({"tdc", __func__});
	return Tdc::Mode( response.outputs.at(0).get<int>() );
}

bool TdcController::tdcMeta() {
	auto response = mConn->send({"tdc", __func__});
	return response.outputs.at(0).get<bool>();
}

void TdcController::setMode(Tdc::Mode mode) {
	auto response = mConn->send({"tdc", __func__, {int(mode)}});
}

void TdcController::setWindowWidth(unsigned width) {
	auto response = mConn->send({"tdc", __func__, {width}});
}

void TdcController::setWindowOffset(int offset) {
	auto response = mConn->send({"tdc", __func__, {offset}});
}

void TdcController::setEdgeDetection(Tdc::EdgeDetection ed) {
	auto response = mConn->send({"tdc", __func__, {int(ed)}});
}

void TdcController::setLsb(int lsb) {
	auto response = mConn->send({"tdc", __func__, {lsb}});
}

void TdcController::setCtrl(uint16_t ctrl) {
	auto response = mConn->send({"tdc", __func__, {ctrl}});
}

void TdcController::setTdcMeta(bool flag) {
	auto response = mConn->send({"tdc", __func__, {flag}});
}

Tdc::Settings TdcController::settings() {
	auto response = mConn->send({"tdc", __func__});
	return Tdc::Settings{
		response.outputs.at(0).get<unsigned>(),
		response.outputs.at(1).get<int>(),
		Tdc::EdgeDetection(response.outputs.at(2).get<int>()),
		response.outputs.at(3).get<unsigned>(),
	};
}

Tdc::Settings TdcController::updateSettings() {
	auto response = mConn->send({"tdc", __func__});
	return Tdc::Settings{
		response.outputs.at(0).get<unsigned>(),
		response.outputs.at(1).get<int>(),
		Tdc::EdgeDetection(response.outputs.at(2).get<int>()),
		response.outputs.at(3).get<unsigned>(),
	};
}
