#ifndef PLOT2D_H
#define PLOT2D_H

#include <vector>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QPen>
#include <QColor>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QGraphicsScene>
#include <eigen3/Eigen/Dense>

using namespace Eigen;

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
  plotData(const VectorXd x, const VectorXd y);
  double m_minValX;
  double m_maxValX;
  double m_minValY;
  double m_maxValY;
  uint16_t size();
  QLineSeries* createSeries(QString color);
  QLineSeries* returnSeries();

private:
  void calcMinMax(const VectorXd x, double* minVal, double* maxVal);
  VectorXd m_x;
  VectorXd m_y;
  QLineSeries* m_series;
};

class plot2D : public QChartView
{
public:
  plot2D(const VectorXd x1, const VectorXd y1);
  plot2D(const VectorXd x1, const VectorXd y1,
         const VectorXd x2, const VectorXd y2);
  void updateData(bool active1, const VectorXd x1, const VectorXd y1,
                  bool active2, const VectorXd x2, const VectorXd y2);
  void setTitle(QString str);
  QLineSeries* returnSeries1();
  QLineSeries* returnSeries2();
  QChart* getChart();

private:
  void addData(plotData &data, QString color);
  plotData *plotData1, *plotData2;
  QChart* createLineChart(plotData &data, QString color);
  VectorXd m_x1, m_y1;
  VectorXd m_x2, m_y2;
  QChart* m_chart;
  uint8_t numberPlotDataPairs;
};

}
#endif // PLOT2D_H
