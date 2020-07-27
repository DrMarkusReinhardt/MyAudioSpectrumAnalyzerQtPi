#ifndef SIMMAINWINDOW_H
#define SIMMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "SignalPlotView.h"
#include "SpectrumPlotView.h"
#include "SpectrumParameter.h"

QT_BEGIN_NAMESPACE
class QMainWindow;
QT_END_NAMESPACE

namespace MR_SIM
{

class SimMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  SimMainWindow(QMainWindow *parent = nullptr);
  ~SimMainWindow();
  void setParameters();
  void readParametersFromFile();
  void step();

private:
  void calcFrequencyRange();
  void setupWidgetsAndLayouts();

private:
  QTimer *m_timer;
  // widgets
  SignalPlotView *m_SignalPlotView;
  SpectrumPlotView *m_SpectrumPlotView;

  // fixed parameters
  const double initSampleFrequency = 44100.0;
  const double initsamplePeriod = 1.0 / initSampleFrequency;
  uint32_t m_discreteTime;
  uint32_t m_discreteSimulationSteps;
  double m_samplePeriod;
  double m_sampleFrequency;

  // parameters to define the frequency range and resolution
  const uint16_t initNoSpectrumSamples = 500;
  uint16_t m_noSpectrumSamples;
  const double minFrequencyRange = 0.0;
  const double maxFrequencyRange = 20000.0;
  double m_deltaF;
  vector<double> m_frequencyRange;
  SpectrumParameter m_spectrumParameter;
};

} // namespace MR_Sim

#endif // QCSIMMAINWINDOW_H
