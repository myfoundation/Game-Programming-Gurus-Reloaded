
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <memory.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#include "graph1.h" // include our graphics library

// D E F I N E S /////////////////////////////////////////////////////////////

#define BIRD_START_COLOR_REG 16
#define BIRD_END_COLOR_REG   28

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal
                                                          // 18.2 clicks/sec

pcx_picture birds;

//////////////////////////////////////////////////////////////////////////////

void Timer(int clicks)
{
// this function uses the internal time keeper timer i.e. the one that goes
// at 18.2 clicks/sec to to a time delay.  You can find a 32 bit value of
// this timer at 0000:046Ch

unsigned int now;

// get current time

now = *clock;

// wait till time has gone past current time plus the amount we eanted to
// wait.  Note each click is approx. 55 milliseconds.

while(abs(*clock - now) < clicks){}

} // end Timer

//////////////////////////////////////////////////////////////////////////////

void Animate_Birds(void)
{
// this function animates a bird drawn with 13 different colors by turning
// on a single color and turning off all the others in a sequence

RGB_color color_1, color_2;
int index;

// clear out each of the color registers used by birds

color_1.red   = 0;
color_1.green = 0;
color_1.blue  = 0;

color_2.red   = 0;
color_2.green = 63;
color_2.blue  = 0;

// clear all the colors out

for (index=BIRD_START_COLOR_REG; index<=BIRD_END_COLOR_REG; index++)
    {

    Set_Palette_Register(index, (RGB_color_ptr)&color_1);

    } // end for index

// make first bird green and then rotate colors

Set_Palette_Register(BIRD_START_COLOR_REG, (RGB_color_ptr)&color_2);

// animate the colors

while(!kbhit())
     {
     // rotate colors

     Get_Palette_Register(BIRD_END_COLOR_REG,(RGB_color_ptr)&color_1);

     for (index=BIRD_END_COLOR_REG-1; index>=BIRD_START_COLOR_REG; index--)
         {

         Get_Palette_Register(index,(RGB_color_ptr)&color_2);
         Set_Palette_Register(index+1,(RGB_color_ptr)&color_2);

         } // end for

         Set_Palette_Register(BIRD_START_COLOR_REG,(RGB_color_ptr)&color_1);

     // wait a while

     Timer(3);

     } // end while

} // end Animate_Birds

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{
int index,
    done=0;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// initialize the pcx file that holds all the birds

PCX_Init((pcx_picture_ptr)&birds);

// load the pcx file that holds the cells

PCX_Load("birds.pcx", (pcx_picture_ptr)&birds,1);

PCX_Show_Buffer((pcx_picture_ptr)&birds);

PCX_Delete((pcx_picture_ptr)&birds);

_settextposition(0,0);
printf("Hit any key to see animation.");

getch();

_settextposition(0,0);
printf("Hit any key to Exit.           ");

Animate_Birds();

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



