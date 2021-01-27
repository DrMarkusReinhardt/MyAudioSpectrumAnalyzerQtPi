#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
#include <QTextStream>
#include <QtCore/QTimer>
#include <QRect>
#include <QtGlobal>
#include <iostream>
#include <chrono>
#include "SimMainWindow.h"

using namespace MR_SIM;

uint16_t signalBufferSize; // the signal buffer size
uint16_t spectrumBufferSize; // the spectrum buffer size
QSemaphore SemSignalBuffer1(1);   // semaphore to access signal buffer 1, left and right
QSemaphore SemSignalBuffer2(1);   // semaphore to access signal buffer 2, left and right
QSemaphore SemSpectrumBuffer1(1); // semaphore to access spectrum buffer 1, left and right
QSemaphore SemSpectrumBuffer2(1); // semaphore to access spectrum buffer 2, left and right
QSemaphore SemSpectrumParameter(1); // semaphore to exchange spectrum parameter between threads
SpectrumParameter *spectrumParameter; // the pointer to spectrum parameter data structure to exchange
bool resetSpectrumThreadFlag;  // flag to signal that the spectrum calculation thread shall reset
VectorXd SignalTimeBuffer1Left; // the first buffer for time samples of the left signal
VectorXd SignalTimeBuffer2Left; // the second buffer for time samples of the left signal
VectorXd SignalTimeBuffer1Right; // the first buffer for time samples of the right signal
VectorXd SignalTimeBuffer2Right; // the second buffer for time samples of the right signal
VectorXd SignalBuffer1Left;   // the first buffer for signal samples of the left channel
VectorXd SignalBuffer2Left;   // the second buffer for signal samples of the left channel
VectorXd SignalBuffer1Right;  // the first buffer for signal samples of the right channel
VectorXd SignalBuffer2Right;  // the second buffer for signal samples of the right channel
VectorXd SpectrumBuffer1Left; // the first buffer for magnitude spectrum samples of the left channel
VectorXd SpectrumBuffer2Left; // the second buffer for magnitude spectrum samples of the left channel
VectorXd SpectrumBuffer1Right; // the first buffer for magnitude spectrum samples of the right channel
VectorXd SpectrumBuffer2Right; // the second buffer for magnitude spectrum samples of the right channel
bool signalBuffer1Filled;  // flag to signal when the signal buffer is filled for further processing
bool signalBuffer2Filled;  // flag to signal when the signal buffer is filled for further processing
bool spectrumBuffer1Filled;// flag to signal when the spectrum buffer is filled for further processing
bool spectrumBuffer2Filled;// flag to signal when the spectrum buffer is filled for further processing
uint8_t normalizationMode;  // normalization mode

SimMainWindow::SimMainWindow(QMainWindow *parent)
    : QMainWindow(parent)
{

    // add menu bar with actions
    createMenuAndActions();

    // get and set the main calculation parameters
    setParameters();

    // create buffer
    createBuffer();

    // setup the widgets and the layouts
    setupWidgetsAndLayouts();

    // define the simulation loop timer
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, QOverload<>::of(&SimMainWindow::step));
    const uint16_t delayTime_ms = 10;
    m_timer->start(delayTime_ms);
    m_discreteSimulationSteps = 30;

    // spectrum calculation thread shall not restart for now
    resetSpectrumThreadFlag = false;

    std::cout << "Spectrum analyzer started" << std::endl;
}

// create buffer
void SimMainWindow::createBuffer()
{
    signalBufferSize = sizeDataToPlot;
    SignalTimeBuffer1Left.resize(signalBufferSize);
    SignalTimeBuffer2Left.resize(signalBufferSize);
    SignalTimeBuffer1Right.resize(signalBufferSize);
    SignalTimeBuffer2Right.resize(signalBufferSize);
    SignalBuffer1Left.resize(signalBufferSize);
    SignalBuffer2Left.resize(signalBufferSize);
    SignalBuffer1Right.resize(signalBufferSize);
    SignalBuffer2Right.resize(signalBufferSize);
    spectrumBufferSize = initNoSpectrumSamples;
    SpectrumBuffer1Left.resize(spectrumBufferSize);
    SpectrumBuffer2Left.resize(spectrumBufferSize);
    SpectrumBuffer1Right.resize(spectrumBufferSize);
    SpectrumBuffer2Right.resize(spectrumBufferSize);
}

void SimMainWindow::createMenuAndActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    // QToolBar *fileToolBar = addToolBar(tr("File"));
    /*
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);
    */

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the spectrum data to disk"));
    // connect(saveAct, &QAction::triggered, this, &SimMainWindow::save);
    fileMenu->addAction(saveAct);
    // fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    // connect(saveAct, &QAction::triggered, this, &SimMainWindow::save);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
}

/*
void SimMainWindow::getSignalSpectrumData()
{
    m_t = m_SignalPlotView->returnTimeLeftSignal();
    m_leftSignal = m_SignalPlotView->returnLeftSignal();
    m_rightSignal = m_SignalPlotView->returnRightSignal();

    m_f = m_SpectrumPlotView->returnFrequencyRange();
    m_leftSpectrum = m_SpectrumPlotView->returnMagnitudeSpectrumLeft();
    m_rightSpectrum = m_SpectrumPlotView->returnMagnitudeSpectrumRight();
}

void SimMainWindow::save()
{
    // std::cout << "save() called" << std::endl;
    getSignalSpectrumData();
    m_storeSignalSpectrumData = new StoreSignalSpectrumData(m_t, m_leftSignal, m_rightSignal,
                                                            m_f, m_leftSpectrum, m_rightSpectrum);
    m_storeSignalSpectrumData->save();
    delete m_storeSignalSpectrumData;
}

void SimMainWindow::saveAs()
{
    // std::cout << "saveAs() called" << std::endl;
    // missing: get file name
    getSignalSpectrumData();
    m_storeSignalSpectrumData = new StoreSignalSpectrumData(m_t, m_leftSignal, m_rightSignal,
                                                            m_f, m_leftSpectrum, m_rightSpectrum);
    m_storeSignalSpectrumData->save();
    delete m_storeSignalSpectrumData;
}
*/ 

void SimMainWindow::setParameters()
{
    m_discreteTime = 0;
    m_sampleFrequency = initSampleFrequency;
    m_samplePeriod = 1.0 / m_sampleFrequency;
    m_noSpectrumSamples = initNoSpectrumSamples;
    m_spectrumParameter = new SpectrumParameter(minFrequencyRange, maxFrequencyRange, m_noSpectrumSamples);
    m_peakSpectrumValueLeft = -180;
    m_peakSpectrumValueRight = -180;
    m_peakSpectrumFrequencyLeft = 0.0;
    m_peakSpectrumFrequencyRight = 0.0;

    // set running flag
    m_applicationRunning = true;

    // set channel activity flags
    m_leftChannelActive = true;
    m_rightChannelActive = true;
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
    if (m_leftChannelActive)
        m_SignalPlotView->activateLeftChannel();
    else
        m_SignalPlotView->deactivateLeftChannel();
    if (m_rightChannelActive)
        m_SignalPlotView->activateRightChannel();
    else
        m_SignalPlotView->deactivateRightChannel();

    QString signalAxisLabelStr("time / ms -->");
    m_signalAxisLabel = new QLabel(signalAxisLabelStr, this);
    m_signalAxisLabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );
    // std::cout << "SignalPlotView created" << std::endl;

    // setup the spectrum plot view
    // std::cout << "setup SpectrumPlotView" << std::endl;
    m_SpectrumPlotView = new SpectrumPlotView(m_sampleFrequency, m_spectrumParameter, this);
    if (m_leftChannelActive)
        m_SpectrumPlotView->activateLeftChannel();
    else
        m_SpectrumPlotView->deactivateLeftChannel();
    if (m_rightChannelActive)
        m_SpectrumPlotView->activateRightChannel();
    else
        m_SpectrumPlotView->deactivateRightChannel();

    QString frequencyAxisLabelStr("frequency / Hz -->");
    m_frequencyAxisLabel = new QLabel(frequencyAxisLabelStr, this);
    m_frequencyAxisLabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );
    // std::cout << "SpectrumPlotView created" << std::endl;

    // activation button for left and right channel
    m_leftChannelActivationButton = new QPushButton(this);
    m_leftChannelActivationButton->setStyleSheet("QPushButton { background-color : red; }");
    m_leftChannelActivationButton->setText("Left channel stop");
    m_rightChannelActivationButton = new QPushButton(this);
    m_rightChannelActivationButton->setStyleSheet("QPushButton { background-color : red; }");
    m_rightChannelActivationButton->setText("Right channel stop");
    m_processingOnOff = new QPushButton(this);
    m_processingOnOff->setStyleSheet("QPushButton { background-color : red; }");
    m_processingOnOff->setText("Stop");
    m_processingStatus = new QLabel(this);
    m_processingStatus->setText("Running");
    m_processingStatus->setFrameStyle(QFrame::Sunken);
    m_processingStatus->setStyleSheet("QLabel { background-color : white; }");
    m_processingStatus->setAlignment( Qt::AlignHCenter );

    // peak spectrum values and frequency location
    m_peakSpectrumValuesLabel = new QLabel(this);
    m_peakSpectrumValuesLabel->setText("Peak spectrum values / dB:");
    m_leftChannelPeakSpectrumValueDisplay = new QLabel(this);
    m_leftChannelPeakSpectrumValueDisplay->setText(" ");
    m_rightChannelPeakSpectrumValueDisplay = new QLabel(this);
    m_rightChannelPeakSpectrumValueDisplay->setText(" ");
    m_peakFrequencyValuesLabel = new QLabel(this);
    m_peakFrequencyValuesLabel->setText("at frequencies:");
    m_leftChannelPeakSpectrumFrequencyDisplay = new QLabel(this);
    m_leftChannelPeakSpectrumFrequencyDisplay->setText(" ");
    m_rightChannelPeakSpectrumFrequencyDisplay = new QLabel(this);
    m_rightChannelPeakSpectrumFrequencyDisplay->setText(" ");

    // spectra normalization mode selection
    QString normalizationModeLabelStr("Select normalization mode:");
    m_normalizationModeLabel = new QLabel(normalizationModeLabelStr,this);
    m_normalizationModeLabel->setAlignment(Qt::AlignLeft);
    m_normalizationModeLabel->setFont(font);
    m_normalizationModeSelector = new QComboBox(this);
    m_normalizationModeSelector->addItems(m_normalizationModeList);
    m_normalizationModeSelector->setCurrentIndex(0);

    // max. time knob
    m_maxNumberSignalSamplesKnob = new Knob( "No. samples in signal display:", 100.0, 500.0, this );
    m_maxNumberSignalSamplesKnob->setValue(500.0);

    // prepare for THD calculations
    // std::cout << "setup THD" << std::endl;
    double defaultBaseFrequencyLeft_kHz = 1.0;
    double defaultNumberOvertonesLeft = 20;
    double defaultBaseFrequencyRight_kHz = 1.0;
    double defaultNumberOvertonesRight = 20;
    // handler to parameterize and show the THD calculations
    QString THDHeaderLeftStr("THD Left Channel");
    THDHandlerLeft  = new THDHandler(THDHeaderLeftStr, defaultBaseFrequencyLeft_kHz,
                                     defaultNumberOvertonesLeft, m_sampleFrequency, this);
    QString THDHeaderRightStr("THD Right Channel");
    THDHandlerRight = new THDHandler(THDHeaderRightStr, defaultBaseFrequencyRight_kHz,
                                     defaultNumberOvertonesRight, m_sampleFrequency, this);

    // Combo box for the min. frequency
    m_minFrequencyDial = new QComboBox(this);
    m_minFrequencyDial->addItems(m_frequencyDialList);
    m_minFrequencyDial->setCurrentIndex(0);
    QString minFrequencyLabelStr("Select min. frequency:");
    m_minFrequencyLabel = new QLabel(minFrequencyLabelStr, this);
    m_minFrequencyLabel->setWordWrap(true);
    m_minFrequencyLabel->setAlignment(Qt::AlignLeft);
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
    QRect Rect1(10,30,1200,420);
    m_SignalPlotView->setGeometry(Rect1);
    QRect RectSignalLabel(540,400,200,50);
    m_signalAxisLabel->setGeometry(RectSignalLabel);
    m_signalAxisLabel->setVisible(true);
    m_signalAxisLabel->setStyleSheet("QLabel { color : white; }");

    QRect Rect2(10,450,1200,420);
    m_SpectrumPlotView->setGeometry(Rect2);

    QRect RectFrequencyLabel(530,820,200,50);
    m_frequencyAxisLabel->setGeometry(RectFrequencyLabel);
    m_frequencyAxisLabel->setVisible(true);
    m_frequencyAxisLabel->setStyleSheet("QLabel { color : white; }");

    // left and right channel activation buttons
    QRect RectLeftChannelActivationButton(10,890,130,20);
    m_leftChannelActivationButton->setGeometry(RectLeftChannelActivationButton);
    m_leftChannelActivationButton->setVisible(true);
    QRect RectRightChannelActivationButton(150,890,130,20);
    m_rightChannelActivationButton->setGeometry(RectRightChannelActivationButton);
    m_rightChannelActivationButton->setVisible(true);
    QRect ProcessingActivationButton(10,920,130,20);
    m_processingOnOff->setGeometry(ProcessingActivationButton);
    m_processingOnOff->setVisible(true);
    QRect RectProcessingStatus(150,920,130,20);
    m_processingStatus->setGeometry(RectProcessingStatus);
    m_processingStatus->setVisible(true);

    // measurement widgets
    QRect RectPeakSpectrumValuesLabel(295,920,180,20);
    m_peakSpectrumValuesLabel->setGeometry(RectPeakSpectrumValuesLabel);
    m_peakSpectrumValuesLabel->setFont(font);
    QRect RectLeftChannelPeakSpectrumValueDisplay(480,920,120,20);
    m_leftChannelPeakSpectrumValueDisplay->setGeometry(RectLeftChannelPeakSpectrumValueDisplay);
    m_leftChannelPeakSpectrumValueDisplay->setFrameStyle(QFrame::Sunken);
    m_leftChannelPeakSpectrumValueDisplay->setStyleSheet("QLabel { background-color : white; }");
    m_leftChannelPeakSpectrumValueDisplay->setAlignment( Qt::AlignRight );
    QRect RectRightChannelPeakSpectrumValueDisplay(620,920,120,20);
    m_rightChannelPeakSpectrumValueDisplay->setGeometry(RectRightChannelPeakSpectrumValueDisplay);
    m_rightChannelPeakSpectrumValueDisplay->setFrameStyle(QFrame::Sunken);
    m_rightChannelPeakSpectrumValueDisplay->setStyleSheet("QLabel { background-color : white; }");
    m_rightChannelPeakSpectrumValueDisplay->setAlignment( Qt::AlignRight );
    QRect RectPeakFrequencyValuesLabel(760,920,160,20);
    m_peakFrequencyValuesLabel->setGeometry(RectPeakFrequencyValuesLabel);
    m_peakFrequencyValuesLabel->setAlignment( Qt::AlignRight );
    m_peakFrequencyValuesLabel->setFont(font);
    QRect RectLeftChannelPeakSpectrumFrequencyDisplay(940,920,120,20);
    m_leftChannelPeakSpectrumFrequencyDisplay->setGeometry(RectLeftChannelPeakSpectrumFrequencyDisplay);
    m_leftChannelPeakSpectrumFrequencyDisplay->setFrameStyle(QFrame::Sunken);
    m_leftChannelPeakSpectrumFrequencyDisplay->setStyleSheet("QLabel { background-color : white; }");
    m_leftChannelPeakSpectrumFrequencyDisplay->setAlignment( Qt::AlignRight );
    QRect RectRightChannelPeakSpectrumFrequencyDisplay(1080,920,120,20);
    m_rightChannelPeakSpectrumFrequencyDisplay->setGeometry(RectRightChannelPeakSpectrumFrequencyDisplay);
    m_rightChannelPeakSpectrumFrequencyDisplay->setFrameStyle(QFrame::Sunken);
    m_rightChannelPeakSpectrumFrequencyDisplay->setStyleSheet("QLabel { background-color : white; }");
    m_rightChannelPeakSpectrumFrequencyDisplay->setAlignment( Qt::AlignRight );
    
    // copyright label
    m_copyrightLabel = new QLabel(this);
    m_copyrightLabel->setText("CR Dr. Markus Reinhardt");
    QRect RectCopyrightLabel(1260,940,240,30);
    m_copyrightLabel->setGeometry(RectCopyrightLabel);
    m_copyrightLabel->setFont(font);

    // Selector for the normalization mode
    QRect RectNormalizationModeLabel(295,890,180,20);
    m_normalizationModeLabel->setGeometry(RectNormalizationModeLabel);
    QRect RectNormalizationModeSelector(480,890,120,20);
    m_normalizationModeSelector->setGeometry(RectNormalizationModeSelector);

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

    // connect Start/Stop push button
    connect(m_processingOnOff, SIGNAL(clicked()), this, SLOT(toggleOnOff()));

    // connect left / right channel activation buttons
    connect(m_leftChannelActivationButton, SIGNAL(clicked()), this, SLOT(toggleLeftChannel()));
    connect(m_rightChannelActivationButton, SIGNAL(clicked()), this, SLOT(toggleRightChannel()));

    // connect normalization mode selector
    connect(m_normalizationModeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){ changeNormalizationMode(index); });
}

void SimMainWindow::changeNormalizationMode(uint8_t newNormalizationModeIndex)
{
    // std::cout << "normalization mode changed: newNormalizationModeIndex = " << newNormalizationModeIndex << std::endl;
    m_SpectrumPlotView->setNormalizationMode(newNormalizationModeIndex);
    normalizationMode = newNormalizationModeIndex;
}

void SimMainWindow::toggleOnOff()
{
    // std::cout << "OnOff clicked" << std::endl;
    //
    if (m_applicationRunning)
    {
        m_applicationRunning = false;
        m_processingOnOff->setStyleSheet("QPushButton { background-color : green; }");
        m_processingOnOff->setText("Start");
        m_processingStatus->setText("Stopped");
        m_timer->stop();
    }
    else
    {
        m_applicationRunning = true;
        m_processingOnOff->setStyleSheet("QPushButton { background-color : red; }");
        m_processingOnOff->setText("Stop");
        m_processingStatus->setText("Running");
        m_SpectrumPlotView->restart();
        const uint16_t delayTime_ms = 10;
        m_timer->start(delayTime_ms);
    }
}

void SimMainWindow::toggleLeftChannel()
{
    if (m_leftChannelActive)
    {
        m_leftChannelActive = false;
        m_leftChannelActivationButton->setStyleSheet("QPushButton { background-color : green; }");
        m_leftChannelActivationButton->setText("Left channel start");
        m_leftChannelPeakSpectrumValueDisplay->setText(" ");
        m_leftChannelPeakSpectrumFrequencyDisplay->setText(" ");
        m_SignalPlotView->deactivateLeftChannel();
        m_SpectrumPlotView->deactivateLeftChannel();
    }
    else
    {
        m_leftChannelActive = true;
        m_leftChannelActivationButton->setStyleSheet("QPushButton { background-color : red; }");
        m_leftChannelActivationButton->setText("Left channel stop");
        m_SignalPlotView->activateLeftChannel();
        m_SpectrumPlotView->restart();
        m_SpectrumPlotView->activateLeftChannel();
    }
}

void SimMainWindow::toggleRightChannel()
{
    if (m_rightChannelActive)
    {
        m_rightChannelActive = false;
        m_rightChannelActivationButton->setStyleSheet("QPushButton { background-color : green; }");
        m_rightChannelActivationButton->setText("Right channel start");
        m_rightChannelPeakSpectrumValueDisplay->setText(" ");
        m_rightChannelPeakSpectrumFrequencyDisplay->setText(" ");
        m_SignalPlotView->deactivateRightChannel();
        m_SpectrumPlotView->deactivateRightChannel();
    }
    else
    {
        m_rightChannelActive = true;
        m_rightChannelActivationButton->setStyleSheet("QPushButton { background-color : red; }");
        m_rightChannelActivationButton->setText("Right channel stop");
        m_SignalPlotView->activateRightChannel();
        m_SpectrumPlotView->restart();
        m_SpectrumPlotView->activateRightChannel();
    }
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
    m_SpectrumPlotView->restart();
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
    m_SpectrumPlotView->restart();
}

void SimMainWindow::step()
{
    // run a simulation step ...
    m_discreteTime++;
    std::cout << "SimMainWindow::step: m_discreteTime = " << m_discreteTime << std::endl;

    // auto startSignals = std::chrono::system_clock::now();
    std::cout << " Update signals" << std::endl;
    m_SignalPlotView->updateSignals();
                                   
    // auto endSignals = std::chrono::system_clock::now();
    // std::chrono::duration<double> deltaSignals = endSignals - startSignals;
    // std::cout << "delta get signals = " << deltaSignals.count() << "s" << std::endl;

    std::cout << " Update spectra" << std::endl;
    m_SpectrumPlotView->updateSpectra();
    
    // auto endSpectra = std::chrono::system_clock::now();
    // std::chrono::duration<double> deltaSpectra = endSpectra - endSignals;
    // std::cout << "delta calculate spectra = " << deltaSpectra.count() << "s" << std::endl;

    // THD calculations
    if (m_leftChannelActive)
        THDHandlerLeft->run(m_SignalPlotView->returnLeftSignal());
    if (m_rightChannelActive)
        THDHandlerRight->run(m_SignalPlotView->returnRightSignal());

    // update the peak spectrum value and frequency displays
    updatePeakSpectrumDisplays();
    
    /*
    // stop the processing after a given number of steps
    if (m_discreteTime >= m_discreteSimulationSteps)
    {
          m_timer->stop();
          m_SignalPlotView->stop();
          m_SpectrumPlotView->stop();
          std::cout << "Simulation loop finished" << std::endl;
          close();
    }
    */
}

void SimMainWindow::updatePeakSpectrumDisplays()
{
    if (m_leftChannelActive)
    {
        // get the values
        m_SpectrumPlotView->getMaxMagnitudeSpectrumLeft(m_peakSpectrumValueLeft, m_peakSpectrumFrequencyLeft);
        
        // update the displays
        m_leftChannelPeakSpectrumValueDisplay->setText(QString::number(m_peakSpectrumValueLeft,'f',3));
        m_leftChannelPeakSpectrumFrequencyDisplay->setText(QString::number(m_peakSpectrumFrequencyLeft,'f',3));
    }

    if (m_rightChannelActive)
    {
        // get the values
        m_SpectrumPlotView->getMaxMagnitudeSpectrumRight(m_peakSpectrumValueRight, m_peakSpectrumFrequencyRight); 
        
        // update the displays
        m_rightChannelPeakSpectrumValueDisplay->setText(QString::number(m_peakSpectrumValueRight,'f',3));
        m_rightChannelPeakSpectrumFrequencyDisplay->setText(QString::number(m_peakSpectrumFrequencyRight,'f',3));
    }
}

SimMainWindow::~SimMainWindow()
{
    delete m_SignalPlotView;
    delete m_SpectrumPlotView;
    delete m_timer;
}
