#include "qcontrol.hpp"
#include "utils.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>

#include <stdexcept>


using std::exception;
using std::string;

QControlWidget::QControlWidget(std::shared_ptr<TdcController> tdc, QWidget* parent)
	: QTableWidget(parent), mController(tdc), mControl(0) {
	setRowCount(11);
	setColumnCount(2);
	createItems();

	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	horizontalHeader()->hide();
	verticalHeader()->hide();

	connect(this, &QTableWidget::itemChanged,
			this, &QControlWidget::setCtrl);
}

void QControlWidget::refresh() {
	getCtrl();
}

void QControlWidget::setCtrl() {
	getValues();
	doAction("TDC set ctrl", [&] {
		mController->setCtrl(mControl);
	});
}

void QControlWidget::getCtrl() {
	doAction("TDC set ctrl", [&] {
		mControl = mController->ctrl();
		setValues();
	});
}

void QControlWidget::setValues() {
	for (short i = 0; i < 10; ++i)
		mItems[i][1]->setText(QString::number(mControl >> i & 1));
	mItems[10][1]->setText(QString::number((mControl >> 12) & 1));
}

void QControlWidget::getValues() {
	mControl = 0;
	for (auto i = 0; i < 10; ++i) {
		auto item = mItems[i][1]->text().toInt() != 0 ? 1 : 0;
		if (i != 10)
			mControl |= item << i;
		else
			mControl |= item << 12;
	}
}

void QControlWidget::createItems() {
	mItems[0][0] = new QTableWidgetItem("Bus ERR");
	mItems[1][0] = new QTableWidgetItem("TERM");
	mItems[2][0] = new QTableWidgetItem("TERM SW");
	mItems[3][0] = new QTableWidgetItem("EMPTY EVENT");
	mItems[4][0] = new QTableWidgetItem("ALIGN 64");
	mItems[5][0] = new QTableWidgetItem("COMPENSATION EN");
	mItems[6][0] = new QTableWidgetItem("TEST FIFO EN");
	mItems[7][0] = new QTableWidgetItem("READ COMP SRAM EN");
	mItems[8][0] = new QTableWidgetItem("EVENT FIFO EN");
	mItems[9][0] = new QTableWidgetItem("ETTG EN");
	mItems[10][0] = new QTableWidgetItem("16MB ADDR RANGE MEB ACCESS EN");
	for (short i = 0; i < 11; ++i) {
		mItems[i][1] = new QTableWidgetItem("0");
		mItems[i][0]->setFlags(Qt::ItemIsEnabled);
		mItems[i][1]->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
		for (short k = 0; k < 2; ++k)
			setItem(i, k, mItems[i][k]);
	}
}
