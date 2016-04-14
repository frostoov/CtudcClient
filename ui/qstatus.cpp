#include "qstatus.hpp"


#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

#include <stdexcept>

using std::shared_ptr;
using std::exception;
using std::string;

QStatusWidget::QStatusWidget(shared_ptr<TdcController> controller,
                             TdcView* view,
                             QWidget* parent)
    : QTableWidget(parent),
      mController(controller),
      mView(view) {
    setRowCount(15);
    setColumnCount(2);
    createItems();

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->hide();
    verticalHeader()->hide();

    connect(mView, &TdcView::stat, this, [this](auto status, auto stat) {
        if(status.isEmpty())
            this->fillTable(stat);
    });
}

void QStatusWidget::createItems() {
    mItems[0][0] = new QTableWidgetItem("Data_Ready");
    mItems[1][0] = new QTableWidgetItem("Almost_Full");
    mItems[2][0] = new QTableWidgetItem("Full");
    mItems[3][0] = new QTableWidgetItem("Trg_Match");
    mItems[4][0] = new QTableWidgetItem("headerEn");
    mItems[5][0] = new QTableWidgetItem("Term_On");
    mItems[6][0] = new QTableWidgetItem("Error_1");
    mItems[7][0] = new QTableWidgetItem("Error_1");
    mItems[8][0] = new QTableWidgetItem("Error_3");
    mItems[9][0] = new QTableWidgetItem("Error_4");
    mItems[10][0] = new QTableWidgetItem("BErr_Flag");
    mItems[11][0] = new QTableWidgetItem("Purged");
    mItems[12][0] = new QTableWidgetItem("Res");
    mItems[13][0] = new QTableWidgetItem("Pair_Mode");
    mItems[14][0] = new QTableWidgetItem("Trigger_Lost");
    for(int i = 0; i < 15; ++i) {
        mItems[i][1] = new QTableWidgetItem("0");
        mItems[i][0]->setFlags(Qt::ItemIsEnabled);
        mItems[i][1]->setFlags(Qt::ItemIsEnabled);
        for(int k = 0; k < 2; ++k)
            setItem(i, k, mItems[i][k]);
    }
}

void QStatusWidget::fillTable(uint16_t stat) {
    for(int i = 0; i < 15; ++i) {
        if(i != 13) {
            if(i != 12)
                mItems[i][1]->setText( QString::number( ((stat >> i) & 1)) );
            else
                mItems[i][1]->setText(QString::number( ((stat >> i) & 3)) );
        }
    }
}
