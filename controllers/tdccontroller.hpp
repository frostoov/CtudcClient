#pragma once

#include "net/ctudcconn.hpp"
#include "tdc/tdc.hpp"

#include <memory>

class TdcController {
public:
	TdcController(std::shared_ptr<CtudcConn> conn) : mConn(conn) { }

	void open() {
		mConn->send({"tdc", __func__});
	}
	void close() {
		mConn->send({"tdc", __func__});
	}
	void isOpen() {
		mConn->send({"tdc", __func__});
	}

	void clear() {
		mConn->send({"tdc", __func__});
	}
	void reset() {
		mConn->send({"tdc", __func__});
	}
	void stat() {
		mConn->send({"tdc", __func__});
	}
	void ctrl() {
		mConn->send({"tdc", __func__});
	}
	void mode() {
		mConn->send({"tdc", __func__});
	}
	void tdcMeta() {
		mConn->send({"tdc", __func__});
	}
	void setMode(Tdc::Mode mode) {
		mConn->send({"tdc", __func__, {int(mode)}});
	}
	void setWindowWidth(unsigned width) {
		mConn->send({"tdc", __func__, {width}});
	}
	void setWindowOffset(int offset) {
		mConn->send({"tdc", __func__, {offset}});
	}
	void setEdgeDetection(Tdc::EdgeDetection ed) {
		mConn->send({"tdc", __func__, {int(ed)}});
	}
	void setLsb(int lsb) {
		mConn->send({"tdc", __func__, {lsb}});
	}
	void setCtrl(uint16_t ctrl) {
		mConn->send({"tdc", __func__, {ctrl}});
	}
	void setTdcMeta(bool flag) {
		mConn->send({"tdc", __func__, {flag}});
	}
	void settings() {
		mConn->send({"tdc", __func__});
	}
	void updateSettings() {
		mConn->send({"tdc", __func__});
	}
private:
	std::shared_ptr<CtudcConn> mConn;
};
