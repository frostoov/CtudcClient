#pragma once

#include "view.hpp"

#include <chrono>

class ExpoView : public View {
    Q_OBJECT
public:
    using ChamberFreq  = std::array<float, 4>;
    using TrekFreq     = std::unordered_map<unsigned, ChamberFreq>;
public:
    ExpoView(QObject* parent = nullptr);
protected:
    Methods createMethods();
    static TrekFreq convertFreq(const trek::net::Response::JsonArray& data);
signals:
    void type(QString status, QString type);
    void run(QString status, unsigned int nRun);
    void startRead(QString status);
    void stopRead(QString status);
    void startFreq(QString status);
    void stopFreq(QString status);
    void triggerCount(QString status, unsigned int count);
    void packageCount(QString status, unsigned int count);
    void duration(QString status, std::chrono::milliseconds dur);
    void freq(QString status, ExpoView::TrekFreq freq);
};
