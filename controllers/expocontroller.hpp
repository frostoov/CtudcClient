#pragma once

#include "net/ctudcconn.hpp"

#include <array>
#include <unordered_map>
#include <chrono>

class ExpoController {
public:
    using ChamberFreq  = std::array<float, 4>;
    using TrekFreq     = std::unordered_map<unsigned, ChamberFreq>;
public:
	ExpoController(std::shared_ptr<CtudcConn> mConn);

	std::string type();
	uintmax_t run();
	void startRead();
	void stopRead();
	void startFreq(int delay);
	void stopFreq();
	TrekFreq freq();
	uintmax_t triggerCount() const;
	uintmax_t packageCount() const;
	std::chrono::milliseconds duration() const;
protected:
	TrekFreq convertFreq(const trek::net::Request::JsonArray& data);
private:
	std::shared_ptr<CtudcConn> mConn;
};
