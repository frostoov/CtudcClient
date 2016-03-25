#include "tdcview.hpp"

using trek::net::Response;

TdcView::TdcView(QObject* parent)
    : View("tdc", createMethods(), parent) {
    qRegisterMetaType<Tdc::Settings>("Tdc::Settings");
    qRegisterMetaType<Tdc::Mode>("Tdc::Mode");
    qRegisterMetaType<uint16_t>("uint16_t");
}

TdcView::Methods TdcView::createMethods() {
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
		{"clear", [&](const Response& r) {
            emit this->clear(QString::fromStdString(r.status));
        }},
		{"reset", [&](const Response& r) {
            emit this->reset(QString::fromStdString(r.status));
        }},
		{"stat", [&](const Response& r) {
            auto stat = r.status.empty() ? r.outputs.at(0).get<uint16_t>() : 0;
            emit this->stat(QString::fromStdString(r.status), stat);
        }},
		{"ctrl", [&](const Response& r) {
            auto ctrl = r.status.empty() ? r.outputs.at(0).get<uint16_t>() : 0;
            emit this->ctrl(QString::fromStdString(r.status), ctrl);
        }},
		{"tdcMeta", [&](const Response& r) {
            auto flag = r.status.empty() ? r.outputs.at(0).get<bool>() : false;
            emit this->tdcMeta(QString::fromStdString(r.status), flag);
        }},
		{"setMode", [&](const Response& r) {
            emit this->setMode(QString::fromStdString(r.status));
        }},
		{"mode", [&](const Response& r) {
            auto mode = r.status.empty() ? r.outputs.at(0).get<int>() : 0;
            emit this->mode(QString::fromStdString(r.status), Tdc::Mode(mode));
        }},
		{"setWindowWidth", [&](const Response& r) {
            emit this->setWindowWidth(QString::fromStdString(r.status));
        }},
		{"setWindowOffset", [&](const Response& r) {
            emit this->setWindowOffset(QString::fromStdString(r.status));
        }},
		{"setEdgeDetection", [&](const Response& r) {
            emit this->setEdgeDetection(QString::fromStdString(r.status));
        }},
		{"setLsb", [&](const Response& r) {
            emit this->setLsb(QString::fromStdString(r.status));
        }},
		{"setCtrl", [&](const Response& r) {
            emit this->setCtrl(QString::fromStdString(r.status));
        }},
		{"setTdcMeta", [&](const Response& r) {
            emit this->setTdcMeta(QString::fromStdString(r.status));
        }},
		{"settings", [&](const Response& r) {
            auto settings = convertSettings(r);
            emit this->settings(QString::fromStdString(r.status), settings);
        }},
		{"updateSettings", [&](const Response& r) {
            auto settings = convertSettings(r);
            emit this->updateSettings(QString::fromStdString(r.status), settings);
        }},
	};
}


Tdc::Settings TdcView::convertSettings(const trek::net::Response& r) {
    if(r.status.empty())
        return {
            r.outputs.at(0).get<unsigned>(),
            r.outputs.at(1).get<int>(),
            Tdc::EdgeDetection(r.outputs.at(2).get<int>()),
            r.outputs.at(3).get<unsigned>(),
        };
    return {0, 0, Tdc::EdgeDetection(0), 0};
}
