
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include "sndlib.h"

// G L O B A L S  ////////////////////////////////////////////////////////////

char __far *driver_ptr;
unsigned version;
char __huge *data_ptr;
unsigned ct_voice_status;

// F U N C T I O N S /////////////////////////////////////////////////////////

void Voc_Get_Version(void)
{
// gets the version of the driver and prints it out

_asm
   {
   mov bx,0          ; function 0 get version number
   call driver_ptr   ; call the driver
   mov version,ax    ; store in version variable

   } // end inline asm

printf("\nVersion of Driver = %X.0%X",((version>>8) & 0x00ff), (version&0x00ff));

} // end Voc_Get_Version

//////////////////////////////////////////////////////////////////////////////

int Voc_Init_Driver(void)
{
// intialize the driver and return the status

int status;

_asm
   {
   mov bx,3          ; function 3 initialize the driver
   call driver_ptr   ; call the driver
   mov status,ax     ; store in version variable

   } // end inline asm

// return status

printf("\nDriver Initialized");

return(status);

} // end Voc_Init_Driver

//////////////////////////////////////////////////////////////////////////////

int Voc_Terminate_Driver(void)
{
// terminate the driver

_asm
   {
   mov bx,9          ; function 9 terminate the driver
   call driver_ptr   ; call the driver

   } // end inline asm

// de-allocate memory

_dos_freemem(_FP_SEG(driver_ptr));

printf("\nDriver Terminated");

} // end Voc_Terminate_Driver

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Port(unsigned int port)
{

// sets the I/O port of the sound blaster

_asm
   {
   mov bx,1          ; function 1 set port address
   mov ax,port       ; move the port number into ax
   call driver_ptr   ; call the driver

   } // end inline asm

} // Voc_Set_Port

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Speaker(unsigned int on)
{

// turns the speaker on or off

_asm
   {
   mov bx,4          ; function 4 turn speaker on or off
   mov ax,on         ; move the on/off flag into ax
   call driver_ptr   ; call the driver

   } // end inline asm

} // Voc_Set_Speaker

/////////////////////////////////////////////////////////////////////////////

int Voc_Play_Sound(unsigned char far *addr,unsigned char header_length)
{
// plays a pre-loaded VOC file

unsigned segm,offm;


segm = _FP_SEG(addr);
offm = _FP_OFF(addr) + header_length;

_asm
   {
   mov bx,6          ; function 6 play a VOC file
   mov ax, segm      ; can only mov a register into segment so we need this
   mov es, ax        ; es gets the segment
   mov di, offm      ; di gets offset
   call driver_ptr   ; call the driver

   } // end inline asm

} // end Voc_Play_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Stop_Sound(void)
{
// stops a sound that is playing

_asm
   {
   mov bx,8          ; function 8 stop a sound
   call driver_ptr   ; call the driver

   } // end inline asm

} // end Voc_Stop_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Pause_Sound(void)
{
// pauses a sound that is playing

_asm
   {
   mov bx,10         ; function 10 pause a sound
   call driver_ptr   ; call the driver

   } // end inline asm

} // end Voc_Pause_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Continue_Sound(void)
{
// continue a paused sound a sound that is playing

_asm
   {
   mov bx,11         ; function 11 continue play
   call driver_ptr   ; call the driver

   } // end inline asm

} // end Voc_Continue_Sound

/////////////////////////////////////////////////////////////////////////////

int Voc_Break_Sound(void)
{
// break a sound loop

_asm
   {
   mov bx,12         ; function 12 break loop
   call driver_ptr   ; call the driver

   } // end inline asm

} // end Voc_Break_Sound

/////////////////////////////////////////////////////////////////////////////

void Voc_Set_DMA(unsigned int dma)
{

_asm
   {
   mov bx,2          ; function 2 set DMA interupt number
   mov ax,dma        ; move the dma number into ax
   call driver_ptr   ; call the driver

   } // end inline asm

} // Voc_Set_DMA

//////////////////////////////////////////////////////////////////////////////

void Voc_Set_Status_Addr(char far *status)
{

unsigned segm,offm;

segm = _FP_SEG(status);
offm = _FP_OFF(status);

_asm
   {
   mov bx,5          ; function 5 set status varible address
   mov es, segm        ; es gets the segment
   mov di, offm      ; di gets offset
   call driver_ptr   ; call the driver

   } // end inline asm

} // Voc_Set_Status_Addr

//////////////////////////////////////////////////////////////////////////////

void Voc_Load_Driver(void)
{
// loads the ct-voice.drv

int driver_handle;

unsigned errno,segment,offset,num_para,bytes_read;


// open the driver file

_dos_open("CT-VOICE.DRV", _O_RDONLY, &driver_handle);

// allocate the memory

num_para = 1 + (filelength(driver_handle))/16;

_dos_allocmem(num_para,&segment);

// point driver pointer to data area

_FP_SEG(driver_ptr) = segment;
_FP_OFF(driver_ptr) = 0;

// load in the driver code

data_ptr = driver_ptr;

do
 {
 _dos_read(driver_handle,data_ptr, 0x4000, &bytes_read);
 data_ptr += bytes_read;

 } while(bytes_read==0x4000);

// close the file

_dos_close(driver_handle);

} // end Voc_Load_Driver

//////////////////////////////////////////////////////////////////////////////

char far *Voc_Load_Sound(char *filename, unsigned char *header_length)
{
// loads a sound off disk into memory and points a pointer to it

char far *temp_ptr;
char far *data_ptr;

unsigned int sum;

int sound_handle,t;

unsigned errno,segment,offset,num_para,bytes_read;

// open the sound file

_dos_open(filename, _O_RDONLY, &sound_handle);

// allocate the memory

num_para = 1 + (filelength(sound_handle))/16;

_dos_allocmem(num_para,&segment);

// point data pointer to allocated data area

_FP_SEG(data_ptr) = segment;
_FP_OFF(data_ptr) = 0;

// load in the sound data

temp_ptr = data_ptr;

do
 {
 _dos_read(sound_handle,temp_ptr, 0x4000, &bytes_read);
 temp_ptr += bytes_read;

 sum+=bytes_read;

 } while(bytes_read==0x4000);

// make sure it's a voc file

   if ((data_ptr[0] != 'C') || (data_ptr[1] != 'r'))
      {
      printf("\n%s is not a voc file!",filename);
      _dos_freemem(_FP_SEG(data_ptr));
      return(0);

      } // end if voc file

   *header_length = (unsigned char)data_ptr[20];


// close the file

_dos_close(sound_handle);

return(data_ptr);

} // end Voc_Load_Sound

//////////////////////////////////////////////////////////////////////////////

void Voc_Unload_Sound(char far *sound_ptr)
{

// delete the sound from memory

_dos_freemem(_FP_SEG(sound_ptr));

} // end Voc_Unload_Sound


//////////////////////////////////////////////////////////////////////////////



































































































