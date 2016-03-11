#pragma once

#include "controllers/expocontroller.hpp"
#include "controllers/voltagecontroller.hpp"

#include <thread>
#include <future>

#include <QBoxLayout>
#include <QFormLayout>
#include <QTableView>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QKeyEvent>
#include <QGroupBox>
#include <QTabWidget>

#include "qloopfreq.hpp"

class QFrequencyWidget : public QWidget {
	using ChamberItems = std::array<QTableWidgetItem*, 5>;
	using TrekItems    = std::vector<ChamberItems>;
	Q_OBJECT
public:
	QFrequencyWidget(std::shared_ptr<ExpoController> expo,
					 std::shared_ptr<VoltageController> voltage,
					 QWidget* parent = nullptr);
	~QFrequencyWidget();
protected:
	void setFreq(const ExpoController::TrekFreq& freq);
	void keyPressEvent(QKeyEvent* evt) override;
private:
	void createLayouts();
	void createWidgets();
	void packWidgets();
	void setupTable();
	void createItems();
	void createRootItems();
private:
	std::shared_ptr<ExpoController> mExpo;
	std::shared_ptr<VoltageController> mVoltage;

	std::future<void> mFuture;
	std::atomic<bool> mLoopActive;
private:
	QTableWidget* table;

	QLoopFreqWidget* loopWidget;

	QPushButton* launchFreqB;
	QPushButton* stopFreqB;

	QPushButton* launchLoopB;
	QPushButton* stopLoopB;
	QLineEdit* startVolt;
	QLineEdit* endVolt;
	QLineEdit* stepVolt;
	QLineEdit* timerL;

	QTabWidget* tabWidget;
	QHBoxLayout* mainLayout;
	QVBoxLayout* subLayout;
	QGroupBox*   loopGroup;
	QFormLayout* loopLayout;
protected slots:
	void launchFreq();
	void stopFreq();

	void launchLoop();
	void stopLoop();

	void updateFreq(int volt, ExpoController::TrekFreq freq);
	void flushLoop(QString msg);
signals:
	void freqReady(int volt, ExpoController::TrekFreq freq);
	void loopDone(QString msg);
};
