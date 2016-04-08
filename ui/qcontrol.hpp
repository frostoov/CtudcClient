#pragma once

#include "controllers/tdccontroller.hpp"
#include "views/tdcview.hpp"

#include <QTableWidget>

class QControlWidget : public QTableWidget {
    Q_OBJECT
public:
    QControlWidget(std::shared_ptr<TdcController> controller,
                   TdcView* view,
                   QWidget* parent = nullptr);
protected:
    void createItems();
    uint16_t readCtrl();
    void fillTable(uint16_t ctrl);
private:
    std::shared_ptr<TdcController> mController;
    TdcView* mView;
    QTableWidgetItem*	mItems[11][2];
};
