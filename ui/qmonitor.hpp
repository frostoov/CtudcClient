#pragma once

#include "controllers/expocontroller.hpp"
#include "views/expoview.hpp"
#include "qchambermonitor.hpp"


#include <qcustomplot.h>

#include <QSplitter>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QTabWidget>


class QMonitor : public QSplitter {
	Q_OBJECT
public:
	QMonitor(std::shared_ptr<ExpoController> expoContr,
			 ExpoView* expoView,
			 QWidget *parent = nullptr);
private:
	void setupGUI();
	QCustomPlot* makePlot();
	void createConnections();
	void updateGraph(QCPGraph& graph, int val);
	ChamberFreq convertCount(const ChamberFreq& count, const ChamberFreq& prev, int sec);
	TrekFreq convertCount(const TrekFreq& count, const TrekFreq& prev, int sec);
private:
	std::shared_ptr<ExpoController> mExpoContr;
	std::unique_ptr<uintmax_t> mTriggerCount[2];
	std::unique_ptr<uintmax_t> mPackageCount[2];
	ExpoView* mExpoView;
	QCustomPlot* mPlot;
	std::array<QChamberMonitor*, 16> mChambers;

	std::unique_ptr<TrekFreq> mChambersCount[2];

	QPushButton* mToggle;
	QLineEdit* mTick;
	QLineEdit* mCurrentRun;
	QLineEdit* mType;
	QLineEdit* mTriggerLine;
	QLineEdit* mPackageLine;
	QTimer* mTimer;
};
