#include "voltagecontroller.hpp"

using std::string;
using std::shared_ptr;

VoltageController::VoltageController(shared_ptr<CtudcConn> conn)
    : mConn(conn) { }

void VoltageController::open() {
    mConn->send({"vlt", "open"});
}

void VoltageController::close() {
    mConn->send({"vlt", "close"});
}

bool VoltageController::isOpen() {
    auto response = mConn->send({"vlt", "isOpen"});
    return response.outputs.at(0).get<bool>();
}


VoltageController::Stat VoltageController::stat(const string& device) {
	auto response = mConn->send({"vlt", "stat", {device}});
    return Stat(response.outputs.at(0).get<uint8_t>());
}
void VoltageController::turnOn(const string& device) {
    mConn->send({"vlt", "turnOn", {device}});
}

void VoltageController::turnOff(const string& device) {
    mConn->send({"vlt", "turnOff", {device}});
}

void VoltageController::setVoltage(const string& device, int value) {
    mConn->send({"vlt", "setVoltage", {device, value}});
}

void VoltageController::setSpeedUp(const string& device, int value) {
    mConn->send({"vlt", "setSpeedUp", {device, value}});
}

void VoltageController::setSpeedDn(const string& device, int value) {
	mConn->send({"vlt", "setSpeedDn", {device, value}});
}

int VoltageController::speedUp(const string& device) {
	auto response = mConn->send({"vlt", __func__, {device}});
	return response.outputs.at(0).get<int>();
}

int VoltageController::speedDn(const string& device) {
	auto response = mConn->send({"vlt", __func__, {device}});
	return response.outputs.at(0).get<int>();
}

int VoltageController::voltage(const string& device) {
    auto response = mConn->send({"vlt", "voltage", {device}});
    return response.outputs.at(0).get<int>();
}

int VoltageController::amperage(const string& device) {
    auto response = mConn->send({"vlt", "amperage", {device}});
    return response.outputs.at(0).get<int>();
}
