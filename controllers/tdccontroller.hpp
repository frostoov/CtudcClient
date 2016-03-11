#pragma once

#include "net/ctudcconn.hpp"
#include "tdc/tdc.hpp"

#include <memory>

class TdcController {
public:
	TdcController(std::shared_ptr<CtudcConn> conn);

	void open();
	void close();
	bool isOpen();

	void clear();
	void reset();
	uint16_t stat();
	uint16_t ctrl();
	Tdc::Mode mode();
	bool tdcMeta();
	void setMode(Tdc::Mode mode);
	void setWindowWidth(unsigned width);
	void setWindowOffset(int offset);
	void setEdgeDetection(Tdc::EdgeDetection ed);
	void setLsb(int lsb);
	void setCtrl(uint16_t ctrl);
	void setTdcMeta(bool flag);
	Tdc::Settings settings();
	Tdc::Settings updateSettings();
private:
	std::shared_ptr<CtudcConn> mConn;
};
