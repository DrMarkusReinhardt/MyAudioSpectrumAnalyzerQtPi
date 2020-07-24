#include <iostream>
#include <vector>
#include <QtWidgets/QGridLayout>
#include <QWidget>
#include "SimPlotWidgets.h"
#include "DefineDataSizes.h"

using namespace MR_SIM;

SimPlotWidgets::SimPlotWidgets(QWidget *parent)
    : QWidget(parent)
{
    // const double initSamplePeriod = 0.01;
    const uint16_t arraySize = sizeDataToPlot;

    signalTimeLeft.resize(arraySize);
    signalLeft.resize(arraySize);
    signalTimeRight.resize(arraySize);
    signalRight.resize(arraySize);

    // plot signals
    const QString initLeftSignalLabel = "amplitude left -->";
    const QString initRightSignalLabel = "amplitude right -->";
    const QString initTimeLabel = "time -->";
    
    // generate zero data
    createZeroData(signalTimeLeft,signalLeft,signalTimeRight,signalRight);

    const QString initTitleString = "Left and right channel signal";
    plotSignalChannelLeftRight = new plot2D(signalTimeLeft,signalLeft,signalTimeRight,signalRight);
    plotSignalChannelLeftRight->setTitle(initTitleString);

    const uint16_t arraySizeSpectrum = 400;
    vector<double> spectrumFrequencyLeft(arraySizeSpectrum);
    vector<double> spectrumMagnitudeLeft(arraySizeSpectrum);
    vector<double> spectrumFrequencyRight(arraySizeSpectrum);
    vector<double> spectrumMagnitudeRight(arraySizeSpectrum);

    // plot spectra
    const QString initLeftSpectrumLabel = "magnitude spectrum left -->";
    const QString initRightSpectrumLabel = "magnitude spectrum right -->";
    const QString initFrequencyLabel = "frequency -->";
    
    // generate random data
    createRandomData(spectrumFrequencyLeft,spectrumMagnitudeLeft,spectrumFrequencyRight,spectrumMagnitudeRight);

    // plot data left
    const QString initTitleStringSpectrumLeft = "Left and right channel spectrum";
    plotSpectrumChannelLeftRight = new plot2D(spectrumFrequencyLeft,spectrumMagnitudeLeft);
    plotSpectrumChannelLeftRight->setTitle(initTitleStringSpectrumLeft);

    // create layout
    QVBoxLayout *VLayout = new QVBoxLayout;
    VLayout->addWidget(plotSignalChannelLeftRight);
    VLayout->addWidget(plotSpectrumChannelLeftRight);

    setLayout(VLayout);

    // create the Portaudio handler
    uint16_t initNoFrames = 512;
    uint8_t initNoChannels = 2;
    uint32_t initSampleRate = 44100;
    paHandler = new PortaudioHandler(initNoFrames,initNoChannels,initSampleRate);
    if (paHandler->open())
    {
      std::cout << "stream opened successfully" << std::endl;
    }
    else
      std::cout << "stream open failed" << std::endl;

    // create the spectrum calculator
    spectrumCalculator = new SpectrumCalculator(arraySizeSpectrum);
}

void SimPlotWidgets::updatePA()
{
    // std::cout << "PA handler before start" << std::endl;
    paHandler->start();                                          // start input stream processing
    // std::cout << "PA handler started and waiting" << std::endl;
    paHandler->wait();                                           // wait until the Portaudio handler returns from sampling
    // paHandler->write2screen();
    // std::cout << "PA handler after wait" << std::endl;
    paHandler->transferSampledData2Channels();
    paHandler->stop();                                           // stop input stream processing
    // std::cout << "PA handler stopped" << std::endl;
}

void SimPlotWidgets::updateSignals()
{
    // std::cout << "Update left and right signal" << std::endl;
    signalTimeLeft = paHandler->returnDiscreteTimeSamples(); // get the discrete time samples from the Portaudio handler
    signalTimeRight = signalTimeLeft;
    signalLeft = paHandler->returnSignalLeft();              // get the data of the left channel from the Portaudio handler
    signalRight = paHandler->returnSignalRight();            // get the data of the right channel from the Portaudio handler
    // createRandomData(signalTimeLeft,signalLeft,signalTimeRight,signalRight);
    // std::cout << "call update data" << std::endl;
    plotSignalChannelLeftRight->updateData(signalTimeLeft,signalLeft,signalTimeRight,signalRight);
}

void SimPlotWidgets::updateSpectra()
{
    VectorXd frequencyRange = spectrumCalculator->returnFrequencyRange();
    spectrumCalculator->calculateSpectrum(signalLeft);
    spectrumCalculator->normalizeMagnitudeSpectrum();
    VectorXd magnitudeSpectrumLeft = spectrumCalculator->returnMagnitudeSpectrum();
    spectrumCalculator->calculateSpectrum(signalRight);
    spectrumCalculator->normalizeMagnitudeSpectrum();
    VectorXd magnitudeSpectrumRight = spectrumCalculator->returnMagnitudeSpectrum();
    plotSpectrumChannelLeftRight->updateData(frequencyRange,magnitudeSpectrumLeft,frequencyRange,magnitudeSpectrumRight);
}

void SimPlotWidgets::createZeroData(vector<double>& x1,vector<double>& y1,
                                    vector<double>& x2,vector<double>& y2)
{
    for(uint16_t k = 0; k < x1.size(); k++)
        x1[k] = k*1.0;
    for(uint16_t k = 0; k < y1.size(); k++)
        y1[k] = 0.0;
    for(uint16_t k = 0; k < x2.size(); k++)
        x2[k] = k*1.0;
    for(uint16_t k = 0; k < y2.size(); k++)
        y2[k] = 0.0;
}

void SimPlotWidgets::createRandomData(vector<double>& x1,vector<double>& y1,
                                      vector<double>& x2,vector<double>& y2)
{
    y1 = m_nrg.generate(y1.size());
    for(uint16_t k = 0; k < x1.size(); k++)
        x1[k] = k*1.0;
    y2 = m_nrg.generate(y2.size());    
    for(uint16_t k = 0; k < x2.size(); k++)
        x2[k] = k*1.0;
}
