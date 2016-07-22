#pragma once

#include "tdc/tdc.hpp"

#include <trek/net/response.hpp>

#include <QObject>

#include <memory>
#include <unordered_map>

class CtudcConn;

class TdcController : public QObject {
    Q_OBJECT
    using Handler = std::function<void(const trek::net::Response&)>;
public:
    TdcController(const std::string& name, std::shared_ptr<CtudcConn> conn);
    const std::string& name() const { return mName; }
    void open();
    void close();
    bool isOpen();
    void clear();
    void reset();
    uint16_t stat();
    uint16_t ctrl();
    Tdc::Mode mode();
    bool tdcMeta();
    void setMode(Tdc::Mode mode);
    void setWindowWidth(unsigned width);
    void setWindowOffset(int offset);
    void setEdgeDetection(Tdc::EdgeDetection ed);
    void setLsb(int lsb);
    void setCtrl(uint16_t ctrl);
    void setTdcMeta(bool flag);
    Tdc::Settings settings();
    Tdc::Settings updateSettings();
    void handleResponse(const trek::net::Response& r);
protected:
    static Tdc::Settings convertSettings(const trek::net::Response& r);
signals:
    void stateChanged(bool isOpen);
    void settingsChanged(Tdc::Settings settings);
    void ctrlChanged(uint16_t ctrl);
    void statChanged(uint16_t stat);
    void modeChanged(Tdc::Mode mode);
    void tdcMetaChanged(bool flag);
private:
    std::string mName;
    std::shared_ptr<CtudcConn> mConn;
    std::unordered_map<std::string, Handler> mHandlers;
};
