#include <QClipboard>
#include <set>
#include <stdexcept>
#include <QMessageBox>

#include "qfrequency.hpp"
#include "qconsole.hpp"
#include "utils.hpp"

#include <thread>

using std::this_thread::get_id;
using std::chrono::seconds;
using std::set;
using std::string;
using std::exception;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::shared_ptr;

QFrequencyWidget::QFrequencyWidget(shared_ptr<ExpoController> expo,
								   shared_ptr<VoltageController> voltage,
								   QWidget* parent)
	: QWidget(parent),
	  mExpo(expo),
	  mVoltage(voltage) {
	createWidgets();
	createLayouts();
	packWidgets();
	setupTable();

	qRegisterMetaType<ExpoController::TrekFreq>("ExpoController::TrekFreq");
	connect(launchFreqB, &QPushButton::clicked,
			this, &QFrequencyWidget::launchFreq);
	connect(stopFreqB, &QPushButton::clicked,
			this, &QFrequencyWidget::stopFreq);
	connect(launchLoopB, &QPushButton::clicked,
			this, &QFrequencyWidget::launchLoop);
	connect(stopLoopB, &QPushButton::clicked,
			this, &QFrequencyWidget::stopLoop);

	connect(this, &QFrequencyWidget::freqReady,
			this, &QFrequencyWidget::updateFreq);
	connect(this, &QFrequencyWidget::loopDone,
			this, &QFrequencyWidget::flushLoop);

	resize(800, 600);
}

QFrequencyWidget::~QFrequencyWidget() {
	delete table;
	delete launchFreqB;
	delete timerL;
	delete loopWidget;
	delete subLayout;
	delete mainLayout;
}

void QFrequencyWidget::createLayouts() {
	mainLayout = new QHBoxLayout;
	subLayout = new QVBoxLayout;
	loopLayout = new QFormLayout;
	setLayout(mainLayout);
}
void QFrequencyWidget::packWidgets() {
	mainLayout->addWidget(tabWidget, 4);
	mainLayout->addLayout(subLayout, 1);
	subLayout->setAlignment(Qt::AlignTop);
	subLayout->addWidget(launchFreqB);
	subLayout->addWidget(stopFreqB);
	subLayout->addWidget(loopGroup);
	loopLayout->addRow(launchLoopB);
	loopLayout->addRow(stopLoopB);
	loopLayout->addRow("Timer", timerL);
	loopLayout->addRow("Start Code", startVolt);
	loopLayout->addRow("End Code", endVolt);
	loopLayout->addRow("Step Code", stepVolt);
	loopGroup->setLayout(loopLayout);
}

void QFrequencyWidget::createWidgets() {
	tabWidget		= new QTabWidget(this);
	table			= new QTableWidget(33, 5, this);
	loopWidget		= new QLoopFreqWidget(this);
	launchFreqB	= new QPushButton("Start Measure", this);
	stopFreqB   = new QPushButton("Stop Measure", this);
	launchLoopB	= new QPushButton("Strat Loop", this);
	stopLoopB   = new QPushButton("Stop Loop", this);
	timerL			= new QLineEdit("10", this);
	startVolt		= new QLineEdit("0", this);
	endVolt			= new QLineEdit("100", this);
	stepVolt		= new QLineEdit("1", this);

	loopGroup = new QGroupBox("Measure Loop", this);
	tabWidget->addTab(table, "Single");
	tabWidget->addTab(loopWidget, "Loop");
}

void QFrequencyWidget::setupTable() {
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->horizontalHeader()->hide();
	table->verticalHeader()->hide();
	createItems();
}

void QFrequencyWidget::createRootItems() {
	QTableWidgetItem* root[5]{
		new QTableWidgetItem("Chamber/Wire"),
		new QTableWidgetItem("Wire 1"),
		new QTableWidgetItem("Wire 2"),
		new QTableWidgetItem("Wire 3"),
		new QTableWidgetItem("Wire 4"),
	};
	for (short i = 0; i < 5; ++i) {
		table->setItem(0, i, root[i]);
	}
}

void QFrequencyWidget::createItems() {
	createRootItems();
	for (int i = 0; i < 32; ++i) {
		QTableWidgetItem* items[5]{
			new QTableWidgetItem("Chamber"),
			new QTableWidgetItem("-"),
			new QTableWidgetItem("-"),
			new QTableWidgetItem("-"),
			new QTableWidgetItem("-"),
		};
		for (int j = 0; j < 5; ++j)
			table->setItem(i + 1, j, items[j]);
	}
	for (int i = 0; i < 33; ++i)
		for (int j = 0; j < 5; ++j)
			if (table->item(i, j) != nullptr)
				table->item(i, j)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void QFrequencyWidget::setFreq(const ExpoController::TrekFreq& freq) {
	set<uintmax_t> triggChambers;
	for(const auto& chamFreq : freq)
		triggChambers.insert(chamFreq.first);
	table->setRowCount(freq.size() + 1);
	int row = 1;
	for(const auto& cham : triggChambers) {
		QTableWidgetItem* items[5]{
			new QTableWidgetItem(QString("Chamber №%1").arg(cham + 1)),
					new QTableWidgetItem(QString::number(freq.at(cham).at(0) )),
					new QTableWidgetItem(QString::number(freq.at(cham).at(1) )),
					new QTableWidgetItem(QString::number(freq.at(cham).at(2) )),
					new QTableWidgetItem(QString::number(freq.at(cham).at(3) )),
		};
		for (short j = 0; j < 5; ++j) {
			table->setItem(row, j, items[j]);
			table->item(row, j)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		}
		++row;
	}

	table->update();
}

void QFrequencyWidget::keyPressEvent(QKeyEvent* evt) {
	if (evt->key() == Qt::Key_C && (evt->modifiers() & Qt::ControlModifier)) {
		auto ranges = table->selectedRanges();
		if (ranges.empty())
			return;
		QString text;
		for (const auto& range : ranges)
			for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
				for (int col = range.leftColumn(); col <= range.rightColumn(); ++col) {
					if (table->item(row, col))
						text.append(table->item(row, col)->text());
					if (col != range.rightColumn())
						text.append("\t");
				}
				if (row != range.bottomRow())
					text.append("\n");
			}
		QApplication::clipboard()->setText(text);
	} else
		QWidget::keyPressEvent(evt);
}

void QFrequencyWidget::launchFreq() {
	std::string type;
	try {
		type = mExpo->type();
	} catch(exception& e) {
		console->printWithTime(QString("Expo get type failed ") + e.what());
		return;
	}

	if(type != "none") return;
	doAction("Expo start freq", [this] {
		mExpo->startFreq(100);
	});
}

void QFrequencyWidget::stopFreq() {
	string type;
	try {
		type = mExpo->type();
	} catch(exception& e) {
		console->printWithTime(QString("Expo get type failed ") + e.what());
		return;
	}
	if(type != "freq") return;
	doAction("Expo stop freq", [this] {
		mExpo->stopFreq();
		setFreq(mExpo->freq());
	});
}

void QFrequencyWidget::launchLoop() {
	if(mFuture.valid()) return;
	string type;
	try {
		type = mExpo->type();
	} catch(exception& e) {
		console->printWithTime(QString("Expo get type failed ") + e.what());
		return;
	}
	if(type != "none") return;

	auto start = startVolt->text().toInt();
	auto end = endVolt->text().toInt();
	auto step = stepVolt->text().toInt();
	auto time = timerL->text().toInt();
	mLoopActive.store(true);
	loopWidget->resetData();

	mFuture = std::async(std::launch::async,[this, start, end, step, time] {
		try {
			for(int volt = start; volt <= end && mLoopActive.load(); volt += step) {
				std::cout << "start freq" << std::endl;
				mVoltage->setVoltage("signal", volt);
				mExpo->startFreq(100);
				std::this_thread::sleep_for(seconds(time));
				mExpo->stopFreq();
				std::cout << "emit freqReady" << std::endl;
				emit freqReady(volt, mExpo->freq());
			}
			emit loopDone("success");
		} catch(exception& e) {
			emit loopDone(QString("failed ") + e.what() );
		}
		std::cout << "loop doneReady" << std::endl;
		mLoopActive.store(false);
	});
}

void QFrequencyWidget::stopLoop() {
	mLoopActive.store(false);
}

void QFrequencyWidget::updateFreq(int volt, ExpoController::TrekFreq freq) {
	std::cout << "updateFreq" << std::endl;
	loopWidget->addFreq(volt, freq);
	loopWidget->updateData();
}

void QFrequencyWidget::flushLoop(QString msg) {
	std::cout << "flush loop" << std::endl;
	mFuture.get();
	std::cout << "flush loop get" << std::endl;
	console->printWithTime("Freq loop " + msg);
}
