#pragma once

#include "controllers/voltagecontroller.hpp"

#include <qcustomplot.h>

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QSplitter>
#include <QTimer>

class QVoltageWidget : public QSplitter {
	Q_OBJECT
public:
	explicit QVoltageWidget(std::shared_ptr<VoltageController> controller, QWidget *parent = nullptr);
protected:
	void setupGUI();
	void setupPlot(QCustomPlot& plot, const QString& name);
protected slots:
	void setDriftVoltage();
	void setSignalVoltage();
	void setDriftSpeedUp();
	void setDriftSpeedDn();
	void setSignalSpeedUp();
	void setSignalSpeedDn();
	void setDriftState(bool flag);
	void setSignalState(bool flag);
	void updateValues();
	void updatePlot();
private:
	std::shared_ptr<VoltageController> mController;

	QLineEdit* mDriftVolt;
	QLineEdit* mDriftSpeedUp;
	QLineEdit* mDriftSpeedDn;
	QCheckBox* mDriftState;
	QCustomPlot* mDriftPlot;

	QLineEdit* mSignalVolt;
	QLineEdit* mSignalSpeedUp;
	QLineEdit* mSignalSpeedDn;
	QCheckBox* mSignalState;
	QCustomPlot* mSignalPlot;

	QPushButton* mOpen;
	QPushButton* mClose;
	QPushButton* mToggle;
	QPushButton* mUpdate;
	QLabel* mIsOpen;
	int mFreq;

	QTimer* timer;
};
