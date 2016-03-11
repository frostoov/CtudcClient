#pragma once

#include "net/ctudcconn.hpp"

class VoltageController {
public:
	class Stat {
    public:
        explicit Stat(uint8_t stat) : mStat(stat) {}
        bool ceb() const   { return (mStat >> 0) & 1; }
        bool gs() const    { return (mStat >> 1) & 1; }
        bool acceb() const { return (mStat >> 2) & 1; }
        bool iovld() const { return (mStat >> 3) & 1; }
        bool stdby() const { return (mStat >> 4) & 1; }
        bool rdact() const { return (mStat >> 6) & 1; }
        bool ruact() const { return (mStat >> 7) & 1; }
        uint8_t value() const { return mStat; }
    private:
        uint8_t mStat;
    };
public:
	VoltageController(std::shared_ptr<CtudcConn> conn);

	void open();
	void close();
	bool isOpen();

	Stat stat(const std::string& device);
	void turnOn(const std::string& device);
	void turnOff(const std::string& device);
    void setVoltage(const std::string& device, int value);
	void setSpeedUp(const std::string& device, int value);
	void setSpeedDn(const std::string& device, int value);
	int speedUp(const std::string& device);
	int speedDn(const std::string& device);
	int voltage(const std::string& device);
	int amperage(const std::string& device);
private:
    std::shared_ptr<CtudcConn> mConn;
};
