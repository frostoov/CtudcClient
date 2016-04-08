#pragma once

#include "controllers/tdccontroller.hpp"
#include "views/tdcview.hpp"

#include <QTableWidget>

class QStatusWidget : public QTableWidget {
public:
    explicit QStatusWidget(std::shared_ptr<TdcController> controller,
                           TdcView* view,
                           QWidget* parent = nullptr);
protected:
    void fillTable(uint16_t stat);
    void createItems();
private:
    std::shared_ptr<TdcController> mController;
    TdcView* mView;
    QTableWidgetItem*   mItems[15][2];
};
