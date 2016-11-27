#pragma once

#include "controllers/tdccontroller.hpp"
#include "controllers/voltagecontroller.hpp"
#include "controllers/expocontroller.hpp"

#include "qfrequency.hpp"
#include "qstatus.hpp"
#include "qcontrol.hpp"
#include "qsettings.hpp"
#include "qconsole.hpp"
#include "qvoltage.hpp"
#include "qmonitor.hpp"



#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include <memory>


namespace trek {
namespace net {
class MulticastReceiver;
}
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(std::shared_ptr<CtudcConn> conn,
               std::shared_ptr<trek::net::MulticastReceiver> recv,
               const QChamberTable::Config& tableConf,
               QWidget* parent = nullptr);
    ~MainWindow();
protected:
    void setupGUI();
private:
    std::shared_ptr<CtudcConn> mConn;
    std::shared_ptr<trek::net::MulticastReceiver> mRecv;
    
    std::shared_ptr<TdcController> mTdcContr;
    std::shared_ptr<VoltageController> mVoltContr;
    std::shared_ptr<ExpoController> mExpoContr;

    std::thread mRecvThread;
private:
    QStatusWidget*    mStatus;
    QControlWidget*   mControl;
    QSettingsWidget*  mSettings;
    QFrequencyWidget* mFrequency;
    QVoltageWidget* mVoltage;
    QMonitor* mMonitor;

    QPushButton* mRefresh;
    QPushButton* mOpen;
    QPushButton* mStart;
    QPushButton* mReset;
    QPushButton* mClear;

    QPushButton* freqB;
    QPushButton* voltB;
    QPushButton* monitB;
signals:
    void serverFail(QString obj, QString method, QString status);
};
