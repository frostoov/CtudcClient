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
                                 TdcView* view,
                                 QWidget* parent)
    : QGroupBox("Settings", parent),
      mController(controller),
      mView(view) {
    setupGUI();

    connect(mLsb, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this] {
        mController->setLsb(mLsb->currentData().toInt());
    });
    connect(mEdgeDetection, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [this] {
        mController->setEdgeDetection(Tdc::EdgeDetection(mEdgeDetection->currentData().toInt()));
    });
    connect(mWinWidth, &QLineEdit::returnPressed, this, [this] {
        mController->setWindowWidth(mWinWidth->text().toUInt());
    });
    connect(mWinOffset, &QLineEdit::returnPressed, this, [this] {
        mController->setWindowOffset(mWinOffset->text().toInt());
    });
    connect(mMode, &QCheckBox::clicked, this, [this] {
        auto mode = mMode->isChecked() ? Tdc::Mode::trigger : Tdc::Mode::continuous;
        mController->setMode(mode);
    });
    connect(mTdcMeta, &QCheckBox::clicked, this, [this] {
        mController->setTdcMeta(mTdcMeta->isChecked());
    });

    connect(mView, &TdcView::settings, this, [this](auto status, auto settings) {
        if(status.isEmpty()) {
            mEdgeDetection->setCurrentIndex(int(settings.edgeDetection));
            mWinOffset->setText( QString::number(settings.windowOffset));
            mWinWidth->setText(  QString::number(settings.windowWidth) );
            mLsb->setCurrentIndex(lsb2index(settings.lsb));
        }
    });
    connect(mView, &TdcView::mode, this, [this](auto status, auto mode) {
        if(status.isEmpty())
            mMode->setChecked( mode == Tdc::Mode::trigger );
    });
    connect(mView, &TdcView::tdcMeta, this, [this](auto status, auto flag) {
        if(status.isEmpty())
            mTdcMeta->setChecked(flag);
    });
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
