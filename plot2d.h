#ifndef PLOT2D_H
#define PLOT2D_H

#include <vector>
#include <QObject>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QGraphicsScene>

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

using namespace std;

namespace MR_SIM
{

class plotData
{
public:
    plotData(const vector<double> x, const vector<double> y);
    double m_minValX;
    double m_maxValX;
    double m_minValY;
    double m_maxValY;
    uint16_t size();
    QLineSeries* createSeries() const;

private:
    void calcMinMax(const vector<double> x, double* minVal, double* maxVal);
    vector<double> m_x;
    vector<double> m_y;
};

class plot2D : public QChartView
{
public:
    plot2D(const vector<double> x1, const vector<double> y1);
    plot2D(const vector<double> x1, const vector<double> y1,
           const vector<double> x2, const vector<double> y2);
    void updateData(const vector<double> x1, const vector<double> y1,
                                 const vector<double> x2, const vector<double> y2);
    void setTitle(QString str);

private:
    void addData(const plotData &data);
    plotData *plotData1, *plotData2;
    QChart* createLineChart(const plotData &data);
    vector<double> m_x1, m_y1;
    vector<double> m_x2, m_y2;
    QChart* m_chart;
    uint8_t numberPlotDataPairs;
};

}
#endif // PLOT2D_H
