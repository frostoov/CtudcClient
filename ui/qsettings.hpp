#pragma once

#include "controllers/tdccontroller.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>

class QSettingsWidget : public QGroupBox {
	Q_OBJECT
public:
	explicit QSettingsWidget(std::shared_ptr<TdcController> controller,
							 QWidget* parent = nullptr);
	void refresh();
protected:
	void setupGUI();
	static int lsb2index(unsigned lsb);
protected slots:
	void setLsb();
	void setEdgeDetection();
	void setWindowWidth();
	void setWindowOffset();
	void setMode();
	void setTdcMeta();
private:

private:
	std::shared_ptr<TdcController> mController;

	QCheckBox*   mMode;
	QCheckBox*   mTdcMeta;

	QComboBox* mLsb;
	QComboBox* mEdgeDetection;
	QLineEdit* mWinWidth;
	QLineEdit* mWinOffset;
};
