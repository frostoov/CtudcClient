#pragma once

#include "controllers/voltagecontroller.hpp"

#include <qcustomplot.h>

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QSplitter>
#include <QTimer>

#include <QHash>

class QVoltageWidget : public QSplitter {
    Q_OBJECT
public:
    explicit QVoltageWidget(std::shared_ptr<VoltageController> controller,
                            QWidget* parent = nullptr);
protected:
    void setupGUI();
    void setupPlot(QCustomPlot& plot, const QString& name);
    void createConnections();
    void updateGraph(QCPGraph& graph, int val);
    void updateCell(const std::string& type);
private:
    std::shared_ptr<VoltageController> mContr;

    QHash<QString, QLineEdit*> mVolt;
    QHash<QString, QLineEdit*> mStatus;
    QHash<QString, QLineEdit*> mSpeedUp;
    QHash<QString, QLineEdit*> mSpeedDn;
    QHash<QString, QCheckBox*> mState;
    QHash<QString, QCustomPlot*> mPlot;

    QPushButton* mOpen;
    QPushButton* mToggle;
    QPushButton* mUpdate;
    QLineEdit* mTick;

    QTimer* timer;
};
