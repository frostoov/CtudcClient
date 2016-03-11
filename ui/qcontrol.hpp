#pragma once

#include "controllers/tdccontroller.hpp"

#include <QTableWidget>

class QControlWidget : public QTableWidget {
	Q_OBJECT
public:
	QControlWidget(std::shared_ptr<TdcController> tdc, QWidget* parent = nullptr);
	void refresh();
protected slots:
	void setCtrl();
	void getCtrl();
protected:
	void createItems();
	void getValues();
	void setValues();
private:
	std::shared_ptr<TdcController> mController;
	QTableWidgetItem*	mItems[11][2];
	uint16_t			mControl;
};
