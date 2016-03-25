#pragma once

#include "controllers/tdccontroller.hpp"
#include "views/tdcview.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>

class QSettingsWidget : public QGroupBox {
public:
	explicit QSettingsWidget(std::shared_ptr<TdcController> controller,
                             TdcView* view,
							 QWidget* parent = nullptr);
protected:
	void setupGUI();
	static int lsb2index(unsigned lsb);
private:
	std::shared_ptr<TdcController> mController;
	TdcView*       mView;

	QCheckBox*   mMode;
	QCheckBox*   mTdcMeta;

	QComboBox* mLsb;
	QComboBox* mEdgeDetection;
	QLineEdit* mWinWidth;
	QLineEdit* mWinOffset;
};
