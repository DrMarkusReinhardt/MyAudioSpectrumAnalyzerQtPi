/* A vectorial IIR filter, the filter averages vectors instead of scalar samples like a scalar IIR filter
 * 
 * Author: Dr. Markus Reinhardt
 * History: 24th Sept. 2020 creation
 */

#include "VectorIIRFilter.h"

VectorIIRFilter::VectorIIRFilter(uint16_t initLengthVector, double initAlpha) :
    m_lengthVector(initLengthVector), m_alpha(initAlpha)
{
  m_init = true;
  m_oneMinusAlpha = 1.0 - m_alpha;
  m_state.resize(initLengthVector);
}

VectorXd VectorIIRFilter::filter(VectorXd newSample)
{
  if (m_init)
  {
      m_state = newSample;
      std::cout << "IIR init" << std::endl;
  }
  else
  {
      std::cout << "IIR avg" << std::endl;
      m_state = m_alpha * m_state + m_oneMinusAlpha * newSample;
      m_init = false;
  }
  return m_state;
}
