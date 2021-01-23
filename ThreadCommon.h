/* ThreadCommon.h
 *
 * Common data structures and related semaphores for all signal processing threads
 *
 */
#pragma once

#include <QSemaphore>
#include <QVector>
#include <eigen3/Eigen/Dense>

extern const uint16_t signalBufferSize;   // the signal buffer size
extern const uint16_t spectrumBufferSize; // the spectrum buffer size
extern QSemaphore SemSignalBuffer1;    // semaphore to access signal buffer 1, left and right
extern QSemaphore SemSignalBuffer2;    // semaphore to access signal buffer 2, left and right
extern QSemaphore SemSpectrumBuffer1;  // semaphore to access spectrum buffer 1, left and right
extern QSemaphore SemSpectrumBuffer2;  // semaphore to access spectrum buffer 2, left and right
extern VectorXd SignalTimeBuffer1Left; // the first buffer for time samples of the left signal
extern VectorXd SignalTimeBuffer2Left; // the second buffer for time samples of the left signal
extern VectorXd SignalTimeBuffer1Right;// the first buffer for time samples of the right signal
extern VectorXd SignalTimeBuffer2Right;// the second buffer for time samples of the right signal
extern VectorXcd SignalBuffer1Left;   // the first buffer for signal samples of the left channel
extern VectorXcd SignalBuffer2Left;   // the second buffer for signal samples of the left channel
extern VectorXcd SignalBuffer1Right;  // the first buffer for signal samples of the right channel
extern VectorXcd SignalBuffer2Right;  // the second buffer for signal samples of the right channel
extern VectorXcd SpectrumBuffer1Left; // the first buffer for magnitude spectrum samples of the left channel
extern VectorXcd SpectrumBuffer2Left; // the second buffer for magnitude spectrum samples of the left channel
extern VectorXcd SpectrumBuffer1Right;// the first buffer for magnitude spectrum samples of the right channel
extern VectorXcd SpectrumBuffer2Right;// the second buffer for magnitude spectrum samples of the right channel
extern bool signalBuffer1Filled;  // flag to signal when the buffer is filled for further processing
extern bool signalBuffer2Filled;  // flag to signal when the buffer is filled for further processing
extern bool spectrumBuffer1Filled;// flag to signal when the buffer is filled for further processing
extern bool spectrumBuffer2Filled;// flag to signal when the buffer is filled for further processing
