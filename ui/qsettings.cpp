#include "tdc/tdc.hpp"
#include "qsettings.hpp"
#include "qconsole.hpp"


#include <QFormLayout>
#include <QMessageBox>

#include <stdexcept>

using std::exception;
using std::function;
using std::string;
using std::shared_ptr;

QSettingsWidget::QSettingsWidget(shared_ptr<TdcController> controller,
                                 QWidget* parent)
    : QGroupBox("Settings", parent),
      mContr(controller)  {
    setupGUI();

    connect(mLsb, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this] {
        try {
            mContr->setLsb(mLsb->currentData().toInt());
        } catch(exception& e) {
            QMessageBox::warning(this, "Set lsb", e.what());
        }
    });
    connect(mEdgeDetection, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this] {
        try {
            mContr->setEdgeDetection(Tdc::EdgeDetection(mEdgeDetection->currentData().toInt()));
        } catch(exception& e) {
            QMessageBox::warning(this, "Set edge detection", e.what());
        }
    });
    connect(mWinWidth, &QLineEdit::returnPressed, this, [this] {
        try {
            mContr->setWindowWidth(mWinWidth->text().toUInt());
        } catch(exception& e) {
            QMessageBox::warning(this, "Set window width", e.what());
        }
    });
    connect(mWinOffset, &QLineEdit::returnPressed, this, [this] {
        try {
            mContr->setWindowOffset(mWinOffset->text().toInt());
        } catch(exception& e) {
            QMessageBox::warning(this, "Set window offset", e.what());
        }
    });
    connect(mMode, &QCheckBox::clicked, this, [this] {
        auto mode = mMode->isChecked() ? Tdc::Mode::trigger : Tdc::Mode::continuous;
        try {
            mContr->setMode(mode);
        } catch(exception& e) {
            QMessageBox::warning(this, "Set tdc mode", e.what());
        }
    });
    connect(mTdcMeta, &QCheckBox::clicked, this, [this] {
        try {
            mContr->setTdcMeta(mTdcMeta->isChecked());
        } catch(exception& e) {
            QMessageBox::warning(this, "Set tdc meta", e.what());
        }
    });

    connect(mContr.get(), &TdcController::settingsChanged ,
            this, &QSettingsWidget::displaySettings);
    connect(mContr.get(), &TdcController::modeChanged,
            this, &QSettingsWidget::displayMode);
    connect(mContr.get(), &TdcController::tdcMetaChanged,
            this, &QSettingsWidget::displayTdcMeta);
}

void QSettingsWidget::updateSettings() {
    displaySettings(mContr->settings());
}

void QSettingsWidget::updateTdcMeta() {
    displayTdcMeta(mContr->tdcMeta());
}
void QSettingsWidget::updateMode() {
    displayMode(mContr->mode());
}

void QSettingsWidget::displaySettings(const Tdc::Settings& settings) {
    mEdgeDetection->setCurrentIndex(int(settings.edgeDetection));
    mWinOffset->setText( QString::number(settings.windowOffset));
    mWinWidth->setText(  QString::number(settings.windowWidth) );
    mLsb->setCurrentIndex(lsb2index(settings.lsb));
}

void QSettingsWidget::displayTdcMeta(bool flag) {
    mTdcMeta->setChecked(flag);
}

void QSettingsWidget::displayMode(Tdc::Mode mode) {
    mMode->setChecked( mode == Tdc::Mode::trigger );
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
