#pragma once

#include <QWidget>
#include <QLineEdit>

class QCustomPlot;

class QPlotSettings : public QWidget {
public:
    QPlotSettings(QCustomPlot* plot, QWidget* parent = nullptr);
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    QCustomPlot* mPlot;
    QLineEdit* yAxisLower;
    QLineEdit* yAxisUpper;

    QLineEdit* yAxis2Lower;
    QLineEdit* yAxis2Upper;
    bool mHasYAxis;
    bool mHasYAxis2;
};
