#pragma once
#include <QObject>
#include <QThread>
#include "DTFT.h"
#include <vector>
#include <complex>
#include <cmath>
#include "SpectrumCalculator.h"
#include "SpectrumAverager.h"
#include "SpectrumParameter.h"
#include <eigen3/Eigen/Dense>
#include "ThreadCommon.h"

using namespace Eigen;

class SpectrumCalculationThread : public QThread
{
    Q_OBJECT

public:
    SpectrumCalculationThread(double initSampleFrequency, SpectrumParameter *initSpectrumParameter);
    virtual void run() override;
    void fillSpectrumBuffer(int bufferIndex);
    void normalizeSpectra();

signals:
    void onSpectrumBufferFilled(int bufferIndex);

private:
    void checkParameterUpdate();

    double m_sampleFrequency;
    double m_samplePeriod;
    SpectrumParameter *m_spectrumParameter;
    SpectrumCalculator *spectrumCalculatorLeft;
    SpectrumCalculator *spectrumCalculatorRight;
    const bool initIIRorFIR = true;
    SpectrumAverager *spectrumAveragerLeft;
    SpectrumAverager *spectrumAveragerRight;
    int m_signalBufferIndex;
    int m_spectrumBufferIndex;

    VectorXd m_averageMagnitudeSpectrum1Left;
    VectorXd m_averageMagnitudeSpectrum1Right;
    VectorXd m_averageMagnitudeSpectrum2Left;
    VectorXd m_averageMagnitudeSpectrum2Right;
    bool m_spectrumValid;
    const double spectrumValidityThreshold = -120.0; // a spectrum is considered valid if its maximum is above the threshold

    // peak spectrum values
    double m_maxMagnitudeLeft;
    double m_maxFrequencyValueLeft;
    double m_maxMagnitudeRight;
    double m_maxFrequencyValueRight;

    // channel activity flags
    bool m_leftChannelActive;
    bool m_rightChannelActive;

    // normalization mode
    uint8_t m_normalizationMode;
};
