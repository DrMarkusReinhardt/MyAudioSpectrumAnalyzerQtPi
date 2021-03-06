#ifndef SIGNALPLOTVIEW_H
#define SIGNALPLOTVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QtCharts/QChartGlobal>
#include "plot2D.h"
#include <vector>
#include "RandomGenerator.h"
#include "PortaudioHandler.h"
#include "SignalAcquisitionThread.h"
#include "callout.h"
#include <eigen3/Eigen/Dense>
#include "ThreadCommon.h"

using namespace Eigen;

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Callout;

namespace MR_SIM
{

class SignalPlotView : public QGraphicsView
{
  Q_OBJECT

public:
  SignalPlotView(double initSampleFrequency,QWidget *parent = nullptr);
  ~SignalPlotView();
  void createZeroData(VectorXd& x1,VectorXd& y1,
                      VectorXd& x2,VectorXd& y2);
  void createRandomData(VectorXd& x1,VectorXd& y1,
                        VectorXd& x2,VectorXd& y2);
  void updateSignals();
  VectorXd returnTimeLeftSignal();
  VectorXd returnTimeRightSignal();
  VectorXd returnLeftSignal();
  VectorXd returnRightSignal();
  void activateLeftChannel();
  void activateRightChannel();
  void deactivateLeftChannel();
  void deactivateRightChannel();
  void stop();

protected:
  void resizeEvent(QResizeEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

public slots:
  void keepCallout();
  void tooltip(QPointF point, bool state);
  void updateMaxSamplesToPlot(double newNoSamples);

private:
  PortaudioHandler *paHandler;
  SignalAcquisitionThread *signalAcquisitionThread;
  NormalRandomGenerator m_nrg{1.0,1.0};
  plot2D* plotSignalChannelLeftRight;      // signal plot for the left and right channel
  VectorXd signalTimeLeft;
  VectorXd signalLeft;
  VectorXd signalTimeRight;
  VectorXd signalRight;
  double m_sampleFrequency;
  double m_samplePeriod;
  double m_noSamplesToPlot;
  QGraphicsSimpleTextItem *m_coordX;
  QGraphicsSimpleTextItem *m_coordY;
  Callout *m_tooltip;
  QList<Callout *> m_callouts;

  // channel activity flags
  bool m_leftChannelActive;
  bool m_rightChannelActive;

  // toggle the signal buffers to read from
  int m_signalBufferIndex;
};

}

#endif // QCPLOTWIDGETS_H
