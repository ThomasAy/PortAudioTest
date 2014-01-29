#include <QDebug>

#include <portaudio.h>

#define SAMPLE_RATE (44100)
#define NUM_SECONDS (3)

typedef struct
{
	float left_phase;
	float right_phase;
}
paTestData;
/* This routine will be called by the PortAudio engine when audio is needed.
   It may called at interrupt level on some machines so don't do anything
   that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
						   unsigned long framesPerBuffer,
						   const PaStreamCallbackTimeInfo* timeInfo,
						   PaStreamCallbackFlags statusFlags,
						   void *userData )
{
	/* Cast data passed through stream to our structure. */
	paTestData *data = (paTestData*)userData;

	if(inputBuffer)
	{
		char max = 0;
		char* in = (char*)inputBuffer;
		for(int i=0; i<framesPerBuffer; i++)
		{
			if(*in > max)
				max = *in;
		}
		qDebug() << "input" << framesPerBuffer << (int)max;
	}

	if(outputBuffer)
	{
		qDebug() << "output";
		float *out = (float*)outputBuffer;
		for(int i=0; i<framesPerBuffer; i++ )
		{
			*out++ = data->left_phase;  /* left */
			*out++ = data->right_phase;  /* right */
			/* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
			data->left_phase += 0.01f;
			/* When signal reaches top, drop back down. */
			if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
			/* higher pitch so we can distinguish left and right. */
			data->right_phase += 0.03f;
			if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	PaError err = Pa_Initialize();
	if( err != paNoError )
		goto error;

	qDebug() <<"Hello!";

	int numDevices;

	numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 )
	{
		qDebug() << "ERROR: Pa_CountDevices returned " << numDevices;
		err = numDevices;
		//   goto error;
	}

	const   PaDeviceInfo *deviceInfo;
	for(int i=0; i<numDevices; i++ )
	{
		deviceInfo = Pa_GetDeviceInfo( i );
		qDebug() << deviceInfo->name << deviceInfo->maxInputChannels << deviceInfo->maxOutputChannels;
	}

	PaStream *stream;
	paTestData data;
	data.left_phase = 0;
	data.right_phase = 0;
	/* Open an audio I/O stream. */
	err = Pa_OpenDefaultStream( &stream,
								1,          /* no input channels */
								0,          /* stereo output */
								paInt8,  /* 32 bit floating point output */
								SAMPLE_RATE,
								256,        /* frames per buffer, i.e. the number
																										of sample frames that PortAudio will
																										request from the callback. Many apps
																										may want to use
																										paFramesPerBufferUnspecified, which
																										tells PortAudio to pick the best,
																										possibly changing, buffer size.*/
								patestCallback, /* this is your callback function */
								&data ); /*This is a pointer that will be passed to
													   your callback*/
	if( err != paNoError )
		goto error;

	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;

	/* Sleep for several seconds. */
	Pa_Sleep(NUM_SECONDS*1000);

	err = Pa_StopStream( stream );
	if( err != paNoError ) goto error;

	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto error;

	err = Pa_Terminate();
	if( err != paNoError )
		goto error;
	qDebug() << "Bye!";
	return 0;

error:
	qDebug() << "PortAudio error:" << Pa_GetErrorText( err );
	return err;

}
