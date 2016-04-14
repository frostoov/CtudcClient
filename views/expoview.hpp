#pragma once

#include "view.hpp"

#include <chrono>

using ChamberFreq  = std::array<double, 4>;
using TrekFreq     = std::unordered_map<unsigned, ChamberFreq>;

class ExpoView : public View {
    Q_OBJECT
public:
    ExpoView(QObject* parent = nullptr);
protected:
    Methods createMethods();
signals:
    void type(QString status, QString type);
    void run(QString status, unsigned nRun);
    void launchRead(QString status);
    void stopRead(QString status);
    void launchFreq(QString status);
    void stopFreq(QString status);
    void triggerCount(QString status, uintmax_t count, uintmax_t drop);
    void packageCount(QString status, uintmax_t count, uintmax_t drop);
    void chambersCount(QString status, TrekFreq count, TrekFreq drop);
    void freq(QString status, TrekFreq freq);
};
