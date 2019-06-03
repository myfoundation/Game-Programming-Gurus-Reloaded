
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define VGA_INPUT_STATUS_1   0x3DA // vga status reg 1, bit 3 is the vsync
                                   // when 1 - retrace in progress
                                   // when 0 - no retrace

#define VGA_VSYNC_MASK 0x08        // masks off unwanted bit of status reg


// G L O B A L S /////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr


// F U N C T I O N S ////////////////////////////////////////////////////////

void Wait_For_Vsync(void )
{
// this function waits for the start of a vertical retrace, if a vertical
// retrace is in progress then it waits until the next one

while(_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK)
     {
     // do nothing, vga is in retrace
     } // end while

// now wait for vysnc and exit

while(!(_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK))
     {
     // do nothing, wait for start of retrace
     } // end while

// at this point a vertical retrace is occuring, so return back to caller

} // Wait_For_Vsync

// M A I N //////////////////////////////////////////////////////////////////

void main(void )
{

long number_vsyncs=0;  // tracks number of retrace cycles

while(!kbhit())
     {

     // wait for a vsync

     Wait_For_Vsync();

     // do graphics or whatever now that we know electron gun is retracing
     // we only have 1/70 of a second though! Usually, we would copy the
     // double buffer to the video ram

     // ....

     // tally vsyncs

     number_vsyncs++;

     // print to screen

     _settextposition(0,0);
     printf("Number of vsync's = %ld   ",number_vsyncs);


     } // end while

} // end main

