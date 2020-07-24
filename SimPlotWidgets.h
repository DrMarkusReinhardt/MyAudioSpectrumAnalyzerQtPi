#ifndef QCPLOTWIDGETS_H
#define QCPLOTWIDGETS_H

#include <QObject>
#include <QWidget>
#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartView>
#include "plot2d.h"
#include <vector>
#include "RandomGenerator.h"
#include "PortaudioHandler.h"
#include "SpectrumCalculator.h"

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace MR_SIM
{

class SimPlotWidgets : public QWidget
{
    Q_OBJECT

public:
    SimPlotWidgets(QWidget *parent = nullptr);
    void createZeroData(vector<double>& x1,vector<double>& y1,
                                        vector<double>& x2,vector<double>& y2);
    void createRandomData(vector<double>& x1,vector<double>& y1,
                          vector<double>& x2,vector<double>& y2);
    void updateSignals();
    void updateSpectra();
    void updatePA();

public slots:

private:
    PortaudioHandler *paHandler;
    NormalRandomGenerator m_nrg{1.0,1.0};
    plot2D* plotSignalChannelLeftRight;      // signal plot for the left and right channel
    plot2D* plotSpectrumChannelLeftRight;         // spectrum plot for the left and right channel
    vector<double> signalTimeLeft;
    vector<double> signalLeft;
    vector<double> signalTimeRight;
    vector<double> signalRight;
    SpectrumCalculator *spectrumCalculator;
};

}

#endif // QCPLOTWIDGETS_H
