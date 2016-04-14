#pragma once

#include "views/expoview.hpp"

#include <qcustomplot.h>

#include <QSplitter>
#include <QLineEdit>
#include <QTableWidget>

class QChamberMonitor : public QSplitter {
	Q_OBJECT
public:
	QChamberMonitor(const QString& name, QWidget* parent = nullptr);
	void addFreq(const ExpoView::ChamberFreq& freq);
	void setCount(const ExpoView::ChamberFreq& count);
	void reset();
	void setTick(int tick);
protected:
	void mouseDoubleClickEvent(QMouseEvent*) override;
	void setupGUI();
	void updateGraph(QCPGraph& graph, double val);
	QCustomPlot* setupPlot();
private:
	QCustomPlot* mPlot;
	QString mName;
	int mTick;

	QLineEdit* mWireCount[4];
signals:
	void mouseDoubleClick();
};