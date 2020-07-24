#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QTextStream>
#include <QtCore/QTimer>
#include <iostream>
#include "SimMainWindow.h"
#include "SimPlotWidgets.h"

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

   // setup all plot widgets
   m_PlotWidgets = new SimPlotWidgets(this);
   setCentralWidget(m_PlotWidgets);

   // define the simulation loop timer
   m_timer = new QTimer(this);
   connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&SimMainWindow::step));
   const uint16_t delayTime_ms = 200;
   m_timer->start(delayTime_ms);
   // std::cout << "Simulation loop started" << std::endl;
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

   m_PlotWidgets->updatePA();
   m_PlotWidgets->updateSignals();
   m_PlotWidgets->updateSpectra();

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
  delete m_PlotWidgets;
  delete m_timer;
}
