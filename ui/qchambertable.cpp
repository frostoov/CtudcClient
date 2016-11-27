#include "qchambertable.hpp"

#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>

#include <set>
#include <iterator>

QChamberTable::QChamberTable(int rows, const Config& conf, QWidget* parent)
    : QTableWidget(rows + 1, 5, parent),
      mConf(conf) {
    std::cout << conf.marshal().dump(4) << std::endl;
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    createItems();
}

void QChamberTable::setTrekFreq(const TrekFreq& freq) {
    item(0, 0)->setText("Chamber/Wire");
    setRowCount(freq.size() + 1);
    std::set<unsigned> triggChambers;
    std::transform(freq.begin(), freq.end(), std::inserter(triggChambers, triggChambers.begin()), [](auto & pair) {
        return pair.first;
    });
    int row = 1;
    for(const auto& cham : triggChambers) {
        QTableWidgetItem* items[5] {
            new QTableWidgetItem(QString("Chamber %1").arg(cham + 1)),
            new QTableWidgetItem(QString::number(freq.at(cham).at(0) )),
            new QTableWidgetItem(QString::number(freq.at(cham).at(1) )),
            new QTableWidgetItem(QString::number(freq.at(cham).at(2) )),
            new QTableWidgetItem(QString::number(freq.at(cham).at(3) )),
        };
        for (short j = 0; j < 5; ++j) {
            if(j > 0)
                items[j]->setBackgroundColor(itemColor(*items[j], mConf));
            items[j]->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            setItem(row, j, items[j]);
        }
        ++row;
    }
}

void QChamberTable::setChamFreqSeries(const ChamFreqSeries& series) {
    item(0, 0)->setText("Voltage/Wire");
    setRowCount(series.size() + 1);

    int row = 1;
    for(auto& voltFreq : series) {
        QTableWidgetItem* items[5] {
            new QTableWidgetItem(QString::number(voltFreq.first)),
            new QTableWidgetItem(QString::number(voltFreq.second.at(0) )),
            new QTableWidgetItem(QString::number(voltFreq.second.at(1) )),
            new QTableWidgetItem(QString::number(voltFreq.second.at(2) )),
            new QTableWidgetItem(QString::number(voltFreq.second.at(3) ))
        };
        for(int j = 0; j < 5; ++j) {
            items[j]->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            items[j]->setBackgroundColor(Qt::white);
            setItem(row, j, items[j]);
        }
        ++row;
    }
}

void QChamberTable::createHeader() {
    QTableWidgetItem* root[5] {
        new QTableWidgetItem,
        new QTableWidgetItem("Wire 1"),
        new QTableWidgetItem("Wire 2"),
        new QTableWidgetItem("Wire 3"),
        new QTableWidgetItem("Wire 4"),
    };
    for (short i = 0; i < 5; ++i) {
        setItem(0, i, root[i]);
    }
}

void QChamberTable::clearData() {
    for(int i = 1; i < rowCount(); ++i)
        removeRow(i);
}

void QChamberTable::createItems() {
    createHeader();
    for (int i = 0; i < 32; ++i) {
        QTableWidgetItem* items[5] {
            new QTableWidgetItem,
            new QTableWidgetItem,
            new QTableWidgetItem,
            new QTableWidgetItem,
            new QTableWidgetItem,
        };
        for (int j = 0; j < 5; ++j)
            setItem(i + 1, j, items[j]);
    }
    for (int i = 0; i < 33; ++i)
        for (int j = 0; j < 5; ++j)
            if (item(i, j) != nullptr)
                item(i, j)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QColor QChamberTable::itemColor(const QTableWidgetItem& item, const Config& conf) {
    auto val = item.text().toDouble();
    if(val < conf.minCrit)
        return Qt::red;
    else if(val < conf.minNorm)
        return Qt::yellow;
    else if(val < conf.maxNorm)
        return Qt::green;
    else if(val < conf.maxCrit)
        return Qt::yellow;
    else
        return Qt::red;
}

void QChamberTable::keyPressEvent(QKeyEvent* evt) {
    if(evt->key() == Qt::Key_C && (evt->modifiers() & Qt::ControlModifier)) {
        auto ranges = selectedRanges();
        if(ranges.empty())
            return;
        QString text;
        for(const auto& range : ranges) {
            for(int row = range.topRow(); row <= range.bottomRow(); ++row) {
                for(int col = range.leftColumn(); col <= range.rightColumn(); ++col) {
                    if( item(row, col) )
                        text.append(item(row, col)->text());
                    if(col != range.rightColumn())
                        text.append("\t");
                }
                if(row != range.bottomRow())
                    text.append("\n");
            }
        }
        QApplication::clipboard()->setText(text);
    } else
        QWidget::keyPressEvent(evt);
}

nlohmann::json QChamberTable::Config::marshal() const {
    return {
        {"min_crit", minCrit },
        {"min_norm", minNorm },
        {"max_norm", maxNorm },
        {"max_crit", maxCrit },
    };
}

void QChamberTable::Config::unMarshal(const nlohmann::json& doc) {
    minCrit = doc.at("min_crit").get<double>();
    minNorm = doc.at("min_norm").get<double>();
    maxNorm = doc.at("max_norm").get<double>();
    maxCrit = doc.at("max_crit").get<double>();
}
