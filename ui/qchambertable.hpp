#pragma once

#include "controllers/expocontroller.hpp"

#include <QTableWidget>

using ChamCodeFreq   = std::pair<int, ChamberFreq>;
using ChamFreqSeries = std::vector<ChamCodeFreq>;
using TrekFreqSeries = std::unordered_map<uintmax_t, ChamFreqSeries>;

class QChamberTable : public QTableWidget {
public:
    struct Config {
        Config(double minc = 100, double minn = 500, double maxn = 2000, double maxc = 4000)
            : minCrit(minc), minNorm(minn), maxNorm(maxn), maxCrit(maxc) { }
        double minCrit;
        double minNorm;
        double maxNorm;
        double maxCrit;
    };
public:
    QChamberTable(int rows, const Config& conf = {}, QWidget* parent = nullptr);
    void setTrekFreq(const TrekFreq& freq);
    void setChamFreqSeries(const ChamFreqSeries& series);
    void keyPressEvent(QKeyEvent* evt) override;
    void clearData();
protected:
    void createHeader();
    void createItems();
    QColor itemColor(const QTableWidgetItem& item, const Config& conf);
private:
    Config mConf;
};
