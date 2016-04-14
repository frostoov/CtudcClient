#include "qchamber.hpp"


QChamberMonitor::QChamberMonitor(const QString& name, QWidget* parent)
	: mName(name) {
	setupGUI();

	connect(mPlot, &QCustomPlot::mouseDoubleClick, this, [this](QMouseEvent *event){
		emit this->mouseDoubleClick();
	});
	connect(mPlot, &QCustomPlot::mousePress, this, [this](QMouseEvent* event) {
		if(event->button() == Qt::RightButton) {
			mPlot->legend->setVisible(!mPlot->legend->visible());
			mPlot->replot();
		}
	});
}

void QChamberMonitor::addFreq(const ExpoView::ChamberFreq& freq) {
	for(size_t i = 0; i < freq.size(); ++i) {
		updateGraph(*mPlot->graph(i), freq.at(i));
	}
	mPlot->xAxis->rescale(true);
	mPlot->yAxis->rescale(true);
	mPlot->replot();
}

void QChamberMonitor::setCount(const ExpoView::ChamberFreq& count) {
	// for(int row = 0; row < count.size(); ++row) {
	//    mWireCount[row]->setText(tr("%1 | %2").arg(count[row]).arg(drop[row]));
	//}
}
void QChamberMonitor::reset() {
	//TODO?
}

void QChamberMonitor::setTick(int tick) {
	mTick = tick;
	mPlot->xAxis->setTickStep(4 * tick);
}

void QChamberMonitor::setupGUI() {
	mPlot = setupPlot();

	//auto infoLayout = new QFormLayout;
	//for(int i = 0; i < 4; ++i) {
	//mWireCount[i] = new QLineEdit("0 | 0");
	// infoLayout->addRow(tr("Wire %1").arg(i+1), mWireCount[i]);
	//}
	//auto infoGroup = new QGroupBox("Info");
	//infoGroup->setLayout(infoLayout);

	setOrientation(Qt::Vertical);
	addWidget(mPlot);
	//addWidget(infoGroup);
	//setStretchFactor(5, 1);
}

QCustomPlot* QChamberMonitor::setupPlot() {
	auto plot = new QCustomPlot;
	plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	plot->xAxis->setDateTimeFormat("hh:mm:ss");
	plot->xAxis->setAutoTickStep(false);

	plot->plotLayout()->insertRow(0);
	auto plotTitle = new QCPPlotTitle(plot, mName);
	plot->plotLayout()->addElement(0, 0, plotTitle);
	for(int i = 0 ; i < 4 ; ++i) {
		QColor color {
			255 * ( ((i + 1) >> 0) & 1 ),
			255 * ( ((i + 1) >> 1) & 1 ),
			255 * ( ((i + 1) >> 2) & 1 ),
		};
		QPen pen(color);
		pen.setWidth(2);
		pen.setStyle(Qt::DotLine);
		plot->addGraph();
		plot->graph(i)->setName(tr("Wire №%1").arg(i + 1));
		plot->graph(i)->setPen(pen);
		plot->graph(i)->setLineStyle(QCPGraph::lsLine);
		plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
	}
	return plot;
}


void QChamberMonitor::updateGraph( QCPGraph &graph, double val) {
	auto key = double(QDateTime::currentMSecsSinceEpoch()) / 1000;
	graph.addData(key, val);
	graph.removeDataBefore(key - 50*mTick);
}

void QChamberMonitor::mouseDoubleClickEvent(QMouseEvent*) {
	emit mouseDoubleClick();
}
