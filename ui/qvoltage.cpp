#include "qvoltage.hpp"


#include <QGroupBox>
#include <QFormLayout>
#include <QSplitter>

using std::shared_ptr;
using std::exception;

constexpr auto cellList = {"drift", "signal"};

QVoltageWidget::QVoltageWidget(shared_ptr<VoltageController> controller,
							   VoltageView* view,
							   QWidget* parent)
	: QSplitter(parent),
	  mController(controller),
	  mView(view),
	  mFreq(10) {
	setupGUI();
	for(auto& name : cellList)
		setupPlot(*mPlot[name], name);

	timer = new QTimer(this);
	timer->setInterval(mFreq * 1000);
	timer->setSingleShot(false);

	createConnections();
	resize(800, 600);
}

void QVoltageWidget::createConnections() {
	//Controller connections
	for(auto& name : cellList) {
		connect(mVolt[name], &QLineEdit::returnPressed, this, [this, name] {
			mController->setVoltage(name, mVolt[name]->text().toInt());
		});
		connect(mSpeedUp[name], &QLineEdit::returnPressed, this, [this, name] {
			mController->setSpeedUp(name, mSpeedUp[name]->text().toInt());
		});
		connect(mSpeedDn[name], &QLineEdit::returnPressed, this, [this, name] {
			mController->setSpeedDn(name, mSpeedDn[name]->text().toInt());
		});
		connect(mState[name], &QCheckBox::clicked, this, [this, name](auto flag) {
			if(flag)
				mController->turnOn(name);
			else
				mController->turnOff(name);
		});
	}

	connect(mToggle, &QPushButton::clicked, [&] {
		if(timer->isActive())
			timer->stop();
		else
			timer->start();
		mToggle->setText(timer->isActive() ? "Stop" : "Start");
	});
	connect(mUpdate, &QPushButton::clicked, this, [this](auto flag) {
		for(auto& name : cellList)
			this->updateCell(name);
	});
	connect(mOpen, &QPushButton::clicked, this, [this](auto flag) {
		if(mOpen->text() == "Open")
			mController->open();
		else if(mOpen->text() == "Close")
			mController->close();
	});
	connect(timer, &QTimer::timeout, this, [this] {
		for(auto& name : cellList)
			this->updateCell(name);
	});
	//View connections
	connect(mView, &VoltageView::voltage, this, [this](auto status, auto type, auto val) {
		if(!status.isEmpty())
			QMessageBox::critical(this, "Get voltage", status);
		else {
			mVolt[type]->setText(QString::number(val));
			this->updateGraph(*mPlot[type]->graph(0), val);
		}
	});
	connect(mView, &VoltageView::amperage, this, [this](auto status, auto type, auto val) {
		if(status.isEmpty())
			this->updateGraph(*mPlot[type]->graph(1), val);
	});
	connect(mView, &VoltageView::speedUp, this, [this](auto status, auto type, auto val) {
		if(status.isEmpty())
			mSpeedUp[type]->setText(QString::number(val));
	});
	connect(mView, &VoltageView::speedDn, this, [this](auto status, auto type, auto val) {
		if(status.isEmpty())
			mSpeedDn[type]->setText(QString::number(val));
	});
	connect(mView, &VoltageView::stat, this, [this](auto status, auto type, VoltageView::Stat stat) {
		if(status.isEmpty())
			mState[type]->setChecked(stat.gs());
	});
	connect(mView, &VoltageView::isOpen, this, [this](auto status, auto flag) {
		if(status.isEmpty()) {
			if(flag)
				mOpen->setText("Close");
			else
				mOpen->setText("Open");
		}
	});
}

void QVoltageWidget::setupGUI() {
	auto controlLayout = new QVBoxLayout;

	for(auto& name : cellList) {
		mVolt.insert(name, new QLineEdit("0"));
		mSpeedUp.insert(name, new  QLineEdit("0"));
		mSpeedDn.insert(name, new QLineEdit("0"));
		mState.insert(name, new QCheckBox);

		auto layout = new QFormLayout;
		layout->addRow("Voltage", mVolt[name]);
		layout->addRow("Speed Up", mSpeedUp[name]);
		layout->addRow("Speed Down", mSpeedDn[name]);
		layout->addRow("State", mState[name]);
		auto group = new QGroupBox(name);
		group->setLayout(layout);
		controlLayout->addWidget(group);
	}

	mOpen  = new QPushButton("Open");
	mToggle = new QPushButton("Start");
	mUpdate = new QPushButton("Update");

	auto freq = new QLineEdit("10");
	connect(freq, &QLineEdit::returnPressed, [this, freq] {
		bool ok;
		auto val = freq->text().toInt(&ok);
		if(ok) mFreq = val;
		freq->setText(QString::number(mFreq));
		timer->setInterval(1000 * mFreq);
		for(auto& plot : mPlot)
			plot->xAxis->setTickStep(4 * mFreq);
	});

	auto actionLayout = new QFormLayout;
	actionLayout->addRow(mOpen);
	actionLayout->addRow(mToggle);
	actionLayout->addRow(mUpdate);
	actionLayout->addRow("Frequency", freq);
	auto actionGroup = new QGroupBox("Actions");
	actionGroup->setLayout(actionLayout);
	controlLayout->addWidget(actionGroup);

	auto plotLayout = new QVBoxLayout;
	for(auto& name : cellList) {
		mPlot.insert(name, new QCustomPlot);
		plotLayout->addWidget(mPlot[name]);
	}

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

void QVoltageWidget::updateGraph(QCPGraph& graph, int val) {
	double key = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0;
	graph.addData(key, val);
	graph.removeDataBefore(key - 50 * mFreq);
}

void QVoltageWidget::updateCell(const std::string& type) {
	mController->voltage(type);
	mController->amperage(type);
	mController->speedUp(type);
	mController->speedDn(type);
	mController->stat(type);
}
