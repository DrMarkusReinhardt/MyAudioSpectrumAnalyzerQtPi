#include <iostream>
#include <vector>
#include <QtWidgets/QGridLayout>
#include <QWidget>
#include "SpectrumPlotView.h"
#include "DefineDataSizes.h"
#include "SpectrumParameter.h"

using namespace MR_SIM;

SpectrumPlotView::SpectrumPlotView(double initSampleFrequency, SpectrumParameter *initSpectrumParameter,
                                   QWidget *parent)
  : QGraphicsView(new QGraphicsScene, parent), m_sampleFrequency(initSampleFrequency),
    m_spectrumParameter(initSpectrumParameter)
{
  // std::cout << "start SpectrumPlotView" << std::endl;

  // create data arrays for the initial display
  const uint16_t arraySizeSpectrum = m_spectrumParameter->getNoFrequencySamples();
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
  plotSpectrumChannelLeftRight = new plot2D(spectrumFrequencyLeft,spectrumMagnitudeLeft,spectrumFrequencyRight,spectrumMagnitudeRight);
  plotSpectrumChannelLeftRight->setTitle(initTitleStringSpectrumLeft);
  QChart* pChart = plotSpectrumChannelLeftRight->getChart();
  pChart->legend()->hide();
  pChart->createDefaultAxes();
  pChart->setMinimumSize(1200.0,400.0);
  pChart->setMaximumSize(1200.0,400.0);
  pChart->axisY()->setRange(-100.0,0.0);
  pChart->axisX()->setRange(m_spectrumParameter->getMinFrequency(),
                            m_spectrumParameter->getMaxFrequency());
  scene()->addItem(pChart);

  // initialize the first tooltip
  std::cout << "create tooltip" << std::endl;
  m_tooltip = new Callout(pChart);

  m_coordX = new QGraphicsSimpleTextItem(pChart);
  m_coordX->setPos(pChart->size().width()/2 - 50, pChart->size().height()-30);
  m_coordX->setText("X: ");
  m_coordY = new QGraphicsSimpleTextItem(pChart);
  m_coordY->setPos(pChart->size().width()/2 + 50, pChart->size().height()-30);
  m_coordY->setText("Y: ");

  // create the spectrum calculators
  spectrumCalculatorLeft = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
  spectrumCalculatorRight = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
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
  // VectorXd frequencyRange = spectrumCalculator->returnFrequencyRange();
  spectrumCalculatorLeft->calculateSpectrum(signalLeft);
  double maxMagnitudeLeft = spectrumCalculatorLeft->getMaxMagnitudeSpectrum();
  // std::cout << "maxMagnitudeLeft = " << maxMagnitudeLeft << std::endl;

  spectrumCalculatorRight->calculateSpectrum(signalRight);
  double maxMagnitudeRight = spectrumCalculatorRight->getMaxMagnitudeSpectrum();
  // std::cout << "maxMagnitudeRight = " << maxMagnitudeRight << std::endl;

  // normalize spectra
  double overallMaxMagnitude = max(maxMagnitudeLeft,maxMagnitudeRight);
  // std::cout << "overallMaxMagnitude = " << overallMaxMagnitude << std::endl;
  spectrumCalculatorLeft->normalizeMagnitudeSpectrumVal(overallMaxMagnitude);
  spectrumCalculatorRight->normalizeMagnitudeSpectrumVal(overallMaxMagnitude);

  VectorXd magnitudeSpectrumLeft = spectrumCalculatorLeft->returnMagnitudeSpectrum();
  VectorXd magnitudeSpectrumRight = spectrumCalculatorRight->returnMagnitudeSpectrum();

  VectorXd frequencyRange = m_spectrumParameter->getFrequencyRange();
  plotSpectrumChannelLeftRight->updateData(frequencyRange, magnitudeSpectrumLeft,
                                           frequencyRange, magnitudeSpectrumRight);
  QChart* pChart = plotSpectrumChannelLeftRight->getChart();
  pChart->axisY()->setRange(-100.0,0.0);
  pChart->axisX()->setRange(m_spectrumParameter->getMinFrequency(),
                            m_spectrumParameter->getMaxFrequency());

  // with new series added to the chart, it is necessary to update the signal connections for the hovering mouse
  // connect the hovering over the first data series of the spectrum chart with the tooltip display
  connect(plotSpectrumChannelLeftRight->returnSeries1(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
  // connect the hovering over the second data series of the spectrum chart with the tooltip display
  connect(plotSpectrumChannelLeftRight->returnSeries2(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
  // connect the clicking on the first data series of the spectrum chart with the routine to keep the callout
  connect(plotSpectrumChannelLeftRight->returnSeries1(), &QLineSeries::clicked, this, &SpectrumPlotView::keepCallout);
  // connect the clicking on the second data series of the spectrum chart with the routine to keep the callout
  connect(plotSpectrumChannelLeftRight->returnSeries2(), &QLineSeries::clicked, this, &SpectrumPlotView::keepCallout);
}

void SpectrumPlotView::updateMinFrequency(double newMinFrequency)
{
   std::cout << " new min. frequency = " << newMinFrequency << std::endl;
   m_spectrumParameter->setMinFrequency(newMinFrequency);
   delete spectrumCalculatorLeft;
   delete spectrumCalculatorRight;
   spectrumCalculatorLeft = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   spectrumCalculatorRight = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   updateSpectra();
}

void SpectrumPlotView::updateMaxFrequency(double newMaxFrequency)
{
   std::cout << " new max. frequency = " << newMaxFrequency << std::endl;
   m_spectrumParameter->setMaxFrequency(newMaxFrequency);
   delete spectrumCalculatorLeft;
   delete spectrumCalculatorRight;
   spectrumCalculatorLeft = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   spectrumCalculatorRight = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   updateSpectra();
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
    m_coordX->setPos(plotSpectrumChannelLeftRight->size().width()/2 - 50,
                     plotSpectrumChannelLeftRight->size().height()-30);
    m_coordY->setPos(plotSpectrumChannelLeftRight->size().width()/2 + 50,
                     plotSpectrumChannelLeftRight->size().height()-30);

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
  if (state)
  {
      m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
      m_tooltip->setAnchor(point);
      m_tooltip->setZValue(11);
      m_tooltip->updateGeometry();
      m_tooltip->show();
  }
  else
  {
      m_tooltip->hide();
  }
}
