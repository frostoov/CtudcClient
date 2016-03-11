#pragma once

#include "net/ctudcconn.hpp"

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
public slots:
	void refresh();
	void startExpo();
	void stopExpo();
protected:
	void setupGUI();
private:
	std::shared_ptr<CtudcConn> mConn;
	std::shared_ptr<TdcController> mTdcContr;
	std::shared_ptr<VoltageController> mVoltContr;
	std::shared_ptr<ExpoController> mExpoContr;
private:
	QStatusWidget*    mStatus;
	QControlWidget*   mControl;
	QSettingsWidget*  mSettings;
	QFrequencyWidget* mFrequency;
	QVoltageWidget* mVoltage;

	QPushButton* mRefresh;
	QLabel*      mState;
	QPushButton* mOpen;
	QPushButton* mClose;
	QPushButton* mReset;
	QPushButton* mClear;
	QPushButton* mStartExpo;
	QPushButton* mStopExpo;
	QPushButton* freqB;
	QPushButton* voltB;

	inline void createOutput();
	inline void readEvents();
};
