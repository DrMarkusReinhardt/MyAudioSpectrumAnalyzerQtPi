#ifndef STORESIGNALSPECTRUMDATA_H
#define STORESIGNALSPECTRUMDATA_H

#include <iostream>
#include <vector>
#include <complex>
#include <QString>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <eigen3/Eigen/Dense>

using namespace Eigen;

class StoreSignalSpectrumData
{
public:
    StoreSignalSpectrumData(VectorXd init_t, VectorXd init_leftSignal, VectorXd init_rightSignal,
                            VectorXd init_f, VectorXd init_leftSpectrum, VectorXd init_rightSpectrum);

    void save();
    void saveAs(QString FileName);

private:
    void saveData(QString FileName);
    VectorXd m_t;
    VectorXd m_leftSignal;
    VectorXd m_rightSignal;
    VectorXd m_f;
    VectorXd m_leftSpectrum;
    VectorXd m_rightSpectrum;
    const QString m_defaultFileName = "SignalAndSpectra.csv";
};

#endif // STORESIGNALSPECTRUMDATA_H
