#include "expocontroller.hpp"

using std::string;
using std::chrono::milliseconds;
using std::shared_ptr;

ExpoController::ExpoController(shared_ptr<CtudcConn> conn)
    : mConn(conn) { }

string ExpoController::type() {
	auto response = mConn->send({"expo", __func__});
    return response.outputs.at(0).get<string>();
}

uintmax_t ExpoController::run() {
	auto response = mConn->send({"expo", __func__});
    return response.outputs.at(0).get<uintmax_t>();
}

void ExpoController::startRead() {
	mConn->send({"expo", __func__});
}

void ExpoController::stopRead() {
	mConn->send({"expo", __func__});
}

void ExpoController::startFreq(int delay) {
	mConn->send({"expo", __func__, {delay}});
}

void ExpoController::stopFreq() {
	mConn->send({"expo", __func__});
}

ExpoController::TrekFreq ExpoController::freq() {
	auto response = mConn->send({"expo", __func__});
	return convertFreq(response.outputs);
}

uintmax_t ExpoController::triggerCount() const {
	auto response = mConn->send({"expo", __func__});
    return response.outputs.at(0).get<uintmax_t>();
}

uintmax_t ExpoController::packageCount() const {
	auto response = mConn->send({"expo", __func__});
    return response.outputs.at(0).get<uintmax_t>();
}

milliseconds ExpoController::duration() const {
	auto response = mConn->send({"expo", __func__});
	return milliseconds(response.outputs.at(0).get<intmax_t>());
}

ExpoController::TrekFreq ExpoController::convertFreq(const trek::net::Request::JsonArray& data) {
	TrekFreq trekFreq;
	for(auto& item : data) {
		auto num = item.at("chamber").get<unsigned>();
		auto freq = item.at("freq");
		trekFreq.emplace(num, ChamberFreq{{freq.at(0), freq.at(1), freq.at(2), freq.at(3)}});
	}
	return trekFreq;
}
