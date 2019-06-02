
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>
#include "serlib.h"

// D E F I N E S /////////////////////////////////////////////////////////////

#define TIME_KEEPER_INT 0x1C


// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt _far *Old_Isr)();  // holds old com port interrupt handler

long time=0;

// F U N C T I O N S ////////////////////////////////////////////////////////

void _interrupt _far Timer()
{

// increment global time variable, note: we can do this since on entry
// DS points to global data segment

time++;

} // end Timer

/////////////////////////////////////////////////////////////////////////////

Plot_Responder()
{

static int first_time=1;
static long old_time;

// test if this is first time

if (first_time)
   {
   // reset first time

   first_time=0;

   old_time = time;

   } // end if first time
else
   { // not first time

   // have 5 clicks past?

   if ( (time-old_time)>=5)
      {
      old_time = time; // save new old time

      // plot the pixel

      _setcolor(rand()%16);
      _setpixel(rand()%320,rand()%200);

      } // end if

   } // end else

} // end Plot_Responder

// M A I N ///////////////////////////////////////////////////////////////////

main()
{

_setvideomode(_MRES256COLOR);

printf("Hit any key to exit...");

// install our ISR

Old_Isr = _dos_getvect(TIME_KEEPER_INT);

_dos_setvect(TIME_KEEPER_INT, Timer);

// wait for user to hit a key

while(!kbhit())
     {
     // .. game code

     // call all responders

     Plot_Responder();

     // .. more game code

     } // end while

_setvideomode(_DEFAULTMODE);

// replace old ISR

_dos_setvect(TIME_KEEPER_INT, Old_Isr);

} // end main

