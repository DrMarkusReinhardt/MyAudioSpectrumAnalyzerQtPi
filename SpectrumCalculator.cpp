#include "SpectrumCalculator.h"

SpectrumCalculator::SpectrumCalculator(double initSampleFrequency, SpectrumParameter *initSpectrumParameter) :
    m_sampleFrequency(initSampleFrequency), m_spectrumParameter(initSpectrumParameter)
{
  // initialize the spectrum calculation arrays
  m_magnitudeSpectrum.resize(m_spectrumParameter->getFrequencyRange().size());
  m_fourierTransform.resize(m_spectrumParameter->getFrequencyRange().size());

  // create the DTFT calculator object
  dtft = new DTFT(m_sampleFrequency);
}

// calculate the spectrum
void SpectrumCalculator::calculateSpectrum(const VectorXd& inputSignal)
{
  m_frequencyRange = m_spectrumParameter->getFrequencyRange();
  dtft->calculateSpectrum(m_magnitudeSpectrum, m_fourierTransform, inputSignal, m_frequencyRange);
  // std::cout << "Fourier transform = " << std::endl << fourierTransform << std::endl;
  // std::cout << "Magnitude spectrum = " << std::endl << magnitudeSpectrum << std::endl;
}

void SpectrumCalculator::getMaxMagnitudeSpectrum(double& maximumMagnitudeValue, double& maxFrequencyValue)
{
    maximumMagnitudeValue = -160.0;
    maxFrequencyValue = 0.0;
    for(uint16_t k = 0; k < m_magnitudeSpectrum.size(); k++)
    {
      if (m_magnitudeSpectrum[k] > maximumMagnitudeValue)
      {
        maximumMagnitudeValue = m_magnitudeSpectrum[k];
        maxFrequencyValue = m_frequencyRange[k];
      }
    }
}

// normalize the spectrum
void SpectrumCalculator::normalizeMagnitudeSpectrum()
{
  double maximumMagnitudeValue;
  double maxFrequencyValue;
  getMaxMagnitudeSpectrum(maximumMagnitudeValue, maxFrequencyValue);
  for(uint16_t k = 0; k < m_magnitudeSpectrum.size(); k++)
    m_magnitudeSpectrum[k] -= maximumMagnitudeValue;
}

// normalize the spectrum
void SpectrumCalculator::normalizeMagnitudeSpectrumVal(double maxValue)
{
  for(uint16_t k = 0; k < m_magnitudeSpectrum.size(); k++)
    m_magnitudeSpectrum[k] -= maxValue;
}

VectorXd SpectrumCalculator::returnFrequencyRange()
{
  return m_spectrumParameter->getFrequencyRange();
}

VectorXcd SpectrumCalculator::returnFourierTransform()
{
  return m_fourierTransform;
}

VectorXd SpectrumCalculator::returnMagnitudeSpectrum()
{
  return m_magnitudeSpectrum;
}
