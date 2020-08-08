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
  VectorXd spectrumFrequencyLeft(arraySizeSpectrum);
  VectorXd spectrumMagnitudeLeft(arraySizeSpectrum);
  VectorXd spectrumFrequencyRight(arraySizeSpectrum);
  VectorXd spectrumMagnitudeRight(arraySizeSpectrum);

  // plot spectra
  const QString initLeftSpectrumLabel = "magnitude spectrum left -->";
  const QString initRightSpectrumLabel = "magnitude spectrum right -->";
  const QString initFrequencyLabel = "frequency -->";

  // generate random data
  createRandomData(spectrumFrequencyLeft,spectrumMagnitudeLeft,spectrumFrequencyRight,spectrumMagnitudeRight);

  // setup spectrum plot widget
  // std::cout << "setup spectrum plot widget" << std::endl;
  const QString initTitleStringSpectrumLeft = "Left (green) and right (red) channel magnitude spectrum";
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
  // std::cout << "create tooltip" << std::endl;
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


void SpectrumPlotView::activateLeftChannel()
{
    m_leftChannelActive = true;
}

void SpectrumPlotView::activateRightChannel()
{
  m_rightChannelActive = true;
}

void SpectrumPlotView::deactivateLeftChannel()
{
  m_leftChannelActive = false;
}

void SpectrumPlotView::deactivateRightChannel()
{
  m_rightChannelActive = false;
}

void SpectrumPlotView::getSignals(VectorXd x1,VectorXd y1,
                                  VectorXd x2,VectorXd y2)
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
  spectrumCalculatorLeft->getMaxMagnitudeSpectrum(m_maxMagnitudeLeft, m_maxFrequencyValueLeft);
  // std::cout << "maxMagnitudeLeft = " << maxMagnitudeLeft << std::endl;

  spectrumCalculatorRight->calculateSpectrum(signalRight);
  spectrumCalculatorRight->getMaxMagnitudeSpectrum(m_maxMagnitudeRight, m_maxFrequencyValueRight);
  // std::cout << "maxMagnitudeRight = " << maxMagnitudeRight << std::endl;

  // normalize spectra
  normalizeSpectra();
  
  VectorXd magnitudeSpectrumLeft = spectrumCalculatorLeft->returnMagnitudeSpectrum();
  VectorXd magnitudeSpectrumRight = spectrumCalculatorRight->returnMagnitudeSpectrum();

  VectorXd frequencyRange = m_spectrumParameter->getFrequencyRange();
  plotSpectrumChannelLeftRight->updateData(m_leftChannelActive, frequencyRange, magnitudeSpectrumLeft,
                                           m_rightChannelActive, frequencyRange, magnitudeSpectrumRight);
  QChart* pChart = plotSpectrumChannelLeftRight->getChart();
  pChart->axisY()->setRange(-100.0,0.0);
  pChart->axisX()->setRange(m_spectrumParameter->getMinFrequency(),
                            m_spectrumParameter->getMaxFrequency());

  // with new series added to the chart, it is necessary to update the signal connections for the hovering mouse
  // connect the hovering over the first data series of the spectrum chart with the tooltip display
  if (m_leftChannelActive)
    connect(plotSpectrumChannelLeftRight->returnSeries1(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
  // connect the hovering over the second data series of the spectrum chart with the tooltip display
  if (m_rightChannelActive)
    connect(plotSpectrumChannelLeftRight->returnSeries2(), &QLineSeries::hovered, this, &SpectrumPlotView::tooltip);
  // connect the clicking on the first data series of the spectrum chart with the routine to keep the callout
  if (m_leftChannelActive)
    connect(plotSpectrumChannelLeftRight->returnSeries1(), &QLineSeries::clicked, this, &SpectrumPlotView::keepCallout);
  // connect the clicking on the second data series of the spectrum chart with the routine to keep the callout
  if (m_rightChannelActive)
    connect(plotSpectrumChannelLeftRight->returnSeries2(), &QLineSeries::clicked, this, &SpectrumPlotView::keepCallout);
}

void SpectrumPlotView::setNormalizationMode(uint8_t newNormalizationMode)
{
  m_normalizationMode = newNormalizationMode;
}

void SpectrumPlotView::normalizeSpectra()
{
  double newMagnitudeNormalizationValueLeft;
  double newMagnitudeNormalizationValueRight;
  
  // determine the normalization values for both channels depending on the normalization mode
  switch (m_normalizationMode)
  {
    case 0: // with max of both channels
    {
      double overallMaxMagnitude = max(m_maxMagnitudeLeft,m_maxMagnitudeRight);
      newMagnitudeNormalizationValueLeft = overallMaxMagnitude;
      newMagnitudeNormalizationValueRight = overallMaxMagnitude;
      break;
    }
    case 1:  // with max of left channel
    {
      newMagnitudeNormalizationValueLeft = m_maxMagnitudeLeft;
      newMagnitudeNormalizationValueRight = m_maxMagnitudeLeft;
      break;
    }  
    case 2:  // with max of right channel
    {
      newMagnitudeNormalizationValueLeft = m_maxMagnitudeRight;
      newMagnitudeNormalizationValueRight = m_maxMagnitudeRight;
      break;
    }  
    case 3:  // with max of each channel separately
    {
      newMagnitudeNormalizationValueLeft = m_maxMagnitudeLeft;
      newMagnitudeNormalizationValueRight = m_maxMagnitudeRight;
      break;
    }
    default:
    {
      double overallMaxMagnitude = max(m_maxMagnitudeLeft,m_maxMagnitudeRight);
      newMagnitudeNormalizationValueLeft = overallMaxMagnitude;
      newMagnitudeNormalizationValueRight = overallMaxMagnitude;
    }
  }
  
  // do the normalization of both channel spectra
  spectrumCalculatorLeft->normalizeMagnitudeSpectrumVal(newMagnitudeNormalizationValueLeft);
  spectrumCalculatorRight->normalizeMagnitudeSpectrumVal(newMagnitudeNormalizationValueRight);
}

void SpectrumPlotView::updateMinFrequency(double newMinFrequency)
{
   // std::cout << " new min. frequency = " << newMinFrequency << std::endl;
   m_spectrumParameter->setMinFrequency(newMinFrequency);
   delete spectrumCalculatorLeft;
   delete spectrumCalculatorRight;
   spectrumCalculatorLeft = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   spectrumCalculatorRight = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   updateSpectra();
}

void SpectrumPlotView::updateMaxFrequency(double newMaxFrequency)
{
   // std::cout << " new max. frequency = " << newMaxFrequency << std::endl;
   m_spectrumParameter->setMaxFrequency(newMaxFrequency);
   delete spectrumCalculatorLeft;
   delete spectrumCalculatorRight;
   spectrumCalculatorLeft = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   spectrumCalculatorRight = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
   updateSpectra();
}

void SpectrumPlotView::createZeroData(VectorXd& x1,VectorXd& y1,
                                      VectorXd& x2,VectorXd& y2)
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

void SpectrumPlotView::createRandomData(VectorXd& x1,VectorXd& y1,
                                        VectorXd& x2,VectorXd& y2)
{
    for(uint16_t k = 0; k < x1.size(); k++)
      x1[k] = (double)k * 1.0;
    vector<double> y1_tmp = m_nrg.generate(y1.size());
    for(uint16_t k = 0; k < y1.size(); k++)
      y1[k] = y1_tmp[k];
    for(uint16_t k = 0; k < x2.size(); k++)
      x2[k] = (double)k * 1.0;
    vector<double> y2_tmp = m_nrg.generate(y2.size());
    for(uint16_t k = 0; k < y2.size(); k++)
      y2[k] = y2_tmp[k];
}

VectorXd SpectrumPlotView::returnFrequencyRange()
{
   return spectrumCalculatorLeft->returnFrequencyRange();
}

VectorXd SpectrumPlotView::returnMagnitudeSpectrumLeft()
{
   return spectrumCalculatorLeft->returnMagnitudeSpectrum();
}

VectorXd SpectrumPlotView::returnMagnitudeSpectrumRight()
{
    return spectrumCalculatorRight->returnMagnitudeSpectrum();
}

void SpectrumPlotView::getMaxMagnitudeSpectrumLeft(double& maximumMagnitudeValue, double& maxFrequencyValue)
{
  spectrumCalculatorLeft->getMaxMagnitudeSpectrum(maximumMagnitudeValue, maxFrequencyValue);
}

void SpectrumPlotView::getMaxMagnitudeSpectrumRight(double& maximumMagnitudeValue, double& maxFrequencyValue)
{
  spectrumCalculatorRight->getMaxMagnitudeSpectrum(maximumMagnitudeValue, maxFrequencyValue);
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
