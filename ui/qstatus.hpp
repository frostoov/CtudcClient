#pragma once

#include "controllers/tdccontroller.hpp"

#include <QTableWidget>

class QStatusWidget : public QTableWidget {
public:
    explicit QStatusWidget(std::shared_ptr<TdcController> controller,
                           QWidget* parent = nullptr);
    void updateStatus();
protected:
    void fillTable(uint16_t stat);
    void createItems();
private:
    std::shared_ptr<TdcController> mContr;
    QTableWidgetItem*   mItems[15][2];
};
