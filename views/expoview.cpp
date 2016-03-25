#include "expoview.hpp"

#include <trek/net/request.hpp>

using trek::net::Response;
using std::string;
using std::chrono::milliseconds;

ExpoView::ExpoView(QObject* parent)
    : View("expo", createMethods(), parent) {
    qRegisterMetaType<ExpoView::TrekFreq>("ExpoView::TrekFreq");
    qRegisterMetaType<std::chrono::milliseconds>("std::chrono::milliseconds");
}

ExpoView::Methods ExpoView::createMethods() {
    return {
        {"type", [this](const Response& r) {
            auto type = r.status.empty() ? QString::fromStdString(r.outputs.at(0).get<string>()) : QString();
            emit this->type(QString::fromStdString(r.status), type);
        }},
        {"run", [this](const Response& r) {
            auto run = r.status.empty() ? r.outputs.at(0).get<unsigned int>() : 0;
            emit this->run(QString::fromStdString(r.status), run);
        }},
        {"startRead", [this](const Response& r) {
            emit this->startRead(QString::fromStdString(r.status));
        }},
        {"stopRead", [this](const Response& r) {
            emit this->stopRead(QString::fromStdString(r.status));
        }},
        {"startFreq", [this](const Response& r) {
            emit this->startFreq(QString::fromStdString(r.status));
        }},
        {"stopFreq", [this](const Response& r) {
            emit this->stopFreq(QString::fromStdString(r.status));
        }},
        {"triggerCount", [this](const Response& r) {
            auto count = r.status.empty() ? r.outputs.at(0).get<unsigned int>() : 0;
            emit this->triggerCount(QString::fromStdString(r.status), count);
        }},
        {"packageCount", [this](const Response& r) {
            auto count = r.status.empty() ? r.outputs.at(0).get<unsigned int>() : 0;
            emit this->packageCount(QString::fromStdString(r.status), count);
        }},
        {"duration", [this](const Response& r) {
            auto millis = r.status.empty() ? r.outputs.at(0).get<int>() : 0;
            emit this->duration(QString::fromStdString(r.status), milliseconds(millis));
        }},
        {"freq", [this](const Response& r) {
            auto freq = r.status.empty() ? convertFreq(r.outputs) : TrekFreq();
            emit this->freq(QString::fromStdString(r.status), freq);
        }},
    };
}

ExpoView::TrekFreq ExpoView::convertFreq(const Response::JsonArray& data) {
    TrekFreq trekFreq;
    for(auto& item : data) {
        auto num = item.at("chamber").get<unsigned>();
        auto freq = item.at("freq");
        trekFreq.emplace(num, ChamberFreq{{freq.at(0), freq.at(1), freq.at(2), freq.at(3)}});
    }
    return trekFreq;
}
