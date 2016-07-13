#include "tdccontroller.hpp"
#include "net/ctudcconn.hpp"

#include <stdexcept>

using trek::net::Response;
using std::exception;

TdcController::TdcController(const std::string& name, std::shared_ptr<CtudcConn> conn)
    : mName(name), mConn(conn) {
    mHandlers = {
        {"isOpen", [&](const Response& r) {emit stateChanged(r.outputs.at(0).get<bool>()); }},
        {"ctrl", [&](const Response& r) {emit ctrlChanged(r.outputs.at(0).get<uint16_t>());}},
        {"stat", [&](const Response& r) {emit statChanged(r.outputs.at(0).get<uint16_t>());}},
        {"settings", [&](const Response& r) {emit settingsChanged(convertSettings(r));}},
        {"updateSettings", [&](const Response& r) {emit settingsChanged(convertSettings(r));}},
        {"tdcMeta", [&](const Response& r) {emit tdcMetaChanged(r.outputs.at(0).get<bool>());}},
        {"mode", [&](const Response& r) {emit modeChanged(Tdc::Mode(r.outputs.at(0).get<int>()));}},
    };
}

void TdcController::open() {
    mConn->send({mName, __func__});
}

void TdcController::close() {
    mConn->send({mName, __func__});
}

bool TdcController::isOpen() {
    return mConn->send({mName, __func__}).outputs.at(0).get<bool>();
}

void TdcController::clear() {
    mConn->send({mName, __func__});
}

void TdcController::reset() {
    mConn->send({mName, __func__});
}

uint16_t TdcController::stat() {
    return mConn->send({mName, __func__}).outputs.at(0).get<uint16_t>();
}

uint16_t TdcController::ctrl() {
    return mConn->send({mName, __func__}).outputs.at(0).get<uint16_t>();
}

Tdc::Mode TdcController::mode() {
    return Tdc::Mode(mConn->send({mName, __func__}).outputs.at(0).get<int>());
}

bool TdcController::tdcMeta() {
    return mConn->send({mName, __func__}).outputs.at(0).get<bool>();
}

void TdcController::setMode(Tdc::Mode mode) {
    mConn->send({mName, __func__, {int(mode)}});
}

void TdcController::setWindowWidth(unsigned width) {
    mConn->send({mName, __func__, {width}});
}
void TdcController::setWindowOffset(int offset) {
    mConn->send({mName, __func__, {offset}});
}

void TdcController::setEdgeDetection(Tdc::EdgeDetection ed) {
    mConn->send({mName, __func__, {int(ed)}});
}

void TdcController::setLsb(int lsb) {
    mConn->send({mName, __func__, {lsb}});
}

void TdcController::setCtrl(uint16_t ctrl) {
    mConn->send({mName, __func__, {ctrl}});
}
void TdcController::setTdcMeta(bool flag) {
    mConn->send({mName, __func__, {flag}});
}

Tdc::Settings TdcController::settings() {
    return convertSettings(mConn->send({mName, __func__}));
}

Tdc::Settings TdcController::updateSettings() {
    return convertSettings(mConn->send({mName, __func__}));
}

void TdcController::handleResponse(const Response& r) {
    try {
        if(r.object != mName)
            throw std::logic_error("TdcController::handleResponse invalid object");
        if(!r.status.empty())
            throw std::runtime_error(r.status);
        mHandlers.at(r.method)(r);
    } catch(exception& e) {
        std::cerr << __PRETTY_FUNCTION__ << "failed handle udp: " << e.what() << std::endl;
    }
}

Tdc::Settings TdcController::convertSettings(const Response& r) {
    if(!r.status.empty())
        throw std::logic_error("TdcController::converSettings failed response");
    return {
        r.outputs.at(0).get<unsigned>(),
        r.outputs.at(1).get<int>(),
        Tdc::EdgeDetection(r.outputs.at(2).get<int>()),
        r.outputs.at(3).get<unsigned>(),
    };
}
