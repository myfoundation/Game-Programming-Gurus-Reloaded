

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

// G L O B A L S  ////////////////////////////////////////////////////////////

char __far *driver_ptr;
unsigned version;
char __huge *data_ptr;
unsigned ct_voice_status;

unsigned char *current_sound_play = NULL;
unsigned time_sound_end_play = 0;

// F U N C T I O N S /////////////////////////////////////////////////////////

void Voc_Get_Version(void)
{
// gets the version of the driver and prints it out

	version = 1;

printf("\nVersion of Driver = %X.0%X",((version>>8) & 0x00ff), (version&0x00ff));

} // end Voc_Get_Version

//////////////////////////////////////////////////////////////////////////////

int Voc_Init_Driver(void)
{
// intialize the driver and return the status

int status = 1;

printf("\nDriver Initialized");

return(status);

} // end Voc_Init_Driver

//////////////////////////////////////////////////////////////////////////////

int Voc_Terminate_Driver(void)
{
// terminate the driver
printf("\nDriver Terminated");
return 1;
} // end Voc_Terminate_Driver

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Port(unsigned port)
{

// sets the I/O port of the sound blaster
} // Voc_Set_Port

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Speaker(unsigned on)
{

// turns the speaker on or off
} // Voc_Set_Speaker

/////////////////////////////////////////////////////////////////////////////

int Voc_Play_Sound(unsigned char far *addr,unsigned char header_length)
{
// plays a pre-loaded VOC file

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

/////////////////////////////////////////////////////////////////////////////

void Voc_Set_DMA(unsigned dma)
{
} // Voc_Set_DMA

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Status_Addr(char __far *status)
{
} // Voc_Set_Status_Addr

//////////////////////////////////////////////////////////////////////////////

void Voc_Load_Driver(void)
{
} // end Voc_Load_Driver

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


//////////////////////////////////////////////////////////////////////////////

void main2(void)
{

char far *sounds[4];
unsigned char lengths[4];
int done=0,sel;

Voc_Load_Driver();

Voc_Init_Driver();

Voc_Set_Port(0x220);

Voc_Set_DMA(5);

Voc_Get_Version();

Voc_Set_Status_Addr((char __far *)&ct_voice_status);

// load in sounds

sounds[0] = Voc_Load_Sound("beav.wav" , &lengths[0]);
sounds[1] = Voc_Load_Sound("ed209.wav" ,&lengths[1]);
sounds[2] = Voc_Load_Sound("term.wav" , &lengths[2]);
sounds[3] = Voc_Load_Sound("driver.wav",&lengths[3]);

Voc_Set_Speaker(1);

// main event loop, let user select a sound to play, note you can interupt
// a sound that is currenlty playing

while(!done)
     {
     printf("\n\nSound Demo Menu");
     printf("\n1 - Beavis");
     printf("\n2 - ED 209");
     printf("\n3 - Terminator");
     printf("\n4 - Exit");
     printf("\n\nSelect One ? ");
     scanf("%d",&sel);

     switch (sel)
            {
            case 1:
                  {
                  Voc_Stop_Sound();
                  Voc_Play_Sound((unsigned char *)sounds[0] , lengths[0]);
                  } break;

            case 2:
                  {
                  Voc_Stop_Sound();
                  Voc_Play_Sound((unsigned char *)sounds[1] , lengths[1]); ;
                  } break;

            case 3:
                  {
                  Voc_Stop_Sound();
                  Voc_Play_Sound((unsigned char *)sounds[2] , lengths[2]); ;
                  } break;

            case 4:
                  {
                  done = 1;
                  } break;

            default:
                   {
                   printf("\nFunction %d is not a selection.",sel);
                   } break;

            } // end switch

     } // end while

// terminate

Voc_Play_Sound((unsigned char *)sounds[3] , lengths[3]); ;

// wait for end sequence to stop, the status variable will be -1 when a sound is
// playing and 0 otherwise

while(ct_voice_status!=0) {}

Voc_Set_Speaker(0);

// unload sounds

Voc_Unload_Sound(sounds[0]);
Voc_Unload_Sound(sounds[1]);
Voc_Unload_Sound(sounds[2]);
Voc_Unload_Sound(sounds[3]);

Voc_Terminate_Driver();

} // end main


