#pragma once
#include "PortaudioHandler.h"
#include <eigen3/Eigen/Dense>
#include <chrono>
#include <QObject>
#include <QThread>
#include <iostream>
#include <cmath>
#include "ThreadCommon.h"

class SignalAcquisitionThread : public QThread
{ 
    Q_OBJECT

public:
    SignalAcquisitionThread(PortaudioHandler *initPAHandler);
    virtual void run() override;
    void updatePA();
    void fillSignalBuffer();

signals:
    void onSignalBufferFilled(int bufferIndex);

private:
    PortaudioHandler *paHandler;

    // channel activity flags
    bool m_leftChannelActive;
    bool m_rightChannelActive;
    int m_signalBufferIndex;
};
