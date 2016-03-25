#include <iostream>
#include <stdexcept>
#include <QBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

#include "mainwindow.hpp"

bool isOpenRefresher = true;
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
	  mTdcView(new TdcView(this)),
	  mVoltView(new VoltageView(this)),
	  mExpoView(new ExpoView(this)),
	  mStatus(new QStatusWidget (mTdcContr, mTdcView, this)),
	  mControl(new QControlWidget (mTdcContr, mTdcView, this)),
	  mSettings(new QSettingsWidget (mTdcContr, mTdcView, this)),
	  mFrequency(new QFrequencyWidget(mExpoContr, mVoltContr, mExpoView, mVoltView)),
	  mVoltage(new QVoltageWidget(mVoltContr, mVoltView)) {
	mClient.addView(mTdcView);
	mClient.addView(mVoltView);
	mClient.addView(mExpoView);
	mConn->onRecv([this](auto& response){
		mClient.handleReponse(response);
	});
	resize(800, 600);

	setupGUI();

	connect(mRefresh, &QPushButton::clicked, this, [this](auto){
		mTdcContr->isOpen();
		mTdcContr->settings();
		mTdcContr->mode();
		mTdcContr->ctrl();
		mTdcContr->stat();
		mTdcContr->tdcMeta();
		mExpoContr->type();
	});

	connect(mOpen, &QPushButton::clicked, this, [this] {
			if(mOpen->text() == "Open")
				mTdcContr->open();
			else
				mTdcContr->close();
	});
	connect(mClear, &QPushButton::clicked, this, [this] {
			mTdcContr->clear();
	});
	connect(mReset, &QPushButton::clicked, this, [this] {
			mTdcContr->reset();
	});
	connect(mStart, &QPushButton::clicked, this, [this] {
		if(mStart->text() == "Start")
			mExpoContr->startRead();
		else if(mStart->text() == "Stop")
			mExpoContr->stopRead();
	});
	connect(freqB, &QPushButton::clicked, this, [this]{
		if(!mFrequency->isVisible())
			mFrequency->show();
		else
			mFrequency->hide();
	});
	connect(mExpoView, &ExpoView::type, this, [this](auto status, auto type){
		if(!status.isEmpty()) {
			QMessageBox::critical(this, "Expo type", status);
		} else {
			mStart->setEnabled(type == "none" || type == "expo");
			if(type == "none")
				mStart->setText("Start");
			else if(type == "expo")
				mStart->setText("Stop");
		}
	});
	connect(mExpoView, &ExpoView::startRead, this, [this](auto status){
		if(!status.isEmpty())
			QMessageBox::critical(this, "startRead", status);
	});
	connect(mExpoView, &ExpoView::stopRead, this, [this](auto status){
		if(!status.isEmpty())
			QMessageBox::critical(this, "stopRead", status);
	});

	connect(mTdcView, &TdcView::isOpen, this, [this](auto status, auto isOpen) mutable {
		if(!status.isEmpty()) {
			QMessageBox::critical(this, "Tdc isOpen", status);
		} else {
			mOpen->setText(isOpen ? "Close" : "Open");
			if(isOpen && isOpenRefresher) {
				std::cout << "isOpenRefresher: " << isOpenRefresher << std::endl;
				mRefresh->click();
				isOpenRefresher = false;
			};
		}
	});
	connect(mTdcView, &TdcView::open, this, [this](auto status){
		if(!status.isEmpty())
			QMessageBox::critical(this, "Tdc open", status);
	});
	connect(mTdcView, &TdcView::close, this, [this](auto status){
		if(!status.isEmpty())
			QMessageBox::critical(this, "Tdc close", status);
	});
	connect(mTdcView, &TdcView::clear, this, [this](auto status){
		if(!status.isEmpty())
			QMessageBox::critical(this, "Tdc clear", status);
	});
	connect(mTdcView, &TdcView::reset, this, [this](auto status){
		if(!status.isEmpty())
			QMessageBox::critical(this, "Tdc reset", status);
	});
	connect(voltB, &QPushButton::clicked, this, [this]{
		if(mVoltage->isVisible())
			mVoltage->hide();
		else
			mVoltage->show();
	});
	mTdcContr->isOpen();
}

MainWindow::~MainWindow() {
	delete mFrequency;
	delete mVoltage;
}

void MainWindow::setupGUI() {
	mRefresh = new QPushButton("Refresh");
	mOpen = new QPushButton("Open");
	mReset = new QPushButton("Reset");
	mClear = new QPushButton("Clear");
	mStart = new QPushButton("Start");
	freqB = new QPushButton("frequency");
	voltB = new QPushButton("voltage");

	auto actionLayout = new QGridLayout;
	actionLayout->setAlignment(Qt::AlignBaseline);
	actionLayout->addWidget(mOpen,	0, 0);
	actionLayout->addWidget	(mStart, 0, 1);
	actionLayout->addWidget	(mReset, 1, 0);
	actionLayout->addWidget	(mClear, 1, 1);
	actionLayout->addWidget(freqB, 2, 0);
	actionLayout->addWidget(voltB, 2, 1);
	actionLayout->addWidget(mRefresh, 3, 0);
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
