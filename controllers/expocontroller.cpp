#include "expocontroller.hpp"

#include "net/ctudcconn.hpp"

using trek::net::Response;

using std::tuple;
using std::string;
using std::exception;

ExpoController::ExpoController(const std::string& name, std::shared_ptr<CtudcConn> conn)
    : mName(name), mConn(conn) {
    mHandlers = {
        {"type", [&](const Response& r) {emit typeChanged(QString::fromStdString(r.outputs.at(0).get<string>()));}},
        {"run",  [&](const Response& r) {emit newRun(r.outputs.at(0).get<unsigned>());}},
        {"monitoring", [&](const Response& r) {emit monitoring(convertFreq(r.outputs));}},
    };
}

string ExpoController::type() {
    return mConn->send({mName, __func__}).outputs.at(0).get<string>();
}
unsigned ExpoController::run() {
    return mConn->send({mName, __func__}).outputs.at(0).get<unsigned>();
}
void ExpoController::launchRead() {
    mConn->send({mName, __func__, {"nevod", "CaenV2718"}});
}
void ExpoController::stopRead() {
    mConn->send({mName, __func__});
}
void ExpoController::launchFreq(int delay) {
    mConn->send({mName, __func__, {"CaenV2718", delay}});
}
void ExpoController::stopFreq() {
    mConn->send({mName, __func__});
}
TrekFreq ExpoController::freq() {
    return convertFreq(mConn->send({mName, __func__}).outputs);
}
std::tuple<uintmax_t, uintmax_t> ExpoController::triggerCount() const {
    auto response = mConn->send({mName, __func__});
    auto count = response.outputs.at(0).get<uintmax_t>();
    auto drop  = response.outputs.at(1).get<uintmax_t>();
    return std::make_tuple(count, drop);
}
std::tuple<uintmax_t, uintmax_t> ExpoController::packageCount() const {
    auto response = mConn->send({mName, __func__});
    auto count = response.outputs.at(0).get<uintmax_t>();
    auto drop  = response.outputs.at(1).get<uintmax_t>();
    return std::make_tuple(count, drop);
}
std::tuple<TrekFreq, TrekFreq> ExpoController::chambersCount() const {
    auto response = mConn->send({mName, __func__});
    auto count = convertCount(response.outputs.at(0));
    auto drop = convertCount(response.outputs.at(1));
    return std::make_tuple(count, drop);
}
TrekFreq ExpoController::convertFreq(const trek::net::Response::JsonArray& data) {
    TrekFreq trekFreq;
    for(auto& item : data) {
        auto num = item.at("chamber").get<unsigned>();
        auto freq = item.at("freq");
        trekFreq.emplace(num, ChamberFreq{{freq.at(0), freq.at(1), freq.at(2), freq.at(3)}});
    }
    return trekFreq;
}


TrekFreq ExpoController::convertCount(const trek::net::Response::JsonArray& data) {
    TrekFreq trekFreq;
    for(auto& item : data) {
        auto num = item.at("chamber").get<unsigned>();
        auto freq = item.at("count");
        trekFreq.emplace(num, ChamberFreq{{freq.at(0), freq.at(1), freq.at(2), freq.at(3)}});
    }
    return trekFreq;
}

void ExpoController::handleResponse(const Response& r) {
    try {
        if(r.object != mName)
            throw std::logic_error("ExpoController::handleResponse invalid object");
        if(!r.status.empty())
            throw std::runtime_error(r.status);
        mHandlers.at(r.method)(r);
    } catch(exception& e) {
        std::cerr << __PRETTY_FUNCTION__ << "failed handle udp: " << e.what() << std::endl;
    }
}
