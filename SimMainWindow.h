#ifndef SIMMAINWINDOW_H
#define SIMMAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QList>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <vector>
#include "SignalPlotView.h"
#include "SpectrumPlotView.h"
#include "SpectrumParameter.h"
#include "knob.h"
#include "THDCalculator.h"
#include "THDHandler.h"
#include "StoreSignalSpectrumData.h"
#include <eigen3/Eigen/Dense>

using namespace Eigen;

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
  void createMenuAndActions();
  void save();
  void saveAs();
  void setParameters();
  void readParametersFromFile();
  void step();
  void updatePeakSpectrumDisplays();

private:
  void setupWidgetsAndLayouts();
  void getSignalSpectrumData();

private Q_SLOTS:
  void minFrequencyDialChanged(int newIndex);
  void maxFrequencyDialChanged(int newIndex);
  void toggleOnOff();
  void toggleLeftChannel();
  void toggleRightChannel();

private:
  QTimer *m_timer;

  // signal and spectrum plots
  SignalPlotView *m_SignalPlotView;
  QLabel *m_signalAxisLabel;
  SpectrumPlotView *m_SpectrumPlotView;
  QLabel *m_frequencyAxisLabel;

  // on/off buttons, status and measurement fields
  QPushButton *m_leftChannelActivationButton;
  QPushButton *m_rightChannelActivationButton;
  QPushButton *m_processingOnOff;
  QLabel *m_processingStatus;

  // peak spectrum values and frequency location
  QLabel *m_peakSpectrumValuesLabel;
  QLabel *m_leftChannelPeakSpectrumValueDisplay;
  QLabel *m_rightChannelPeakSpectrumValueDisplay;
  QLabel *m_peakFrequencyValuesLabel;
  QLabel *m_leftChannelPeakSpectrumFrequencyDisplay;
  QLabel *m_rightChannelPeakSpectrumFrequencyDisplay;

  // know to control the shown signal samples
  Knob *m_maxNumberSignalSamplesKnob;

  // THD handlers
  THDHandler *THDHandlerLeft;
  THDHandler *THDHandlerRight;

  // combo box to control min. and max. frequency of spectrum plot
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
  bool m_applicationRunning;

  // channel activity flags
  bool m_leftChannelActive;
  bool m_rightChannelActive;

  // parameters to define the frequency range and resolution
  const uint16_t initNoSpectrumSamples = 1000;
  uint16_t m_noSpectrumSamples;
  const double minFrequencyRange = 0.0;
  const double maxFrequencyRange = 20000.0;
  double m_deltaF;
  VectorXd m_frequencyRange;
  SpectrumParameter *m_spectrumParameter;

  // peak spectrum data
  double m_peakSpectrumValueLeft;
  double m_peakSpectrumValueRight;
  double m_peakSpectrumFrequencyLeft;
  double m_peakSpectrumFrequencyRight;

  // to store data
  StoreSignalSpectrumData* m_storeSignalSpectrumData;
  VectorXd m_t;
  VectorXd m_leftSignal;
  VectorXd m_rightSignal;
  VectorXd m_f;
  VectorXd m_leftSpectrum;
  VectorXd m_rightSpectrum;

};

} // namespace MR_Sim

#endif // QCSIMMAINWINDOW_H
