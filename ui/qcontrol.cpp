#include "qcontrol.hpp"


#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

#include <stdexcept>

using std::shared_ptr;
using std::exception;
using std::string;

QControlWidget::QControlWidget(shared_ptr<TdcController> controller,
	                           TdcView* view,
	                           QWidget* parent)
	: QTableWidget(parent),
	  mController(controller),
	  mView(view) {
	setRowCount(11);
	setColumnCount(2);
	createItems();

	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	horizontalHeader()->hide();
	verticalHeader()->hide();

	auto changeConn = connect(this, &QTableWidget::itemChanged, [this]{
		mController->setCtrl(readCtrl());
	});
	connect(mView, &TdcView::setCtrl, this, [this](auto status){
		if(!status.isEmpty())
			QMessageBox::critical(this, "setCtrl", status);
	});
	connect(mView, &TdcView::ctrl, this, [this, changeConn](auto status, auto ctrl) mutable {
		if(!status.isEmpty())
			QMessageBox::critical(this, "Get Ctrl", status);
		else {
			this->disconnect(changeConn);
			fillTable(ctrl);
			changeConn = this->connect(this, &QTableWidget::itemChanged, [this]{
				mController->setCtrl(readCtrl());
			});
		}
	});
}

void QControlWidget::fillTable(uint16_t ctrl) {
	for (short i = 0; i < 10; ++i)
		mItems[i][1]->setText(QString::number(ctrl >> i & 1));
	mItems[10][1]->setText(QString::number((ctrl >> 12) & 1));
}

uint16_t QControlWidget::readCtrl() {
	uint16_t ctrl = 0;
	for (int i = 0; i < 10; ++i) {
		auto item = mItems[i][1]->text().toInt() != 0 ? 1 : 0;
		if (i != 10)
			ctrl |= item << i;
		else
			ctrl |= item << 12;
	}
	return ctrl;
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
