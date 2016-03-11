#include <iostream>
#include <stdexcept>
#include <QBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

#include "mainwindow.hpp"
#include "utils.hpp"

using std::string;
using std::function;
using std::ostringstream;
using std::exception;
using std::make_shared;

MainWindow::MainWindow(std::shared_ptr<CtudcConn> conn, QWidget* parent)
	: QMainWindow(parent),
	  mConn(conn),
	  mTdcContr(make_shared<TdcController>(mConn)),
	  mVoltContr(make_shared<VoltageController>(mConn)),
	  mExpoContr(make_shared<ExpoController>(mConn)),
	  mStatus(new QStatusWidget		(mTdcContr, this)),
	  mControl(new QControlWidget	(mTdcContr, this)),
	  mSettings(new QSettingsWidget	(mTdcContr, this)),
	  mFrequency(new QFrequencyWidget(mExpoContr, mVoltContr)),
	  mVoltage(new QVoltageWidget(mVoltContr)) {
	resize(800, 600);

	setupGUI();

	connect(mRefresh, &QPushButton::clicked,
			this, &MainWindow::refresh);

	connect(mOpen, &QPushButton::clicked, [&] {
		doAction("open", [&]{
			mTdcContr->open();
		});
	});
	connect(mClose, &QPushButton::clicked, [&] {
		doAction("close", [&]{
			mTdcContr->close();
		});
	});
	connect(mClear, &QPushButton::clicked, [&] {
		doAction("TDC clear", [&]{
			mTdcContr->clear();
		});
	});
	connect(mReset, &QPushButton::clicked, [&] {
		doAction("TDC reset", [&] {
			mTdcContr->reset();
		});
	});
	connect(mStartExpo, &QPushButton::clicked,
			this, &MainWindow::startExpo);
	connect(mStopExpo,  &QPushButton::clicked,
			this, &MainWindow::stopExpo);
	connect(freqB, &QPushButton::clicked, [&]{
		if(!mFrequency->isVisible())
			mFrequency->show();
		else
			mFrequency->hide();
	});

	connect(voltB, &QPushButton::clicked, [&]{
		if(mVoltage->isVisible())
			mVoltage->hide();
		else
			mVoltage->show();
	});
}

MainWindow::~MainWindow() {
	delete mFrequency;
	delete mVoltage;
}

void MainWindow::setupGUI() {
	mState = new QLabel("none");
	mRefresh = new QPushButton("Refresh");
	mOpen = new QPushButton("Open");
	mClose = new QPushButton("Close");
	mReset = new QPushButton("Reset");
	mClear = new QPushButton("Clear");
	mStartExpo = new QPushButton("Start Expo");
	mStopExpo = new QPushButton("Stop Expo");
	freqB = new QPushButton("frequency");
	voltB = new QPushButton("voltage");

	auto actionLayout = new QGridLayout;
	actionLayout->setAlignment(Qt::AlignBaseline);
	actionLayout->addWidget(mState, 0, 0);
	actionLayout->addWidget(mRefresh, 0, 1);
	actionLayout->addWidget(mOpen,	1, 0);
	actionLayout->addWidget(mClose,	1, 1);
	actionLayout->addWidget	(mReset, 2, 0);
	actionLayout->addWidget	(mClear,	2, 1);
	actionLayout->addWidget	(mStartExpo, 3, 0);
	actionLayout->addWidget	(mStopExpo, 3, 1);
	actionLayout->addWidget(freqB, 4, 0);
	actionLayout->addWidget(voltB, 4, 1);
	auto actionGroup = new QGroupBox("Actions");
	actionGroup->setLayout(actionLayout);

	auto subLayout = new QVBoxLayout;
	subLayout->setAlignment(Qt::AlignTop);
	subLayout->addWidget(actionGroup);
	subLayout->addWidget(mSettings);

	auto statusLabel = new QLabel("Control Table");
	auto controlLabel = new QLabel("Status Table");
	auto infoLayout	= new QGridLayout;
	infoLayout->setAlignment(Qt::AlignTop);
	infoLayout->addWidget(controlLabel, 0, 0);
	infoLayout->addWidget(statusLabel, 0, 1);
	infoLayout->addWidget(mStatus, 1, 0);
	infoLayout->addWidget(mControl, 1, 1);
	infoLayout->addWidget(console, 2, 0, 2, 2);
	infoLayout->setRowStretch(2, 1);
	infoLayout->setRowStretch(1, 1);

	auto mainLayout	= new QHBoxLayout;
	mainLayout->setAlignment(Qt::AlignTop);
	mainLayout->addLayout(subLayout, 1);
	mainLayout->addLayout(infoLayout, 5);

	auto w = new QWidget;
	w->setLayout(mainLayout);
	setCentralWidget(w);
}

void MainWindow::startExpo() {
	try {
		auto type = mExpoContr->type();
		if(type != "none")
			return;
	} catch(exception& e) {
		errorMessage("Get Expo State", e.what());
		return;
	}

	doAction("Expo start", [this] {
		mExpoContr->startRead();
	});
}

void MainWindow::stopExpo() {
	try {
		auto type = mExpoContr->type();
		if(type != "expo")
			return;
	} catch(exception& e) {
		errorMessage("Get Expo State", e.what());
		return;
	}

	doAction("Expo stop", [this] {
		mExpoContr->stopRead();
	});
}



void MainWindow::refresh() {
	mSettings->refresh();
	mStatus->refresh();
	mControl->refresh();
	doAction("Get expo state", [&] {
		mState->setText(QString::fromStdString(mExpoContr->type()));
	});
}
