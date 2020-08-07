#ifndef SPECTRUMPARAMETER_H
#define SPECTRUMPARAMETER_H

#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class SpectrumParameter
{

 public:
   SpectrumParameter(double initMinFrequencyRange, double initMaxFrequencyRange, uint16_t initNoFrequencySamples)
       : m_minFrequency(initMinFrequencyRange), m_maxFrequency(initMaxFrequencyRange),
         m_noFrequencySamples(initNoFrequencySamples)
   {
       if (initNoFrequencySamples > 0)
          calcFrequencyRange();
       else
          std::cout << "error: number of frequency samples is less than 1" << std::endl;
   }

   double getMinFrequency() { return m_minFrequency; }

   double getMaxFrequency() { return m_maxFrequency; }

   uint16_t getNoFrequencySamples() { return m_noFrequencySamples; }

   VectorXd getFrequencyRange() { return m_frequencyRange; }

   void setMinFrequency(double minFrequency)
   {
       m_minFrequency = minFrequency;
       calcFrequencyRange();
   }

   void setMaxFrequency(double maxFrequency)
   {
       m_maxFrequency = maxFrequency;
       calcFrequencyRange();
   }

   void setNoFrequencySamples(uint16_t noFrequencySamples)
   {
       m_noFrequencySamples = noFrequencySamples;
       calcFrequencyRange();
   }

   void calcFrequencyRange()
   {
       double deltaF = (m_maxFrequency - m_minFrequency) / (m_noFrequencySamples - 1);
       m_frequencyRange.resize(m_noFrequencySamples);
       for(uint16_t k = 0; k < m_noFrequencySamples; k++)
           m_frequencyRange[k] = k * deltaF + m_minFrequency;
   }

 private:
   double m_minFrequency;
   double m_maxFrequency;
   uint16_t m_noFrequencySamples;
   VectorXd m_frequencyRange;
};

#endif // SPECTRUMPARAMETER_H
