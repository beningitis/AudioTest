#include "portaudio.h"

#include <stdio.h>

#define SAMPLE_RATE (44100)
#define NUM_SECONDS (5)



typedef int PaStreamCallback (const void *inputBuffer, void *outputBuffer,
							 unsigned long framesPerBuffer,
							 const PaStreamCallbackTimeInfo* timeInfo,
							 PaStreamCallbackFlags statusFlags,
							 void *userData);

typedef struct {
	float left_phase;
	float right_phase;
} paTestData;

static int patestCallback(const void *inputBuffer, void *outputBuffer,
						  unsigned long framesPerBuffer,
						  const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData)
{
	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	unsigned int i;
	(void) inputBuffer; /* Prevent unused variable warning. */

	for (i = 0; i < framesPerBuffer; i++)
	{
		*out++ = data->left_phase; /* left */
		*out++ = data->right_phase; /* right */
		/* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
		data->left_phase += 0.01f;
		/* When signal reaches top, drop back down. */
		if (data->left_phase >= 1.0f) data->left_phase -= 2.0f;
		/* higher pitch so we can distinguish left and right. */
		data->right_phase += 0.015f;
		if (data->right_phase >= 1.0f) data->right_phase -= 2.0f;
	}
	return 0;
}

int main(int argc, char** argv) {
	static paTestData data;

	PaError err = Pa_Initialize();
	if (err != paNoError) printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	PaStream* stream;
	err = Pa_OpenDefaultStream(&stream,
							   0, // no input channels
							   2, // stereo output
							   paFloat32, // 32 bit floating point output
							   SAMPLE_RATE,
							   256, // frames per buffer
							   patestCallback,
							   &data);
	if (err != paNoError) printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	err = Pa_StartStream(stream);

	if (err != paNoError) printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	Pa_Sleep(NUM_SECONDS * 1000);

	err = Pa_StopStream(stream);
	if (err != paNoError) printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	err = Pa_CloseStream(stream);
	if (err != paNoError) printf("PortAudio error: %s\n", Pa_GetErrorText(err));

	err = Pa_Terminate();
	if ( err != paNoError ) printf("PortAudio error: %s\n", Pa_GetErrorText(err));
}