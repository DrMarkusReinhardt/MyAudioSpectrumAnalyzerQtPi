#ifndef SIMMAINWINDOW_H
#define SIMMAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QList>
#include <QString>
#include <QLabel>
#include "SignalPlotView.h"
#include "SpectrumPlotView.h"
#include "SpectrumParameter.h"
#include "knob.h"

QT_BEGIN_NAMESPACE
class QMainWindow;
QT_END_NAMESPACE

class QLabel;
class QComboBox;

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
  void setupWidgetsAndLayouts();

private Q_SLOTS:
  void minFrequencyDialChanged(int newIndex);
  void maxFrequencyDialChanged(int newIndex);

private:
  QTimer *m_timer;
  // widgets
  SignalPlotView *m_SignalPlotView;
  QLabel *m_signalAxisLabel;
  SpectrumPlotView *m_SpectrumPlotView;
  QLabel *m_frequencyAxisLabel;
  Knob *m_maxNumberSignalSamplesKnob;

  const QList<QString> m_frequencyDialList = {"0.0", "100.0", "200.0", "500.0", "1000.0", "2000.0", "5000.0", "10000.0", "20000.0"};
  QComboBox *m_minFrequencyDial;
  QLabel *m_minFrequencyLabel;
  QComboBox *m_maxFrequencyDial;
  QLabel *m_maxFrequencyLabel;

  // fixed parameters
  const double initSampleFrequency = 96000.0;  // alternatively 44100.0 or 48000.0
  const double initsamplePeriod = 1.0 / initSampleFrequency;
  uint32_t m_discreteTime;
  uint32_t m_discreteSimulationSteps;
  double m_samplePeriod;
  double m_sampleFrequency;

  // parameters to define the frequency range and resolution
  const uint16_t initNoSpectrumSamples = 1000;
  uint16_t m_noSpectrumSamples;
  const double minFrequencyRange = 0.0;
  const double maxFrequencyRange = 20000.0;
  double m_deltaF;
  vector<double> m_frequencyRange;
  SpectrumParameter *m_spectrumParameter;

};

} // namespace MR_Sim

#endif // QCSIMMAINWINDOW_H
