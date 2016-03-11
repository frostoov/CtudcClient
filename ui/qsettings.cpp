#include "tdc/tdc.hpp"
#include "qsettings.hpp"
#include "qconsole.hpp"
#include "utils.hpp"

#include <QFormLayout>

#include <stdexcept>

using std::exception;
using std::function;
using std::string;
using std::shared_ptr;

QSettingsWidget::QSettingsWidget(std::shared_ptr<TdcController> controller, QWidget* parent)
	: QGroupBox("Settings", parent),
	  mController(controller) {
	setupGUI();

	connect(mLsb,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
			this, &QSettingsWidget::setLsb);
	connect(mEdgeDetection, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
			this, &QSettingsWidget::setEdgeDetection);
	connect(mWinWidth, &QLineEdit::editingFinished,
			this, &QSettingsWidget::setWindowWidth);
	connect(mWinOffset, &QLineEdit::editingFinished,
			this, &QSettingsWidget::setWindowOffset);
	connect(mMode, &QCheckBox::clicked,
			this, &QSettingsWidget::setMode);
	connect(mTdcMeta, &QCheckBox::clicked,
			this, &QSettingsWidget::setTdcMeta);
}

void QSettingsWidget::setupGUI() {
	mEdgeDetection = new QComboBox;
	mLsb = new QComboBox;
	mWinWidth = new QLineEdit("0");
	mWinOffset = new QLineEdit("0");
	mMode = new QCheckBox();
	mTdcMeta = new QCheckBox();

	mEdgeDetection->addItem("leading",	QVariant(int(Tdc::EdgeDetection::leading)));
	mEdgeDetection->addItem("trailing",	QVariant(int(Tdc::EdgeDetection::trailing)));
	mEdgeDetection->addItem("leading & trailing",	QVariant(int(Tdc::EdgeDetection::leadingTrailing)));

	mLsb->addItem("800ps", QVariant(781));
	mLsb->addItem("200ps", QVariant(195));
	mLsb->addItem("100ps", QVariant(98));

	auto forms	= new QFormLayout;
	forms->addRow("Edge Detection", mEdgeDetection);
	forms->addRow("Lsb Resolution", mLsb);
	forms->addRow("Window Width", mWinWidth);
	forms->addRow("Window Offset", mWinOffset);
	forms->addRow("Trigger Mode", mMode);
	forms->addRow("Tdc Meta", mTdcMeta);
	setLayout(forms);
}

int QSettingsWidget::lsb2index(unsigned lsb) {
	switch(lsb) {
	case 781:
		return 0;
	case 195:
		return 1;
	case 98:
		return 2;
	default:
		throw std::logic_error("QSettingsWidget::lsb2index invalid value");
	}
}

void QSettingsWidget::setLsb() {
	doAction("TDC set lsb", [&]{
		mController->setLsb(mLsb->currentData().toInt());
	});
}

void QSettingsWidget::setEdgeDetection() {
	doAction("TDC set edge detection ", [&]{
		mController->setEdgeDetection(Tdc::EdgeDetection(mEdgeDetection->currentData().toInt()));
	});
}

void QSettingsWidget::setWindowOffset() {
	doAction("TDC set windows offset ", [&]{
		mController->setWindowOffset(mWinOffset->text().toInt());
	});
}

void QSettingsWidget::setWindowWidth() {
	doAction("TDC set window width", [&]{
		mController->setWindowWidth(mWinWidth->text().toUInt());
	});
}

void QSettingsWidget::setMode() {
	doAction("TDC set mode", [&]{
		auto mode = mMode->isChecked() ? Tdc::Mode::trigger : Tdc::Mode::continuous;
		mController->setMode(mode);
	});
}

void QSettingsWidget::setTdcMeta() {
	doAction("TDC set meta", [&]{
		mController->setTdcMeta(mTdcMeta->isChecked());
	});
}

void QSettingsWidget::refresh() {
	try {
		auto settings = mController->settings();
		mEdgeDetection->setCurrentIndex(int(settings.edgeDetection));
		mWinOffset->setText( QString::number(settings.windowOffset));
		mWinWidth->setText(  QString::number(settings.windowWidth) );
		mLsb->setCurrentIndex(lsb2index(settings.lsb));
	} catch(exception& e) {
		console->printWithTime(QString("TDC read settings failed ") +  e.what());
	}

	try {
		mMode->setChecked( mController->mode() == Tdc::Mode::trigger );
	} catch(exception& e) {
		console->printWithTime(QString("TDC read mode failed ") +  e.what());
	}
	try {
		mTdcMeta->setChecked( mController->tdcMeta());
	} catch(exception& e) {
		console->printWithTime(QString("TDC read meta failed ") +  e.what());
	}
}
