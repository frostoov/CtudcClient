#include "voltageview.hpp"

using trek::net::Response;
using std::string;

VoltageView::VoltageView(QObject* parent)
    : View("vlt", createMethods(), parent) {
    qRegisterMetaType<Stat>("Stat");
}

VoltageView::Methods VoltageView::createMethods() {
    return {
        {"open", [this](const Response& r) {
            emit this->open(QString::fromStdString(r.status));
        }},
		{"close", [this](const Response& r) {
            emit this->close(QString::fromStdString(r.status));
        }},
		{"isOpen", [this](const Response& r) {
            auto flag = r.status.empty() ? r.outputs.at(0).get<bool>() : false;
            emit this->isOpen(QString::fromStdString(r.status), flag);
        }},
        {"turnOn", [this](const Response& r) {
            emit this->turnOn(QString::fromStdString(r.status));
        }},
		{"turnOff", [this](const Response& r) {
            emit this->turnOff(QString::fromStdString(r.status));
        }},
		{"stat", [this](const Response& r) {
            auto type = r.status.empty() ? r.outputs.at(0).get<string>() : string();
            auto stat = r.status.empty() ? r.outputs.at(1).get<uint8_t>() : 0;
            emit this->stat(QString::fromStdString(r.status), QString::fromStdString(type), Stat(stat));
        }},
        {"setVoltage", [this](const Response& r) {
            emit this->setVoltage(QString::fromStdString(r.status));
        }},
		{"setSpeedUp", [this](const Response& r) {
            emit this->setSpeedUp(QString::fromStdString(r.status));
        }},
		{"setSpeedDn", [this](const Response& r) {
            emit this->setSpeedDn(QString::fromStdString(r.status));
        }},
		{"speedUp", [this](const Response& r) {
            auto type = r.status.empty() ? r.outputs.at(0).get<string>() : string();
            auto speed = r.status.empty() ? r.outputs.at(1).get<int>() : 0;
            emit this->speedUp(QString::fromStdString(r.status), QString::fromStdString(type), speed);
        }},
		{"speedDn", [this](const Response& r) {
            auto type = r.status.empty() ? r.outputs.at(0).get<string>() : string();
            auto speed = r.status.empty() ? r.outputs.at(1).get<int>() : 0;
            emit this->speedDn(QString::fromStdString(r.status), QString::fromStdString(type), speed);
        }},
		{"voltage", [this](const Response& r) {
            auto type = r.status.empty() ? r.outputs.at(0).get<string>() : string();
            auto voltage = r.status.empty() ? r.outputs.at(1).get<int>() : 0;
            emit this->voltage(QString::fromStdString(r.status), QString::fromStdString(type), voltage);
        }},
		{"amperage", [this](const Response& r) {
            auto type = r.status.empty() ? r.outputs.at(0).get<string>() : string();
            auto amperage = r.status.empty() ? r.outputs.at(1).get<int>() : 0;
            emit this->amperage(QString::fromStdString(r.status), QString::fromStdString(type), amperage);
        }},
    };
}
