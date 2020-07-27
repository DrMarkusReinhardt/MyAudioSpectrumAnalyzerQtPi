#include "SpectrumCalculator.h"

SpectrumCalculator::SpectrumCalculator(double initSampleFrequency, SpectrumParameter initSpectrumParameter) :
    m_sampleFrequency(initSampleFrequency), m_spectrumParameter(initSpectrumParameter)
{
  // initialize the spectrum calculation arrays
  m_magnitudeSpectrum.resize(m_spectrumParameter.frequencyRange.size());
  m_fourierTransform.resize(m_spectrumParameter.frequencyRange.size());

  // create the DTFT calculator object
  dtft = new DTFT(m_sampleFrequency, m_spectrumParameter.frequencyRange);
}

void SpectrumCalculator::calculateSpectrum(const VectorXd& inputSignal)
{
  // calculate the spectrum
  dtft->calculateSpectrum(m_magnitudeSpectrum, m_fourierTransform, inputSignal);
  // std::cout << "Fourier transform = " << std::endl << fourierTransform << std::endl;
  // std::cout << "Magnitude spectrum = " << std::endl << magnitudeSpectrum << std::endl;
}

void SpectrumCalculator::normalizeMagnitudeSpectrum()
{
  double maximumMagnitudeValue = -160.0;
  for(uint16_t k = 0; k < m_magnitudeSpectrum.size(); k++)
    maximumMagnitudeValue = std::fmax(maximumMagnitudeValue,m_magnitudeSpectrum[k]);

  for(uint16_t k = 0; k < m_magnitudeSpectrum.size(); k++)
    m_magnitudeSpectrum[k] = m_magnitudeSpectrum[k] - maximumMagnitudeValue;
}

VectorXd SpectrumCalculator::returnFrequencyRange()
{
  return m_spectrumParameter.frequencyRange;
}

VectorXd SpectrumCalculator::returnMagnitudeSpectrum()
{
  return m_magnitudeSpectrum;
}
