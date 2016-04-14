#pragma once

#include "views/expoview.hpp"

#include <qcustomplot.h>

#include <QSplitter>
#include <QLineEdit>
#include <QTableWidget>

class QChamberMonitor : public QSplitter {
    Q_OBJECT
public:
    void addFreq(const ChamberFreq& freq);
    void setTick(int tick) { mTick = tick; }
protected:
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void setupGUI();
    void updateGraph(QCPGraph& graph, double val);
    QCustomPlot* setupPlot();
private:
    QCustomPlot* mPlot;
    QString mName;
    int mTick;

    QLineEdit* mWireCount[4];
signals:
    void mouseDoubleClick();
};
