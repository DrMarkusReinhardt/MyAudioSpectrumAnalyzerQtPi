#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QTextStream>
#include <QtCore/QTimer>
#include <QRect>
#include <iostream>
#include "SimMainWindow.h"

using namespace MR_SIM;

SimMainWindow::SimMainWindow(QMainWindow *parent)
  : QMainWindow(parent)
{
  // get and set the main simulation parameters
  m_discreteTime = 0;
  getParameters();

  // add menu bar
  QMenu *menu = menuBar()->addMenu(tr("&File"));
  menu->addAction(tr("&Quit"), this, &QWidget::close);

  // setup the signal plot view
  std::cout << "setup SignalPlotView" << std::endl;
  m_SignalPlotView = new SignalPlotView(this);
  // m_SignalPlotView->setMinimumSize(1000,400);
  std::cout << "SignalPlotView created" << std::endl;

  // setup the spectrum plot view
  std::cout << "setup SpectrumPlotView" << std::endl;
  m_SpectrumPlotView = new SpectrumPlotView(this);
  // m_SpectrumPlotView->setMinimumSize(1000,400);
  std::cout << "SpectrumPlotView created" << std::endl;

  // setCentralWidget(m_SignalPlotView);
  // create layout
  QHBoxLayout *HLayout1 = new QHBoxLayout;
  HLayout1->addWidget(m_SignalPlotView);
  QRect RH1(30,30,700,450);
  HLayout1->setGeometry(RH1);

  QHBoxLayout *HLayout2 = new QHBoxLayout;
  HLayout2->addWidget(m_SpectrumPlotView);
  QRect RH2(30,490,1200,450);
  HLayout2->setGeometry(RH2);

  QVBoxLayout *VLayout = new QVBoxLayout;
  VLayout->addLayout(HLayout1);
  VLayout->addLayout(HLayout2);
  setLayout(VLayout);

  // define the simulation loop timer
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&SimMainWindow::step));
  const uint16_t delayTime_ms = 200;
  m_timer->start(delayTime_ms);
  std::cout << "Simulation loop started" << std::endl;
}


void SimMainWindow::getParameters()
{
  QFile file("SimParams.txt");
  if(!file.open(QIODevice::ReadOnly))
  {
    std::cout << "error" << file.errorString().toStdString() << std::endl;
  }
  QTextStream in(&file);

  // QString strSimulationSteps = in.readLine();
  QString strSimulationSteps = "10";
  std::cout << "Read discrete simulation steps string = " << strSimulationSteps.toStdString() << std::endl;
  m_discreteSimulationSteps = strSimulationSteps.toUInt();
  std::cout << "Read discrete simulation steps = " << m_discreteSimulationSteps << std::endl;

  // QString strSamplePeriod = in.readLine();
  QString strSamplePeriod = "0.01";
  std::cout << "Read sample period string = " << strSamplePeriod.toStdString() << std::endl;
  m_samplePeriod = strSamplePeriod.toDouble();
  std::cout << "Read sample period = " << m_samplePeriod << std::endl;

  file.close();
}


void SimMainWindow::step()
{
  // run a simulation step ...
  m_discreteTime++;
  // std::cout << "SimMainWindow::step: m_discreteTime = " << m_discreteTime << std::endl;

  m_SignalPlotView->updatePA();
  m_SignalPlotView->updateSignals();
  m_SpectrumPlotView->getSignals(m_SignalPlotView->returnTimeLeftSignal(),
                                 m_SignalPlotView->returnLeftSignal(),
                                 m_SignalPlotView->returnTimeRightSignal(),
                                 m_SignalPlotView->returnRightSignal());
  m_SpectrumPlotView->updateSpectra();

  /*
  if (m_discreteTime >= m_discreteSimulationSteps)
  {
      m_timer->stop();
      std::cout << "Simulation loop finished" << std::endl;
  }
  */
}


SimMainWindow::~SimMainWindow()
{
  delete m_SignalPlotView;
  delete m_SpectrumPlotView;
  delete m_timer;
}