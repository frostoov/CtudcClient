#pragma once

#include <trek/net/response.hpp>

#include <QObject>

#include <string>
#include <memory>
#include <bitset>
#include <cstdint>
#include <unordered_map>

class CtudcConn;

class VoltageController : public QObject {
    Q_OBJECT
    using Handler = std::function<void(const trek::net::Response&)>;
public:
    class Stat {
    public:
        explicit Stat(uint8_t stat = 0) : mStat(stat) {}
        operator uint8_t() const { return mStat; }
        operator std::string() const {
            return std::bitset<8>(mStat).to_string();
        }
        bool ceb() const   { return (mStat >> 0) & 1; }
        bool gs() const    { return (mStat >> 1) & 1; }
        bool acceb() const { return (mStat >> 2) & 1; }
        bool iovld() const { return (mStat >> 3) & 1; }
        bool stdby() const { return (mStat >> 4) & 1; }
        bool rdact() const { return (mStat >> 6) & 1; }
        bool ruact() const { return (mStat >> 7) & 1; }
    private:
        uint8_t mStat;
    };
    VoltageController(const std::string& name, std::shared_ptr<CtudcConn> conn);

    void open();
    void close();
    bool isOpen();

    Stat stat(const std::string& device);
    void turnOn(const std::string& device);
    void turnOff(const std::string& device);
    void setVoltage(const std::string& device, int value);
    void setSpeedUp(const std::string& device, int value);
    void setSpeedDn(const std::string& device, int value);
    int speedUp(const std::string& device);
    int speedDn(const std::string& device);
    int voltage(const std::string& device);
    int amperage(const std::string& device);

    void handleResponse(const trek::net::Response& r);
signals:
    void stateChanged(bool isOpen);
    void statChanged(QString device, Stat stat);
    void voltageChanged(QString device, int voltage);
    void speedUpChanged(QString device, int speed);
    void speedDnChanged(QString device, int speed);
private:
    std::string mName;
    std::shared_ptr<CtudcConn> mConn;
    std::unordered_map<std::string, Handler> mHandlers;
};
