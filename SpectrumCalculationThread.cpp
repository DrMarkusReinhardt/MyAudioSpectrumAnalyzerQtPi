#include "SpectrumCalculationThread.h"

SpectrumCalculationThread::SpectrumCalculationThread(double initSampleFrequency,
                                                     SpectrumParameter *initSpectrumParameter) :
    m_sampleFrequency(initSampleFrequency), m_spectrumParameter(initSpectrumParameter)
{
    // create the spectrum calculators
    spectrumCalculatorLeft = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
    spectrumCalculatorRight = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);

    // create the spectrum averagers
    // default shall be IIR averaging --> true
    spectrumAveragerLeft = new SpectrumAverager(initIIRorFIR, m_spectrumParameter, 100);
    spectrumAveragerRight = new SpectrumAverager(initIIRorFIR, m_spectrumParameter, 100);

    // start with buffer 1 and then toggle buffer 1 and buffer 2
    m_spectrumBufferIndex = 1;
}

void SpectrumCalculationThread::run()
{
    while(true)
    {
        // check if a spectrum parameter update and reset is requested
        checkParameterUpdate();

        if (m_spectrumBufferIndex == 1)
        {
            std::cout << "spectrum calculator run(): buffer index = " << m_spectrumBufferIndex << std::endl;

            // acquire the semaphore for signal buffer 1, signal and spectrum buffer indices are aligned
            SemSignalBuffer1.acquire(1);

            if (signalBuffer1Filled)
            {
                // wait and fill spectrum buffer 1 with spectrum from signal buffer 1
                fillSpectrumBuffer(m_spectrumBufferIndex);

                m_signalBufferIndex = 2;
                signalBuffer1Filled = false;   // indicate that the signal buffer shall be updated now
                emit onSpectrumBufferFilled(m_spectrumBufferIndex);
            }

            // release the semaphore for signal buffer 1
            SemSignalBuffer1.release(1);
        }
        else if (m_spectrumBufferIndex == 2)
        {
            std::cout << "spectrum calculator run(): buffer index = " << m_spectrumBufferIndex << std::endl;

            // acquire the semaphore for signal buffer 2, signal and spectrum buffer indices are aligned
            SemSignalBuffer2.acquire(1);

            if (signalBuffer2Filled)
            {
                // wait and fill spectrum buffer 2 with spectrum from signal buffer 2
                fillSpectrumBuffer(m_spectrumBufferIndex);

                m_signalBufferIndex = 1;
                signalBuffer2Filled = false;  // indicate that the signal buffer shall be updated now
                emit onSpectrumBufferFilled(m_spectrumBufferIndex);
            }

            // release the semaphore for signal buffer 2
            SemSignalBuffer2.release(1);
        }
    }
}

void SpectrumCalculationThread::checkParameterUpdate()
{
    // get the (updated) spectrum parameter
    SemSpectrumParameter.acquire(1);
    // shall a reset be done with new spectrum parameters ?
    if (resetSpectrumThreadFlag)
    {
        // get the new spectrum parameters
        m_spectrumParameter = spectrumParameter;

        // reset the calculation instances
        delete spectrumCalculatorLeft;
        delete spectrumCalculatorRight;
        spectrumCalculatorLeft = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
        spectrumCalculatorRight = new SpectrumCalculator(m_sampleFrequency, m_spectrumParameter);
        delete spectrumAveragerLeft;
        delete spectrumAveragerRight;
        spectrumAveragerLeft = new SpectrumAverager(initIIRorFIR, m_spectrumParameter, 100);
        spectrumAveragerRight = new SpectrumAverager(initIIRorFIR, m_spectrumParameter, 100);
        resetSpectrumThreadFlag = false;
    }
    SemSpectrumParameter.release(1);
}

void SpectrumCalculationThread::fillSpectrumBuffer(int bufferIndex)
{
    if (bufferIndex == 1)  // signal and spectrum buffer indices are aligned
    {
        SemSpectrumBuffer1.acquire(1);
        std::cout << "fill spectrum buffer 1" << std::endl;
        spectrumBuffer1Filled = false;

        // VectorXd frequencyRange = spectrumCalculator->returnFrequencyRange();
        spectrumCalculatorLeft->calculateSpectrum(SignalBuffer1Left);
        spectrumCalculatorLeft->getMaxMagnitudeSpectrum(m_maxMagnitudeLeft, m_maxFrequencyValueLeft);
        // std::cout << "maxMagnitudeLeft = " << maxMagnitudeLeft << std::endl;

        spectrumCalculatorRight->calculateSpectrum(SignalBuffer1Right);
        spectrumCalculatorRight->getMaxMagnitudeSpectrum(m_maxMagnitudeRight, m_maxFrequencyValueRight);
        // std::cout << "maxMagnitudeRight = " << maxMagnitudeRight << std::endl;

        // normalize spectra
        normalizeSpectra();

        VectorXd magnitudeSpectrumLeft = spectrumCalculatorLeft->returnMagnitudeSpectrum();
        VectorXd magnitudeSpectrumRight = spectrumCalculatorRight->returnMagnitudeSpectrum();

        // update average magnitude spectra
        m_averageMagnitudeSpectrum1Left = spectrumAveragerLeft->updateAverageMagnitudeSpectrum(magnitudeSpectrumLeft);
        m_averageMagnitudeSpectrum1Right = spectrumAveragerRight->updateAverageMagnitudeSpectrum(magnitudeSpectrumRight);
        
        // store in the spectrum buffers
        SpectrumBuffer1Left = m_averageMagnitudeSpectrum1Left;
        SpectrumBuffer1Right = m_averageMagnitudeSpectrum1Right;

        // set the filled flag and toggle the buffer
        spectrumBuffer1Filled = true;
        m_spectrumBufferIndex = 2;

        // release the semaphore for buffer 1
        SemSpectrumBuffer1.release(1);
    }
    else if (bufferIndex == 2)
    {
        SemSpectrumBuffer2.acquire(1);
        std::cout << "fill spectrum buffer 2" << std::endl;
        spectrumBuffer2Filled = false;

        // VectorXd frequencyRange = spectrumCalculator->returnFrequencyRange();
        spectrumCalculatorLeft->calculateSpectrum(SignalBuffer2Left);
        spectrumCalculatorLeft->getMaxMagnitudeSpectrum(m_maxMagnitudeLeft, m_maxFrequencyValueLeft);
        // std::cout << "maxMagnitudeLeft = " << maxMagnitudeLeft << std::endl;

        spectrumCalculatorRight->calculateSpectrum(SignalBuffer2Right);
        spectrumCalculatorRight->getMaxMagnitudeSpectrum(m_maxMagnitudeRight, m_maxFrequencyValueRight);
        // std::cout << "maxMagnitudeRight = " << maxMagnitudeRight << std::endl;

        // normalize spectra
        normalizeSpectra();

        VectorXd magnitudeSpectrumLeft = spectrumCalculatorLeft->returnMagnitudeSpectrum();
        VectorXd magnitudeSpectrumRight = spectrumCalculatorRight->returnMagnitudeSpectrum();

        // update average magnitude spectra
        m_averageMagnitudeSpectrum2Left = spectrumAveragerLeft->updateAverageMagnitudeSpectrum(magnitudeSpectrumLeft);
        m_averageMagnitudeSpectrum2Right = spectrumAveragerRight->updateAverageMagnitudeSpectrum(magnitudeSpectrumRight);

        // store in the spectrum buffers
        SpectrumBuffer2Left = m_averageMagnitudeSpectrum2Left;
        SpectrumBuffer2Right = m_averageMagnitudeSpectrum2Right;
        
        // set the filled flag and toggle the buffer
        spectrumBuffer2Filled = true;
        m_spectrumBufferIndex = 1;

        // release the semaphore for buffer 2
        SemSpectrumBuffer2.release(1);
    }
}


void SpectrumCalculationThread::normalizeSpectra()
{
  double newMagnitudeNormalizationValueLeft;
  double newMagnitudeNormalizationValueRight;

  // determine the normalization values for both channels depending on the normalization mode
  switch (m_normalizationMode)
  {
    case 0: // with max of both channels
    {
      double overallMaxMagnitude = std::max(m_maxMagnitudeLeft,m_maxMagnitudeRight);
      newMagnitudeNormalizationValueLeft = overallMaxMagnitude;
      newMagnitudeNormalizationValueRight = overallMaxMagnitude;
      break;
    }
    case 1:  // with max of left channel
    {
      newMagnitudeNormalizationValueLeft = m_maxMagnitudeLeft;
      newMagnitudeNormalizationValueRight = m_maxMagnitudeLeft;
      break;
    }
    case 2:  // with max of right channel
    {
      newMagnitudeNormalizationValueLeft = m_maxMagnitudeRight;
      newMagnitudeNormalizationValueRight = m_maxMagnitudeRight;
      break;
    }
    case 3:  // with max of each channel separately
    {
      newMagnitudeNormalizationValueLeft = m_maxMagnitudeLeft;
      newMagnitudeNormalizationValueRight = m_maxMagnitudeRight;
      break;
    }
    default:
    {
      double overallMaxMagnitude = std::max(m_maxMagnitudeLeft,m_maxMagnitudeRight);
      newMagnitudeNormalizationValueLeft = overallMaxMagnitude;
      newMagnitudeNormalizationValueRight = overallMaxMagnitude;
    }
  }

  // do the normalization of both channel spectra
  spectrumCalculatorLeft->normalizeMagnitudeSpectrumVal(newMagnitudeNormalizationValueLeft);
  spectrumCalculatorRight->normalizeMagnitudeSpectrumVal(newMagnitudeNormalizationValueRight);
}
