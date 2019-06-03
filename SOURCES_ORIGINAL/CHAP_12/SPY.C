
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>

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



// M A I N ///////////////////////////////////////////////////////////////////

main()
{

// install our ISR

Old_Isr = _dos_getvect(TIME_KEEPER_INT);

_dos_setvect(TIME_KEEPER_INT, Timer);

// wait for user to hit a key

while(!kbhit())
     {
     // print the time variable out, note: the main does NOT touch it...

     _settextposition(0,0);
     printf("\nThe timer reads:%ld   ",time);

     } // end while

// replace old ISR

_dos_setvect(TIME_KEEPER_INT, Old_Isr);


} // end main

