/*
 * $Id: coreaudio.c 402 2009-07-29 13:58:27Z noah-w $
 *
 * Mac OS X CoreAudio audio output driver for the simple client
 * Expects audio with two channels and a sample rate of 44100Hz
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/CoreAudioTypes.h>

#include "audio.h"
#include "util.h"

typedef struct {
	AudioDeviceID adev_id;
	AudioDeviceIOProcID proc_id;
	int playing;
	pthread_mutex_t mutex;
	pthread_cond_t event;
	pthread_cond_t hold;
	short *buf;
	int buflen;
	int bufsize;
} CoreAudioDevice;


static OSStatus audio_callback (AudioDeviceID, const AudioTimeStamp *,
				const AudioBufferList *,
				const AudioTimeStamp *, AudioBufferList *,
				const AudioTimeStamp *, void *);


/*
 * Initialize CoreAudio and return an opaque pointer
 * for keeping track of our audio device
 *
 */
void *audio_init(void) {
	OSStatus s;
	AudioDeviceID adev_id;
	AudioValueRange avr;
	UInt32 framesize = 512;
	UInt32 sz;
	AudioStreamBasicDescription fmt;
	CoreAudioDevice *device;


	DSFYDEBUG("Initializing CoreAudio\n");
	sz = sizeof (adev_id);
	s = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &sz, &adev_id);
	if(s != 0) {
		fprintf(stderr, "AudioHardwareGetProperty() failed with error %d\n", s);
		return NULL;
	}
	else if (adev_id == kAudioDeviceUnknown) {
		fprintf(stderr, "AudioHardwareGetProperty() returned device kAudioDeviceUnknown\n");
		return NULL;
	}


	/* Find out the bounds of buffer frame size */
	sz = sizeof(avr);
	if ((s = AudioDeviceGetProperty(adev_id, 0, false,
				kAudioDevicePropertyBufferFrameSizeRange, &sz, &avr))) {
		printf("AudioDeviceGetProperty() failed with error %d\n", s);
		return NULL;
	}

	/* Keep the requested number of frames within bounds */
	if (framesize < avr.mMinimum)
		framesize = avr.mMinimum;
	else if (framesize > avr.mMaximum)
		framesize = avr.mMaximum;


	/* Set buffer frame size for device */
	sz = sizeof (framesize);
	s = AudioDeviceSetProperty (adev_id, 0, 0, false,
					kAudioDevicePropertyBufferFrameSize, sz, &framesize);
	if (s != kAudioHardwareNoError) {
		fprintf(stderr, "AudioDeviceSetProperty() failed with error %d\n", s);
		return NULL;
	}


	/* Get current audio format */
	sz = sizeof (fmt);
	if (AudioDeviceGetProperty
			(adev_id, 0, false, kAudioDevicePropertyStreamFormat,
			 &sz, &fmt)) {
		DSFYDEBUG ("AudioDeviceGetProperty() failed\n");
		return NULL;
	}


	/* Setup audio format */
	fmt.mFormatID = kAudioFormatLinearPCM;
	fmt.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
	fmt.mSampleRate = 44100;
	fmt.mChannelsPerFrame = 2;
	fmt.mBytesPerFrame = sizeof(Float32) * fmt.mChannelsPerFrame;
	fmt.mFramesPerPacket = 1;
	fmt.mBytesPerPacket = fmt.mFramesPerPacket * fmt.mBytesPerFrame;
	fmt.mReserved = 0;


	/* Update audio format */
	sz = sizeof (fmt);
	if (AudioDeviceSetProperty
			(adev_id, NULL, 0, false,
			 kAudioDevicePropertyStreamFormat, sz, &fmt)) {
		DSFYDEBUG ("AudioDeviceSetProperty() failed\n");
		return NULL;
	}

	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mSampleRate %f\n",
		   fmt.mSampleRate);
	DSFYDEBUG
		("kAudioDevicePropertyStreamFormat: mFormatFlags 0x%08x (IsSignedInteger:%s, isFloat:%s, isBigEndian:%s, kLinearPCMFormatFlagIsNonInterleaved:%s, kAudioFormatFlagIsPacked:%s)\n",
		 fmt.mFormatFlags,
		 fmt.mFormatFlags & kLinearPCMFormatFlagIsSignedInteger ?
		 "yes" : "no",
		 fmt.mFormatFlags & kLinearPCMFormatFlagIsFloat ? "yes" :
		 "no",
		 fmt.mFormatFlags & kLinearPCMFormatFlagIsBigEndian ? "yes" :
		 "no",
		 fmt.mFormatFlags & kLinearPCMFormatFlagIsNonInterleaved ?
		 "yes" : "no",
		 fmt.mFormatFlags & kAudioFormatFlagIsPacked ? "yes" : "no");

	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mBitsPerChannel %u\n",
		   fmt.mBitsPerChannel);
	DSFYDEBUG
		("kAudioDevicePropertyStreamFormat: mChannelsPerFrame %u\n",
		 fmt.mChannelsPerFrame);
	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mFramesPerPacket %u\n",
		   fmt.mFramesPerPacket);
	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mBytesPerFrame %u\n",
		   fmt.mBytesPerFrame);
	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mBytesPerPacket %u\n",
		   fmt.mBytesPerPacket);



	device = (CoreAudioDevice *)malloc(sizeof(CoreAudioDevice));
	if(!device)
		return NULL;


	device->adev_id = adev_id;
	device->playing = 0;
	device->buflen = 0;
	device->bufsize = sizeof(short) * 32768;
	if (AudioDeviceCreateIOProcID
			(device->adev_id, audio_callback, device, &device->proc_id)) {
		DSFYDEBUG ("AudioDeviceCreateIOProcID() returned FAIL!\n");

		free(device);
		return NULL;
	}


	device->buf = (short *)malloc(device->bufsize);
	pthread_mutex_init(&device->mutex, NULL);
	pthread_cond_init(&device->event, NULL);
	pthread_cond_init(&device->hold, NULL);

	DSFYDEBUG("Done initializing CoreAudio\n");

	return device;
}


int audio_exit(void *private) {
	CoreAudioDevice *device = (CoreAudioDevice *)private;

	DSFYDEBUG("Attempting exit with device at %p\n", device);
	if(!device)
		return -1;

	pthread_mutex_lock(&device->mutex);
	if(device->playing) {
		device->playing = 0;
		DSFYDEBUG("Currently playing, signalling event and stopping CoreAudio\n");
		pthread_cond_signal(&device->event);
		if(AudioDeviceStop (device->adev_id, device->proc_id)) {
			DSFYDEBUG ("audio_exit(): AudioDeviceStop() failed\n");
			return -1;
		}
	}
	pthread_mutex_unlock(&device->mutex);


	DSFYDEBUG("Destryoing mutex and condition variables\n");
	pthread_cond_destroy(&device->hold);
	pthread_cond_destroy(&device->event);
	pthread_mutex_destroy(&device->mutex);

	free(device->buf);

	return 0;
}


int audio_play_pcm (void* private, struct pcm_data* pcm) {
	CoreAudioDevice *device = (CoreAudioDevice *)private;
	int bytes_to_copy;

	if(!device) {
		DSFYDEBUG ("Attempt to play with no output device\n");
		return -1;
	}
	else if(pcm->channels != 2) {
		DSFYDEBUG ("Unsupported number of channels: %d!\n", pcm->channels);
		return -1;
	}
	else if(pcm->samplerate != 44100) {
		DSFYDEBUG ("Unsupported sample rate: %d!\n", pcm->samplerate);
		return -1;
	}

	pthread_mutex_lock(&device->mutex);


	/* Sleep if the we can't handle the sent PCM data right now */
	while(device->buflen + pcm->len >= device->bufsize) {
		DSFYDEBUG ("Buffer too full, sleeping..\n");
		pthread_cond_wait(&device->hold, &device->mutex);
		DSFYDEBUG("Buffer has shrunk in size, now have %d bytes free\n", device->bufsize - device->buflen);
	}

	bytes_to_copy = device->bufsize - device->buflen;
	if(pcm->len < bytes_to_copy)
		bytes_to_copy = pcm->len;


	memcpy((void *)device->buf + device->buflen, pcm->buf, bytes_to_copy);
	device->buflen += bytes_to_copy;

	if(!device->playing) {
		device->playing = 1;

		DSFYDEBUG ("Not yet playing, calling AudioDeviceStart()\n");
		if (AudioDeviceStart (device->adev_id, device->proc_id)) {
			DSFYDEBUG ("AudioDeviceStart() failed\n");
			pthread_mutex_unlock(&device->mutex);
			return -1;
		}

	}

	pthread_cond_signal(&device->event);
	pthread_mutex_unlock(&device->mutex);

	return 0;
}


static OSStatus audio_callback (AudioDeviceID dev,
				const AudioTimeStamp * ts_now,
				const AudioBufferList * bufin,
				const AudioTimeStamp * ts_inputtime,
				AudioBufferList * bufout,
				const AudioTimeStamp * ts_outputtime,
				void *private)
{
	CoreAudioDevice *device = (CoreAudioDevice *)private;
	int channel, num_channels = 2;
	Float32 *dst = bufout->mBuffers[0].mData;
	int samples_to_write;
	int sample_size;
	short *src;
	int bytes_consumed;

	sample_size = sizeof(Float32) * num_channels;
	samples_to_write = bufout->mBuffers[0].mDataByteSize / sample_size;


	pthread_mutex_lock(&device->mutex);
	while (samples_to_write) {
		while(device->buflen == 0 && device->playing == 1) {
			DSFYDEBUG ("Zero bytes available and playing, waiting..\n");
			pthread_cond_wait(&device->event, &device->mutex);
		}

		if(!device->playing) {
			DSFYDEBUG ("Stopping playback due to device->playing==0\n");
			break;
		}


		src = device->buf;
		bytes_consumed = 0;
		for(; samples_to_write > 0 && bytes_consumed < device->buflen; samples_to_write--) {
			for (channel = 0; channel < num_channels; channel++) {
				if (*src <= 0)
					*dst++ = *src++ / 32767.0;
				else
					*dst++ = *src++ / 32768.0;
			}

			bytes_consumed += sizeof(short) * num_channels;
		}
			
		device->buflen -= bytes_consumed;
		if(device->buflen > 0)
			memcpy(device->buf, (void *)device->buf + bytes_consumed, device->buflen);
	}

	if(device->buflen < device->bufsize/2) {
		DSFYDEBUG("Buffer half empty, signalling condition in audio_pcm_play()\n");
		pthread_cond_signal(&device->hold);
	}

	pthread_mutex_unlock(&device->mutex);

	return 0;
}
