/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2013 Live Networks, Inc.  All rights reserved.
// A class for streaming data from a (static) memory buffer, as if it were a file.
// C++ header

#ifndef _BYTE_STREAM_MEMORY_BUFFER_SOURCE_HH
#define _BYTE_STREAM_MEMORY_BUFFER_SOURCE_HH

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#endif

#include <sl_rtsp.h>
//#define RINGBUFF

#ifdef RINGBUFF
typedef struct
{
	unsigned char *membase;
	unsigned int buffer_size;
	unsigned int read_offset_bytes;
	unsigned int write_offset_bytes;
}RingBuffer;
#endif

typedef int (*audio_pull)(unsigned char *buffer, int len);

class ByteStreamMemoryBufferSource: public FramedSource {
public:
  static ByteStreamMemoryBufferSource* createNew(UsageEnvironment& env,
						 u_int8_t* buffer, u_int64_t bufferSize,
						 Boolean deleteBufferOnClose = True,
						 unsigned preferredFrameSize = 0,
						 unsigned playTimePerFrame = 0);
      // "preferredFrameSize" == 0 means 'no preference'
      // "playTimePerFrame" is in microseconds
  #ifdef RINGBUFF
  int InitRingBuffer();
  int FreeRingBuffer();
  int PushToRingBuffer(unsigned char *buffer, int dlen);
  int PullFromRingBuffer(unsigned char *buffer, int dlen);
  RingBuffer *Rb;
  int CountRingBufferUsed();
  int CountRingBufferFree();  
  #endif
  int setSamplerate(int samplerate);
  int setchannels(int channle);
  int setBitsPerSample(int bitsPerSample);
  int setUpDate();
  
  unsigned char getAudioFormat();
  unsigned char bitsPerSample();
  unsigned samplingFrequency();
  unsigned char numChannels();  
  
  char *inFileName;
  audio_pull audio_pull_method;
  virtual void releaseMdev1() {};
protected:
  ByteStreamMemoryBufferSource(UsageEnvironment& env,
			       unsigned preferredFrameSize,
			       unsigned playTimePerFrame);
	// called only by createNew()

  virtual ~ByteStreamMemoryBufferSource();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();

private:
  FILE* fid;
  unsigned fNumChannels;
  unsigned fBitsPerSample;
  unsigned bytesPerSample;
  unsigned fSamplingFrequency;
  double fPlayTimePerSample;
  unsigned fPreferredFrameSize;
  unsigned fPlayTimePerFrame;
  unsigned fLastPlayTime;
};

#endif
