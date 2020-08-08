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
#include <eigen3/Eigen/Dense>

using namespace Eigen;

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
  SpectrumPlotView(double initSampleFrequency, SpectrumParameter *initSpectrumParameter,
                   QWidget *parent = nullptr);
  void createZeroData(VectorXd& x1,VectorXd& y1,
                      VectorXd& x2,VectorXd& y2);
  void createRandomData(VectorXd& x1,VectorXd& y1,
                        VectorXd& x2,VectorXd& y2);
  void updateSpectra();
  void getSignals(VectorXd x1,VectorXd y1,
                  VectorXd x2,VectorXd y2);
  void updateMinFrequency(double newMinFrequency);
  void updateMaxFrequency(double newMaxFrequency);
  VectorXd returnFrequencyRange();
  VectorXd returnMagnitudeSpectrumLeft();
  VectorXd returnMagnitudeSpectrumRight();
  void getMaxMagnitudeSpectrumLeft(double& maximumMagnitudeValue, double& maxFrequencyValue);
  void getMaxMagnitudeSpectrumRight(double& maximumMagnitudeValue, double& maxFrequencyValue);
  void activateLeftChannel();
  void activateRightChannel();
  void deactivateLeftChannel();
  void deactivateRightChannel();
  void setNormalizationMode(uint8_t newNormalizationMode);
  void normalizeSpectra(); 

protected:
  void resizeEvent(QResizeEvent *event);
  void mouseMoveEvent(QMouseEvent *event);

public slots:
  void keepCallout();
  void tooltip(QPointF point, bool state);

private:
  NormalRandomGenerator m_nrg{1.0,1.0};
  plot2D* plotSpectrumChannelLeftRight;         // spectrum plot for the left and right channel
  VectorXd signalTimeLeft;
  VectorXd signalLeft;
  VectorXd signalTimeRight;
  VectorXd signalRight;
  double m_sampleFrequency;
  double m_samplePeriod;
  SpectrumParameter *m_spectrumParameter;
  SpectrumCalculator *spectrumCalculatorLeft;
  SpectrumCalculator *spectrumCalculatorRight;
  QGraphicsSimpleTextItem *m_coordX;
  QGraphicsSimpleTextItem *m_coordY;
  Callout *m_tooltip;
  QList<Callout *> m_callouts;

  // peak spectrum values
  double m_maxMagnitudeLeft;
  double m_maxFrequencyValueLeft;
  double m_maxMagnitudeRight;
  double m_maxFrequencyValueRight;

  // channel activity flags
  bool m_leftChannelActive;
  bool m_rightChannelActive;
  
  // normalization mode
  uint8_t m_normalizationMode;
  
};

} // namespace MR_SIM

#endif // QCPLOTWIDGETS_H
