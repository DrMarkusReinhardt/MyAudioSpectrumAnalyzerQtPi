#ifndef SIMMAINWINDOW_H
#define SIMMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "SignalPlotView.h"
#include "SpectrumPlotView.h"

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
  void getParameters();
  void step();

private:
  QTimer *m_timer;
  uint32_t m_discreteTime;
  uint32_t m_discreteSimulationSteps;
  SignalPlotView *m_SignalPlotView;
  SpectrumPlotView *m_SpectrumPlotView;
  double m_samplePeriod = 0.001;
};

} // namespace MR_Sim

#endif // QCSIMMAINWINDOW_H