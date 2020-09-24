/* A vectorial IIR filter of first order, the filter averages vectors
 * instead of scalar samples like a scalar IIR filter
 *
 * Author: Dr. Markus Reinhardt
 * History: 24th Sept. 2020 creation
 */
#ifndef VECTORIIRFILTER_H
#define VECTORIIRFILTER_H

#include <iostream>
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class VectorIIRFilter
{
public:
    VectorIIRFilter(uint16_t initLengthVector, double initAlpha);
    VectorXd filter(VectorXd newSample);

private:
    uint16_t m_lengthVector;
    double m_alpha;
    double m_oneMinusAlpha;
    VectorXd m_state;
    bool m_init;   // init = true at startup of filter, then false
};

#endif // VECTORIIRFILTER_H
