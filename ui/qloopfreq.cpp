#include "qloopfreq.hpp"

#include <stdexcept>
#include <iostream>
#include <set>

using std::vector;
using std::set;
using std::make_unique;

QLoopFreqWidget::QLoopFreqWidget(QWidget* parent)
	: QWidget(parent) {
	createWidgets();
	packWidgets();
	setupTable();
	setupPlot();

	connect(saveTableB, &QPushButton::clicked,
			this,		&QLoopFreqWidget::saveTable);
	connect(savePlotB,	&QPushButton::clicked,
			this,		&QLoopFreqWidget::savePlot);
}

void QLoopFreqWidget::createWidgets() {
	tab			= new QTabWidget(this);
	table		= new QTableWidget(30, 5, this);
	plot		= new QCustomPlot(this);
	list		= new QListWidget(this);
	saveTableB  = new QPushButton("Save Table", this);
	savePlotB	= new QPushButton("Save Plot", this);
	codeOneL	= new QLabel("Code 0: 0", this);
	chamberL	= new QLabel("Chambers", this);
	tab->addTab(table, "Table");
	tab->addTab(plot, "Plot");

	connect(list,	&QListWidget::itemSelectionChanged,
			this,	&QLoopFreqWidget::setChamberData);
}

void QLoopFreqWidget::setupTable() {
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->horizontalHeader()->hide();
	table->verticalHeader()->hide();
	createItems();
}
void QLoopFreqWidget::createItems() {
	createRootItems();
	for(int i = 0; i < 30; ++i) {
		QTableWidgetItem *items[5] {
			new QTableWidgetItem("Volt"),
			new QTableWidgetItem("-"),
			new QTableWidgetItem("-"),
			new QTableWidgetItem("-"),
			new QTableWidgetItem("-")
		};
		for(int j = 0; j < 5; ++j)
			table->setItem(i + 1, j, items[j]);
	}
	for(short i = 0; i < 30; ++i)
		for(short j = 0; j < 5; ++j)
			if(table->item(i, j) != nullptr)
				table->item(i, j)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void QLoopFreqWidget::createRootItems() {
	QTableWidgetItem *root[5]{
		new QTableWidgetItem("Code/Wire"),
		new QTableWidgetItem("Wire 1"),
		new QTableWidgetItem("Wire 2"),
		new QTableWidgetItem("Wire 3"),
		new QTableWidgetItem("Wire 4"),
	};
	for(short i = 0; i < 5; ++i) {
		table->setItem(0, i, root[i]);
	}
}

void QLoopFreqWidget::packWidgets() {
	mainLayout = new QHBoxLayout;
	subLayout  = new QVBoxLayout;
	mainLayout->addLayout(subLayout, 1);
	mainLayout->addWidget(tab, 5);
	subLayout->addWidget(saveTableB);
	subLayout->addWidget(savePlotB);
	subLayout->addWidget(chamberL);
	subLayout->addWidget(list);
	subLayout->addWidget(codeOneL);
	setLayout(mainLayout);
}

void QLoopFreqWidget::addFreq(int volt, const ExpoController::TrekFreq& freq) {
	for(auto& chamFreqPair : freq) {
		auto  chamNum = chamFreqPair.first;
		auto& chamFreq = chamFreqPair.second;
		if(mFreqs.count(chamNum) == 0)
			mFreqs.emplace(chamNum, ChamFreqSeries());
		mFreqs.at(chamNum).emplace_back(volt, chamFreq);
	}
}

void QLoopFreqWidget::updateData() {
	fillList();
	setChamberData();
}

void QLoopFreqWidget::resetData() {
	mFreqs.clear();
	list->clear();
	for(int i = 1; i < table->rowCount(); ++i)
		table->removeRow(i);
	clearPlot();
}

void QLoopFreqWidget::fillList() {
	list->clear();
	set<int> chams;
	for(auto& chamFreqsPair : mFreqs)
		chams.insert(int(chamFreqsPair.first));
	for(auto& cham : chams)
		list->addItem(QString::number(cham + 1 ));
	for(int i = 0 ; i < list->count(); ++i)
		if(list->item(i)->text() == listItemSelTitle)
			list->setCurrentRow(i);
}

void QLoopFreqWidget::saveTable() {
	QString tableTitle = QFileDialog::getSaveFileName(this, "Save table", 0, "*.xlsx");
	if(tableTitle.isEmpty())
		return;

//	QXlsWriter writer;
//	writer.write(mFreqs, tableTitle);
}

void QLoopFreqWidget::savePlot() {
	QString dir = QFileDialog::getExistingDirectory(this, "Save plots");
	if(dir.isEmpty())
		return;
	for(auto& cham : mFreqs) {
		fillPlot(cham.first);
		plot->savePng(tr("chamber_%1.txt").arg(cham.first), 1000, 1000, 1, 100);
	}
	setChamberData();
}

void QLoopFreqWidget::keyPressEvent(QKeyEvent* evt) {
	if(evt->key() == Qt::Key_C && (evt->modifiers() & Qt::ControlModifier)) {
		auto ranges = table->selectedRanges();
		if(ranges.empty())
			return;
		QString text;
		for(const auto& range : ranges)
			for(int row = range.topRow(); row <= range.bottomRow(); ++row) {
				for(int col = range.leftColumn(); col <= range.rightColumn(); ++col) {
					if( table->item(row, col) )
						text.append(table->item(row, col)->text());
					if(col != range.rightColumn())
						text.append("\t");
				}
				if(row != range.bottomRow())
					text.append("\n");
			}
		QApplication::clipboard()->setText(text);
	} else
		QWidget::keyPressEvent(evt);
}

void QLoopFreqWidget::setupPlot() {
	plot->yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
	plot->yAxis->setTickStep(1);

	plot->setAutoAddPlottableToLegend(true);
	for(int i = 0 ; i < 4 ; ++i) {
		QColor color {
			255 * ( ((i + 1) >> 0) & 1 ),
			255 * ( ((i + 1) >> 1) & 1 ),
			255 * ( ((i + 1) >> 2) & 1 ),
		};
		QPen pen(color);
		pen.setWidth(2);
		pen.setStyle(Qt::DotLine);
		plot->addGraph();
		plot->graph(i)->setName(tr("Wire №%1").arg(i + 1));
		plot->graph(i)->setPen(pen);
		plot->graph(i)->setLineStyle(QCPGraph::lsLine);
		plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
	}

	plot->xAxis->setLabel("Voltage,codes");
	plot->yAxis->setLabel("Frequency,Hz");
	plot->legend->setVisible(true);
	plot->plotLayout()->insertRow(0);
	plotTitle = new QCPPlotTitle(plot, "Chamber");
	plot->plotLayout()->addElement(0, 0, plotTitle);
	plot->setAntialiasedElements(QCP::aeAll);
	plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight | Qt::AlignBottom);
	plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);
}

void QLoopFreqWidget::fillTable(int chamNum) {
	const auto& chamFreq = mFreqs.at(chamNum);
	table->setRowCount(chamFreq.size() + 1);

	int row = 1;
	for(auto& codeFreq : chamFreq) {
		std::cout << "setting volt in table: " << codeFreq.first << std::endl;
		QTableWidgetItem *items[5]{
			new QTableWidgetItem(QString::number(codeFreq.first)),
			new QTableWidgetItem(QString::number(codeFreq.second.at(0) )),
			new QTableWidgetItem(QString::number(codeFreq.second.at(1) )),
			new QTableWidgetItem(QString::number(codeFreq.second.at(2) )),
			new QTableWidgetItem(QString::number(codeFreq.second.at(3) ))
		};
		for(int j = 0; j < 5; ++j) {
			table->setItem(row , j, items[j]);
			table->item(row, j)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		}
		++row;
	}
	table->update();
}

void QLoopFreqWidget::fillPlot(int chamNum) {
	auto& chamFreq = mFreqs.at(chamNum);
	QVector<double> xAxis;
	QVector<double> yAxis;

	for(const auto& codeFreq : chamFreq)
		xAxis.push_back(codeFreq.first);
	int wire = 0;
	for(int i = 0; i < 4; ++i) {
		for(const auto& codeFreq : chamFreq)
			yAxis.push_back(codeFreq.second.at(i));
		plot->graph(wire++)->setData(xAxis, yAxis);
		yAxis.clear();
	}
	plotTitle->setText(tr("Chamber №%1").arg(chamNum + 1));

	plot->rescaleAxes();
	plot->replot();
}

void QLoopFreqWidget::setChamberData() {
	if(list->currentItem() == nullptr) {
		for(int i = 1; i < table->rowCount(); ++i)
			table->removeRow(i);
		clearPlot();
		return;
	}
	try  {
		listItemSelTitle = list->currentItem()->text();
		auto chamNum = listItemSelTitle.toInt();
		fillTable(chamNum);
		fillPlot(chamNum);
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void QLoopFreqWidget::clearPlot() {
	for(size_t i = 0; i < 4; ++i)
		plot->graph(i)->clearData();
}
