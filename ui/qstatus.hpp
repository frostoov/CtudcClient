#pragma once

#include "controllers/tdccontroller.hpp"

#include <QTableWidget>

class QStatusWidget : public QTableWidget {
public:
	explicit QStatusWidget(std::shared_ptr<TdcController> controller, QWidget* parent = nullptr);
	void refresh();
protected:
	void setValues();
	void createItems();
private:
	std::shared_ptr<TdcController> mController;
	QTableWidgetItem*   mItems[15][2];
	uint16_t            mStatus;

};
