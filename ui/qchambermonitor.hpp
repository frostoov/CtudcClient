#pragma once

#include "views/expoview.hpp"

#include <qcustomplot.h>

#include <functional>

class QChamberMonitor : public QCustomPlot {
public:
    QChamberMonitor(const QString& name, QWidget* parent = nullptr);
    void addFreq(double key, const ChamberFreq& freq);
    void addFreq(const ChamberFreq& freq);
    void removeDataBefore(double key);
    void rescaleAxes();
    void setTitle(const QString& title);
    void setTick(int tick);
    void clearData();
protected:
    void setupGUI(const QString& name);
    void forEach( std::function<void(size_t, QCPGraph&)>&& func);
private:
    int mTick;
    QCPPlotTitle* mTitle;
};
