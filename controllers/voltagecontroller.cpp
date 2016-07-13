#include "voltagecontroller.hpp"

#include "net/ctudcconn.hpp"

using trek::net::Response;

using std::string;
using std::exception;

VoltageController::VoltageController(const std::string& name, std::shared_ptr<CtudcConn> conn)
    : mName(name), mConn(conn) {
    mHandlers = {
        {"isOpen", [&](const Response& r) {emit stateChanged(r.outputs.at(0).get<bool>());}},
        {"stat", [&](const Response& r) {emit statChanged(QString::fromStdString(r.outputs.at(0)), Stat(r.outputs.at(0).get<uint16_t>()));}},
        {"voltage", [&](const Response& r) {emit voltageChanged(QString::fromStdString(r.outputs.at(0)), r.outputs.at(0));}},
        {"speedUp", [&](const Response& r) {emit speedUpChanged(QString::fromStdString(r.outputs.at(0)), r.outputs.at(0));}},
        {"speedDn", [&](const Response& r) {emit speedDnChanged(QString::fromStdString(r.outputs.at(0)), r.outputs.at(0));}}
    };
}

void VoltageController::open() {
    mConn->send({mName, __func__});
}

void VoltageController::close() {
    mConn->send({mName, __func__});
}

bool VoltageController::isOpen() {
    return mConn->send({mName, __func__}).outputs.at(0).get<bool>();
}

auto VoltageController::stat(const std::string& device) -> Stat {
    auto response = mConn->send({mName, __func__, {device}});
    auto type = response.outputs.at(0).get<string>();
    if(type != device)
        throw std::runtime_error("VoltageController invalid type in stat");
    auto stat = response.outputs.at(1).get<uint8_t>();
    return Stat(stat);
}

void VoltageController::turnOn(const std::string& device) {
    mConn->send({mName, __func__, {device}});
}

void VoltageController::turnOff(const std::string& device) {
    mConn->send({mName, __func__, {device}});
}

void VoltageController::setVoltage(const std::string& device, int value) {
    mConn->send({mName, __func__, {device, value}});
}

void VoltageController::setSpeedUp(const std::string& device, int value) {
    mConn->send({mName, __func__, {device, value}});
}

void VoltageController::setSpeedDn(const std::string& device, int value) {
    mConn->send({mName, __func__, {device, value}});
}

int VoltageController::speedUp(const std::string& device) {
    auto response = mConn->send({mName, __func__, {device}});
    auto type = response.outputs.at(0).get<string>();
    if(type != device)
        throw std::runtime_error("VoltageController invalid type in speedUp");
    auto speed = response.outputs.at(1).get<int>();
    return speed;
}

int VoltageController::speedDn(const std::string& device) {
    auto response = mConn->send({mName, __func__, {device}});
    auto type = response.outputs.at(0).get<string>();
    if(type != device)
        throw std::runtime_error("VoltageController invalid type in speedDn");
    auto speed = response.outputs.at(1).get<int>();
    return speed;

}

int VoltageController::voltage(const std::string& device) {
    auto response = mConn->send({mName, __func__, {device}});
    auto type = response.outputs.at(0).get<string>();
    if(type != device)
        throw std::runtime_error("VoltageController invalid type in voltage");
    auto voltage = response.outputs.at(1).get<int>();
    return voltage;

}

int VoltageController::amperage(const std::string& device) {
    auto response = mConn->send({mName, __func__, {device}});
    auto type = response.outputs.at(0).get<string>();
    if(type != device)
        throw std::runtime_error("VoltageController invalid type in amperage");
    auto amperage = response.outputs.at(1).get<int>();
    return amperage;
}


void VoltageController::handleResponse(const Response& r) {
    try {
        if(r.object != mName)
            throw std::logic_error("VoltageController::handleResponse invalid object");
        if(!r.status.empty())
            throw std::runtime_error(r.status);
        mHandlers.at(r.method)(r);
    } catch(exception& e) {
        std::cerr << __PRETTY_FUNCTION__ << "failed handle udp: " << e.what() << std::endl;
    }
}
