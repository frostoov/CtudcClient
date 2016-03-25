#pragma once

#include "net/ctudcconn.hpp"
#include "net/ctudcclient.hpp"
#include "controllers/tdccontroller.hpp"
#include "controllers/voltagecontroller.hpp"
#include "controllers/expocontroller.hpp"
#include "views/voltageview.hpp"
#include "views/expoview.hpp"
#include "views/tdcview.hpp"

#include "qfrequency.hpp"
#include "qstatus.hpp"
#include "qcontrol.hpp"
#include "qsettings.hpp"
#include "qconsole.hpp"
#include "qvoltage.hpp"

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include <memory>

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(std::shared_ptr<CtudcConn> conn, QWidget* parent = nullptr);
	~MainWindow();
protected:
	void setupGUI();
private:
	std::shared_ptr<CtudcConn> mConn;
	std::shared_ptr<TdcController> mTdcContr;
	std::shared_ptr<VoltageController> mVoltContr;
	std::shared_ptr<ExpoController> mExpoContr;

	CtudcClient mClient;
	TdcView* mTdcView;
	VoltageView* mVoltView;
	ExpoView* mExpoView;
private:
	QStatusWidget*    mStatus;
	QControlWidget*   mControl;
	QSettingsWidget*  mSettings;
	QFrequencyWidget* mFrequency;
	QVoltageWidget* mVoltage;

	QPushButton* mRefresh;
	QPushButton* mOpen;
	QPushButton* mStart;
	QPushButton* mReset;
	QPushButton* mClear;

	QPushButton* freqB;
	QPushButton* voltB;
};
