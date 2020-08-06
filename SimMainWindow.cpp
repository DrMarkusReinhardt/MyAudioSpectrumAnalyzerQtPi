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

    QFont font( "Helvetica", 10 );
    font.setBold( true );

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
    m_maxNumberSignalSamplesKnob = new Knob( "No. samples in signal display:", 100.0, 500.0, this );
    m_maxNumberSignalSamplesKnob->setValue(500.0);

    // prepare for THD calculations
    std::cout << "setup THD" << std::endl;
    double defaultBaseFrequencyLeft = 1000.0;
    double defaultNumberOvertonesLeft = 20;
    double defaultBaseFrequencyRight = 1000.0;
    double defaultNumberOvertonesRight = 20;
    // handler to parameterize and show the THD calculations
    QString THDHeaderLeftStr("THD Left Channel");
    THDHandlerLeft  = new THDHandler(THDHeaderLeftStr, defaultBaseFrequencyLeft,
                                     defaultNumberOvertonesLeft, m_sampleFrequency, this);
    QString THDHeaderRightStr("THD Right Channel");
    THDHandlerRight = new THDHandler(THDHeaderRightStr, defaultBaseFrequencyRight,
                                     defaultNumberOvertonesRight, m_sampleFrequency, this);

    // Combo box for the min. frequency
    m_minFrequencyDial = new QComboBox(this);
    m_minFrequencyDial->addItems(m_frequencyDialList);
    m_minFrequencyDial->setCurrentIndex(0);
    QString minFrequencyLabelStr("Select min. frequency:");
    m_minFrequencyLabel = new QLabel(minFrequencyLabelStr, this);
    m_minFrequencyLabel->setWordWrap(true);
    m_minFrequencyLabel->setAlignment( Qt::AlignLeft );
    m_minFrequencyLabel->setFont(font);

    // Combo box for the max. frequency
    m_maxFrequencyDial = new QComboBox(this);
    m_maxFrequencyDial->addItems(m_frequencyDialList);
    m_maxFrequencyDial->setCurrentIndex(8);
    QString maxFrequencyLabelStr("Select max. frequency:");
    m_maxFrequencyLabel = new QLabel(maxFrequencyLabelStr, this);
    m_maxFrequencyLabel->setWordWrap(true);
    m_maxFrequencyLabel->setAlignment( Qt::AlignLeft );
    m_maxFrequencyLabel->setFont(font);


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

    // number of signal samples knob
    QRect Rect3(1260,20,200,150);
    m_maxNumberSignalSamplesKnob->setGeometry(Rect3);

    // THD handler widgets
    QRect RectTHDLeft(1240,120,260,380);
    THDHandlerLeft->setGeometry(RectTHDLeft);
    THDHandlerLeft->setVisible(true);

    QRect RectTHDRight(1240,460,260,380);
    THDHandlerRight->setGeometry(RectTHDRight);
    THDHandlerRight->setVisible(true);

    // min. and max. frequency selection widgets
    QRect Rect4(1260,850,80,30);
    m_minFrequencyLabel->setGeometry(Rect4);
    m_minFrequencyLabel->setVisible(true);

    QRect Rect5(1360,850,100,30);
    m_minFrequencyDial->setGeometry(Rect5);
    m_minFrequencyDial->setVisible(true);

    QRect Rect6(1260,890,80,30);
    m_maxFrequencyLabel->setGeometry(Rect6);
    m_maxFrequencyLabel->setVisible(true);

    QRect Rect7(1360,890,100,30);
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
  THDHandlerLeft->run(m_SignalPlotView->returnLeftSignal());
  THDHandlerRight->run(m_SignalPlotView->returnRightSignal());


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
