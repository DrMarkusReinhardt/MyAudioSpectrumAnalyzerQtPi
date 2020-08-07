#ifndef SPECTRUMCALCULATOR_H
#define SPECTRUMCALCULATOR_H

#include "DTFT.h"
#include <vector>
#include <complex>
#include <cmath>
#include "SpectrumParameter.h"
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class SpectrumCalculator
{
public:
  SpectrumCalculator(double initSampleFrequency, SpectrumParameter *initSpectrumParameter);
  void calculateSpectrum(const VectorXd& inputSignal);
  VectorXd returnFrequencyRange();
  void getMaxMagnitudeSpectrum(double& maximumMagnitudeValue, double& maxFrequencyValue);
  void normalizeMagnitudeSpectrum();
  void normalizeMagnitudeSpectrumVal(double maxValue);
  VectorXd returnMagnitudeSpectrum();
  VectorXcd returnFourierTransform();

private:
  double m_sampleFrequency;
  SpectrumParameter *m_spectrumParameter;
  VectorXd m_frequencyRange;
  VectorXd m_magnitudeSpectrum;
  VectorXcd m_fourierTransform;
  DTFT *dtft;
};

#endif // SPECTRUMCALCULATOR_H
