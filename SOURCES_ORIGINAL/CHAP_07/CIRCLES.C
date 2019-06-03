
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <graph.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define SCREEN_WIDTH      (unsigned int)320
#define SCREEN_HEIGHT     (unsigned int)200

// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr
unsigned char far *double_buffer = NULL;

// F U N C T I O N S /////////////////////////////////////////////////////////

void Init_Double_Buffer(void)
{

double_buffer = (char far *)_fmalloc(SCREEN_WIDTH * SCREEN_HEIGHT + 1);

_fmemset(double_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT + 1);

} // end Init_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

void Show_Double_Buffer(char far *buffer)
{
// copy the double buffer into the video buffer

_asm
   {
   push ds                 // save the data segment
   les di, video_buffer    // set destination i.e. video buffer
   lds si, buffer          // set source i.e. double buffer
   mov cx,320*200/2        // want to move 320*200 bytes or half that # of
   cld                     // words.
   rep movsw               // do the movement
   pop ds                  // restore the data segment
   }

} // end Show_Double_Buffer

//////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Fast_D(int x,int y,unsigned char color)
{

// plots pixels into the double buffer

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

double_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast_D

//////////////////////////////////////////////////////////////////////////////

void Circles(void)
{
// this function draw 1000 circles into the double buffer, in a game we would
// never use a crude algorithm, like this to draw circles, we would use
// look up tables or other means; however, we just want something to be drawn
// in the double buffer

int index,xo,yo,radius,x,y,color,ang;

// draw 100 circles at random positions with random colors and sizes

for (index=0; index<1000; index++)
    {

    // get parameters for next circle

    xo     = 20 + rand()%300;
    yo     = 20 + rand()%180;
    radius = 1 + rand()%20;
    color  = rand()%256;

    for (ang=0; ang<360; ang++)
        {

        x = xo + cos(ang*3.14/180) * radius;
        y = yo + sin(ang*3.14/180) * radius;

        Plot_Pixel_Fast_D(x,y,(unsigned char)color);

        } // end ang

    } // end index

} // end Circles


// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

// set the videomode to 320x256x256

_setvideomode(_MRES256COLOR);


// create a double buffer and clear it

Init_Double_Buffer();

_settextposition(0,0);
printf("Drawing 1000 circles to double buffer. \nPlease wait...");

// draw the circles to the double buffer

Circles();

printf("Done, press any key.");

// wait for user to hit key then blast double buffer to video screen

getch();

Show_Double_Buffer(double_buffer);

_settextposition(0,0);
printf("That was quick. Hit any key to exit.");

getch();

// restore video mode

_setvideomode(_DEFAULTMODE);

} // end main

