#pragma once

#include "controllers/expocontroller.hpp"

#include <json.hpp>

#include <QTableWidget>

using ChamCodeFreq   = std::pair<int, ChamberFreq>;
using ChamFreqSeries = std::vector<ChamCodeFreq>;
using TrekFreqSeries = std::unordered_map<uintmax_t, ChamFreqSeries>;

class QChamberTable : public QTableWidget {
public:
    struct Config {
        Config(double minc = 0, double minn = 0, double maxn = 0, double maxc = 0)
            : minCrit(minc), minNorm(minn), maxNorm(maxn), maxCrit(maxc) { }
        double minCrit;
        double minNorm;
        double maxNorm;
        double maxCrit;

        nlohmann::json marshal() const;
        void unMarshal(const nlohmann::json& doc);
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
