#include "qmonitor.hpp"

#include <trek/common/timeprint.hpp>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QSizePolicy>

using std::shared_ptr;
using std::make_unique;

QMonitor::QMonitor(std::shared_ptr<ExpoController> expoContr,
				   ExpoView* expoView,
				   QWidget *parent)
	: QSplitter(parent),
	  mExpoContr(expoContr),
	  mExpoView(expoView) {
	setupGUI();

	mTimer = new QTimer(this);
	mTimer->setInterval(mTick->text().toInt() * 1000);
	mTimer->setSingleShot(false);
	for(auto& c : mChambers) c->setTick(mTick->text().toInt());
	mPlot->xAxis->setTickStep(mTick->text().toInt());

	createConnections();
	resize(800, 600);
}

void setTab(QTabWidget& tab, int index, QWidget* widget) {
	tab.removeTab(index);
	tab.insertTab(index, widget, "chambers");
}

void QMonitor::setupGUI() {
	auto ctrlLayout = new QVBoxLayout;

	mToggle = new QPushButton("Start");
	mTick   = new QLineEdit("30");
	auto actionLayout = new QFormLayout;
	actionLayout->addRow(mToggle);
	actionLayout->addRow("Frequency", mTick);
	auto actionGroup = new QGroupBox("Actions");
	actionGroup->setLayout(actionLayout);

	mCurrentRun = new QLineEdit("-");
	mCurrentRun->setReadOnly(true);
	mType = new QLineEdit("idle");
	mType->setReadOnly(true);
	mTriggerLine = new QLineEdit("-");
	mTriggerLine->setReadOnly(true);
	mPackageLine = new QLineEdit("-");
	mPackageLine->setReadOnly(true);
	auto statLayout = new QFormLayout;
	statLayout->addRow("Type",mType);
	statLayout->addRow("Run", mCurrentRun);
	statLayout->addRow("Triggers", mTriggerLine);
	statLayout->addRow("Packages", mPackageLine);
	auto statGroup = new QGroupBox("Status");
	statGroup->setLayout(statLayout);

	ctrlLayout->addWidget(actionGroup);
	ctrlLayout->addWidget(statGroup);

	mPlot = makePlot();

	auto chambersLayout = new QGridLayout;
	for(int i = 0; i < 16; ++i) {
		mChambers[i] = new QChamberMonitor(tr("Chamber %1").arg(i + 1));
		chambersLayout->addWidget(mChambers[i], i%4, 4 - i/4);

		connect(mChambers[i], &QChamberMonitor::mouseDoubleClick, this, [this, i] {
			static bool flag = true;
			if(flag) {
				for(auto& c : mChambers) c->hide();
				mChambers[i]->show();
				flag = !flag;
			} else {
				for(auto& c : mChambers) c->show();
				flag = !flag;
			}
		});
	}
	auto chambersWidget = new QWidget;
	chambersWidget->setLayout(chambersLayout);

	auto tab = new QTabWidget;
	tab->addTab( mPlot, "main" );
	tab->addTab(chambersWidget, "chambers");

	auto ctrlWidget = new QWidget;
	ctrlWidget->setLayout(ctrlLayout);

	setOrientation(Qt::Horizontal);
	addWidget(ctrlWidget);
	addWidget(tab);
	setStretchFactor(1, 5);
}


QCustomPlot* QMonitor::makePlot() {
	auto* plot = new QCustomPlot;
	plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	plot->xAxis->setDateTimeFormat("hh:mm:ss");
	plot->xAxis->setAutoTickStep(false);
	plot->xAxis->setTickStep(3);
	plot->legend->setVisible(true);
	plot->yAxis->setLabel("Trigger freq");
	plot->yAxis2->setLabel("Pacakge freq");
	plot->yAxis2->setVisible(true);
	{
		// Trigger
		plot->addGraph(plot->xAxis, plot->yAxis);

		QBrush brush(Qt::red);
		QPen pen(Qt::red);
		pen.setWidth(2);
		pen.setStyle(Qt::PenStyle::SolidLine);

		plot->graph(0)->setName("Trigger freq");
		plot->graph(0)->setPen(pen);
		plot->graph(0)->setLineStyle(QCPGraph::lsLine);
		plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
	}

	{
		// Package
		plot->addGraph(plot->xAxis, plot->yAxis2);

		QBrush brush(Qt::blue);
		QPen pen(Qt::blue);
		pen.setWidth(2);
		pen.setStyle(Qt::DotLine);

		plot->graph(1)->setName("Package freq");
		plot->graph(1)->setPen(pen);
		plot->graph(1)->setLineStyle(QCPGraph::lsLine);
		plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
	}
	return plot;
}

void QMonitor::createConnections() {
	connect(mTick, &QLineEdit::editingFinished, [this]{
		auto tick = (mTick->text().toInt() > 0) ? mTick->text().toInt() : 1;
		mTick->setText( QString::number(tick) );
		if(mTimer->interval() != tick * 1000) {
			mTimer->setInterval(tick * 1000);
			mPlot->xAxis->setTickStep(4*tick);
			for(auto& c : mChambers) c->setTick(tick);
			for(auto& t : mTriggerCount) t.reset();
			for(auto& p : mPackageCount) p.reset();
			for(auto& c : mChambersCount) c.reset();
			if(mTimer->isActive()) mTimer->start();
		}
	});
	connect(mToggle, &QPushButton::clicked, [this]{
		if(mToggle->text() == "Start") {
			mToggle->setText("Stop");
			mTimer->start();
		} else {
			mToggle->setText("Start");
			mTimer->stop();
		}
	});
	connect(mTimer, &QTimer::timeout, [this]{
		mExpoContr->triggerCount();
		mExpoContr->packageCount();
		mExpoContr->chambersCount();
	});


	connect(mExpoView, &ExpoView::type, [this](auto status, auto type) {
		if(status.isEmpty()) {
			mType->setText(type);
			mToggle->setEnabled(type == "expo");
			if(type != "read") {
				mToggle->setText("Start");
				mTimer->stop();
			}
		}
	});
	connect(mExpoView, &ExpoView::run, this, [this](auto status, auto run) {
		if(status.isEmpty()){
			mCurrentRun->setText(QString::number(run));
		}
	});
	connect(mExpoView, &ExpoView::triggerCount, this, [this](auto status, auto count, auto drop){
		if(status.isEmpty()) {
			if(mTriggerCount[0] != nullptr) {
				this->updateGraph(*mPlot->graph(0), (count - *mTriggerCount[0])/mTick->text().toInt());
				mPlot->replot();
				*mTriggerCount[0] = count;
				*mTriggerCount[1] = drop;
			} else {
				mTriggerCount[0] = make_unique<uintmax_t>(count);
				mTriggerCount[1] = make_unique<uintmax_t>(drop);
			}
			mTriggerLine->setText(tr("%1 | %2").arg(count).arg(drop));
		}
	});
	connect(mExpoView, &ExpoView::packageCount, this, [this](auto status, auto count, auto drop){
		if(status.isEmpty()) {
			if(mPackageCount[0] != nullptr) {
				this->updateGraph(*mPlot->graph(1), (count - *mPackageCount[0])/mTick->text().toInt());
				mPlot->replot();
				*mPackageCount[0] = count;
				*mPackageCount[1] = drop;
			} else {
				mPackageCount[0] = make_unique<uintmax_t>(count);
				mPackageCount[1] = make_unique<uintmax_t>(drop);
			}
			mPackageLine->setText(tr("%1 | %2").arg(count).arg(drop));
		}
	});
	connect(mExpoView, &ExpoView::chambersCount, this, [this](auto status, auto count, auto drop) {
		if(status.isEmpty()) {
			if(mChambersCount[0] != nullptr) {
				for(auto& c : this->convertCount(count, *mChambersCount[0], mTick->text().toInt()))
					mChambers.at(c.first)->addFreq(c.second);
				*mChambersCount[0] = count;
			} else {
				mChambersCount[0] = make_unique<TrekFreq>(count);
			}
		}
	});
}

void QMonitor::updateGraph(QCPGraph& graph, int val) {
	double key = double(QDateTime::currentDateTime().toMSecsSinceEpoch()) / 1000;
	graph.addData(key, val);
	graph.removeDataBefore(key - 50 * mTick->text().toInt());
	graph.rescaleAxes();
}

ChamberFreq QMonitor::convertCount(const ChamberFreq& count, const ChamberFreq& prev, int sec) {
	ChamberFreq freq{{0, 0, 0, 0}};
	for(size_t i = 0; i < count.size(); ++i)
		freq[i] = (count.at(i) - prev.at(i))/sec;
	return freq;
}

TrekFreq QMonitor::convertCount(const TrekFreq& count, const TrekFreq& prev, int sec) {
	TrekFreq freq;
	constexpr ChamberFreq nullfreq{{0, 0, 0, 0}};
	for(auto& c : count) {
		auto p = prev.find(c.first);
		if(p == prev.end())
			freq.emplace(c.first, convertCount(c.second, nullfreq, sec));
		else
			freq.emplace(c.first, convertCount(c.second, p->second, sec));
	}
	for(auto& p : count)
		if(freq.count(p.first) == 0)
			freq.emplace(p.first, nullfreq);

	return freq;
}
