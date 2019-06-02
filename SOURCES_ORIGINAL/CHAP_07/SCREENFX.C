
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

#include "graph0.h" // include our graphics library

// D E F I N E S /////////////////////////////////////////////////////////////


// S T R U C T U R E S //////////////////////////////////////////////////////

typedef struct worm_typ
        {
        int y;       // current y position of worm
        int color;   // color of worm
        int speed;   // speed of worm
        int counter; // counter

        } worm, *worm_ptr;

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal
                                                          // 18.2 clicks/sec

pcx_picture screen_fx; // our test screen

worm worms[320]; // used to make the screen melt

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

void Fade_Lights(void)
{
// this functions fades the lights by slowly decreasing the color values
// in all color registers

int index,pal_reg;
RGB_color color,color_1,color_2,color_3;

for (index=0; index<30; index++)
    {

    for (pal_reg=1; pal_reg<255; pal_reg++)
        {
        // get the color to fade

        Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        if (color.red   > 5) color.red-=3;
        else
           color.red = 0;

        if (color.green > 5) color.green-=3;
        else
           color.green = 0;
        if (color.blue  > 5) color.blue-=3;
        else
           color.blue = 0;

        // set the color to a diminished intensity

        Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);

        } // end for pal_reg

    // wait a bit

    Timer(2);

    } // end fade for

} // end Fade_Lights

//////////////////////////////////////////////////////////////////////////////

void Disolve(void )
{
// disolve screen by ploting zillions of black pixels

unsigned long index;

for (index=0; index<=300000; index++,Plot_Pixel_Fast(rand()%320, rand()%200, 0));

} // end Disolve

//////////////////////////////////////////////////////////////////////////////

void Melt(void )
{

// this function "melts" the screen by moving little worms at different speeds
// down the screen.  These worms change to the color thy are eating

int index,ticks=0;

// initialize the worms

for (index=0; index<160; index++)
    {

    worms[index].color   = Get_Pixel(index,0);
    worms[index].speed   = 3 + rand()%9;
    worms[index].y       = 0;
    worms[index].counter = 0;

    // draw the worm

    Plot_Pixel_Fast((index<<1),0,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1),1,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1),2,(char)worms[index].color);


    Plot_Pixel_Fast((index<<1)+1,0,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1)+1,1,(char)worms[index].color);
    Plot_Pixel_Fast((index<<1)+1,2,(char)worms[index].color);

    } // end index

// do screen melt

while(++ticks<1800)
     {

     // process each worm

     for (index=0; index<320; index++)
         {
         // is it time to move worm

         if (++worms[index].counter == worms[index].speed)
            {
            // reset counter

            worms[index].counter = 0;

            worms[index].color = Get_Pixel(index,worms[index].y+4);

            // has worm hit bottom?

            if (worms[index].y < 193)
               {

               Plot_Pixel_Fast((index<<1),worms[index].y,0);
               Plot_Pixel_Fast((index<<1),worms[index].y+1,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1),worms[index].y+2,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1),worms[index].y+3,(char)worms[index].color);

               Plot_Pixel_Fast((index<<1)+1,worms[index].y,0);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+1,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+2,(char)worms[index].color);
               Plot_Pixel_Fast((index<<1)+1,worms[index].y+3,(char)worms[index].color);

               worms[index].y++;

               } // end if worm isn't at bottom yet

            } // end if time to move worm

         } // end index

     // accelerate melt

     if (!(ticks % 500))
        {

        for (index=0; index<160; index++)
            worms[index].speed--;

        } // end if time to accelerate melt

     } // end while

} // end Melt

// M A I N ///////////////////////////////////////////////////////////////////

void main(void )
{
int index,
    done=0,
    sel;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);


PCX_Init((pcx_picture_ptr)&screen_fx);

PCX_Load("war.pcx", (pcx_picture_ptr)&screen_fx,1);

PCX_Show_Buffer((pcx_picture_ptr)&screen_fx);

PCX_Delete((pcx_picture_ptr)&screen_fx);

_settextposition(22,0);
printf("1 - Fade Lights.\n2 - Disolve.\n3 - Meltdown.");

// which special fx did user want to see

switch(getch())
      {
      case '1':  // dim lights
              {

              Fade_Lights();

              } break;

      case '2': // disolve screen
              {
              Disolve();

              } break;


      case '3': // melt screen
              {

              Melt();

              } break;

      } // end switch

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



