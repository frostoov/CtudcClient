#pragma once

#include "controllers/tdccontroller.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>

class QSettingsWidget : public QGroupBox {
public:
    explicit QSettingsWidget(std::shared_ptr<TdcController> controller,
                             QWidget* parent = nullptr);
    void updateSettings();
    void updateTdcMeta();
    void updateMode();
protected:
    void displaySettings(const Tdc::Settings& settings);
    void displayTdcMeta(bool flag);
    void displayMode(Tdc::Mode mode);
    void setupGUI();
    static int lsb2index(unsigned lsb);
private:
    std::shared_ptr<TdcController> mContr;

    QCheckBox*   mMode;
    QCheckBox*   mTdcMeta;

    QComboBox* mLsb;
    QComboBox* mEdgeDetection;
    QLineEdit* mWinWidth;
    QLineEdit* mWinOffset;
};
