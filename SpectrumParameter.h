#ifndef SPECTRUMPARAMETER_H
#define SPECTRUMPARAMETER_H

#include <vector>

typedef std::vector<double> VectorXd;

struct SpectrumParameter
{
   double minFrequencyRange;
   double maxFrequencyRange;
   VectorXd frequencyRange;
   uint16_t noSpectrumSamples;
};

#endif // SPECTRUMPARAMETER_H
