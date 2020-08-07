#include "plot2D.h"
#include <iostream>
#include <stdlib.h>

using namespace MR_SIM;

plotData::plotData(const VectorXd x, const VectorXd y) : m_x(x), m_y(y)
{
  calcMinMax(m_x,&m_minValX,&m_maxValX);
  calcMinMax(m_y,&m_minValY,&m_maxValY);
}

uint16_t plotData::size()
{
  return m_x.size();
}

QLineSeries* plotData::createSeries(QString color)
{
  QLineSeries* series = new QLineSeries();
  for(uint16_t k = 0; k < m_x.size(); k++)
    series->append(m_x[k],m_y[k]);
  QPen pen = series->pen();
  pen.setColor(color);
  series->setPen(pen);
  m_series = series;
  return series;
}

QLineSeries* plotData::returnSeries()
{
  return m_series;
}

void plotData::calcMinMax(const VectorXd x, double* minVal, double* maxVal)
{
  *minVal = x[0];
  *maxVal = x[0];
  for(uint16_t k = 1; k < x.size(); k++)
  {
    *minVal = min(*minVal,x[k]);
    *maxVal = max(*maxVal,x[k]);
  }
}


plot2D::plot2D(const VectorXd x1, const VectorXd y1) : m_x1(x1), m_y1(y1)
{
  if (x1.size() != y1.size())
  {
    std::cout << "plot2D: size of array x and array y must be equal" << std::endl;
    exit(1);
  }
  numberPlotDataPairs = 1;
  plotData1 = new plotData(m_x1,m_y1);
  m_chart = new QChart();
  addData(*plotData1,"green");
  m_chart->setAcceptHoverEvents(true);
  m_chart->setMinimumSize(1200, 400);
  QBrush backgroundBrush(Qt::black);
  m_chart->setBackgroundBrush(backgroundBrush);
  m_chart->createDefaultAxes();
  setChart(m_chart);
  setRenderHint(QPainter::Antialiasing);
}

plot2D::plot2D(const VectorXd x1, const VectorXd y1,
               const VectorXd x2, const VectorXd y2
              ) : m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2)
{
  if (x1.size() != y1.size())
  {
    std::cout << "plot2D(x1,y1,x2,y2): size of array x1 and array y1 must be equal" << std::endl;
    exit(1);
  }
  if (x2.size() != y2.size())
  {
    std::cout << "plot2D(x1,y1,x2,y2): size of array x2 and array y2 must be equal" << std::endl;
    exit(1);
  }

  numberPlotDataPairs = 2;
  m_chart = new QChart();
  plotData1 = new plotData(m_x1,m_y1);
  addData(*plotData1,"green");
  m_chart->createDefaultAxes();
  plotData2 = new plotData(m_x2,m_y2);
  addData(*plotData2,"red");
  m_chart->createDefaultAxes();
  m_chart->setAcceptHoverEvents(true);
  QBrush backgroundBrush(Qt::black);
  m_chart->setBackgroundBrush(backgroundBrush);
  setChart(m_chart);
  setRenderHint(QPainter::Antialiasing);
}

void plot2D::setTitle(QString str)
{
  m_chart->setTitle(str);
}

QChart* plot2D::getChart()
{
  return m_chart;
}

void plot2D::addData(plotData &data, QString color)
{
  m_chart->addSeries(data.createSeries(color));
  // m_chart->createDefaultAxes();
  // m_chart->axes(Qt::Horizontal).first()->setRange(data.m_minValX, data.m_maxValX);
  // m_chart->axes(Qt::Vertical).first()->setRange(data.m_minValY, data.m_maxValY);
}

void plot2D::updateData(const VectorXd x1, const VectorXd y1,
                        const VectorXd x2, const VectorXd y2)
{
  // std::cout << "updateData: size of vector x1 = " << x1.size() << std::endl;
  // std::cout << "updateData: size of vector y1 = " << y1.size() << std::endl;
  m_chart->removeAllSeries();
  plotData1 = new plotData(x1,y1);
  addData(*plotData1,"green");
  m_chart->createDefaultAxes();
  plotData2 = new plotData(x2,y2);
  addData(*plotData2,"red");
  m_chart->createDefaultAxes();
  m_chart->setAcceptHoverEvents(true);
  setChart(m_chart);
  setRenderHint(QPainter::Antialiasing);
  m_chart->show();
}

QLineSeries* plot2D::returnSeries1()
{
  return plotData1->returnSeries();
}

QLineSeries* plot2D::returnSeries2()
{
  return plotData2->returnSeries();
}


QChart *plot2D::createLineChart(plotData &data, QString color)
{
  QChart *chart = new QChart();
  chart->addSeries(data.createSeries(color));
  chart->legend()->hide();
  // chart->createDefaultAxes();
  // chart->axes(Qt::Horizontal).first()->setRange(data.m_minValX, data.m_maxValX);
  // chart->axes(Qt::Vertical).first()->setRange(data.m_minValY, data.m_maxValY);
  // Add space to label to add space between labels and axis
  QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
  Q_ASSERT(axisY);
  axisY->setLabelFormat("%.1f  ");
  return chart;
}

