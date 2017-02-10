#include "qplotsettings.hpp"

#include <qcustomplot.h>
#include <QLineEdit>
#include <algorithm>

bool hasValueAxis(QCustomPlot& plot, QCPAxis* axis) {
    for(int i = 0; i < plot.graphCount(); ++i) {
        if (plot.graph(i)->valueAxis() == axis) {
            return true;
        }
    }
    return false;
}

QPlotSettings::QPlotSettings(QCustomPlot* plot, QWidget* parent)
    : QWidget(parent),
      mPlot(plot),
      yAxisLower(new QLineEdit(QString::number(plot->yAxis->range().lower))),
      yAxisUpper(new QLineEdit(QString::number(plot->yAxis->range().upper))),

      yAxis2Lower(new QLineEdit(QString::number(plot->yAxis2->range().lower))),
      yAxis2Upper(new QLineEdit(QString::number(plot->yAxis2->range().upper))),
      mHasYAxis(hasValueAxis(*plot, plot->yAxis)),
      mHasYAxis2(hasValueAxis(*plot, plot->yAxis2)) {

    auto ok = new QPushButton("ok");
    connect(ok, &QPushButton::clicked,
            this, &QWidget::close);

    auto forms	= new QFormLayout;

    if(mHasYAxis) {
        auto yAxisLayout = new QHBoxLayout;
        yAxisLayout->addWidget(yAxisLower);
        yAxisLayout->addWidget(yAxisUpper);
        forms->addRow("Y1", yAxisLayout);
    }
    if(mHasYAxis2) {
        auto yAxis2Layout = new QHBoxLayout;
        yAxis2Layout->addWidget(yAxis2Lower);
        yAxis2Layout->addWidget(yAxis2Upper);
        forms->addRow("Y2", yAxis2Layout);
    }

    forms->addWidget(ok);

    setLayout(forms);
}

void QPlotSettings::closeEvent(QCloseEvent* event) {
    if(mHasYAxis) {
        mPlot->yAxis->setRange(yAxisLower->text().toDouble(), yAxisUpper->text().toDouble());
    }
    if(mHasYAxis2) {
        mPlot->yAxis2->setRange(yAxis2Lower->text().toDouble(), yAxis2Upper->text().toDouble());
    }
    mPlot->replot();
    QWidget::closeEvent(event);
}
