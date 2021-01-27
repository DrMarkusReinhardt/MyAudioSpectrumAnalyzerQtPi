#include <iostream>
#include <vector>
#include <QtWidgets/QGridLayout>
#include <QWidget>
#include "SpectrumPlotView.h"
#include "DefineDataSizes.h"
#include "SpectrumParameter.h"
#include "ThreadCommon.h"

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

  // create the spectrum calculator thread and start it
  m_spectrumCalculationThread = new SpectrumCalculationThread(initSampleFrequency, initSpectrumParameter);
  m_spectrumCalculationThread->start();
  m_spectrumBufferIndex = 1;
}

SpectrumPlotView::~SpectrumPlotView()
{
    m_spectrumCalculationThread->quit();
    m_spectrumCalculationThread->terminate();
    m_spectrumCalculationThread->wait(1100);
    delete m_spectrumCalculationThread;
    delete plotSpectrumChannelLeftRight;
}

void SpectrumPlotView::stop()
{
    m_spectrumCalculationThread->quit();
    m_spectrumCalculationThread->terminate();
    m_spectrumCalculationThread->wait(1100);
}

void SpectrumPlotView::restart()
{

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
  std::cout << "SpectrumPlotView: updateSpectra" << std::endl;
  // get the average spectra
  bool gotSpectra = false;
  if (m_spectrumBufferIndex == 1)
  {
      std::cout << "SpectrumPlotView: acquire the semaphore for spectrum buffer 1" << std::endl;
      SemSpectrumBuffer1.acquire(1);
      gotSpectra = false;
      if (spectrumBuffer1Filled)
      {
          std::cout << "SpectrumPlotView: copy spectrum buffer 1" << std::endl;
          m_averageMagnitudeSpectrumLeft = SpectrumBuffer1Left;
          m_averageMagnitudeSpectrumRight = SpectrumBuffer1Right;
          m_spectrumBufferIndex = 2;
          spectrumBuffer1Filled = false;
          gotSpectra = true;
      }
      SemSpectrumBuffer1.release(1);
      std::cout << "SpectrumPlotView: release the semaphore for spectrum buffer 1" << std::endl;
  }
  else if (m_spectrumBufferIndex == 2)
  {
      std::cout << "SpectrumPlotView: acquire the semaphore for spectrum buffer 2" << std::endl;
      SemSpectrumBuffer2.acquire(1);
      gotSpectra = false;
      if (spectrumBuffer2Filled)
      {
          std::cout << "SpectrumPlotView: copy spectrum buffer 2" << std::endl;
          m_averageMagnitudeSpectrumLeft = SpectrumBuffer2Left;
          m_averageMagnitudeSpectrumRight = SpectrumBuffer2Right;
          m_spectrumBufferIndex = 1;
          spectrumBuffer2Filled = false;
          gotSpectra = true;
      }
      SemSpectrumBuffer2.release(1);
      std::cout << "SpectrumPlotView: release the semaphore for spectrum buffer 2" << std::endl;
  }
  std::cout << "SpectrumPlotView: got spectra = " << gotSpectra << std::endl;

  if (gotSpectra)
  {
    std::cout << "SpectrumPlotView: got spectra to display" << std::endl;
    VectorXd frequencyRange = m_spectrumParameter->getFrequencyRange();
    
    plotSpectrumChannelLeftRight->updateData(m_leftChannelActive, frequencyRange, m_averageMagnitudeSpectrumLeft,
                                             m_rightChannelActive, frequencyRange, m_averageMagnitudeSpectrumRight);
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
}

void SpectrumPlotView::setNormalizationMode(uint8_t newNormalizationMode)
{
  m_normalizationMode = newNormalizationMode;
}

void SpectrumPlotView::updateMinFrequency(double newMinFrequency)
{
   // std::cout << " new min. frequency = " << newMinFrequency << std::endl;
   m_spectrumParameter->setMinFrequency(newMinFrequency);

   // set the (updated) spectrum parameter
   SemSpectrumParameter.acquire(1);
   spectrumParameter = m_spectrumParameter;
   resetSpectrumThreadFlag = true; // the spectrum calculation thread shall reset
   SemSpectrumParameter.release(1);
}

void SpectrumPlotView::updateMaxFrequency(double newMaxFrequency)
{
   // std::cout << " new max. frequency = " << newMaxFrequency << std::endl;
   m_spectrumParameter->setMaxFrequency(newMaxFrequency);

   // set the (updated) spectrum parameter
   SemSpectrumParameter.acquire(1);
   spectrumParameter = m_spectrumParameter;
   resetSpectrumThreadFlag = true; // the spectrum calculation thread shall reset
   SemSpectrumParameter.release(1);
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

void SpectrumPlotView::getMaxMagnitudeSpectrumLeft(double& maximumMagnitudeValue, double& maxFrequencyValue)
{
    maximumMagnitudeValue = -160.0;
    maxFrequencyValue = 0.0;
    m_frequencyRange = m_spectrumParameter->getFrequencyRange();
    for(uint16_t k = 0; k < m_averageMagnitudeSpectrumLeft.size(); k++)
    {
      if (m_averageMagnitudeSpectrumLeft[k] > maximumMagnitudeValue)
      {
        maximumMagnitudeValue = m_averageMagnitudeSpectrumLeft[k];
        maxFrequencyValue = m_frequencyRange[k];
      }
    }
}

void SpectrumPlotView::getMaxMagnitudeSpectrumRight(double& maximumMagnitudeValue, double& maxFrequencyValue)
{
    maximumMagnitudeValue = -160.0;
    maxFrequencyValue = 0.0;
    m_frequencyRange = m_spectrumParameter->getFrequencyRange();
    for(uint16_t k = 0; k < m_averageMagnitudeSpectrumRight.size(); k++)
    {
      if (m_averageMagnitudeSpectrumRight[k] > maximumMagnitudeValue)
      {
        maximumMagnitudeValue = m_averageMagnitudeSpectrumRight[k];
        maxFrequencyValue = m_frequencyRange[k];
      }
    }
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

VectorXd SpectrumPlotView::returnFrequencyRange()
{
  return m_frequencyRange;
}

VectorXd SpectrumPlotView::returnMagnitudeSpectrumLeft()
{
  return m_averageMagnitudeSpectrumLeft;
}

VectorXd SpectrumPlotView::returnMagnitudeSpectrumRight()
{
  return m_averageMagnitudeSpectrumRight;
}
