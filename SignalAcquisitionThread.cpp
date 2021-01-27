#include "SignalAcquisitionThread.h"

SignalAcquisitionThread::SignalAcquisitionThread(PortaudioHandler *initPAHandler)
 : paHandler(initPAHandler)
{
  m_signalBufferIndex = 1;
}

void SignalAcquisitionThread::updatePA()
{
  // std::cout << "PA handler before start" << std::endl;
  paHandler->start();                                          // start input stream processing
  // std::cout << "PA handler started and waiting" << std::endl;
  paHandler->wait();                      // wait until the Portaudio handler returns from sampling
  // paHandler->write2screen();
  // std::cout << "PA handler after wait" << std::endl;
  paHandler->transferSampledData2Channels();
  paHandler->stop();                                           // stop input stream processing
  // std::cout << "PA handler stopped" << std::endl;
}

void SignalAcquisitionThread::run()
{
    while(true)
    {
        if (m_signalBufferIndex == 1)
        {
            std::cout << "SignalAcquisitionThread: run(): buffer index = " << m_signalBufferIndex << std::endl;

            // acquire the semaphore for signal buffer 1 (left and right channels)
            SemSignalBuffer1.acquire(1);
            std::cout << "SignalAcquisitionThread: acquired the semaphore for signal buffer 1" << std::endl;

            if (signalBuffer1Filled == false)
            {
                // fill signal buffer 1
                // sleep(1);
                fillSignalBuffer();
                signalBuffer1Filled = true;

                // switch to the second buffer
                m_signalBufferIndex = 2;

                // signal to the parent routine that the buffer is filled
                emit onSignalBufferFilled(m_signalBufferIndex);
            }

            // release the semaphore for signal buffer 1
            SemSignalBuffer1.release(1);
            std::cout << "SignalAcquisitionThread: released the semaphore for signal buffer 1" << std::endl;
        }
        else if (m_signalBufferIndex == 2)
        {
            std::cout << "SignalAcquisitionThread: run(): buffer index = " << m_signalBufferIndex << std::endl;

            // acquire the semaphore for signal buffer 2
            SemSignalBuffer2.acquire(1);
            std::cout << "SignalAcquisitionThread: acquired the semaphore for signal buffer 2" << std::endl;

            if (signalBuffer2Filled == false)
            {
                // fill signal buffer 2
                fillSignalBuffer();
                signalBuffer2Filled = true;

                // switch to the first buffer
                m_signalBufferIndex = 1;

                // signal to the parent routine that the buffer is filled
                emit onSignalBufferFilled(m_signalBufferIndex);
            }

            // release the semaphore for signal buffer 2
            SemSignalBuffer2.release(1);
            std::cout << "SignalAcquisitionThread: released the semaphore for signal buffer 2" << std::endl;
        }
        msleep(100);
    }
}

void SignalAcquisitionThread::fillSignalBuffer()
{
    if (m_signalBufferIndex == 1)
    {
        std::cout << "SignalAcquisitionThread: fill signal buffer 1" << std::endl;
        updatePA();
        SignalTimeBuffer1Left = paHandler->returnDiscreteTimeSamples(); // get the discrete time samples from the Portaudio handler
        SignalTimeBuffer1Right = SignalTimeBuffer1Left;      // time samples for left and right channel are equal
        SignalBuffer1Left = paHandler->returnSignalLeft();   // get the data of the left channel from the Portaudio handler
        SignalBuffer1Right = paHandler->returnSignalRight(); // get the data of the right channel from the Portaudio handler
    }
    else if (m_signalBufferIndex == 2)
    {
        std::cout << "SignalAcquisitionThread: fill signal buffer 2" << std::endl;
        updatePA();
        SignalTimeBuffer2Left = paHandler->returnDiscreteTimeSamples(); // get the discrete time samples from the Portaudio handler
        SignalTimeBuffer2Right = SignalTimeBuffer2Left;      // time samples for left and right channel are equal
        SignalBuffer2Left = paHandler->returnSignalLeft();   // get the data of the left channel from the Portaudio handler
        SignalBuffer2Right = paHandler->returnSignalRight(); // get the data of the right channel from the Portaudio handler
    }
}
