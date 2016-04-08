#pragma once

#include "view.hpp"

class VoltageView : public View {
    Q_OBJECT
public:
    class Stat {
    public:
        explicit Stat(uint8_t stat = 0) : mStat(stat) {}
        bool ceb() const   { return (mStat >> 0) & 1; }
        bool gs() const    { return (mStat >> 1) & 1; }
        bool acceb() const { return (mStat >> 2) & 1; }
        bool iovld() const { return (mStat >> 3) & 1; }
        bool stdby() const { return (mStat >> 4) & 1; }
        bool rdact() const { return (mStat >> 6) & 1; }
        bool ruact() const { return (mStat >> 7) & 1; }
        uint8_t value() const { return mStat; }
    private:
        uint8_t mStat;
    };
public:
    VoltageView(QObject* parent);
private:
    Methods createMethods();
signals:
    void open(QString status);
    void close(QString status);
    void isOpen(QString status, bool flag);
    void stat(QString status, QString type, Stat stat);
    void turnOn(QString status);
    void turnOff(QString status);
    void setVoltage(QString status);
    void setSpeedUp(QString status);
    void setSpeedDn(QString status);
    void speedUp(QString status, QString type, int speed);
    void speedDn(QString status, QString type, int speed);
    void voltage(QString status, QString type, int voltage);
    void amperage(QString status, QString type, int amperage);
};
