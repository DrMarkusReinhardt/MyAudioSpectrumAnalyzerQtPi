#ifndef SPECTRUMCALCULATOR_H
#define SPECTRUMCALCULATOR_H

#include "DTFT.h"
#include <vector>
#include <complex>
#include <cmath>

// double vector type
typedef std::vector<double> VectorXd;
typedef std::vector<std::complex<double> > VectorXcd;

class SpectrumCalculator
{
public:
  SpectrumCalculator(uint16_t initNoFrequencySamples);
  void calculateSpectrum(const VectorXd& inputSignal);
  VectorXd returnFrequencyRange();
  void normalizeMagnitudeSpectrum();
  VectorXd returnMagnitudeSpectrum();

private:
  const double samplingFrequency = 44100.0;
  uint16_t noFrequencySamples;
  const double minFrequencyRange = 1000;
  const double maxFrequencyRange = 10000;
  double deltaF;
  VectorXd frequencyRange;
  VectorXd magnitudeSpectrum;
  VectorXcd fourierTransform;
  DTFT *dtft;
};

#endif // SPECTRUMCALCULATOR_H
