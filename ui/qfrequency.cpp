#include <QClipboard>
#include <set>
#include <stdexcept>
#include <QMessageBox>

#include "qfrequency.hpp"
#include "qconsole.hpp"


#include <thread>

using std::chrono::seconds;
using std::chrono::microseconds;
using std::shared_ptr;

QFrequencyWidget::QFrequencyWidget(shared_ptr<ExpoController> expoContr,
                                   shared_ptr<VoltageController> voltContr,
                                   ExpoView* expoView,
                                   VoltageView* voltView,
                                   QWidget* parent)
    : QWidget(parent),
      mExpoContr(expoContr),
      mVoltContr(voltContr),
      mExpoView(expoView),
      mVoltView(voltView),
      mState(State::None) {
    qRegisterMetaType<TrekFreq>("TrekFreq");
    createWidgets();
    createLayouts();
    packWidgets();

    connect(launchFreq, &QPushButton::clicked, this, [this] {
        if(launchFreq->text() == "Start freq") {
            mState = State::Freq;
            mExpoContr->launchFreq(100);
            mFreqConn = connect(mExpoView, &ExpoView::freq, this, [this](auto status, auto freq) {
                if(status.isEmpty()) {
                    mTable->setTrekFreq(freq);
                    mTable->update();
                }
                this->disconnect(mFreqConn);
            });
        } else if(launchFreq->text() == "Stop freq") {
            mExpoContr->stopFreq();
            mExpoContr->freq();
            mState = State::None;
        }
    });
    connect(startLoop, &QPushButton::clicked, this, [this] {
        if(startLoop->text() == "Start loop") {
            mState = State::Loop;
            launchLoop();
            mFreqConn = connect(mExpoView, &ExpoView::freq, this, [this](auto status, auto freq) {
                auto code = startVolt->text().toInt();
                auto step = stepVolt->text().toInt();
                startVolt->setText(QString::number(code + step));
                if(status.isEmpty()) {
                    loopWidget->addFreq(code, freq);
                    loopWidget->updateData();
                }
                if(code >= endVolt->text().toInt()) {
                    startVolt->setText("0");
                    this->disconnect(mFreqConn);
                }
            });
        } else if(startLoop->text() == "Stop loop") {
            mLoopActive.store(false);
        }
    });

    //View connections
    connect(mExpoView, &ExpoView::type, this, [this](auto status, auto type) {
        if(status.isEmpty()) {
            if(type == "idle") {
                launchFreq->setText("Start freq");
                startLoop->setText("Start loop");
            } else if(type == "freq") {
                if(mState == State::Freq) launchFreq->setText("Stop freq");
                if(mState == State::Loop) startLoop->setText("Stop loop");
            }
            launchFreq->setEnabled((mState == State::Freq || mState == State::None) && (type == "idle" || type == "freq"));
            startLoop->setEnabled((mState == State::Loop || mState == State::None) && (type == "idle" || type == "freq"));
        }
    });

    resize(800, 600);
}

QFrequencyWidget::~QFrequencyWidget() {
    //TODO
}

void QFrequencyWidget::createLayouts() {
    mainLayout = new QHBoxLayout;
    subLayout = new QVBoxLayout;
    loopLayout = new QFormLayout;
    setLayout(mainLayout);
}
void QFrequencyWidget::packWidgets() {
    mainLayout->addWidget(tabWidget, 4);
    mainLayout->addLayout(subLayout, 1);
    subLayout->setAlignment(Qt::AlignTop);
    subLayout->addWidget(launchFreq);
    subLayout->addWidget(loopGroup);
    loopLayout->addRow(startLoop);
    loopLayout->addRow("Timer", timerL);
    loopLayout->addRow("Start Code", startVolt);
    loopLayout->addRow("End Code", endVolt);
    loopLayout->addRow("Step Code", stepVolt);
    loopLayout->addRow("Volt cooldown", voltCD);
    loopGroup->setLayout(loopLayout);
}

void QFrequencyWidget::createWidgets() {
    tabWidget = new QTabWidget(this);
    mTable = new QChamberTable(16);
    loopWidget = new QLoopFreqWidget(this);
    launchFreq = new QPushButton("Start freq", this);
    startLoop = new QPushButton("Start loop", this);
    timerL = new QLineEdit("10", this);

    startVolt = new QLineEdit("0", this);
    endVolt = new QLineEdit("100", this);
    stepVolt = new QLineEdit("1", this);
    voltCD = new QLineEdit("5", this);

    loopGroup = new QGroupBox("Measure Loop", this);
    tabWidget->addTab(mTable, "Single");
    tabWidget->addTab(loopWidget, "Loop");
}

void QFrequencyWidget::launchLoop() {
    auto start = startVolt->text().toInt();
    auto end = endVolt->text().toInt();
    auto step = stepVolt->text().toInt();
    auto time = timerL->text().toInt();
    auto cooldown = voltCD->text().toInt();
    mLoopActive.store(true);
    loopWidget->clearData();

    mFuture = std::async(std::launch::async, [this, start, end, step, time, cooldown] {
        for(int volt = start; volt <= end && mLoopActive.load(); volt += step) {
            mVoltContr->setVoltage("signal", volt);
            std::this_thread::sleep_for(seconds(cooldown));
            mExpoContr->launchFreq(100);
            std::this_thread::sleep_for(seconds(time));
            mExpoContr->stopFreq();
            mExpoContr->freq();
        }
        mLoopActive.store(false);
        mState = State::None;
    });
}
