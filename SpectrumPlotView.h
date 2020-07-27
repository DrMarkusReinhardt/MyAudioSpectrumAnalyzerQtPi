#ifndef SPECTRUMPLOTVIEW_H
#define SPECTRUMPLOTVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QtCharts/QChartGlobal>
#include "plot2D.h"
#include <vector>
#include "RandomGenerator.h"
#include "SpectrumCalculator.h"
#include "SpectrumParameter.h"
#include "callout.h"

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Callout;

namespace MR_SIM
{

class SpectrumPlotView : public QGraphicsView
{
  Q_OBJECT

public:
  SpectrumPlotView(double initSampleFrequency, SpectrumParameter initSpectrumParameter,
                   QWidget *parent = nullptr);
  void createZeroData(vector<double>& x1,vector<double>& y1,
                      vector<double>& x2,vector<double>& y2);
  void createRandomData(vector<double>& x1,vector<double>& y1,
                        vector<double>& x2,vector<double>& y2);
  void updateSpectra();
  void getSignals(vector<double> x1,vector<double> y1,
                  vector<double> x2,vector<double> y2);

protected:
  void resizeEvent(QResizeEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

public slots:
  void keepCallout();
  void tooltip(QPointF point, bool state);

private:
  NormalRandomGenerator m_nrg{1.0,1.0};
  plot2D* plotSpectrumChannelLeftRight;         // spectrum plot for the left and right channel
  vector<double> signalTimeLeft;
  vector<double> signalLeft;
  vector<double> signalTimeRight;
  vector<double> signalRight;
  double m_sampleFrequency;
  double m_samplePeriod;
  SpectrumParameter m_spectrumParameter;
  SpectrumCalculator *spectrumCalculator;
  QGraphicsSimpleTextItem *m_coordX;
  QGraphicsSimpleTextItem *m_coordY;
  Callout *m_tooltip;
  QList<Callout *> m_callouts;
};

}

#endif // QCPLOTWIDGETS_H
