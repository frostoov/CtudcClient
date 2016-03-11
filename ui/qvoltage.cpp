#include "qvoltage.hpp"
#include "utils.hpp"

#include <QGroupBox>
#include <QFormLayout>
#include <QSplitter>

using std::shared_ptr;
using std::exception;

QVoltageWidget::QVoltageWidget(shared_ptr<VoltageController> controller, QWidget *parent)
	: QSplitter(parent),
	  mController(controller),
	  mFreq(10) {
	setupGUI();
	setupPlot(*mDriftPlot, "drift");
	setupPlot(*mSignalPlot, "signal");

	timer = new QTimer(this);
	timer->setInterval(mFreq*1000);
	timer->setSingleShot(false);

	connect(mDriftVolt, &QLineEdit::returnPressed,
			this, &QVoltageWidget::setDriftVoltage);
	connect(mSignalVolt, &QLineEdit::returnPressed,
			this, &QVoltageWidget::setSignalVoltage);

	connect(mDriftSpeedUp, &QLineEdit::returnPressed,
			this, &QVoltageWidget::setDriftSpeedUp);
	connect(mDriftSpeedDn, &QLineEdit::returnPressed,
			this, &QVoltageWidget::setDriftSpeedDn);
	connect(mSignalSpeedUp, &QLineEdit::returnPressed,
			this, &QVoltageWidget::setSignalSpeedUp);
	connect(mSignalSpeedDn, &QLineEdit::returnPressed,
			this, &QVoltageWidget::setSignalSpeedDn);

	connect(mDriftState, &QCheckBox::clicked,
			this, &QVoltageWidget::setDriftState);
	connect(mSignalState, &QCheckBox::clicked,
			this, &QVoltageWidget::setSignalState);
	connect(mToggle, &QPushButton::clicked, [&]{
		if(timer->isActive())
			timer->stop();
		else
			timer->start();
		mToggle->setText(timer->isActive() ? "Stop" : "Start");
	});
	connect(mUpdate, &QPushButton::clicked,
			this, &QVoltageWidget::updateValues);
	connect(mOpen, &QPushButton::clicked, [&] {
		try {
			mController->open();
			mIsOpen->setText("Opened");
		} catch(exception& e) {
			errorMessage("Open voltage", e.what());
		}
	});
	connect(mClose, &QPushButton::clicked, [&] {
		try {
			mController->close();
			mIsOpen->setText("Closed");
		} catch(exception& e) {
			errorMessage("Close voltage", e.what());
		}
	});

	connect(timer, &QTimer::timeout,
			this, &QVoltageWidget::updatePlot);
}

void QVoltageWidget::setupGUI() {
	auto controlLayout = new QVBoxLayout;
	mDriftVolt = new QLineEdit("0");
	mDriftSpeedUp = new QLineEdit("0");
	mDriftSpeedDn = new QLineEdit("0");
	mDriftState = new QCheckBox;

	auto driftLayout = new QFormLayout;
	driftLayout->addRow("Voltage", mDriftVolt);
	driftLayout->addRow("Speed Up", mDriftSpeedUp);
	driftLayout->addRow("Speed Down", mDriftSpeedDn);
	driftLayout->addRow("State", mDriftState);
	auto driftGroup = new QGroupBox("Drift");
	driftGroup->setLayout(driftLayout);
	controlLayout->addWidget(driftGroup);

	mSignalVolt = new QLineEdit("0");
	mSignalSpeedUp = new QLineEdit("0");
	mSignalSpeedDn = new QLineEdit("0");
	mSignalState = new QCheckBox;

	auto signalLayout = new QFormLayout;
	signalLayout->addRow("Voltage", mSignalVolt);
	signalLayout->addRow("Speed Up", mSignalSpeedUp);
	signalLayout->addRow("Speed Down", mSignalSpeedDn);
	signalLayout->addRow("State", mSignalState);
	auto signalGroup = new QGroupBox("Signal");
	signalGroup->setLayout(signalLayout);
	controlLayout->addWidget(signalGroup);

	mIsOpen  = new QLabel("Closed");
	mOpen  = new QPushButton("Open");
	mClose = new QPushButton("Close");
	mToggle = new QPushButton("Start");
	mUpdate = new QPushButton("Update");

	auto freq = new QLineEdit("10");
	connect(freq, &QLineEdit::returnPressed, [this, freq]{
		bool ok;
		auto val = freq->text().toInt(&ok);
		if(ok) mFreq = val;
		freq->setText(QString::number(mFreq));
		timer->setInterval(1000*mFreq);
		mDriftPlot->xAxis->setTickStep(4*mFreq);
	});

	auto actionLayout = new QFormLayout;
	actionLayout->addRow(mIsOpen);
	actionLayout->addRow(mOpen);
	actionLayout->addRow(mClose);
	actionLayout->addRow(mToggle);
	actionLayout->addRow(mUpdate);
	actionLayout->addRow("Frequency", freq);
	auto actionGroup = new QGroupBox("Actions");
	actionGroup->setLayout(actionLayout);
	controlLayout->addWidget(actionGroup);

	auto plotLayout = new QVBoxLayout;
	mDriftPlot = new QCustomPlot;
	mSignalPlot = new QCustomPlot;
	plotLayout->addWidget(mDriftPlot);
	plotLayout->addWidget(mSignalPlot);

	auto controlWidget = new QWidget;
	controlWidget->setLayout(controlLayout);
	auto plotWidget = new QWidget;
	plotWidget->setLayout(plotLayout);

	setOrientation(Qt::Horizontal);
	addWidget(controlWidget);
	addWidget(plotWidget);
	setStretchFactor(1, 5);
}

void QVoltageWidget::setupPlot(QCustomPlot& plot, const QString& name) {
	plot.xAxis->setTickLabelType(QCPAxis::ltDateTime);
	plot.xAxis->setDateTimeFormat("hh:mm:ss");
	plot.xAxis->setAutoTickStep(false);
	plot.xAxis->setTickStep(3);
	plot.legend->setVisible(true);
	plot.yAxis->setLabel("Voltage");
	plot.yAxis2->setLabel("Amperage");
	plot.yAxis2->setVisible(true);
	{
		// voltage
		plot.addGraph(plot.xAxis, plot.yAxis);

		QBrush brush(Qt::red);
		QPen pen(Qt::red);
		pen.setWidth(2);
		pen.setStyle(Qt::PenStyle::SolidLine);

		plot.graph(0)->setName(name + " voltage");
		plot.graph(0)->setPen(pen);
		plot.graph(0)->setLineStyle(QCPGraph::lsLine);
		plot.graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
	}

	{
		//amperage
		plot.addGraph(plot.xAxis, plot.yAxis2);

		QBrush brush(Qt::blue);
		QPen pen(Qt::blue);
		pen.setWidth(2);
		pen.setStyle(Qt::DotLine);

		plot.graph(1)->setName(name + " amperage");
		plot.graph(1)->setPen(pen);
		plot.graph(1)->setLineStyle(QCPGraph::lsLine);
		plot.graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
	}

}

void QVoltageWidget::setDriftVoltage() {
	try {
		auto val = mDriftVolt->text().toInt();
		mController->setVoltage("drift", val);
	} catch(exception& e) {
		errorMessage("Set Drift Voltage", e.what());
	}
}

void QVoltageWidget::setSignalVoltage() {
	try {
		auto val = mSignalVolt->text().toInt();
		mController->setVoltage("signal", val);
	} catch(exception& e) {
		errorMessage("Set Signal Voltage", e.what());
	}
}

void QVoltageWidget::setDriftSpeedUp() {
	try {
		auto val = mDriftSpeedUp->text().toInt();
		mController->setSpeedUp("drift", val);
	} catch(exception& e) {
		errorMessage("Set Drift Speed Up", e.what());
	}
}

void QVoltageWidget::setDriftSpeedDn() {
	try {
		auto val = mDriftSpeedDn->text().toInt();
		mController->setSpeedDn("drift", val);
	} catch(exception& e) {
		errorMessage("Set Drift Speed Down", e.what());
	}

}

void QVoltageWidget::setSignalSpeedUp() {
	try {
		auto val = mSignalSpeedUp->text().toInt();
		mController->setSpeedUp("signal", val);
	} catch(exception& e) {
		errorMessage("Set Signal Speed Up", e.what());
	}
}

void QVoltageWidget::setSignalSpeedDn() {
	try {
		auto val = mSignalSpeedDn->text().toInt();
		mController->setSpeedDn("signal", val);
	} catch(exception& e) {
		errorMessage("Set Signal Speed Down", e.what());
	}
}

void QVoltageWidget::setDriftState(bool flag) {
	try {
		if(flag)
			mController->turnOn("drift");
		else
			mController->turnOff("drift");
	} catch(exception& e) {
		errorMessage("Set Drift State", e.what());
	}
}

void QVoltageWidget::setSignalState(bool flag) {
	try {
		if(flag)
			mController->turnOn("signal");
		else
			mController->turnOff("signal");
	} catch(exception& e) {
		errorMessage("Set Signal State", e.what());
	}
}

void QVoltageWidget::updateValues() {
	try {
		mIsOpen->setText(mController->isOpen() ? "Opened" : "Closed");
		mDriftVolt->setText(QString::number(mController->voltage("drift")));
		mDriftSpeedUp->setText(QString::number(mController->speedUp("drift")));
		mDriftSpeedUp->setText(QString::number(mController->speedDn("drift")));
		mDriftState->setChecked(mController->stat("drift").gs());

		mSignalVolt->setText(QString::number(mController->voltage("signal")));
		mSignalSpeedUp->setText(QString::number(mController->speedUp("signal")));
		mSignalSpeedUp->setText(QString::number(mController->speedDn("signal")));
		mSignalState->setChecked(mController->stat("signal").gs());
	} catch(exception& e) {
		errorMessage("Update", e.what());
	}
}

void QVoltageWidget::updatePlot() {
	double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
	try {
		auto driftVolt = mController->voltage("drift");
		auto driftAmp = mController->amperage("drift");
		auto signalVolt = mController->voltage("signal");
		auto signalAmp = mController->amperage("signal");

		mDriftPlot->graph(0)->addData(key, driftVolt);
		mDriftPlot->graph(1)->addData(key, driftAmp);
		mDriftPlot->graph(0)->removeDataBefore(key-50*mFreq);
		mDriftPlot->graph(1)->removeDataBefore(key-50*mFreq);
		mDriftPlot->rescaleAxes();
		mDriftPlot->replot();

		mSignalPlot->graph(0)->addData(key, signalVolt);
		mSignalPlot->graph(1)->addData(key, signalAmp);
		mSignalPlot->graph(0)->removeDataBefore(key-50*mFreq);
		mSignalPlot->graph(1)->removeDataBefore(key-50*mFreq);
		mSignalPlot->rescaleAxes();
		mSignalPlot->replot();
	} catch(exception& e) {
		errorMessage("Voltage Monitor", e.what());
		mToggle->click();
	}

}
