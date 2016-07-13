#pragma once

#include "controllers/expocontroller.hpp"
#include "qchambermonitor.hpp"
#include "qchambertable.hpp"

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>

#include <cstdint>
#include <vector>
#include <unordered_set>

class QLoopFreqWidget : public QWidget {
public:
    QLoopFreqWidget(QWidget* parent = 0);
    void addFreq(int volt, const TrekFreq& freq);
    void clearData();
    void updateData();
protected:
    void printChamberFreq(QTextStream& stream, const ChamFreqSeries& chamFreq);
    void fillPlot(int chamNum, const ChamFreqSeries& series);
    void fillList(const TrekFreqSeries& series);
    QChamberMonitor* createPlot();
    void packWidgets();
    void createWidgets();
    void setupGUI();
private:
    QTabWidget*	tab;
    QChamberTable* mTable;
    QChamberMonitor* mPlot;
    QPushButton* saveTableB;
    QPushButton* savePlotB;
    QLabel* codeOneL;
    QLabel* chamberL;
    QListWidget* list;
    QString listItemSelTitle;

    QHBoxLayout* mainLayout;
    QVBoxLayout* subLayout;

    TrekFreqSeries mFreqs;
protected slots:
    void setChamberData();
    void saveTable();
    void savePlot();
};
