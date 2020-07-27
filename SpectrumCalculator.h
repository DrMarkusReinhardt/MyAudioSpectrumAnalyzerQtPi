#ifndef SPECTRUMCALCULATOR_H
#define SPECTRUMCALCULATOR_H

#include "DTFT.h"
#include <vector>
#include <complex>
#include <cmath>
#include "SpectrumParameter.h"

// double vector type
typedef std::vector<double> VectorXd;
typedef std::vector<std::complex<double> > VectorXcd;

class SpectrumCalculator
{
public:
  SpectrumCalculator(double initSampleFrequency, SpectrumParameter initSpectrumParameter);
  void calculateSpectrum(const VectorXd& inputSignal);
  VectorXd returnFrequencyRange();
  void normalizeMagnitudeSpectrum();
  VectorXd returnMagnitudeSpectrum();

private:
  double m_sampleFrequency;
  SpectrumParameter m_spectrumParameter;
  VectorXd m_magnitudeSpectrum;
  VectorXcd m_fourierTransform;
  DTFT *dtft;
};

#endif // SPECTRUMCALCULATOR_H
