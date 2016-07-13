#pragma once

#include "controllers/expocontroller.hpp"
#include "qchambermonitor.hpp"
#include "qchambertable.hpp"

#include <qcustomplot.h>

#include <QSplitter>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QTabWidget>

#include <fstream>

class QMonitor : public QSplitter {
    template<typename T, size_t N>
    using ArrayPtr = std::unique_ptr<std::array<T, N> >;
public:
    QMonitor(std::shared_ptr<ExpoController> expoContr,
             QWidget* parent = nullptr);
    void updateState();
protected:
    void handleExpoType(const QString& type);
    void launchMonitoring();
    void stopMonitoring();
    void setupGUI();
    QCustomPlot* createMetaPlot(const QString& title, const QVector<QString>& names);

    void createConnections();
    void updateGraph(QCPGraph& graph, double key, double val);

    ChamberFreq convertCount(const ChamberFreq& current, const ChamberFreq& prev, int sec);
    TrekFreq convertCount(const TrekFreq& current, const TrekFreq& prev, int sec);
    uintmax_t reduceCount(const TrekFreq& count);
    void updatePackageCount(uintmax_t count,  uintmax_t drop);
    void updateTriggerCount(uintmax_t count, uintmax_t drop);
    void updateChambersCount(const TrekFreq& count, const TrekFreq& drop);
private:
    std::shared_ptr<ExpoController> mExpoContr;

    std::array<QCustomPlot*, 3> mPlots;
    std::array<QChamberMonitor*, 16> mChambers;
    QChamberTable* mFreq;

    ArrayPtr<uintmax_t , 2> mTriggerCount;
    std::ofstream mTriggerStream;
    ArrayPtr<uintmax_t , 2> mPackageCount;
    std::ofstream mPackageStream;
    ArrayPtr<TrekFreq, 2> mChambersCount;
    std::ofstream mHitStream;
    std::unordered_map<unsigned, std::ofstream> mChambersStream;
    std::unordered_map<unsigned, std::ofstream> mFreqStream;
    QPushButton* mToggle;
    QLineEdit* mTick;
    QLineEdit* mCurrentRun;
    QLineEdit* mType;

    QLineEdit* mHitsLine;
    QLineEdit* mTriggerLine;
    QLineEdit* mPackageLine;
    QTimer* mTimer;
};
