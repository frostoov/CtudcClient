#include "mainwindow.hpp"

#include <iostream>
#include <stdexcept>
#include <QBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

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
      mVoltage(new QVoltageWidget(mVoltContr, mVoltView)),
      mMonitor(new QMonitor(mExpoContr, mExpoView)) {
    mClient.addView(mTdcView);
    mClient.addView(mVoltView);
    mClient.addView(mExpoView);
    mConn->onRecv([this](auto& resp) {
        if(!resp.status.empty()) {
            emit this->serverFail(QString::fromStdString(resp.object),
                                  QString::fromStdString(resp.method),
                                  QString::fromStdString(resp.status));
        }
        mClient.handleReponse(resp);
    });
    resize(800, 600);

    setupGUI();

    connect(this, &MainWindow::serverFail, this, [this](QString obj,QString method,QString status) {
        QMessageBox::warning(this, "Failure",
                             tr("%1.%2: %3")
                             .arg(obj)
                             .arg(method)
                             .arg(status));
    });

    connect(mRefresh, &QPushButton::clicked, this, [this](auto) {
        mTdcContr->isOpen();
        mTdcContr->settings();
        mTdcContr->mode();
        mTdcContr->ctrl();
        mTdcContr->stat();
        mTdcContr->tdcMeta();
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
            mExpoContr->launchRead();
        else if(mStart->text() == "Stop")
            mExpoContr->stopRead();
    });
    connect(voltB, &QPushButton::clicked, this, [this] {
        if(mVoltage->isVisible())
            mVoltage->hide();
        else
            mVoltage->show();
    });
    connect(freqB, &QPushButton::clicked, this, [this] {
        if(!mFrequency->isVisible())
            mFrequency->show();
        else
            mFrequency->hide();
    });
    connect(monitB, &QPushButton::clicked, this, [this] {
        if(!mMonitor->isVisible())
            mMonitor->show();
        else
            mMonitor->hide();
    });
    connect(mExpoView, &ExpoView::type, this, [this](auto status, auto type) {
        static bool single = true;
        if(status.isEmpty()) {
            mStart->setEnabled(type == "idle" || type == "expo");
            if(type == "idle")
                mStart->setText("Start");
            else if(type == "expo")
                mStart->setText("Stop");
            if(single && type == "expo") {
                mExpoContr->run();
                single = false;
            }
        }
    });

    connect(mTdcView, &TdcView::isOpen, this, [this](auto status, auto isOpen) {
        static bool single = true;
        if(status.isEmpty()) {
            mOpen->setText(isOpen ? "Close" : "Open");
            if(single && isOpen) {
                mRefresh->click();
                single = false;
            };
        }
    });
    mTdcContr->isOpen();
    mExpoContr->type();
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
    monitB = new QPushButton("monitor");

    auto actionLayout = new QGridLayout;
    actionLayout->setAlignment(Qt::AlignBaseline);
    actionLayout->addWidget(mOpen,	0, 0);
    actionLayout->addWidget	(mStart, 0, 1);
    actionLayout->addWidget	(mReset, 1, 0);
    actionLayout->addWidget	(mClear, 1, 1);
    actionLayout->addWidget(freqB, 2, 0);
    actionLayout->addWidget(voltB, 2, 1);
    actionLayout->addWidget(mRefresh, 3, 0);
    actionLayout->addWidget(monitB, 3, 1);
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
