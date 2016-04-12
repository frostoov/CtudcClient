#pragma once

#include "qchambertable.hpp"
#include "controllers/expocontroller.hpp"
#include "controllers/voltagecontroller.hpp"
#include "views/expoview.hpp"
#include "views/voltageview.hpp"

#include <thread>
#include <future>

#include <QBoxLayout>
#include <QFormLayout>
#include <QTableView>
#include <QPushButton>
#include <QHeaderView>
#include <QLineEdit>
#include <QKeyEvent>
#include <QGroupBox>
#include <QTabWidget>

#include "qloopfreq.hpp"

class QFrequencyWidget : public QWidget {
    using ChamberItems = std::array<QTableWidgetItem*, 5>;
    using TrekItems    = std::vector<ChamberItems>;
    enum class State {
        None,
        Freq,
        Loop,
    };
    Q_OBJECT
public:
    QFrequencyWidget(std::shared_ptr<ExpoController> expoContr,
                     std::shared_ptr<VoltageController> voltContr,
                     ExpoView* expoView,
                     VoltageView* voltView,
                     QWidget* parent = nullptr);
    ~QFrequencyWidget();
protected:
    void launchLoop();
private:
    void createLayouts();
    void createWidgets();
    void packWidgets();
private:
    std::shared_ptr<ExpoController> mExpoContr;
    std::shared_ptr<VoltageController> mVoltContr;
    ExpoView* mExpoView;
    VoltageView* mVoltView;

    State mState;
    std::future<void> mFuture;
    std::atomic<bool> mLoopActive;
private:
    QChamberTable* mTable;

    QLoopFreqWidget* loopWidget;

    QPushButton* launchFreq;
    QPushButton* startLoop;

    QLineEdit* startVolt;
    QLineEdit* endVolt;
    QLineEdit* stepVolt;
    QLineEdit* timerL;
    QLineEdit* voltCD;

    QTabWidget* tabWidget;
    QHBoxLayout* mainLayout;
    QVBoxLayout* subLayout;
    QGroupBox*   loopGroup;
    QFormLayout* loopLayout;

    QMetaObject::Connection mFreqConn;
};
