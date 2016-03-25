#pragma once

#include "net/ctudcconn.hpp"

class VoltageController {
public:
	VoltageController(std::shared_ptr<CtudcConn> conn) : mConn(conn) { }

	void open() {
		mConn->send({"vlt", "open"});
	}
	void close() {
		mConn->send({"vlt", "close"});
	}
	void isOpen() {
		mConn->send({"vlt", "isOpen"});
	}

	void stat(const std::string& device) {
		mConn->send({"vlt", "stat", {device}});
	}
	void turnOn(const std::string& device) {
		mConn->send({"vlt", "turnOn", {device}});
	}
	void turnOff(const std::string& device) {
		mConn->send({"vlt", "turnOff", {device}});
	}
    void setVoltage(const std::string& device, int value) {
		mConn->send({"vlt", "setVoltage", {device, value}});
	}
	void setSpeedUp(const std::string& device, int value) {
		mConn->send({"vlt", "setSpeedUp", {device, value}});
	}
	void setSpeedDn(const std::string& device, int value) {
		mConn->send({"vlt", "setSpeedDn", {device, value}});
	}
	void speedUp(const std::string& device) {
		mConn->send({"vlt", __func__, {device}});
	}
	void speedDn(const std::string& device) {
		mConn->send({"vlt", __func__, {device}});
	}
	void voltage(const std::string& device) {
		mConn->send({"vlt", "voltage", {device}});
	}
	void amperage(const std::string& device) {
		mConn->send({"vlt", "amperage", {device}});
	}
private:
    std::shared_ptr<CtudcConn> mConn;
};
