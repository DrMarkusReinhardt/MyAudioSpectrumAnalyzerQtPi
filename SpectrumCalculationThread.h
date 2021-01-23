#pragma once
#include <QObject>
#include "DTFT.h"
#include <vector>
#include <complex>
#include <cmath>
#include "SpectrumParameter.h"
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class SpectrumCalculationThread : public QThread
{
public:
    SpectrumCalculationThread();

private:

};
