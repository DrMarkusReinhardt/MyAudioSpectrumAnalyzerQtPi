#ifndef PORTAUDIOHANDLER_H
#define PORTAUDIOHANDLER_H
/** @file PortaudioHandler.h
	@ingroup examples_src
	@brief Class to record audio
	@author Dr. M. Reinhardt, based on example code from Ross Bencina <rossb@audiomulch.com> and
    @author Phil Burk <philburk@softsynth.com>
*/
/*
 * $Id: PortaudioHandler.cpp 1752 2011-09-08 03:21:55Z philburk $
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */
#include <stdio.h>
#include <math.h>
#include <vector>
#include "portaudio.h"
#include <iostream>

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

#define NUM_CHANNELS (2)
#define FRAMES_PER_BUFFER (2*512)

#include "DefineDataSizes.h"

typedef struct
{
  int          frameIndex;  /* Index into sample array. */
  int          maxFrameIndex;
  SAMPLE      *recordedSamples;
}
paTestData;

class PortaudioHandler
{
public:
  PortaudioHandler(uint16_t initNoFrames, uint8_t initNoChannels,uint32_t initSampleRate)
    : _noFrames(initNoFrames), _noChannels(initNoChannels), _sampleRate(initSampleRate)
  {
    _samplePeriod = 1.0 /(double)_sampleRate;

    // initialize the PA system
    _err = initializeDataArray();
    if (_err == paNoError)
    {
      _err = Pa_Initialize();
      if ( _err != paNoError)
      {
        // initialization went wrong
        Pa_Terminate();
        if( _data.recordedSamples )       /* Sure it is NULL or valid. */
          free( _data.recordedSamples );
        if( _err != paNoError )
        {
          fprintf( stderr, "An error occured while using the portaudio stream\n" );
          fprintf( stderr, "Error number: %d\n", _err );
          fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( _err ) );
          _err = 1;          /* Always return 0 or 1, but no other return codes. */
        }
      }
    }
  } // end of constructor

  ~PortaudioHandler()
  {
    if (_err == paNoError)
    {
      Pa_Terminate();
    }
  }

  // open the recording stream
  bool open()
  {
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice)
      return false;
    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(inputParameters.device);
    if (pInfo != 0)
      printf("Input device name: '%s'\r", pInfo->name);
    inputParameters.channelCount = _noChannels;       /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;           /* 32 bit floating point output */
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowOutputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    _err = Pa_OpenStream(
             &_stream,
             &inputParameters,
             NULL,                  /* &outputParameters, */
             _sampleRate,
             FRAMES_PER_BUFFER,
             paClipOff,      /* we won't output out of range samples so don't bother clipping them */
             &PortaudioHandler::paCallback,
             &_data );

    if (_err != paNoError) /* Failed to open stream to device !!! */
      return false;

    _err = Pa_SetStreamFinishedCallback( _stream, &PortaudioHandler::paStreamFinished );

    if (_err != paNoError)
    {
      Pa_CloseStream( _stream );
      _stream = 0;
      return false;
    }
    return true;
  }  // end of open()


  // close the stream
  bool close()
  {
    if (_stream == 0)
      return false;
    PaError err = Pa_CloseStream( _stream );
    _stream = 0;
    return (err == paNoError);
  }

  // This routine will be called by the callback routine when audio is needed.
  // It may be called at interrupt level on some machines so don't do anything
  // that could mess up the system like calling malloc() or free().
  static int recordCallbackMethod( const void *inputBuffer, void *outputBuffer,
                                   unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo,
                                   PaStreamCallbackFlags statusFlags,
                                   void *userData )
  {
    // std::cout << "recordCallbackMethod" << std::endl;

    paTestData *data = (paTestData*)userData;
    // std::cout << "data->maxFrameIndex = " << data->maxFrameIndex << std::endl;
    // std::cout << "data->frameIndex = " << data->frameIndex << std::endl;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    SAMPLE *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;
    // std::cout << "framesLeft = " << framesLeft << std::endl;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer )
    {
      framesToCalc = framesLeft;
      finished = paComplete;
    }
    else
    {
      framesToCalc = framesPerBuffer;
      finished = paContinue;
    }

    if( inputBuffer == NULL )
    {
      for( i=0; i<framesToCalc; i++ )
      {
        *wptr++ = SAMPLE_SILENCE;  /* left */
        if( NUM_CHANNELS == 2 ) *wptr++ = SAMPLE_SILENCE;  /* right */
      }
    }
    else
    {
      for( i=0; i<framesToCalc; i++ )
      {
        *wptr++ = *rptr++;  /* left */
        if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
      }
    }
    data->frameIndex += framesToCalc;
    // std::cout << "data->frameIndex = " << data->frameIndex << std::endl;
    return finished;
  } // end of callback method

  //  start stream processing
  bool start()
  {
    initializeDataArray();
    if (_stream == 0)
      return false;
    PaError err = Pa_StartStream( _stream );
    return (err == paNoError);
  }

  void wait()
  {
    while( ( _err = Pa_IsStreamActive( _stream ) ) == 1 )
    {
      Pa_Sleep(100);
      // printf("index = %d\n", _data.frameIndex );
      fflush(stdout);
    }
  }

  //  stop stream processing
  bool stop()
  {
    if (_stream == 0)
      return false;
    PaError err = Pa_StopStream( _stream );
    return (err == paNoError);
  }

  // double vector type
  typedef std::vector<double> VectorXd;

  void transferSampledData2Channels()                   // transfer the sampled data to separate vectors
  {
    _minLeftSignal =  10000.0;
    _maxLeftSignal = -10000.0;
    _minRightSignal =  10000.0;
    _maxRightSignal = -10000.0;
    timeSamples.resize(_noSamples/2-dataOffset);
    signalLeft.resize(_noSamples/2-dataOffset);
    signalRight.resize(_noSamples/2-dataOffset);
    for (uint32_t k = 0; k < _noSamples/2-dataOffset; k++)
    {
      timeSamples[k] = (double)k * _samplePeriod;
      signalLeft[k] = _data.recordedSamples[2*k+2*dataOffset];
      _minLeftSignal = std::fmin(_minLeftSignal,signalLeft[k]);
      _maxLeftSignal = std::fmax(_maxLeftSignal,signalLeft[k]);
      signalRight[k] = _data.recordedSamples[2*k+1+2*dataOffset];
    }
    // std::cout << "min. left signal = " << _minLeftSignal << std::endl;
    // std::cout << "max. left signal = " << _maxLeftSignal << std::endl;
  }

  VectorXd returnDiscreteTimeSamples()
  {
    return timeSamples;
  }

  VectorXd returnSignalLeft()
  {
    return signalLeft;
  }

  VectorXd returnSignalRight()
  {
    return signalRight;
  }

  void write2screen()
  {
    for (uint32_t k=0; k < _noSamples/2-dataOffset; k++)
    {
      std::cout << "k = " << k << ":" << "left: " << signalLeft[k] << " ";
      std::cout << "right: " << signalRight[k] << std::endl;
    }
  }

  void write2file()
  {
    FILE  *fid;
    fid = fopen("recorded.raw", "wb");
    if( fid == NULL )
    {
      printf("Could not open file.");
    }
    else
    {
      fwrite( _data.recordedSamples, NUM_CHANNELS * sizeof(SAMPLE), _noFrames, fid );
      fclose( fid );
      printf("Wrote data to 'recorded.raw'\n");
    }
  }

  // private methods
private:
  PaError initializeDataArray()
  {
    // calculate the length of the data array to record the given no. of frames
    _data.maxFrameIndex = _noFrames;
    _data.frameIndex = 0;
    _noSamples = _noFrames * _noChannels;
    _noBytes = _noSamples * sizeof(SAMPLE);
    _data.recordedSamples = (SAMPLE *) malloc( _noBytes );         /* From now on, recordedSamples is initialised. */
    if ( _data.recordedSamples != NULL )
    {
      for(uint32_t i=0; i<_noSamples; i++)
        _data.recordedSamples[i] = 0;
    }
    else
    {
      printf("Could not allocate record array.\n");
      _err = paInternalError;
    }
    return _err;
  }

  // Callback: This routine is called by portaudio when  when audio is needed..  (must be static to simplify the use of the method above)
  static int paCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
  {
    /* Here we cast userData to PortaudioHandler* type so we can call the instance method paCallbackMethod, we can do that since
       we called Pa_OpenStream with 'this' for userData */
    return ((PortaudioHandler*)userData)->recordCallbackMethod(
             inputBuffer,
             outputBuffer,
             framesPerBuffer,
             timeInfo,
             statusFlags,
             userData);
  }

  void paStreamFinishedMethod()
  {
    // printf("Stream Completed\n");
  }

  // Callback: This routine is called by portaudio when playback is finished.  (must be static to simplify the use of the method above)
  static void paStreamFinished(void* userData)
  {
    return ((PortaudioHandler*)userData)->paStreamFinishedMethod();
  }

  // private data
private:
  PaError             _err = paNoError;
  // PaStreamParameters  _inputParameters;
  PaStream*           _stream;
  paTestData          _data;
  uint32_t            _noFrames;
  uint32_t            _noSamples;
  uint32_t            _noBytes;
  uint8_t             _noChannels;
  uint32_t            _sampleRate;
  double              _samplePeriod;
  double              _minLeftSignal;
  double              _maxLeftSignal;
  double              _minRightSignal;
  double              _maxRightSignal;
  VectorXd            signalLeft;
  VectorXd            signalRight;
  VectorXd            timeSamples;
};

#endif
