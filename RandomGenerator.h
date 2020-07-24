#include <random>
#include <cmath>
#include <vector>

using namespace std;

class NormalRandomGenerator
{
  public:
    NormalRandomGenerator(double mean, double stddev) : m_mean(mean), m_stddev(stddev)
    {}
    
    vector<double> generate(uint16_t noSamples)
    {
      vector<double> returnArray(noSamples);
	  for(uint16_t n = 0; n < noSamples; n++)
         returnArray[n] = m_distrib(m_generator);
      return returnArray;
    }
    
  private:
    std::random_device m_rd{};
    std::mt19937 m_generator{m_rd()};
 
    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    double m_mean;
    double m_stddev;
    std::normal_distribution<> m_distrib{m_mean,m_stddev};
};

