#pragma once

#include "controllers/tdccontroller.hpp"

#include <QTableWidget>

class QControlWidget : public QTableWidget {
    Q_OBJECT
public:
    QControlWidget(std::shared_ptr<TdcController> controller,
                   QWidget* parent = nullptr);
    void updateCtrl();
protected:
    void displayCtrl(uint16_t ctrl);
    void createItems();
    uint16_t readCtrl();
    void fillTable(uint16_t ctrl);
private:
    std::shared_ptr<TdcController> mContr;
    QTableWidgetItem*	mItems[11][2];
    QMetaObject::Connection mChangeConn;
};
