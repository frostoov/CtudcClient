#pragma once

#include "controllers/expocontroller.hpp"
#include "views/expoview.hpp"
#include "qchamber.hpp"


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
	ExpoView::ChamberFreq convertCount(const ExpoView::ChamberFreq& count, const ExpoView::ChamberFreq& prev, int sec);
	ExpoView::TrekFreq convertCount(const ExpoView::TrekFreq& count, const ExpoView::TrekFreq& prev, int sec);
private:
	std::shared_ptr<ExpoController> mExpoContr;
	std::unique_ptr<uintmax_t> mTriggerCount[2];
	std::unique_ptr<uintmax_t> mPackageCount[2];
	ExpoView* mExpoView;
	QCustomPlot* mPlot;
	std::array<QChamberMonitor*, 16> mChambers;

	std::unique_ptr<ExpoView::TrekFreq> mChambersCount[2];

	QPushButton* mToggle;
	QLineEdit* mTick;
	QLineEdit* mCurrentRun;
	QLineEdit* mType;
	QLineEdit* mTriggerLine;
	QLineEdit* mPackageLine;
	QTimer* mTimer;
};
