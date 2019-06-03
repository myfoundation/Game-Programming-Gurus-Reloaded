
//------------------------------------------
// DOS DEVELOPMENT ENVIRONMENT EMULATION TOOLKIT
//------------------------------------------
#include "stdafx.h"
#include "DOSEmu.h"
//------------------------------------------

// I N C L U D E S ///////////////////////////////////////////////////////////

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
//#include <bios.h>
#include <fcntl.h>
#include "sndlib.h"

// G L O B A L S  ////////////////////////////////////////////////////////////

char __far *driver_ptr;
unsigned version;
char __huge *data_ptr;
unsigned ct_voice_status;

unsigned char *current_sound_play = NULL;
unsigned time_sound_end_play = 0;

struct wav_header{    
char chunk_id[4];
int chunk_size;
char format[4];
char subchunk1_id[4];
int subchunk1_size;
short int audio_format;
short int num_channels;
int sample_rate;            
int byte_rate;
short int block_align;
short int bits_per_sample;
char subchunk2_id[4];
int subchunk2_size;
 };

// F U N C T I O N S /////////////////////////////////////////////////////////

/*
ALSO YOU MAY USE mciSendString WINAPI

MCIERROR mciSendString(
   LPCTSTR lpszCommand,
   LPTSTR  lpszReturnString,
   UINT    cchReturn,
   HANDLE  hwndCallback
);

DO WRAPPER AND USE

mciSendString("open C:\\M0.wav alias MY_SND");
mciSendString("play MY_SND");

// etc

mciSendString("pause MY_SND");

// etc

mciSendString("resume MY_SND");

// etc

mciSendString("stop MY_SND");

// etc 
*/

void Voc_Set_Speaker(unsigned int on)
{

// turns the speaker on or off
} // Voc_Set_Speaker

//////////////////////////////////////////////////////////////////////////////

char far *Voc_Load_Sound(char *filename, unsigned char *header_length)
{
// loads a sound off disk into memory and points a pointer to it

	int len = strlen(filename)+1;

	char* buff = new char[len];
	strcpy(buff, filename);

	(*header_length) = 1;
	return buff;

} // end Voc_Load_Sound

//////////////////////////////////////////////////////////////////////////////

void Voc_Unload_Sound(char far *sound_ptr)
{
// delete the sound from memory

	delete sound_ptr;

} // end Voc_Unload_Sound


/////////////////////////////////////////////////////////////////////////////
ULONG get_wav_time_ms(BYTE* path)
{
    FILE *f;
    f = fopen((char*)path, "rb");
	if(!f) return 0;

	wav_header hd;
	int len = fread(&hd, 1, sizeof(hd), f);
	fclose(f);
	if(len != sizeof(hd)) return 0;

	wav_header* hdr = &hd;
	/*
    double numSamples = hdr->subchunk2_size /
                         (hdr->num_channels * (hdr->bits_per_sample/8));
    double durationSeconds = numSamples / hdr->sample_rate;
	*/

	int fl = file_length(path) - sizeof(hd);
	
	double durationSeconds = fl / (hdr->sample_rate* hdr->num_channels * hdr->bits_per_sample /8);
    return durationSeconds * 1000;
}


/////////////////////////////////////////////////////////////////////////////

int Voc_Play_Sound(unsigned char far *addr,unsigned char header_length)
{
// plays a pre-loaded VOC file

	time_sound_end_play = _clock() + get_wav_time_ms(addr);

	PlaySound((char*)addr, 0, SND_ASYNC);

	current_sound_play = addr;

	return 1;
} // end Voc_Play_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Stop_Sound(void)
{
// stops a sound that is playing
	PlaySound(NULL, 0, 0);
	return 1;
} // end Voc_Stop_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Pause_Sound(void)
{
// pauses a sound that is playing
	Voc_Stop_Sound();
	return 1;
} // end Voc_Pause_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Continue_Sound(void)
{
// continue a paused sound a sound that is playing
	Voc_Play_Sound(current_sound_play, 0);
	return 1;
} // end Voc_Continue_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Break_Sound(void)
{
// break a sound loop
	Voc_Stop_Sound();
	return 1;
} // end Voc_Break_Sound

//////////////////////////////////////////////////////////////////////////////

void Voc_Get_Version(void)
{
} // end Voc_Get_Version

//////////////////////////////////////////////////////////////////////////////

int Voc_Init_Driver(void)
{
// intialize the driver and return the status
	return 1;
} // end Voc_Init_Driver

//////////////////////////////////////////////////////////////////////////////

int Voc_Terminate_Driver(void)
{
// terminate the driver
	return 1;
} // end Voc_Terminate_Driver

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Port(unsigned int port)
{

// sets the I/O port of the sound blaster

} // Voc_Set_Port


/////////////////////////////////////////////////////////////////////////////

void Voc_Set_DMA(unsigned int dma)
{
} // Voc_Set_DMA

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Status_Addr(char far *status)
{

} // Voc_Set_Status_Addr

//////////////////////////////////////////////////////////////////////////////

void Voc_Load_Driver(void)
{
// loads the ct-voice.drv

} // end Voc_Load_Driver

//////////////////////////////////////////////////////////////////////////////



































































































