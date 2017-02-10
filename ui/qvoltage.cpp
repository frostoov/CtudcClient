#include "qvoltage.hpp"

#include "qplotsettings.hpp"

#include <QGroupBox>
#include <QFormLayout>
#include <QSplitter>

using std::shared_ptr;
using std::exception;

constexpr auto cellList = {"drift", "signal"};
constexpr int defaultTick = 10;

QVoltageWidget::QVoltageWidget(shared_ptr<VoltageController> controller,
                               QWidget* parent)
    : QSplitter(parent),
      mContr(controller),
      mTick(new QLineEdit(QString::number(defaultTick), this)) {
    setupGUI();
    for(auto& name : cellList)
        setupPlot(*mPlot[name], name);

    timer = new QTimer(this);
    timer->setInterval(defaultTick * 1000);
    timer->setSingleShot(false);

    createConnections();
    resize(800, 600);
}

void QVoltageWidget::createConnections() {
    //Controller connections
    for(auto& name : cellList) {
        connect(mVolt[name], &QLineEdit::returnPressed, this, [this, name] {
            try {
                mContr->setVoltage(name, mVolt[name]->text().toInt());
            } catch(exception& e) {
                QMessageBox::warning(this, "set voltage", e.what());
            }
        });
        connect(mSpeedUp[name], &QLineEdit::returnPressed, this, [this, name] {
            try {
                mContr->setSpeedUp(name, mSpeedUp[name]->text().toInt());
            } catch(exception& e) {
                QMessageBox::warning(this, "set speed up", e.what());
            }
        });
        connect(mSpeedDn[name], &QLineEdit::returnPressed, this, [this, name] {
            try {
                mContr->setSpeedDn(name, mSpeedDn[name]->text().toInt());
            } catch(exception& e) {
                QMessageBox::warning(this, "set speed down", e.what());
            }
        });
        connect(mState[name], &QCheckBox::clicked, this, [this, name](auto flag) {
            try {
                if(flag)
                    mContr->turnOn(name);
                else
                    mContr->turnOff(name);
            } catch(exception& e) {
                QMessageBox::warning(this, "change state", e.what());
            }
        });
    }

    connect(mToggle, &QPushButton::clicked, [&] {
        if(timer->isActive())
            timer->stop();
        else
            timer->start();
        mToggle->setText(timer->isActive() ? "Stop" : "Start");
    });
    connect(mUpdate, &QPushButton::clicked, this, [this](auto) {
        try {
            auto isOpen = mContr->isOpen();
            mOpen->setText(isOpen ? "Close" : "Open");
            if(!isOpen) return;
            for(auto& name : cellList) {
                this->updateCell(name);
                mSpeedUp[name]->setText(QString::number(mContr->speedUp(name)));
                mSpeedDn[name]->setText(QString::number(mContr->speedDn(name)));
                auto stat = mContr->stat(name);
                mStatus[name]->setText(QString::fromStdString(stat));
                mState[name]->setChecked(stat.gs());
            }
        } catch(exception& e) {
            QMessageBox::warning(this, "updated", e.what());
        }
    });
    connect(mOpen, &QPushButton::clicked, this, [this](auto) {
        try {
            if(mOpen->text() == "Open") {
                mContr->open();
                mOpen->setText("Close");
                mUpdate->click();
            } else if(mOpen->text() == "Close") {
                mContr->close();
                mOpen->setText("Open");
            }
        } catch(exception& e) {
            QMessageBox::warning(this, "open", e.what());
        }
    });
    connect(timer, &QTimer::timeout, this, [this] {
        for(auto& name : cellList)
            this->updateCell(name);
    });

    connect(mContr.get(), &VoltageController::speedUpChanged, this, [this](auto device, auto val) {
        mSpeedUp[device]->setText(QString::number(val));
    });
    connect(mContr.get(), &VoltageController::speedDnChanged, this, [this](auto device, auto val) {
        mSpeedDn[device]->setText(QString::number(val));
    });
    connect(mContr.get(), &VoltageController::statChanged, this, [this](auto device, auto stat) {
        mStatus[device]->setText(QString::fromStdString(stat));
        mState[device]->setChecked(stat.gs());
    });
    connect(mContr.get(), &VoltageController::stateChanged, this, [this](auto flag) {
        mOpen->setText(flag ? "Close" : "Open");
    });
    mUpdate->click();
}

void QVoltageWidget::setupGUI() {
    auto controlLayout = new QVBoxLayout;

    for(auto& name : cellList) {
        mVolt.insert(name, new QLineEdit("0"));
        mStatus.insert(name, new QLineEdit("0"));
        mStatus[name]->setReadOnly(true);
        mSpeedUp.insert(name, new  QLineEdit("0"));
        mSpeedDn.insert(name, new QLineEdit("0"));
        mState.insert(name, new QCheckBox);

        auto layout = new QFormLayout;
        layout->addRow("Voltage", mVolt[name]);
        layout->addRow("Status", mStatus[name]);
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

    connect(mTick, &QLineEdit::editingFinished, [this] {
        auto freq = mTick->text().toInt();
        if(freq <= 0)
            freq = 1;
        mTick->setText(QString::number(freq));
        timer->setInterval(1000 * freq);
    });

    auto actionLayout = new QFormLayout;
    actionLayout->addRow(mOpen);
    actionLayout->addRow(mToggle);
    actionLayout->addRow(mUpdate);
    actionLayout->addRow("Frequency", mTick);
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
    connect(&plot, &QCustomPlot::mouseDoubleClick, [&plot] {
        auto settings = new QPlotSettings(&plot);
        settings->show();
    });
    plot.xAxis->setAutoTicks(true);
    plot.xAxis->setAutoTickCount(5);
    plot.xAxis->setAutoTickLabels(true);
    plot.xAxis->setAutoTickStep(true);
    
    plot.xAxis->setTickLabelType(QCPAxis::ltDateTime);
    plot.xAxis->setDateTimeFormat("hh:mm:ss");
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
    auto key = double(QDateTime::currentMSecsSinceEpoch()) / 1000;
    graph.addData(key, val);
    graph.removeDataBefore(key - 50 * mTick->text().toInt());
}

void QVoltageWidget::updateCell(const std::string& type) {
    auto voltage = mContr->voltage(type);
    auto amperage = mContr->amperage(type);
    {
        QCustomPlot& plot = *mPlot[QString::fromStdString(type)];
        updateGraph(*plot.graph(0), voltage);
        plot.xAxis->rescale();
        plot.xAxis2->rescale();
        plot.replot();
    }
    {
        QCustomPlot& plot = *mPlot[QString::fromStdString(type)];
        updateGraph(*plot.graph(1), amperage);
        plot.xAxis->rescale();
        plot.xAxis2->rescale();
        plot.replot();
    }
}
