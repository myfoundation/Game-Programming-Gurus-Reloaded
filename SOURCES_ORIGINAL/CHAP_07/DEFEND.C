
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

//////////////////////////////////////////////////////////////////////////////

void Show_View_Port(char far *buffer,int pos)
{
// copy a portion of the double buffer to the video screen

unsigned int y,double_off, screen_off;

// there are 100 rows that need to be moved, move the data row by row

for (y=0; y<100; y++)
    {

    // compute starting offset into double buffer
    // y * 640 + pos

    double_off = ((y<<9) + (y<<7) + pos );

    // compute starting offset in video ram
    // y * 320 + 80

    screen_off = (((y+50)<<8) + ((y+50)<<6) + 80 );

    // move the data

    _fmemmove((char far *)&video_buffer[screen_off],
              (char far *)&double_buffer[double_off],160);

    } // end for y

} // end Show_View_Port

//////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Fast_D2(int x,int y,unsigned char color)
{

// plots pixels into the double buffer with our new virtual screen size
// of 640x100

// use the fact that 640*y = 512*y + 128*y = y<<9 + y<<7

double_buffer[((y<<9) + (y<<7)) + x] = color;

} // end Plot_Pixel_Fast_D2

/////////////////////////////////////////////////////////////////////////////

void Draw_Terrain(void)
{

// this function draws the terrain into the double buffer, which in this case
// is thought of as being 640x100 pixels

int x,y=70,index;


// clear out memory first

_fmemset(double_buffer,0,(unsigned int)640*(unsigned int)100);

// draw a few stars

for (index=0; index<200; index++)
    {
    Plot_Pixel_Fast_D2(rand()%640,rand()%70,15);
    } // end for index

// draw some moutains

for (x=0; x<640; x++)
    {

    // compute offset

    y+=-1 + rand()%3;

    // make sure terrain stays within resonable boundary

    if (y>90) y=90;
    else
    if (y<40) y=40;

    // plot the dot in the double buffer

    Plot_Pixel_Fast_D2(x,y,10);

    } // end for x

} // end Draw_Terrain

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

int done=0,sx=0;

// set the videomode to 320x256x256

_setvideomode(_MRES256COLOR);

_settextposition(0,0);

printf("Use < > to move. Press Q to quit.");

// draw a little window

_setcolor(1);

_rectangle(_GBORDER, 80-1,50-1,240+1,150+1);

// allocate memory for double buffer

double_buffer = (char far *)_fmalloc(SCREEN_WIDTH * SCREEN_HEIGHT+1);

Draw_Terrain();

Show_View_Port(double_buffer,sx);

// main loop

while(!done)
     {

     // has user hit a key

     if (kbhit())
        {

        switch(getch())
              {
              case ',': // move window to left, but don't go too far
                      {
                      sx-=2;

                      if (sx<0)
                          sx=0;

                      } break;

              case '.': // move window to right, but dont go too far
                      {
                      sx+=2;

                      if (sx > 640-160)
                         sx=640-160;

                      } break;

              case 'q': // user trying to bail ?
                      {
                      done=1;

                      } break;

              } // end switch

        // copy view port to screen

        Show_View_Port(double_buffer,sx);

        _settextposition(24,0);

        printf("Viewport position = %d  ",sx);

        } // end if

     } // end while

// restore video mode

_setvideomode(_DEFAULTMODE);

} // end main

