#pragma once

#include <trek/net/response.hpp>

#include <QObject>

#include <tuple>
#include <memory>
#include <array>
#include <unordered_map>

using ChamberFreq  = std::array<double, 4>;
using TrekFreq     = std::unordered_map<unsigned, ChamberFreq>;

class CtudcConn;

class ExpoController : public QObject {
    Q_OBJECT
    using Handler = std::function<void(const trek::net::Response&)>;
public:
    ExpoController(const std::string& name, std::shared_ptr<CtudcConn> conn);
    const std::string& name() const { return mName; }
    std::string type();
    unsigned run();
    void launchRead();
    void stopRead();
    void launchFreq(int delay);
    void stopFreq();
    TrekFreq freq();
    std::tuple<uintmax_t, uintmax_t> triggerCount() const;
    std::tuple<uintmax_t, uintmax_t> packageCount() const;
    std::tuple<TrekFreq, TrekFreq> chambersCount() const;
    void handleResponse(const trek::net::Response& r);
protected:
    static TrekFreq convertFreq(const trek::net::Response::JsonArray& data);
    static TrekFreq convertCount(const trek::net::Response::JsonArray& data);
signals:
    void typeChanged(QString type);
    void newRun(unsigned run);
    void monitoring(TrekFreq freq);
private:
    std::string mName;
    std::shared_ptr<CtudcConn> mConn;
    std::unordered_map<std::string, Handler> mHandlers;
};
