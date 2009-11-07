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
	short *buf;
	unsigned int buflen;
	unsigned int bufsize;
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
	fmt.mFramesPerPacket = 1;
	fmt.mBytesPerFrame = fmt.mBytesPerPacket =
		sizeof (short) * fmt.mChannelsPerFrame;
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
		("kAudioDevicePropertyStreamFormat: mFormatFlags 0x%08lx (IsSignedInteger:%s, isFloat:%s, isBigEndian:%s, kLinearPCMFormatFlagIsNonInterleaved:%s, kAudioFormatFlagIsPacked:%s)\n",
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

	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mBitsPerChannel %lu\n",
		   fmt.mBitsPerChannel);
	DSFYDEBUG
		("kAudioDevicePropertyStreamFormat: mChannelsPerFrame %lu\n",
		 fmt.mChannelsPerFrame);
	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mFramesPerPacket %lu\n",
		   fmt.mFramesPerPacket);
	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mBytesPerFrame %lu\n",
		   fmt.mBytesPerFrame);
	DSFYDEBUG ("kAudioDevicePropertyStreamFormat: mBytesPerPacket %lu\n",
		   fmt.mBytesPerPacket);



	device = (CoreAudioDevice *)malloc(sizeof(CoreAudioDevice));
	if(!device)
		return NULL;


	if (AudioDeviceCreateIOProcID
			(adev_id, audio_callback, device, &device->proc_id)) {
		DSFYDEBUG ("AudioDeviceCreateIOProcID() returned FAIL!\n");

		free(device);
		return NULL;
	}


	device->adev_id = adev_id;
	device->playing = 0;
	device->buflen = 0;
	device->bufsize = sizeof(short) * 8192;
	device->buf = (short *)malloc(device->bufsize);
	pthread_mutex_init(&device->mutex, NULL);
	pthread_cond_init(&device->event, NULL);

	DSFYDEBUG("audio_init(): CoreAudio initalized\n");

	return device;
}


int audio_exit(void *private) {
	CoreAudioDevice *device = (CoreAudioDevice *)private;

	DSFYDEBUG("audio_exit(): CoreAudio exiting..\n");

	pthread_mutex_lock(&device->mutex);
	if(device->playing) {
		device->playing = 0;
		pthread_cond_signal(&device->event);
		if(AudioDeviceStop (device->adev_id, device->proc_id)) {
			DSFYDEBUG ("audio_exit(): AudioDeviceStop() failed\n");
			return -1;
		}
	}
	pthread_mutex_unlock(&device->mutex);


	pthread_cond_destroy(&device->event);
	pthread_mutex_destroy(&device->mutex);

	free(device->buf);


	DSFYDEBUG("audio_exit(): CoreAudio has exited\n");
	return 0;
}


int audio_play_pcm (void* private, struct pcm_data* pcm) {
	CoreAudioDevice *device = (CoreAudioDevice *)private;
	int bytes_to_copy;

	DSFYDEBUG("audio_play_pcm(): Adding a buffer\n");
	if(pcm->channels != 2) {
		DSFYDEBUG ("audio_play_pcm(): Unsupported number of channels: %d!\n", pcm->channels)
		return -1;
	}
	else if(pcm->samplerate != 44100) {
		DSFYDEBUG ("audio_play_pcm(): Unsupported sample rate: %d!\n", pcm->samplerate)
		return -1;
	}

	pthread_mutex_lock(&device->mutex);
	bytes_to_copy = device->bufsize - device->buflen;
	if(pcm->len < bytes_to_copy)
		bytes_to_copy = pcm->len;

	DSFYDEBUG("audio_play_pcm(): Appending %d bytes (PCM buffer had %d, already had %d)\n", bytes_to_copy, pcm->len, device->buflen);
	memcpy(device->buf + device->buflen, pcm->buf, bytes_to_copy);
	device->buflen += bytes_to_copy;
	pthread_cond_signal(&device->event);

	if(!device->playing) {
		if (AudioDeviceStart (device->adev_id, device->proc_id)) {
			DSFYDEBUG ("audio_play_pcm(): AudioDeviceStart() failed\n");
			pthread_mutex_unlock(&device->mutex);
			return -1;
		}

		device->playing = 1;
		DSFYDEBUG ("audio_play_pcm(): AudioDeviceStart() succeeded\n");
	}

	pthread_mutex_unlock(&device->mutex);

	DSFYDEBUG ("audio_play_pcm(): Done!\n");

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
	int ret;

	Float32 *dst = bufout->mBuffers[0].mData;
	int num_channels = 2;
	int samples_to_write;
	int buffered_samples;
	int sample_size;
	int channel, sample;

	DSFYDEBUG ("CoreAudio: audio_callback(): Hello!\n");
	sample_size = sizeof(float) * num_channels;
	samples_to_write = bufout->mBuffers[0].mDataByteSize / sample_size;

	/* Zero buffer */
	for (sample = 0; sample < samples_to_write; sample++)
		for(channel = 0; channel < num_channels; channel++)
			dst[2 * sample + channel] = 0.0f;


	ret = 0;
	pthread_mutex_lock(&device->mutex);
	sample = 0;
	while (samples_to_write) {
		while(device->buflen == 0 && device->playing == 1)
			pthread_cond_wait(&device->event, &device->mutex);

		if(!device->playing) 
			break;


		buffered_samples = device->buflen / sizeof (short) / num_channels;
		for (sample = 0; sample < buffered_samples; sample++) {

			for (channel = 0; channel < num_channels; channel++) {
				if (device->buf[2 * sample + channel] <= 0)
					*dst++ = (float) (device->buf
							  [2 * sample +
							   channel]) /
						32767.0;
				else
					*dst++ = (float) (device->buf
							  [2 * sample +
							   channel]) /
						32768.0;
			}

			if(--samples_to_write == 0)
				break;
		}

		device->buflen -= sample * sizeof(short) * num_channels;
		memmove(device->buf, device->buf + sample*sizeof(short)*num_channels, device->buflen);
	}

	pthread_mutex_unlock(&device->mutex);
	DSFYDEBUG ("CoreAudio: audio_callback(): Done!\n");

	return 0;
}
