#include <iostream>
#include <vector>
#include <QtWidgets/QGridLayout>
#include <QWidget>
#include "SpectrumPlotView.h"
#include "DefineDataSizes.h"

using namespace MR_SIM;

SpectrumPlotView::SpectrumPlotView(QWidget *parent)
  : QGraphicsView(new QGraphicsScene, parent)
{
  std::cout << "start SpectrumPlotView" << std::endl;

  const uint16_t arraySizeSpectrum = 400;
  vector<double> spectrumFrequencyLeft(arraySizeSpectrum);
  vector<double> spectrumMagnitudeLeft(arraySizeSpectrum);
  vector<double> spectrumFrequencyRight(arraySizeSpectrum);
  vector<double> spectrumMagnitudeRight(arraySizeSpectrum);

  // plot spectra
  const QString initLeftSpectrumLabel = "magnitude spectrum left -->";
  const QString initRightSpectrumLabel = "magnitude spectrum right -->";
  const QString initFrequencyLabel = "frequency -->";

  // generate random data
  createRandomData(spectrumFrequencyLeft,spectrumMagnitudeLeft,spectrumFrequencyRight,spectrumMagnitudeRight);

  // setup spectrum plot widget
  std::cout << "setup spectrum plot widget" << std::endl;
  const QString initTitleStringSpectrumLeft = "Left and right channel magnitude spectrum";
  plotSpectrumChannelLeftRight = new plot2D(spectrumFrequencyLeft,spectrumMagnitudeLeft);
  plotSpectrumChannelLeftRight->setTitle(initTitleStringSpectrumLeft);
  scene()->addItem(plotSpectrumChannelLeftRight->getChart());

  QChart* chart = plotSpectrumChannelLeftRight->getChart();
  m_coordX = new QGraphicsSimpleTextItem(chart);
  m_coordX->setPos(chart->size().width()/2 - 50, chart->size().height()-30);
  m_coordX->setText("X: ");
  m_coordY = new QGraphicsSimpleTextItem(chart);
  m_coordY->setPos(chart->size().width()/2 + 50, chart->size().height()-30);
  m_coordY->setText("Y: ");

  // connect the slots:
  // std::cout << "connect the slots" << std::endl;
  // connect the hovering over the first data series of the spectrum chart with the tooltip display
  // connect(plotSpectrumChannelLeftRight->returnSeries1(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
  // connect the hovering over the second data series of the spectrum chart with the tooltip display
  // connect(plotSpectrumChannelLeftRight->returnSeries2(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
  // connect the clicking on the first data series of the spectrum chart with the routine to keep the callout
  // connect(plotSpectrumChannelLeftRight->returnSeries1(), &QLineSeries::clicked, this, &SpectrumPlotView::keepCallout);
  // connect the clicking on the second data series of the spectrum chart with the routine to keep the callout
  // connect(plotSpectrumChannelLeftRight->returnSeries2(), &QLineSeries::clicked, this, &SpectrumPlotView::keepCallout);

  // create the spectrum calculator
  spectrumCalculator = new SpectrumCalculator(arraySizeSpectrum);
  std::cout << "SpectrumPlotView created" << std::endl;
}

void SpectrumPlotView::getSignals(vector<double> x1,vector<double> y1,
                                  vector<double> x2,vector<double> y2)
{
   signalTimeLeft = x1;
   signalLeft = y1;
   signalTimeRight = x2;
   signalRight = y2;
}

void SpectrumPlotView::updateSpectra()
{
  VectorXd frequencyRange = spectrumCalculator->returnFrequencyRange();
  spectrumCalculator->calculateSpectrum(signalLeft);
  spectrumCalculator->normalizeMagnitudeSpectrum();
  VectorXd magnitudeSpectrumLeft = spectrumCalculator->returnMagnitudeSpectrum();
  spectrumCalculator->calculateSpectrum(signalRight);
  spectrumCalculator->normalizeMagnitudeSpectrum();
  VectorXd magnitudeSpectrumRight = spectrumCalculator->returnMagnitudeSpectrum();
  plotSpectrumChannelLeftRight->updateData(frequencyRange,magnitudeSpectrumLeft,frequencyRange,magnitudeSpectrumRight);
  connect(plotSpectrumChannelLeftRight->returnSeries1(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
  connect(plotSpectrumChannelLeftRight->returnSeries2(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
}

void SpectrumPlotView::createZeroData(vector<double>& x1,vector<double>& y1,
                                      vector<double>& x2,vector<double>& y2)
{
  for(uint16_t k = 0; k < x1.size(); k++)
    x1[k] = k*1.0;
  for(uint16_t k = 0; k < y1.size(); k++)
    y1[k] = 0.0;
  for(uint16_t k = 0; k < x2.size(); k++)
    x2[k] = k*1.0;
  for(uint16_t k = 0; k < y2.size(); k++)
    y2[k] = 0.0;
}

void SpectrumPlotView::createRandomData(vector<double>& x1,vector<double>& y1,
                                        vector<double>& x2,vector<double>& y2)
{
  y1 = m_nrg.generate(y1.size());
  for(uint16_t k = 0; k < x1.size(); k++)
    x1[k] = k*1.0;
  y2 = m_nrg.generate(y2.size());
  for(uint16_t k = 0; k < x2.size(); k++)
    x2[k] = k*1.0;
}

void SpectrumPlotView::resizeEvent(QResizeEvent *event)
{
  // std::cout << "SpectrumPlotView::resizeEvent" << std::endl;
  if (scene())
  {
    scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
    plotSpectrumChannelLeftRight->resize(event->size());
    m_coordX->setPos(plotSpectrumChannelLeftRight->size().width()/2 - 50, plotSpectrumChannelLeftRight->size().height()-30);
    m_coordY->setPos(plotSpectrumChannelLeftRight->size().width()/2 + 50, plotSpectrumChannelLeftRight->size().height()-30);

    const auto callouts = m_callouts;
    for (Callout *callout : callouts)
      callout->updateGeometry();

  }
  QGraphicsView::resizeEvent(event);
}

void SpectrumPlotView::mouseMoveEvent(QMouseEvent *event)
{
  // std::cout << "SpectrumPlotView::mouseMoveEvent" << std::endl;
  m_coordX->setText(QString("X: %1").arg(plotSpectrumChannelLeftRight->getChart()->mapToValue(event->pos()).x()));
  m_coordY->setText(QString("Y: %1").arg(plotSpectrumChannelLeftRight->getChart()->mapToValue(event->pos()).y()));
  QGraphicsView::mouseMoveEvent(event);
}

void SpectrumPlotView::keepCallout()
{
  m_callouts.append(m_tooltip);
  m_tooltip = new Callout(plotSpectrumChannelLeftRight->getChart());
}

void SpectrumPlotView::tooltip(QPointF point, bool state)
{
  // std::cout << "SpectrumPlotView: tooltip start" << std::endl;
  if (m_tooltip == 0)
    m_tooltip = new Callout(plotSpectrumChannelLeftRight->getChart());

  if (state)
  {
    // std::cout << "SpectrumPlotView: tooltip true state" << std::endl;
    m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
    m_tooltip->setAnchor(point);
    m_tooltip->setZValue(11);
    m_tooltip->updateGeometry();
    m_tooltip->show();
  }
  else
  {
    // std::cout << "SpectrumPlotView: tooltip false state" << std::endl;
    m_tooltip->hide();
  }
}
