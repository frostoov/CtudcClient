#pragma once

#include "controllers/expocontroller.hpp"

#include <qcustomplot.h>

#include <functional>

class QChamberMonitor : public QCustomPlot {
public:
    QChamberMonitor(const QString& name, QWidget* parent = nullptr);
    void addFreq(double key, const ChamberFreq& freq);
    void addFreq(const ChamberFreq& freq);
    void removeDataBefore(double key);
    void rescale();
    void setTitle(const QString& title);
    void setTick(int tick);
    void clearData();
    void rescaleAxis();
protected:
    void setupGUI(const QString& name);
    void forEach( std::function<void(size_t, QCPGraph&)>&& func);
private:
    QCPPlotTitle* mTitle;
};
