/*  Windowed DTFT in C++

    To be included into the Python program of a audio spectrum analyzer
    for the Raspberry Pi 3B+ with Pisound board to speed-up the calculations.

    Notes:
    It is a header only library.

    Author: Dr. Markus Reinhardt
    History: Creation, 27th June 2020
             Tested successfully 30th June 2020

*/

#include <iostream>
#include <vector>
#include <complex>
#include "boost/math/special_functions/chebyshev.hpp"

// double vector type
typedef std::vector<double> VectorXd;

// complex vector type
typedef std::complex<double>  zScalar;
typedef std::vector<std::complex<double> > VectorXcd;

/* windowed Discrete Time Fourier Transform (DTFT)  */
class DTFT
{
public:
  // the constructor
  DTFT(double initSamplingFrequency) : samplingFrequency(initSamplingFrequency)
  {
      m_Ts = 1.0 / samplingFrequency;
  }

  // calculate the spectrum
  void calculateSpectrum(VectorXd &magnitudeSpectrum, VectorXcd& fourierTransform,
                         VectorXd inputSignal, VectorXd frequencyRange)
  {
    uint16_t lenInputSignal = inputSignal.size();
    uint16_t lenOutputSignal = frequencyRange.size();
    VectorXd window(lenInputSignal);
    calculateWindow(window, lenInputSignal);
    // std::cout << "window = " << std::endl << window << std::endl;
    // std::cout << "window calculated" << std::endl;
    zScalar j(0.0,1.0);
    zScalar exparg;
    for(uint16_t freqIndex = 0; freqIndex < lenOutputSignal; freqIndex++)
    {
      zScalar spectrumVal(0.0,0.0);
      double f = frequencyRange[freqIndex];
      for(uint16_t k = 0; k < lenInputSignal; k++)
      {
        exparg = -j * 2.0 * M_PI * f * (double)k * m_Ts;
        spectrumVal += window[k] * inputSignal[k] * exp(exparg);
      }
      // std::cout << "exparg = " << exparg << std::endl;
      // std::cout << "spectrumVal = " << spectrumVal << std::endl;
      fourierTransform[freqIndex] = spectrumVal;
      magnitudeSpectrum[freqIndex] = 10.0 * log10(abs(spectrumVal*conj(spectrumVal)));
      // std::cout << "fourierTransform = " << fourierTransform[freqIndex]  << std::endl;
      // std::cout << "magnitudeSpectrum( = " << magnitudeSpectrum[freqIndex]  << std::endl;
    }
  }

  // private functions:
private:
  // calculate the window
  void calculateWindow(VectorXd &window, uint16_t lenInputSignal)
  {
    double atten = 60.0;
    chebyshev_window(window, lenInputSignal, atten);
  }

  void chebyshev_window(VectorXd &out, int N, double atten)
  {
    int nn, i;
    double M, n, sum = 0.0, max = 0.0;
    double tg = pow(10.0, atten/20.0);  /* 1/r term [2], 10^gamma [2] */
    double x0 = cosh((1.0/(N-1))*acosh(tg));
    M = (N-1) / 2.0;
    if (N % 2 == 0)
      M = M + 0.5; /* handle even length windows */
    for(nn=0; nn<(N/2+1); nn++)
    {
      n = nn - M;
      sum = 0;
      for(i = 1; i <= M; i++)
      {
        sum += cheby_poly2(N - 1, x0 * cos(M_PI * i / N)) * cos(2.0 * n * M_PI * i / N);
        // std::cout << "sum = " << std::endl << sum << std::endl;
      }
      out[nn] = tg + 2.0 * sum;
      out[N - nn - 1] = out[nn];
      if (out[nn] > max)
        max = out[nn];
    }
    for(nn = 0; nn < N; nn++)
      out[nn] /= max; /* normalise everything */
    // std::cout << "window = " << std::endl << out << std::endl;

    return;
  }

  /**************************************************************************
  * This function computes the chebyshev polyomial T_n(x)
  * Note: not working correctly on Raspberry Pi  Ubuntu18.04
  ***************************************************************************/
  double cheby_poly(int n, double x)
  {
    std::cout << "n = " << n << " ";
    std::cout << "x = " << x << " ";
    double res;
    if (abs(x) <= 1)
      res = cos(double(n)*acos(x));
    else
      res = cosh(double(n)*acosh(x));
    std::cout << "res = " << res << std::endl;
    return res;
  }

  /**************************************************************************
  This function computes the chebyshev polyomial T_n(x)
  * Note: working correctly on Raspberry Pi  Ubuntu18.04 with latest
  *       boost library 1.73
  ***************************************************************************/
  double cheby_poly2(int n, double x)
  {
    double res = boost::math::chebyshev_t(n,x);
    return res;
  }

  //  private variables
private:
  double samplingFrequency;
  double m_Ts;
};
