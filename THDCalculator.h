#ifndef THDCALCULATOR_H
#define THDCALCULATOR_H

/* A class to calculate for a given base frequency the total harmonic distortion (THD) of
 * a given signal by calculating the magnitude levels at the overtones (multiples of the
 * base frequency) with the help of the DTFT.
 */
#include <vector>
#include <complex>
#include "DTFT.h"

// double vector type
typedef std::vector<double> VectorXd;
typedef std::vector<std::complex<double> > VectorXcd;

class THDCalculator
{
public:
    THDCalculator(double initBaseFrequency, uint8_t initNumberOvertones,
                  double initSampleFrequency);
    void initTHDCalculation();
    void calcTHD(double& THD_F, double& THD_R, VectorXd inputSignal);
    void setBaseFrequency(double newBaseFrequency);
    void setNoOvertones(double newNumberOvertones);

private:
    double baseFrequency;
    uint8_t numberOvertones;
    VectorXd overtones;
    VectorXd magnitudesOvertones;
    VectorXcd fourierTransformOvertones;
    double sampleFrequency;
    DTFT *dtft;
};

#endif // THDCALCULATOR_H
