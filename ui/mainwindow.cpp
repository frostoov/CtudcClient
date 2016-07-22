#include "mainwindow.hpp"

#include "net/ctudcconn.hpp"
#include <trek/net/multicastreceiver.hpp>
#include <trek/net/response.hpp>

#include <iostream>
#include <stdexcept>
#include <QBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>


using trek::net::MulticastReceiver;
using trek::net::Response;

using std::string;
using std::function;
using std::ostringstream;
using std::exception;
using std::make_shared;
using std::shared_ptr;

MainWindow::MainWindow(shared_ptr<CtudcConn> conn,
                       shared_ptr<MulticastReceiver> recv,
                       QWidget* parent)
    : QMainWindow(parent),
      mConn(conn),
      mRecv(recv),
      mTdcContr(make_shared<TdcController>("caen", mConn)),
      mVoltContr(make_shared<VoltageController>("vlt", mConn)),
      mExpoContr(make_shared<ExpoController>("expo", mConn)),
      mStatus(new QStatusWidget (mTdcContr, this)),
      mControl(new QControlWidget (mTdcContr, this)),
      mSettings(new QSettingsWidget (mTdcContr, this)),
      mFrequency(new QFrequencyWidget(mExpoContr, mVoltContr)),
      mVoltage(new QVoltageWidget(mVoltContr)),
      mMonitor(new QMonitor(mExpoContr)) {
    mRecv->onRecv([&](const auto& r) {
        try {
            auto response = Response({r.begin(), r.end()});
            std::cout << "udp: " << string(response) << std::endl;
            if(response.object == "caen") {
                mTdcContr->handleResponse(response);
            } else if(response.object == "vlt") {
                mVoltContr->handleResponse(response);
            } else if(response.object == "expo") {
                mExpoContr->handleResponse(response);
            }
        } catch(...) {
            //TODO
        }
    });
    mRecvThread = std::thread([&]{
        mRecv->run();
    });
    resize(800, 600);

    setupGUI();

    connect(mRefresh, &QPushButton::clicked, this, [this](auto) {
        try {
            auto type = mExpoContr->type();
            mStart->setEnabled(type == "idle" || type == "expo");
            if(type == "idle")
                mStart->setText("Start");
            else if(type == "expo")
                mStart->setText("Stop");
            auto isOpen = mTdcContr->isOpen();
            mOpen->setText(isOpen ? "Close" : "Open");
            if(isOpen) {
                mSettings->updateSettings();
                mSettings->updateMode();
                mSettings->updateTdcMeta();
                
                mControl->updateCtrl();
                mStatus->updateStatus();
                mMonitor->updateState();
            }
        } catch(exception& e) {
            QMessageBox::warning(this, "failure", e.what());
        }
    });

    connect(mOpen, &QPushButton::clicked, this, [this] {
        try {
            if(mOpen->text() == "Open") {
                mTdcContr->open();
                mOpen->setText("Close");
            } else {
                mTdcContr->close();
                mOpen->setText("Open");
            }
        } catch(exception& e) {
            QMessageBox::warning(this, "tdc open", e.what());
        }
    });
    connect(mClear, &QPushButton::clicked, this, [this] {
        try {
            mTdcContr->clear();
        } catch(exception& e) {
            QMessageBox::warning(this, "tdc clear", e.what());
        }
    });
    connect(mReset, &QPushButton::clicked, this, [this] {
        try {
            mTdcContr->reset();
        } catch(exception& e) {
            QMessageBox::warning(this, "tdc reset", e.what());
        }
    });
    connect(mStart, &QPushButton::clicked, this, [this] {
        try {
            if(mStart->text() == "Start") {
                mExpoContr->launchRead();
                mStart->setText("Stop");
            } else if(mStart->text() == "Stop") {
                mExpoContr->stopRead();
                mStart->setText("Start");
            }
        } catch(exception& e) {
            QMessageBox::warning(this, "expo", e.what());
        }
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
    connect(mExpoContr.get(), &ExpoController::typeChanged, this, [this](auto type) {
        mStart->setEnabled(type == "idle" || type == "expo");
        if(type == "idle")
            mStart->setText("Start");
        else if(type == "expo")
            mStart->setText("Stop");
    });

    connect(mTdcContr.get(), &TdcController::stateChanged, this, [this](auto isOpen) {        
        mOpen->setText(isOpen ? "Close" : "Open");
    });
    mRefresh->click();
    mMonitor->refresh();
}

MainWindow::~MainWindow() {
    delete mFrequency;
    delete mVoltage;
    delete mMonitor;
    mRecv->stop();
    mRecvThread.join();
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
