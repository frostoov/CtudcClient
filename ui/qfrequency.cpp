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
using std::exception;

QFrequencyWidget::QFrequencyWidget(shared_ptr<ExpoController> expoContr,
                                   shared_ptr<VoltageController> voltContr,
                                   const QChamberTable::Config& tableConf,
                                   QWidget* parent)
    : QWidget(parent),
      mExpoContr(expoContr),
      mVoltContr(voltContr),
      mState(State::None) {
    qRegisterMetaType<TrekFreq>("TrekFreq");
    createWidgets(tableConf);
    createLayouts();
    packWidgets();

    connect(launchFreq, &QPushButton::clicked, this, [this] {
        try {
            if(launchFreq->text() == "Start freq") {
                mExpoContr->launchFreq(100);
                mState = State::Freq;
            } else if(launchFreq->text() == "Stop freq") {
                mExpoContr->stopFreq();
                mTable->setTrekFreq(mExpoContr->freq());
                mTable->update();
                mState = State::None;
            }
        } catch(exception& e) {
            QMessageBox::warning(this, "Frequency", e.what());
        }
    });
    connect(startLoop, &QPushButton::clicked, this, [this] {
        try {
            if(startLoop->text() == "Start loop") {
                launchLoop();
                mState = State::Loop;
            } else if(startLoop->text() == "Stop loop") {
                mLoopActive.store(false);
            }
        } catch(exception& e) {
            QMessageBox::warning(this, "Frequency loop", e.what());
        }
    });

    connect(this, &QFrequencyWidget::loopFinished, this, [this] {
        if(mState != State::Loop)
            throw std::logic_error("loopFinished mState != State::Loop");
        mLoopActive.store(false);
        mState = State::None;
        try {
            auto type = mExpoContr->type();
            handleExpoType(QString::fromStdString(type));
        } catch(...) {
            //TODO
        }
    });

    //View connections
    connect(mExpoContr.get(), &ExpoController::typeChanged,
            this, &QFrequencyWidget::handleExpoType);
    connect(this, &QFrequencyWidget::voltFreqReady, this, [this](auto volt, auto freq) {
        loopWidget->addFreq(volt, freq);
        loopWidget->updateData();
    });

    resize(800, 600);
}

QFrequencyWidget::~QFrequencyWidget() {
    //TODO
}

void QFrequencyWidget::handleExpoType(QString type) {
    if(type == "idle") {
        if(mState == State::None) {
            launchFreq->setText("Start freq");
            startLoop->setText("Start loop");
        }
    } else if(type == "freq") {
        if(mState == State::Freq)
            launchFreq->setText("Stop freq");
        if(mState == State::Loop)
            startLoop->setText("Stop loop");
    }
    launchFreq->setEnabled((mState == State::Freq || mState == State::None) && (type == "idle" || type == "freq"));
    startLoop->setEnabled((mState == State::Loop || mState == State::None) && (type == "idle" || type == "freq"));

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

void QFrequencyWidget::createWidgets(const QChamberTable::Config& tableConf) {
    tabWidget = new QTabWidget(this);
    mTable = new QChamberTable(16, tableConf);
    loopWidget = new QLoopFreqWidget(tableConf, this);
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

    auto type = mExpoContr->type();
    if(type != "idle")
        throw std::runtime_error("launchLoop expo is not idle");
    mFuture = std::async(std::launch::async, [this, start, end, step, time, cooldown] {
        try {
            for(int volt = start; volt <= end && mLoopActive.load(); volt += step) {
                mVoltContr->setVoltage("signal", volt);
                std::this_thread::sleep_for(seconds(cooldown));
                mExpoContr->launchFreq(100);
                std::this_thread::sleep_for(seconds(time));
                mExpoContr->stopFreq();
                emit voltFreqReady(volt, mExpoContr->freq());
            }
        } catch(exception& e) {
            std::cerr << "freq loop: " << e.what() << std::endl;
        }
        emit loopFinished();
    });
}
