#include "qmonitor.hpp"

#include <trek/common/stringbuilder.hpp>

#include <boost/filesystem.hpp>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QVector>

using trek::StringBuilder;

using std::shared_ptr;
using std::make_unique;
using std::array;
using std::ostream;
using std::chrono::system_clock;

QMonitor::QMonitor(std::shared_ptr<ExpoController> expoContr,
                   ExpoView* expoView,
                   QWidget *parent)
    : QSplitter(parent),
      mExpoContr(expoContr),
      mExpoView(expoView) {
    boost::filesystem::create_directories("./monitoring/chambers");
    boost::filesystem::create_directories("./monitoring/frequency");
    setupGUI();

    mTimer = new QTimer(this);
    mTimer->setInterval(mTick->text().toInt() * 1000);
    mTimer->setSingleShot(false);
    for(auto& c : mChambers) c->setTick(4 * mTick->text().toInt());

    createConnections();
    resize(800, 600);
}

void QMonitor::setupGUI() {
    auto ctrlLayout = new QVBoxLayout;

    mToggle = new QPushButton("Start");
    mTick   = new QLineEdit("300");
    auto actionLayout = new QFormLayout;
    actionLayout->addRow(mToggle);
    actionLayout->addRow("Frequency", mTick);
    auto actionGroup = new QGroupBox("Actions");
    actionGroup->setLayout(actionLayout);

    mCurrentRun = new QLineEdit("-");
    mCurrentRun->setReadOnly(true);
    mType = new QLineEdit("idle");
    mType->setReadOnly(true);
    mHitsLine = new QLineEdit("-");
    mHitsLine->setReadOnly(true);
    mTriggerLine = new QLineEdit("-");
    mTriggerLine->setReadOnly(true);
    mPackageLine = new QLineEdit("-");
    mPackageLine->setReadOnly(true);
    auto statLayout = new QFormLayout;
    statLayout->addRow("Type",mType);
    statLayout->addRow("Run", mCurrentRun);
    statLayout->addRow("Hits", mHitsLine);
    statLayout->addRow("Triggers", mTriggerLine);
    statLayout->addRow("Packages", mPackageLine);
    auto statGroup = new QGroupBox("Status");
    statGroup->setLayout(statLayout);

    ctrlLayout->addWidget(actionGroup);
    ctrlLayout->addWidget(statGroup);

    mPlots = {{
        createMetaPlot("Hits", {"good", "drops"}),
        createMetaPlot("Triggers", {"good", "drops"}),
        createMetaPlot("Packages", {"good", "drops"}),
    }};

    auto plotsLayout = new QGridLayout;
    plotsLayout->addWidget(mPlots.at(0), 0, 0, 1, 2);
    plotsLayout->addWidget(mPlots.at(1), 1, 0);
    plotsLayout->addWidget(mPlots.at(2), 1, 1);
    auto plotsWidget = new QWidget;
    plotsWidget->setLayout(plotsLayout);

    auto chambersLayout = new QGridLayout;
    for(int i = 0; i < mChambers.size(); ++i) {
        mChambers.at(i) = new QChamberMonitor(tr("Chamber %1").arg(i + 1));
        mChambers.at(i)->xAxis->setTickLabelType(QCPAxis::ltDateTime);
        mChambers.at(i)->xAxis->setDateTimeFormat("hh:mm:ss");
        mChambers.at(i)->xAxis->setAutoTickStep(false);
        chambersLayout->addWidget(mChambers.at(i), i%4, 4 - i/4);

        connect(mChambers[i], &QChamberMonitor::mouseDoubleClick, this, [this, i] {
            static bool flag = true;
            if(flag) {
                for(auto& c : mChambers) c->hide();
                mChambers.at(i)->show();
            } else {
                for(auto& c : mChambers) c->show();
            }
            flag = !flag;
        });
    }
    auto chambersWidget = new QWidget;
    chambersWidget->setLayout(chambersLayout);

    mFreq = new QChamberTable(16);
    auto tab = new QTabWidget;
    tab->addTab(plotsWidget, "main" );
    tab->addTab(chambersWidget, "chambers");
    tab->addTab(mFreq, "freq");

    auto ctrlWidget = new QWidget;
    ctrlWidget->setLayout(ctrlLayout);

    setOrientation(Qt::Horizontal);
    addWidget(ctrlWidget);
    addWidget(tab);
    setStretchFactor(1, 5);
}

static ostream& operator<<(ostream& stream, const system_clock::time_point& tp) {
    auto time = system_clock::to_time_t(tp);
    return stream << std::put_time(std::localtime(&time), "%d.%m.%Y %T");
}

QCustomPlot* QMonitor::createMetaPlot(const QString& title, const QVector<QString>& names) {
    auto* plot = new QCustomPlot;
    
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, title));
    
    plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    plot->xAxis->setDateTimeFormat("hh:mm:ss");
    plot->xAxis->setAutoTickStep(false);

    plot->yAxis->setLabel("Frequency, Hz");
    connect(plot, &QCustomPlot::mousePress, this, [plot](QMouseEvent* event) {
        if(event->button() == Qt::RightButton) {
            plot->legend->setVisible(!plot->legend->visible());
            plot->replot();
        }
    });
    for(int i = 0; i < names.size(); ++i) {
        plot->addGraph(plot->xAxis, plot->yAxis);
        QColor color {
            255 * ( ((i + 1) >> 0) & 1 ),
            255 * ( ((i + 1) >> 1) & 1 ),
            255 * ( ((i + 1) >> 2) & 1 ),
	};

        QBrush brush(color);
        QPen pen(color);
        pen.setWidth(2);
        pen.setStyle(Qt::PenStyle::SolidLine);

        plot->graph(i)->setName(names.at(i));
        plot->graph(i)->setPen(pen);
        plot->graph(i)->setLineStyle(QCPGraph::lsLine);
        plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 6));
    }
    return plot;
}

void QMonitor::createConnections() {
    connect(mTick, &QLineEdit::editingFinished, [this]{
        auto tick = (mTick->text().toInt() > 0) ? mTick->text().toInt() : 1;
        mTick->setText( QString::number(tick) );
        if(mTimer->interval() != tick) {
            mTimer->setInterval(tick * 1000);
            
            for(auto& plot : mPlots)
                plot->xAxis->setTickStep(4*tick);
            for(auto& c : mChambers)
                c->setTick(4*tick);
            mTriggerCount.reset();
            mPackageCount.reset();
            mChambersCount.reset();
            if(mTimer->isActive())
                mTimer->start();
        }
    });
    connect(mToggle, &QPushButton::clicked, [this]{
        if(mToggle->text() == "Start") {
            mToggle->setText("Stop");
            mTimer->start();
        } else {
            mToggle->setText("Start");
            mTimer->stop();
            mTriggerStream.flush();
            mPackageStream.flush();
            for(auto& c : mChambersStream) c.second.flush();
        }
    });
    
    connect(mTimer, &QTimer::timeout, [this]{
        mExpoContr->triggerCount();
        mExpoContr->packageCount();
        mExpoContr->chambersCount();
    });

    connect(mExpoView, &ExpoView::type, [this](auto status, auto type) {
        if(status.isEmpty()) {
            mType->setText(type);
            mToggle->setEnabled(type == "expo");
            if(type == "expo") {
                mToggle->setText("Stop");
                mTimer->start();
            } else {
                mToggle->setText("Start");
                mTimer->stop();
            }
        }
    });
    connect(mExpoView, &ExpoView::run, this, [this](auto status, auto run) {
        if(status.isEmpty()){
            mCurrentRun->setText(QString::number(run));
        }
    });
    connect(mExpoView, &ExpoView::freq, this ,[this](auto status, auto freq) {
        if(status.isEmpty()) {
            mFreq->setTrekFreq(freq);
            mFreq->update();
            for(auto c : freq) {
                if(mFreqStream.count(c.first) == 0) {
                        std::ofstream stream;
                        stream.exceptions(stream.failbit | stream.badbit);
                        stream.open(StringBuilder() << "./monitoring/frequency/chamber_" << (c.first + 1) << ".dat",
                                    stream.app | stream.binary);
                        mFreqStream.emplace(c.first, std::move(stream));
                    }
                    auto& s = mFreqStream.at(c.first);
                    s << system_clock::now();
                    for(auto& w : c.second)
                        s << '\t' << w;
                    s << std::endl;
            }
        }
    });
    connect(mExpoView, &ExpoView::triggerCount, this, [this](auto status, auto count, auto drop){
        if(status.isEmpty()) {
            if(mTriggerCount) {
                auto& plot = *mPlots.at(1);
                auto key = double(QDateTime::currentMSecsSinceEpoch())/1000;
                this->updateGraph(*plot.graph(0), key, double(count - mTriggerCount->at(0))/mTick->text().toInt());
                this->updateGraph(*plot.graph(1), key, double(drop -  mTriggerCount->at(1))/mTick->text().toInt());
                plot.xAxis->rescale();
                plot.yAxis->rescale();
                if(!mTriggerStream.is_open())
                    mTriggerStream.open("./monitoring/triggers.dat", mTriggerStream.app | mTriggerStream.binary);
                plot.replot();
                mTriggerStream << system_clock::now() << '\t' << count << '\t' << drop << '\t'
                               << (double(count - mTriggerCount->at(0))/mTick->text().toInt()) << '\t'
                               << (double(drop -  mTriggerCount->at(1))/mTick->text().toInt()) << '\n';
            } else {
                mTriggerCount = make_unique< array<uintmax_t, 2> >();
            }
            *mTriggerCount = {{count, drop}};
            mTriggerLine->setText(tr("%1 | %2").arg(count).arg(drop));
        }
    });
    connect(mExpoView, &ExpoView::packageCount, this, [this](auto status, auto count, auto drop){
        if(status.isEmpty()) {
            if(mPackageCount) {
                auto& plot = *mPlots.at(2);
                auto key = double(QDateTime::currentMSecsSinceEpoch())/1000;
                this->updateGraph(*plot.graph(0), key, double(count - mPackageCount->at(0))/mTick->text().toInt());
                this->updateGraph(*plot.graph(1), key, double(drop -  mPackageCount->at(1))/mTick->text().toInt());
                plot.xAxis->rescale();
                plot.yAxis->rescale();
                plot.replot();
                if(!mPackageStream.is_open())
                    mPackageStream.open("./monitoring/packages.dat", mTriggerStream.app | mTriggerStream.binary);
                mPackageStream << system_clock::now() << '\t' << count << '\t' << drop << '\t'
                               << (double(count - mPackageCount->at(0))/mTick->text().toInt()) << '\t'
                               << (double(drop -  mPackageCount->at(1))/mTick->text().toInt()) << '\n';
            } else {
                mPackageCount = make_unique< array<uintmax_t, 2> >();
            }
            *mPackageCount = {{count, drop}};
            mPackageLine->setText(tr("%1 | %2").arg(count).arg(drop));
        }
    });
    
    connect(mExpoView, &ExpoView::chambersCount, this, [this](auto status, auto count, auto drop) {
        if(status.isEmpty()) {
            auto totalHits = reduceCount(count);
            auto totalDrops = reduceCount(drop);
            if(mChambersCount) {
                auto key = double(QDateTime::currentMSecsSinceEpoch())/1000;
                auto prevHits = reduceCount(mChambersCount->at(0));
                auto prevDrops = reduceCount(mChambersCount->at(1));
                auto& plot = *mPlots.at(0);
                
                this->updateGraph(*plot.graph(0), key, double(totalHits - prevHits)/mTick->text().toInt());
                this->updateGraph(*plot.graph(1), key, double(totalDrops -  prevDrops)/mTick->text().toInt());
                plot.xAxis->rescale();
                plot.yAxis->rescale();
                plot.replot();
                
                for(auto& c : this->convertCount(count, mChambersCount->at(0), mTick->text().toInt())) {
                    mChambers.at(c.first)->addFreq(key, c.second);
                    mChambers.at(c.first)->removeDataBefore(key - 50 * mTick->text().toInt());
                    mChambers.at(c.first)->rescaleAxis();
                    mChambers.at(c.first)->replot();
                    if(mChambersStream.count(c.first) == 0) {
                        std::ofstream stream;
                        stream.exceptions(stream.failbit | stream.badbit);
                        stream.open(StringBuilder() << "./monitoring/chambers/chamber_" << (c.first + 1) << ".dat",
                                    stream.app | stream.binary);
                        mChambersStream.emplace(c.first, std::move(stream));
                    }
                    auto& s = mChambersStream.at(c.first);
                    s << system_clock::now();
                    for(auto& w : c.second)
                        s << '\t' << w;
                    s << '\n';
                }
            } else {
                mChambersCount = make_unique< array<TrekFreq, 2> >();
            }
            mHitsLine->setText(tr("%1 | %2").arg(totalHits).arg(totalDrops));
            *mChambersCount = {{count, drop}};
        }
    });
}

ChamberFreq QMonitor::convertCount(const ChamberFreq& current, const ChamberFreq& prev, int sec) {
    return{{
        (current.at(0) - prev.at(0))/ sec,
        (current.at(1) - prev.at(1))/ sec,
        (current.at(2) - prev.at(2))/ sec,
        (current.at(3) - prev.at(3))/ sec,
    }};
}

TrekFreq QMonitor::convertCount(const TrekFreq& current, const TrekFreq& prev, int sec) {
    TrekFreq freq;
    constexpr ChamberFreq nullfreq{{0, 0, 0, 0}};
    for(auto& c : current) {
        auto p = prev.find(c.first);
        if(p == prev.end())
            freq.emplace(c.first, convertCount(c.second, nullfreq, sec));
        else
            freq.emplace(c.first, convertCount(c.second, p->second, sec));
    }

    return freq;
}


uintmax_t QMonitor::reduceCount(const TrekFreq& count) {
    return std::accumulate(count.begin(), count.end(), uintmax_t(0), [](uintmax_t val, auto& count) {
        return std::accumulate(count.second.begin(), count.second.end(), uintmax_t(0), [](uintmax_t val, double count) {
            return val + count;
        });
    });
}

void QMonitor::updateGraph(QCPGraph& graph, double key, double val) {
    graph.addData(key, val);
    graph.removeDataBefore(key - 50 * mTick->text().toInt());
}
