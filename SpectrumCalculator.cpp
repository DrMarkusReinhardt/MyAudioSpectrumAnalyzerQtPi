#include "SpectrumCalculator.h"

SpectrumCalculator::SpectrumCalculator(uint16_t initNoFrequencySamples) : noFrequencySamples(initNoFrequencySamples)
{
    deltaF = (maxFrequencyRange - minFrequencyRange) / (noFrequencySamples - 1);
    frequencyRange.resize(noFrequencySamples);
    for(uint16_t k = 0; k < noFrequencySamples; k++)
       frequencyRange[k] = k * deltaF + minFrequencyRange;
    magnitudeSpectrum.resize(frequencyRange.size());
    fourierTransform.resize(frequencyRange.size());

    dtft = new DTFT(samplingFrequency,frequencyRange);
}

void SpectrumCalculator::calculateSpectrum(const VectorXd& inputSignal)
{
    // calculate the spectrum
    // std::cout << "spectrum calculation started!" << std::endl;
    dtft->calculateSpectrum(magnitudeSpectrum, fourierTransform, inputSignal);
    // std::cout << "Fourier transform = " << std::endl << fourierTransform << std::endl;
    // std::cout << "Magnitude spectrum = " << std::endl << magnitudeSpectrum << std::endl;
    // std::cout << "spectrum calculation done!" << std::endl;
}

void SpectrumCalculator::normalizeMagnitudeSpectrum()
{
    double maximumMagnitudeValue = -160.0;
    for(uint16_t k = 0; k < magnitudeSpectrum.size(); k++)
       maximumMagnitudeValue = std::fmax(maximumMagnitudeValue,magnitudeSpectrum[k]);

    for(uint16_t k = 0; k < magnitudeSpectrum.size(); k++)
       magnitudeSpectrum[k] = magnitudeSpectrum[k] - maximumMagnitudeValue;
}


VectorXd SpectrumCalculator::returnFrequencyRange()
{
    return frequencyRange;
}

VectorXd SpectrumCalculator::returnMagnitudeSpectrum()
{
   return magnitudeSpectrum;
}
