#include <QClipboard>
#include <set>
#include <stdexcept>
#include <QMessageBox>

#include "qfrequency.hpp"
#include "qconsole.hpp"


#include <thread>

using std::this_thread::get_id;
using std::chrono::seconds;
using std::set;
using std::string;
using std::exception;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::shared_ptr;

QFrequencyWidget::QFrequencyWidget(shared_ptr<ExpoController> expoContr,
								   shared_ptr<VoltageController> voltContr,
								   ExpoView* expoView,
								   VoltageView* voltView,
								   QWidget* parent)
	: QWidget(parent),
	  mExpoContr(expoContr),
	  mVoltContr(voltContr),
	  mExpoView(expoView),
	  mVoltView(voltView),
	  mState(State::None) {
	qRegisterMetaType<ExpoView::TrekFreq>("ExpoView::TrekFreq");
	createWidgets();
	createLayouts();
	packWidgets();
	setupTable();

	connect(startFreq, &QPushButton::clicked, this, [this]{
		if(startFreq->text() == "Start freq") {
			mState = State::Freq;
			mExpoContr->startFreq(100);
		} else if(startFreq->text() == "Stop freq") {
			disconnect(mFreqConn);
			mFreqConn = connect(mExpoView, &ExpoView::freq, this, [this](auto status, auto freq) {
				if(!status.isEmpty())
					QMessageBox::critical(this, "Expo get freq", status);
				else
					fillFreqTable(freq);
				disconnect(mFreqConn);
			});
			mExpoContr->stopFreq();
			mExpoContr->freq();
			mState = State::None;
		}
	});
	connect(startLoop, &QPushButton::clicked, this, [this]{
		if(startLoop->text() == "Start loop") {
			mState = State::Loop;
			launchLoop();
			disconnect(mFreqConn);
			mFreqConn = connect(mExpoView, &ExpoView::freq, this, [this](auto status, auto freq) {
				if(!status.isEmpty())
					QMessageBox::critical(this, "Expo get freq", status);
				else {
					auto code = startVolt->text().toInt();
					auto step = stepVolt->text().toInt();
					startVolt->setText(QString::number(code + step));
					loopWidget->addFreq(code, freq);
					loopWidget->updateData();
				}
			});
		} else if(startLoop->text() == "Stop loop") {
			mLoopActive.store(false);
		}
	});

	//View connections
	connect(mExpoView, &ExpoView::type, this, [this](auto status, auto type) {
		if(!status.isEmpty())
			QMessageBox::critical(this, "Expo get type", status);
		else {
			if(type == "none") {
				startFreq->setText("Start freq");
				startLoop->setText("Start loop");
			} else if(type == "freq") {
				if(mState == State::Freq) startFreq->setText("Stop freq");
				if(mState == State::Loop) startLoop->setText("Stop loop");
			}
			startFreq->setEnabled((mState == State::Freq || mState == State::None) && (type == "none" || type == "freq"));
			startLoop->setEnabled((mState == State::Loop || mState == State::None) && (type == "none" || type == "freq"));
		}
	});

	connect(mExpoView, &ExpoView::startFreq, this, [this](auto status) {
		if(!status.isEmpty())
			QMessageBox::critical(this, "Expo start freq", status);
	});
	connect(mExpoView, &ExpoView::stopFreq, this, [this](auto status) {
		if(!status.isEmpty())
			QMessageBox::critical(this, "Expo stop freq", status);
	});

	resize(800, 600);
}

QFrequencyWidget::~QFrequencyWidget() {
	//TODO
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
	subLayout->addWidget(startFreq);
	subLayout->addWidget(loopGroup);
	loopLayout->addRow(startLoop);
	loopLayout->addRow("Timer", timerL);
	loopLayout->addRow("Start Code", startVolt);
	loopLayout->addRow("End Code", endVolt);
	loopLayout->addRow("Step Code", stepVolt);
	loopGroup->setLayout(loopLayout);
}

void QFrequencyWidget::createWidgets() {
	tabWidget = new QTabWidget(this);
	table = new QTableWidget(33, 5, this);
	loopWidget = new QLoopFreqWidget(this);
	startFreq = new QPushButton("Start freq", this);
	startLoop = new QPushButton("Start loop", this);
	timerL = new QLineEdit("10", this);
	startVolt = new QLineEdit("0", this);
	endVolt = new QLineEdit("100", this);
	stepVolt = new QLineEdit("1", this);

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

void QFrequencyWidget::fillFreqTable(const ExpoView::TrekFreq& freq) {
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


void QFrequencyWidget::launchLoop() {
	auto start = startVolt->text().toInt();
	auto end = endVolt->text().toInt();
	auto step = stepVolt->text().toInt();
	auto time = timerL->text().toInt();
	mLoopActive.store(true);
	loopWidget->resetData();

	mFuture = std::async(std::launch::async,[this, start, end, step, time] {
		for(int volt = start; volt <= end && mLoopActive.load(); volt += step) {
			mVoltContr->setVoltage("signal", volt);
			mExpoContr->startFreq(100);
			std::this_thread::sleep_for(seconds(time));
			mExpoContr->stopFreq();
			mExpoContr->freq();
		}
		mLoopActive.store(false);
		mState = State::None;
	});
}
