#ifndef SPECTRUMAAVERAGER_H
#define SPECTRUMAAVERAGER_H

#include <eigen3/Eigen/Dense>
#include "SpectrumParameter.h"
#include "VectorIIRFilter.h"

using namespace Eigen;

class SpectrumAverager
{
public:
    SpectrumAverager(bool initIIRorFIR, SpectrumParameter *initSpectrumParameter, uint16_t initTargetNumberAverages);
    VectorXd returnAverageMagnitudeSpectrum();
    VectorXd updateAverageMagnitudeSpectrum(VectorXd newMagnitudeSpectrum);
    void resetAveraging();
    void restart();

private:
    void resetIIRAveraging();
    void resetFIRAveraging();
    VectorXd updateAverageMagnitudeSpectrumFIR(VectorXd newMagnitudeSpectrum);
    VectorXd updateAverageMagnitudeSpectrumIIR(VectorXd newMagnitudeSpectrum);

private:
  // double m_sampleFrequency;
  bool m_IIRorFIR;   // select the filter type: FIR or IIR (if true --> default should be IIR of first order)
  const double m_initAlpha = 0.9;
  SpectrumParameter *m_spectrumParameter;
  VectorXd m_frequencyRange;
  VectorXd m_averageMagnitudeSpectrum;
  uint16_t m_actualNumberAverages;
  uint16_t m_targetNumberAverages;
  VectorIIRFilter* m_vectorIIR;
};

#endif // SPECTRUMAAVERAGER_H
