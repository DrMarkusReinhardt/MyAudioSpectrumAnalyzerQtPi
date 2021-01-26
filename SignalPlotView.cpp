#include <iostream>
#include <vector>
#include <QtWidgets/QGridLayout>
#include <QWidget>
#include "SignalPlotView.h"
#include "DefineDataSizes.h"

using namespace MR_SIM;

SignalPlotView::SignalPlotView(double initSampleFrequency, QWidget *parent)
  : QGraphicsView(new QGraphicsScene, parent), m_sampleFrequency(initSampleFrequency)
{
  // std::cout << "start SignalPlotView" << std::endl;
  m_samplePeriod = 1.0 / m_sampleFrequency;
  m_noSamplesToPlot = (double)sizeDataToPlot/2;

  // const double initSamplePeriod = 0.01;
  const uint16_t arraySize = sizeDataToPlot;

  signalTimeLeft.resize(arraySize);
  signalLeft.resize(arraySize);
  signalTimeRight.resize(arraySize);
  signalRight.resize(arraySize);

  // plot signals
  const QString initLeftSignalLabel = "amplitude left -->";
  const QString initRightSignalLabel = "amplitude right -->";
  const QString initTimeLabel = "time -->";

  // generate ramdom data
  createRandomData(signalTimeLeft, signalLeft, signalTimeRight, signalRight);

  // std::cout << "setup signal plot widget" << std::endl;
  const QString initTitleString = "Left (green) and right (red) channel signal";
  plotSignalChannelLeftRight = new plot2D(signalTimeLeft, signalLeft, signalTimeRight, signalRight);
  plotSignalChannelLeftRight->setTitle(initTitleString);
  QChart* pChart = plotSignalChannelLeftRight->getChart();
  pChart->legend()->hide();
  pChart->createDefaultAxes();
  pChart->axisX()->setRange(0.0,m_noSamplesToPlot*m_samplePeriod*1000.0);
  pChart->setMinimumSize(1200.0,400.0);
  pChart->setMaximumSize(1200.0,400.0);
  scene()->addItem(pChart);

  m_coordX = new QGraphicsSimpleTextItem(pChart);
  m_coordX->setPos(pChart->size().width()/2 - 50, pChart->size().height()-30);
  m_coordX->setText("X: ");
  m_coordY = new QGraphicsSimpleTextItem(pChart);
  m_coordY->setPos(pChart->size().width()/2 + 50, pChart->size().height()-30);
  m_coordY->setText("Y: ");

  // create the Portaudio handler
  // std::cout << "create the Portaudio handler" << std::endl;
  uint16_t initNoFrames = 512;
  uint8_t initNoChannels = 2;
  paHandler = new PortaudioHandler(initNoFrames, initNoChannels, m_sampleFrequency);
  if (paHandler->open())
  {
    std::cout << "stream opened successfully" << std::endl;
  }
  else
    std::cout << "stream open failed" << std::endl;

  // create the signal acquisition thread
  signalAcquisitionThread = new SignalAcquisitionThread(paHandler);
  signalAcquisitionThread->start();
  m_signalBufferIndex = 1;  // start to read from buffer 1, then toggle

  // std::cout << "SignalPlotView created" << std::endl;
}

SignalPlotView::~SignalPlotView()
{
  signalAcquisitionThread->quit();
  signalAcquisitionThread->wait(1100);
  delete signalAcquisitionThread;
  delete paHandler;
  delete plotSignalChannelLeftRight;
}

void SignalPlotView::activateLeftChannel()
{
    m_leftChannelActive = true;
}

void SignalPlotView::activateRightChannel()
{
  m_rightChannelActive = true;
}

void SignalPlotView::deactivateLeftChannel()
{
  m_leftChannelActive = false;
}

void SignalPlotView::deactivateRightChannel()
{
  m_rightChannelActive = false;
}

void SignalPlotView::updateSignals()
{
  std::cout << "Update left and right signal" << std::endl;

  if (m_signalBufferIndex == 1)
  {
      // acquire the semaphore for signal buffer 1 (left and right channels)
      SemSignalBuffer1.acquire(1);
      std::cout << "acquired the semaphore for signal buffer 1" << std::endl;
      if (signalBuffer1Filled)
      {
          std::cout << "signal buffer 1 filled, copy signals" << std::endl;
          // copy the signals
          signalTimeLeft = SignalTimeBuffer1Left;
          signalTimeRight = SignalTimeBuffer1Right;
          signalLeft = SignalBuffer1Left;
          signalRight = SignalBuffer1Right;

          // update the signal plot
          std::cout << "update signal plot, signal buffer 1" << std::endl;
          plotSignalChannelLeftRight->updateData(m_leftChannelActive,signalTimeLeft,signalLeft,
                                                 m_rightChannelActive,signalTimeRight,signalRight);
          QChart* pChart = plotSignalChannelLeftRight->getChart();
          pChart->axisX()->setRange(0.0,m_noSamplesToPlot*m_samplePeriod*1000.0);
          signalBuffer1Filled = false;
          m_signalBufferIndex = 2;
      }
      // release the semaphore for signal buffer 1
      SemSignalBuffer1.release(1);
      std::cout << "released the semaphore for signal buffer 1" << std::endl;
  }
  else if (m_signalBufferIndex == 2)
  {
      // acquire the semaphore for signal buffer 2(left and right channels)
      SemSignalBuffer2.acquire(1);
      std::cout << "acquired the semaphore for signal buffer 2" << std::endl;
      if (signalBuffer2Filled)
      {
          // copy the signals
          signalTimeLeft = SignalTimeBuffer2Left;
          signalTimeRight = SignalTimeBuffer2Right;
          signalLeft = SignalBuffer2Left;
          signalRight = SignalBuffer2Right;

          // update the signal plot
          std::cout << "update signal plot, signal buffer 2" << std::endl;
          plotSignalChannelLeftRight->updateData(m_leftChannelActive,signalTimeLeft,signalLeft,
                                                 m_rightChannelActive,signalTimeRight,signalRight);
          QChart* pChart = plotSignalChannelLeftRight->getChart();
          pChart->axisX()->setRange(0.0,m_noSamplesToPlot*m_samplePeriod*1000.0);
          signalBuffer2Filled = false;
          m_signalBufferIndex = 1;
      }
      // release the semaphore for signal buffer 2
      SemSignalBuffer2.release(1);
      std::cout << "released the semaphore for signal buffer 2" << std::endl;
  }

  // connect(plotSignalChannelLeftRight->returnSeries1(), &QLineSeries::hovered, this, &SignalPlotView::tooltip);
  // connect(plotSignalChannelLeftRight->returnSeries2(), &QLineSeries::hovered, this, &SignalPlotView::tooltip);
}

void SignalPlotView::updateMaxSamplesToPlot(double newNoSamples)
{
    m_noSamplesToPlot = newNoSamples;
    QChart* pChart = plotSignalChannelLeftRight->getChart();
    pChart->legend()->hide();
    pChart->createDefaultAxes();
    pChart->axisX()->setRange(0.0,m_noSamplesToPlot*m_samplePeriod*1000.0);
}

VectorXd SignalPlotView::returnTimeLeftSignal()
{
  return signalTimeLeft;
}

VectorXd SignalPlotView::returnTimeRightSignal()
{
    return signalTimeRight;
}

VectorXd SignalPlotView::returnLeftSignal()
{
  return signalLeft;
}

VectorXd SignalPlotView::returnRightSignal()
{
  return signalRight;
}

void SignalPlotView::createZeroData(VectorXd& x1,VectorXd& y1,
                                    VectorXd& x2,VectorXd& y2)
{
  for(uint16_t k = 0; k < x1.size(); k++)
    x1[k] = (double)k * m_samplePeriod;
  for(uint16_t k = 0; k < y1.size(); k++)
    y1[k] = 0.0;
  for(uint16_t k = 0; k < x2.size(); k++)
    x2[k] = (double)k * m_samplePeriod;
  for(uint16_t k = 0; k < y2.size(); k++)
    y2[k] = 0.0;
}

void SignalPlotView::createRandomData(VectorXd& x1,VectorXd& y1,
                                      VectorXd& x2,VectorXd& y2)
{
  for(uint16_t k = 0; k < x1.size(); k++)
    x1[k] = (double)k * m_samplePeriod;
  vector<double> y1_tmp = m_nrg.generate(y1.size());
  for(uint16_t k = 0; k < y1.size(); k++)
    y1[k] = y1_tmp[k];
  for(uint16_t k = 0; k < x2.size(); k++)
    x2[k] = (double)k * m_samplePeriod;
  vector<double> y2_tmp = m_nrg.generate(y2.size());
  for(uint16_t k = 0; k < y2.size(); k++)
    y2[k] = y2_tmp[k];
}

void SignalPlotView::resizeEvent(QResizeEvent *event)
{
  // std::cout << "SignalPlotView::resizeEvent" << std::endl;
  if (scene())
  {
    scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
    plotSignalChannelLeftRight->resize(event->size());
    m_coordX->setPos(plotSignalChannelLeftRight->size().width()/2 - 50, plotSignalChannelLeftRight->size().height()-30);
    m_coordY->setPos(plotSignalChannelLeftRight->size().width()/2 + 50, plotSignalChannelLeftRight->size().height()-30);

    const auto callouts = m_callouts;
    for (Callout *callout : callouts)
      callout->updateGeometry();
  }
  QGraphicsView::resizeEvent(event);
}

void SignalPlotView::mouseMoveEvent(QMouseEvent *event)
{
  // std::cout << "SignalPlotView::mouseMoveEvent" << std::endl;
  m_coordX->setText(QString("X: %1").arg(plotSignalChannelLeftRight->getChart()->mapToValue(event->pos()).x()));
  m_coordY->setText(QString("Y: %1").arg(plotSignalChannelLeftRight->getChart()->mapToValue(event->pos()).y()));
  QGraphicsView::mouseMoveEvent(event);
}

void SignalPlotView::keepCallout()
{
  m_callouts.append(m_tooltip);
  m_tooltip = new Callout(plotSignalChannelLeftRight->getChart());
}

void SignalPlotView::tooltip(QPointF point, bool state)
{
  std::cout << "SignalPlotView: tooltip start" << std::endl;
  if (m_tooltip == 0)
    m_tooltip = new Callout(plotSignalChannelLeftRight->getChart());

  if (state)
  {
    std::cout << "SignalPlotView: tooltip true state" << std::endl;
    m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
    m_tooltip->setAnchor(point);
    m_tooltip->setZValue(11);
    m_tooltip->updateGeometry();
    m_tooltip->show();
  }
  else
  {
    std::cout << "SignalPlotView: tooltip false state" << std::endl;
    m_tooltip->hide();
  }
}

void SignalPlotView::stop()
{
  signalAcquisitionThread->quit();
  signalAcquisitionThread->terminate();
  signalAcquisitionThread->wait(1100);
}

