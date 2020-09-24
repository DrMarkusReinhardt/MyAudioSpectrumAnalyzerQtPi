#include "SpectrumAverager.h"

SpectrumAverager::SpectrumAverager(bool initIIRorFIR, SpectrumParameter *initSpectrumParameter,
                                   uint16_t initTargetNumberAverages) :
    m_IIRorFIR(initIIRorFIR),
    m_spectrumParameter(initSpectrumParameter),
    m_targetNumberAverages(initTargetNumberAverages)
{
    // reset the magnitude spectrum averaging
    resetAveraging();
}

void SpectrumAverager::resetAveraging()
{
  if (m_IIRorFIR)
    resetIIRAveraging();
  else
    resetFIRAveraging();
}

void SpectrumAverager::resetIIRAveraging()
{
  const double initAlpha = 0.9;
  // if (m_vectorIIR != nullptr)
  //     delete m_vectorIIR;
  m_vectorIIR = new VectorIIRFilter(m_spectrumParameter->getFrequencyRange().size(), initAlpha);
}

void SpectrumAverager::resetFIRAveraging()
{
  // reset the magnitude spectrum averaging
  m_actualNumberAverages = 0;
  m_averageMagnitudeSpectrum = VectorXd::Zero(m_spectrumParameter->getFrequencyRange().size());
}

VectorXd SpectrumAverager::returnAverageMagnitudeSpectrum()
{
   return m_averageMagnitudeSpectrum;
}

VectorXd SpectrumAverager::updateAverageMagnitudeSpectrum(VectorXd newMagnitudeSpectrum)
{
    if (m_IIRorFIR)
        m_averageMagnitudeSpectrum = updateAverageMagnitudeSpectrumIIR(newMagnitudeSpectrum);
    else
        m_averageMagnitudeSpectrum = updateAverageMagnitudeSpectrumFIR(newMagnitudeSpectrum);
    return m_averageMagnitudeSpectrum;
}

VectorXd SpectrumAverager::updateAverageMagnitudeSpectrumFIR(VectorXd newMagnitudeSpectrum)
{
    m_averageMagnitudeSpectrum += newMagnitudeSpectrum;
    if (m_actualNumberAverages > 0)
    {
        double rescaleValue = 1.0; // m_actualNumberAverages/(m_actualNumberAverages+1);
        for(uint16_t k = 0; k < m_averageMagnitudeSpectrum.size(); k++)
            m_averageMagnitudeSpectrum[k] *= rescaleValue;
    }
    m_actualNumberAverages++;
    return m_averageMagnitudeSpectrum;
}

VectorXd SpectrumAverager::updateAverageMagnitudeSpectrumIIR(VectorXd newMagnitudeSpectrum)
{
    m_averageMagnitudeSpectrum = m_vectorIIR->filter(newMagnitudeSpectrum);
    return m_averageMagnitudeSpectrum;
}
