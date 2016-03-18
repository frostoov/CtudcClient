#pragma once

#include "controllers/expocontroller.hpp"

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QClipboard>
#include <QTabWidget>
#include <qcustomplot.h>

#include <cstdint>
#include <vector>
#include <unordered_set>

class QLoopFreqWidget : public QWidget {
	using ChamberFreq    = ExpoController::ChamberFreq;
	using TrekFreq       = ExpoController::TrekFreq;
	using ChamCodeFreq   = std::pair<int, ChamberFreq>;
	using ChamFreqSeries = std::vector<ChamCodeFreq>;
	using TrekFreqSeries = std::unordered_map<uintmax_t, ChamFreqSeries>;

	Q_OBJECT
public:
	explicit QLoopFreqWidget(QWidget* parent = 0);
	void addFreq(int volt, const ExpoController::TrekFreq& freq);
	void updateData();
	void resetData();
protected:
	virtual void keyPressEvent(QKeyEvent* evt);
	void printChamberFreq(QTextStream& stream, const ChamFreqSeries& chamFreq);
	void fillList();
	void fillPlot(int chamNum);
	void fillTable(int chamNum);
	void setupPlot();
	void packWidgets();
	void createWidgets();
	void setupTable();
	void createItems();
	void createRootItems();
	void clearPlot();
private:
	QTabWidget*			tab;
	QTableWidget*		table;
	QCustomPlot*		plot;
	QCPPlotTitle*		plotTitle;
	QPushButton*		saveTableB;
	QPushButton*		savePlotB;
	QLabel*				codeOneL;
	QLabel*				chamberL;
	QListWidget*		list;
	QString				listItemSelTitle;

	QHBoxLayout*	mainLayout;
	QVBoxLayout*	subLayout;

	TrekFreqSeries	mFreqs;

protected slots:
	void setChamberData();
	void saveTable();
	void savePlot();
};
