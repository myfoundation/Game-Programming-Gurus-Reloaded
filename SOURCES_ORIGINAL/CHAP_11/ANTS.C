
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define ANT_NORTH 0
#define ANT_EAST  1
#define ANT_SOUTH 2
#define ANT_WEST  3
#define NUM_ANTS  50

// S T R U C T U R E S ///////////////////////////////////////////////////////

// ant structure

typedef struct ant_typ
        {
        int x,y;              // position of ant
        int state;            // state of ant
        unsigned char color;  // color of ant, red or green
        unsigned back_color;  // background under ant

        } ant, *ant_ptr;

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal

// our little ants

ant ants[NUM_ANTS];


// F U N C T I O N S ////////////////////////////////////////////////////////

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

void Plot_Pixel_Fast(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

video_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast

/////////////////////////////////////////////////////////////////////////////

unsigned char Read_Pixel_Fast(int x,int y)
{

// reads a pixel from the video buffer

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

return(video_buffer[((y<<8) + (y<<6)) + x]);

} // end Read_Pixel_Fast

////////////////////////////////////////////////////////////////////////////

void Draw_Ground(void)
{
int index;

// draw a bunch of grey rocks

for (index=0; index<200; index++)
    {
    Plot_Pixel_Fast(rand()%320,rand()%200, 7 + rand()%2);

    } // end for index

} // end Draw_Ground

////////////////////////////////////////////////////////////////////////////

void Initialize_Ants(void)
{
int index;

for (index=0; index<NUM_ANTS; index++)
    {
    // select a random position, color and state for each ant, also scan
    // their background

    ants[index].x     = rand()%320;
    ants[index].y     = rand()%200;
    ants[index].state = rand()%4;

    if (rand()%2==1)
       ants[index].color = 10;
    else
       ants[index].color = 12;

    // scan background

    ants[index].back_color = Read_Pixel_Fast(ants[index].x, ants[index].y);

    } // end for index

} // end Initialize_Ants

////////////////////////////////////////////////////////////////////////////

void Erase_Ants(void)
{
int index;

// loop through the ant array and erase all ants by replacing what was under
// them

for (index=0; index<NUM_ANTS; index++)
    {
    Plot_Pixel_Fast(ants[index].x, ants[index].y, ants[index].back_color);
    } // end for index

} // end Erase_Ants

////////////////////////////////////////////////////////////////////////////

void Move_Ants(void)
{

int index,rock;

// loop through the ant array and move each ant depending on it's state

for (index=0; index<NUM_ANTS; index++)
    {

    // what state is the ant in?

    switch(ants[index].state)
          {

          case ANT_NORTH:
               {
               ants[index].y--;

               } break;

          case ANT_SOUTH:
               {
               ants[index].y++;

               } break;

          case ANT_WEST:
               {
               ants[index].x--;

               } break;

          case ANT_EAST:
               {
               ants[index].x++;

               } break;

          } // end switch

    // test if the ant hit a screen boundary or a rock

    if (ants[index].x > 319)
        ants[index].x = 0;
    else
    if (ants[index].x <0)
        ants[index].x = 319;

    if (ants[index].y > 200)
        ants[index].y = 200;
    else
    if (ants[index].y <0)
        ants[index].y = 199;

    // now test if we hit a rock

    rock = Read_Pixel_Fast(ants[index].x, ants[index].y);

    if (rock)
       {
       // change states

       ants[index].state = rand()%4;  // select a new state

       } // end if

    } // end for index

} // end Move_Ants

///////////////////////////////////////////////////////////////////////////

void Behind_Ants(void)
{
int index;
// loop through the ant array and scan whats under them

for (index=0; index<NUM_ANTS; index++)
    {
    // read the pixel value and save it for later

    ants[index].back_color = Read_Pixel_Fast(ants[index].x, ants[index].y);

    } // end for index

} // end Behind_Ants

///////////////////////////////////////////////////////////////////////////

void Draw_Ants(void)
{
int index;
// loop through the ant array and draw all the ants blue or red depending
// on their type

for (index=0; index<NUM_ANTS; index++)
    {
    Plot_Pixel_Fast(ants[index].x, ants[index].y, ants[index].color);
    } // end for index
} // end Draw_Ants

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

// 320x200x256 color mode

_setvideomode(_MRES256COLOR);

_settextposition(2,0);
printf("Hit any key to exit.");

// draw the world

Draw_Ground();

// set all the ants up

Initialize_Ants();

while(!kbhit())
     {
     // erase all the ants

     Erase_Ants();

     // move all the ants

     Move_Ants();

     // scan whats under the ant

     Behind_Ants();

     // now draw the ant

     Draw_Ants();

     // wait a little

     Timer(2);

     } // end while

// restore the old video mode

_setvideomode(_DEFAULTMODE);

} // end main

