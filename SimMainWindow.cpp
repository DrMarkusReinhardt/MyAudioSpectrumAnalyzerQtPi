#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QTextStream>
#include <QtCore/QTimer>
#include <QRect>
#include <QtGlobal>
#include <iostream>
#include "SimMainWindow.h"

using namespace MR_SIM;

SimMainWindow::SimMainWindow(QMainWindow *parent)
  : QMainWindow(parent)
{
  // add menu bar
  QMenu *menu = menuBar()->addMenu(tr("&File"));
  menu->addAction(tr("&Quit"), this, &QWidget::close);

  // get and set the main calculation parameters
  setParameters();

  // setup the widgets and the layouts
  setupWidgetsAndLayouts();

  // prepare for THD calculations
  double defaultBaseFrequencyLeft = 1000.0;
  double defaultNumberOvertonesLeft = 20;
  THDCalcLeft = new THDCalculator(defaultBaseFrequencyLeft, defaultNumberOvertonesLeft, m_sampleFrequency);
  THDCalcLeft->initTHDCalculation();

  double defaultBaseFrequencyRight = 1000.0;
  double defaultNumberOvertonesRight = 20;
  THDCalcRight = new THDCalculator(defaultBaseFrequencyRight, defaultNumberOvertonesRight, m_sampleFrequency);
  THDCalcRight->initTHDCalculation();

  // define the simulation loop timer
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&SimMainWindow::step));
  const uint16_t delayTime_ms = 10;
  m_timer->start(delayTime_ms);
  std::cout << "Spectrum analyzer started" << std::endl;
}

void SimMainWindow::setParameters()
{
  m_discreteTime = 0;
  m_sampleFrequency = initSampleFrequency;
  m_samplePeriod = 1.0 / m_sampleFrequency;
  m_noSpectrumSamples = initNoSpectrumSamples;
  m_spectrumParameter = new SpectrumParameter(minFrequencyRange, maxFrequencyRange, m_noSpectrumSamples);
}

void SimMainWindow::readParametersFromFile()
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

void SimMainWindow::setupWidgetsAndLayouts()
{
    // setup the signal plot view
    // std::cout << "setup SignalPlotView" << std::endl;
    m_SignalPlotView = new SignalPlotView(m_sampleFrequency, this);
    QString signalAxisLabelStr("time / ms -->");
    m_signalAxisLabel = new QLabel(signalAxisLabelStr, this);
    m_signalAxisLabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );
    // std::cout << "SignalPlotView created" << std::endl;

    // setup the spectrum plot view
    // std::cout << "setup SpectrumPlotView" << std::endl;
    m_SpectrumPlotView = new SpectrumPlotView(m_sampleFrequency, m_spectrumParameter, this);
    QString frequencyAxisLabelStr("frequency / Hz -->");
    m_frequencyAxisLabel = new QLabel(frequencyAxisLabelStr, this);
    m_frequencyAxisLabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );

    // std::cout << "SpectrumPlotView created" << std::endl;

    // max. time knob
    m_maxNumberSignalSamplesKnob = new Knob( "Max. no. samples", 100.0, 500.0, this );
    m_maxNumberSignalSamplesKnob->setValue(500.0);

    // Combo box for the min. frequency
    m_minFrequencyDial = new QComboBox(this);
    m_minFrequencyDial->addItems(m_frequencyDialList);
    m_minFrequencyDial->setCurrentIndex(0);
    QString minFrequencyLabelStr("min. frequency");
    m_minFrequencyLabel = new QLabel(minFrequencyLabelStr, this);
    m_minFrequencyLabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );

    // Combo box for the max. frequency
    m_maxFrequencyDial = new QComboBox(this);
    m_maxFrequencyDial->addItems(m_frequencyDialList);
    m_maxFrequencyDial->setCurrentIndex(8);
    QString maxFrequencyLabelStr("max. frequency");
    m_maxFrequencyLabel = new QLabel(maxFrequencyLabelStr, this);
    m_maxFrequencyLabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );

    // place all widgets
    QRect Rect1(30,30,1200,450);
    m_SignalPlotView->setGeometry(Rect1);
    QRect RectSignalLabel(550,400,200,50);
    m_signalAxisLabel->setGeometry(RectSignalLabel);
    m_signalAxisLabel->setVisible(true);
    m_signalAxisLabel->setStyleSheet("QLabel { color : white; }");

    QRect Rect2(30,490,1200,450);
    m_SpectrumPlotView->setGeometry(Rect2);

    QRect RectFrequencyLabel(550,860,200,50);
    m_frequencyAxisLabel->setGeometry(RectFrequencyLabel);
    m_frequencyAxisLabel->setVisible(true);
    m_frequencyAxisLabel->setStyleSheet("QLabel { color : white; }");

    QRect Rect3(1260,30,200,200);
    m_maxNumberSignalSamplesKnob->setGeometry(Rect3);

    QRect Rect4(1330,550,100,40);
    m_minFrequencyLabel->setGeometry(Rect4);
    m_minFrequencyLabel->setVisible(true);

    QRect Rect5(1330,570,100,40);
    m_minFrequencyDial->setGeometry(Rect5);
    m_minFrequencyDial->setVisible(true);

    QRect Rect6(1330,650,100,40);
    m_maxFrequencyLabel->setGeometry(Rect6);
    m_maxFrequencyLabel->setVisible(true);

    QRect Rect7(1330,670,100,40);
    m_maxFrequencyDial->setGeometry(Rect7);
    m_maxFrequencyDial->setVisible(true);

    // connect signals and slots
    connect(m_maxNumberSignalSamplesKnob, QOverload<double>::of(&Knob::valueChanged),
            m_SignalPlotView, &SignalPlotView::updateMaxSamplesToPlot);

    // connect combo boxes
    connect(m_minFrequencyDial, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){ minFrequencyDialChanged(index); });
    connect(m_maxFrequencyDial, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){ maxFrequencyDialChanged(index); });
}

void SimMainWindow::minFrequencyDialChanged(int newIndex)
{
   // std::cout << "minFrequencyDialChanged(): " << newIndex <<std::endl;
   int indexMaxFrequencyDial = m_maxFrequencyDial->currentIndex();
   if (newIndex >= indexMaxFrequencyDial)
      newIndex = max(0,indexMaxFrequencyDial-1);
   m_minFrequencyDial->setCurrentIndex(newIndex);
   double newMinFrequency = m_minFrequencyDial->currentText().toDouble();
   m_SpectrumPlotView->updateMinFrequency(newMinFrequency);
}

void SimMainWindow::maxFrequencyDialChanged(int newIndex)
{
   // std::cout << "maxFrequencyDialChanged(): " << newIndex <<std::endl;
   int indexMinFrequencyDial = m_minFrequencyDial->currentIndex();
   if (newIndex <= indexMinFrequencyDial)
      newIndex = min(8,indexMinFrequencyDial+1);
   m_maxFrequencyDial->setCurrentIndex(newIndex);
   double newMaxFrequency = m_maxFrequencyDial->currentText().toDouble();
   m_SpectrumPlotView->updateMaxFrequency(newMaxFrequency);
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

  // THD calculations
  double THD_F_left;
  double THD_R_left;
  THDCalcLeft->calcTHD(THD_F_left, THD_R_left, m_SignalPlotView->returnLeftSignal());
  double THD_F_right;
  double THD_R_right;
  THDCalcRight->calcTHD(THD_F_right, THD_R_right, m_SignalPlotView->returnRightSignal());

  std::cout << "THD_F (left)  = " << THD_F_left  << " THD_R (left)  = " << THD_R_left  << std::endl;
  std::cout << "THD_F (right) = " << THD_F_right << " THD_R (right) = " << THD_R_right << std::endl;

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
