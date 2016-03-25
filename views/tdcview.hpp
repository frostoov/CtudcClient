#pragma once

#include "view.hpp"
#include "tdc/tdc.hpp"

class TdcView : public View {
    Q_OBJECT
public:
    TdcView(QObject* parent = nullptr);
private:
    Methods createMethods();
    Tdc::Settings convertSettings(const trek::net::Response& r);
signals:
    void open(QString status);
    void close(QString status);
    void isOpen(QString status, bool flag);
    void clear(QString status);
    void reset(QString status);
    void stat(QString status, uint16_t stat);
    void ctrl(QString status, uint16_t ctrl);
    void tdcMeta(QString status, bool flag);
    void setMode(QString status);
    void mode(QString status, Tdc::Mode flag);
    void setWindowWidth(QString status);
    void setWindowOffset(QString status);
    void setEdgeDetection(QString status);
    void setLsb(QString status);
    void setCtrl(QString status);
    void setTdcMeta(QString status);
    void settings(QString status, Tdc::Settings);
    void updateSettings(QString status, Tdc::Settings);
};
