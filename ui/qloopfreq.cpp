#include "qloopfreq.hpp"

#include <QDir>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <set>

using std::vector;
using std::set;
using std::make_unique;
using std::setw;
using std::setfill;

QLoopFreqWidget::QLoopFreqWidget(QWidget* parent)
    : QWidget(parent) {
    createWidgets();
    packWidgets();

    connect(saveTableB, &QPushButton::clicked,
            this, &QLoopFreqWidget::saveTable);

    connect(savePlotB, &QPushButton::clicked,
            this, &QLoopFreqWidget::savePlot);

    connect(list, &QListWidget::itemSelectionChanged,
            this, &QLoopFreqWidget::setChamberData);
}

void QLoopFreqWidget::createWidgets() {
    tab	= new QTabWidget;
    mTable = new QChamberTable(20);
    mPlot = createPlot();
    list = new QListWidget;
    saveTableB  = new QPushButton("Save Table");
    savePlotB = new QPushButton("Save Plot");
    codeOneL = new QLabel("Code 0: 0");
    chamberL = new QLabel("Chambers");
    tab->addTab(mTable, "Table");
    tab->addTab(mPlot, "Plot");
}

void QLoopFreqWidget::packWidgets() {
    mainLayout = new QHBoxLayout;
    subLayout  = new QVBoxLayout;
    mainLayout->addLayout(subLayout, 1);
    mainLayout->addWidget(tab, 5);
    subLayout->addWidget(saveTableB);
    subLayout->addWidget(savePlotB);
    subLayout->addWidget(chamberL);
    subLayout->addWidget(list);
    subLayout->addWidget(codeOneL);
    setLayout(mainLayout);
}

void QLoopFreqWidget::addFreq(int volt, const TrekFreq& freq) {
    for(auto& chamFreqPair : freq) {
        auto  chamNum = chamFreqPair.first;
        auto& chamFreq = chamFreqPair.second;
        if(mFreqs.count(chamNum) == 0)
            mFreqs.emplace(chamNum, ChamFreqSeries());
        mFreqs.at(chamNum).emplace_back(volt, chamFreq);
    }
}

void QLoopFreqWidget::updateData() {
    fillList(mFreqs);
    setChamberData();
}

void QLoopFreqWidget::clearData() {
    mFreqs.clear();
    list->clear();
    mTable->clearData();
    mPlot->clearData();
}

void QLoopFreqWidget::fillList(const TrekFreqSeries& series) {
    list->clear();
    set<int> chams;
    std::transform(series.begin(), series.end(), std::inserter(chams, chams.begin()), [](auto & pair) {
        return pair.first;
    });
    for(auto& cham : chams)
        list->addItem(QString::number(cham + 1 ));
    for(int i = 0 ; i < list->count(); ++i)
        if(list->item(i)->text() == listItemSelTitle)
            list->setCurrentRow(i);
}

void QLoopFreqWidget::saveTable() {
    auto dirname = QFileDialog::getExistingDirectory(this, "Save table");
    if(dirname.isEmpty())
        return;

    for(auto& chamPair : mFreqs) {
        QFile file(dirname + tr("/chamber_%1.txt").arg(chamPair.first + 1));
        file.open(QFile::WriteOnly | QFile::Text);
        QTextStream stream(&file);
        printChamberFreq(stream, chamPair.second);
    }
}

void QLoopFreqWidget::printChamberFreq(QTextStream& stream, const ChamFreqSeries& chamFreq) {
    for(auto& codePair : chamFreq) {
        stream << codePair.first;
        for(auto& freq : codePair.second) {
            stream << '\t' << freq;
        }
        stream << '\n';
    }
}

void QLoopFreqWidget::fillPlot(int chamNum, const ChamFreqSeries& series) {
    mPlot->clearData();
    mPlot->setTitle(tr("Chamber %1").arg(chamNum + 1));
    for(auto& voltFreq : series)
        mPlot->addFreq(voltFreq.first, voltFreq.second);
    mPlot->rescaleAxis();
    mPlot->replot();
}


void QLoopFreqWidget::savePlot() {
    auto dirname = QFileDialog::getExistingDirectory(this, "Save plots");
    if(dirname.isEmpty())
        return;
    for(auto& chamPair : mFreqs) {
        fillPlot(chamPair.first, chamPair.second);
        mPlot->savePng(dirname + tr("/chamber_%1.png").arg(chamPair.first + 1), 1000, 1000, 1, 100);
    }
    setChamberData();
}

QChamberMonitor* QLoopFreqWidget::createPlot() {
    auto plot = new QChamberMonitor("Chamber");
    plot->yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
    plot->yAxis->setTickStep(1);

    plot->xAxis->setLabel("Voltage,codes");
    plot->yAxis->setLabel("Frequency,Hz");

    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight | Qt::AlignBottom);
    plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);
    return plot;
}

void QLoopFreqWidget::setChamberData() {
    if(list->currentItem() == nullptr)
        return;
    try  {
        listItemSelTitle = list->currentItem()->text();
        auto chamNum = listItemSelTitle.toUInt() - 1;
        auto chamFreq = mFreqs.at(chamNum);
        mTable->setChamFreqSeries(chamFreq);
        mTable->update();
        fillPlot(chamNum, chamFreq);
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
