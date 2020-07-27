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
#include "callout.h"

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
  void createZeroData(vector<double>& x1,vector<double>& y1,
                      vector<double>& x2,vector<double>& y2);
  void createRandomData(vector<double>& x1,vector<double>& y1,
                        vector<double>& x2,vector<double>& y2);
  void updateSignals();
  void updatePA();
  vector<double> returnTimeLeftSignal();
  vector<double> returnTimeRightSignal();
  vector<double> returnLeftSignal();
  vector<double> returnRightSignal();

protected:
  void resizeEvent(QResizeEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

public slots:
  void keepCallout();
  void tooltip(QPointF point, bool state);

private:
  double m_sampleFrequency;
  PortaudioHandler *paHandler;
  NormalRandomGenerator m_nrg{1.0,1.0};
  plot2D* plotSignalChannelLeftRight;      // signal plot for the left and right channel
  vector<double> signalTimeLeft;
  vector<double> signalLeft;
  vector<double> signalTimeRight;
  vector<double> signalRight;
  QGraphicsSimpleTextItem *m_coordX;
  QGraphicsSimpleTextItem *m_coordY;
  Callout *m_tooltip;
  QList<Callout *> m_callouts;
};

}

#endif // QCPLOTWIDGETS_H
