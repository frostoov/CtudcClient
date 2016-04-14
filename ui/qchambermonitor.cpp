#include "qchambermonitor.hpp"

constexpr size_t graphs = 4;

QChamberMonitor::QChamberMonitor(const QString& name, QWidget* parent)
    : QCustomPlot(parent) {

    setupGUI(name);

    connect(this, &QCustomPlot::mousePress, [this](QMouseEvent * event) {
        if(event->button() == Qt::RightButton) {
            legend->setVisible(!legend->visible());
            replot();
        }
    });
}

void QChamberMonitor::addFreq(const ChamberFreq& freq ) {
    auto key = double(QDateTime::currentMSecsSinceEpoch()) / 1000;
    addFreq(key, freq);
}

void QChamberMonitor::addFreq(double key, const ChamberFreq& freq) {
    forEach([&](auto i, auto & graph) {
        graph.addData(key, freq.at(i));
    });
}

void QChamberMonitor::removeDataBefore(double key) {
    forEach([&](auto, auto & graph) {
        graph.removeDataBefore(key);
    });
}


void QChamberMonitor::rescaleAxis() {
    xAxis->rescale();
    yAxis->rescale();
}

void QChamberMonitor::clearData() {
    forEach([this](auto, auto & graph) {
        graph.clearData();
    });
}

void QChamberMonitor::setTitle(const QString& title) {
    mTitle->setText(title);
}

void QChamberMonitor::setupGUI(const QString& name) {
    plotLayout()->insertRow(0);
    mTitle = new QCPPlotTitle(this, name);
    plotLayout()->addElement(0, 0, mTitle);
    for(size_t i = 0 ; i < graphs; ++i) {
        QColor color {
            255 * ( (int(i + 1) >> 0) & 1 ),
            255 * ( (int(i + 1) >> 1) & 1 ),
            255 * ( (int(i + 1) >> 2) & 1 ),
        };
        QPen pen(color);
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
        addGraph();
        graph(i)->setName(tr("Wire №%1").arg(i + 1));
        graph(i)->setPen(pen);
        graph(i)->setLineStyle(QCPGraph::lsLine);
        graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
    }
}

void QChamberMonitor::setTick(int tick) {
    xAxis->setTickStep(tick);
}

void QChamberMonitor::forEach(std::function<void(size_t, QCPGraph&)>&& func) {
    for(size_t i = 0; i < graphs; ++i)
        func(i, *graph(i));
}
