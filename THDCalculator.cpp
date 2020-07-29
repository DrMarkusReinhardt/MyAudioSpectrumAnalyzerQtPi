#include "THDCalculator.h"

THDCalculator::THDCalculator(double initBaseFrequency, uint8_t initNumberOvertones,
                             double initSampleFrequency)
    : baseFrequency(initBaseFrequency), numberOvertones(initNumberOvertones),
      sampleFrequency(initSampleFrequency)
{
   // initialize the vector of overtones and their Fourier transform and magnitudes
   // to prepare for the THD calculation
   initTHDCalculation();

   // create the DTFT calculator object
   dtft = new DTFT(sampleFrequency);
}

void THDCalculator::calcTHD(double& THD_F, double& THD_R, VectorXd inputSignal)
{
    // determine the DTFT for the base frequency and the overtones
    dtft->calculateSpectrum(magnitudesOvertones, fourierTransformOvertones,
                            inputSignal, overtones);
    double distortionPower = 0.0;
    double totalPower = 0.0;
    double basePower;
    for(uint8_t k = 1; k < numberOvertones; k++)
        distortionPower += std::fabs(fourierTransformOvertones[k] * conj(fourierTransformOvertones[k]));
    basePower = std::fabs(fourierTransformOvertones[0] * conj(fourierTransformOvertones[0]));
    totalPower = distortionPower + basePower;
    THD_F = std::sqrt(distortionPower / basePower);
    THD_R = std::sqrt(distortionPower / totalPower);
}

void THDCalculator::setNoOvertones(double newNumberOvertones)
{
   numberOvertones = newNumberOvertones;
}

void THDCalculator::setBaseFrequency(double newBaseFrequency)
{
   baseFrequency = newBaseFrequency;
}

void THDCalculator::initTHDCalculation()
{
    zScalar complexZero(0.0,0.0);
    overtones.resize(numberOvertones);
    magnitudesOvertones.resize(numberOvertones);
    fourierTransformOvertones.resize(numberOvertones);
    for(uint8_t k = 0; k < numberOvertones; k++)
    {
        overtones[k] = (k+1) * baseFrequency;
        magnitudesOvertones[k] = 0.0;
        fourierTransformOvertones[k] = complexZero;
    }
}
