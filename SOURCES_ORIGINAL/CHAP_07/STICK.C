
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

#define VEL_CONST -1  // flags that motion should use constant velocity

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal
                                                          // 18.2 clicks/sec

sprite object;
pcx_picture stick_cells,
            street_cells;


// motion lookup table, has a separte entry for each frame of animation so
// a more realistic movement can be made based on the current frame

int object_vel[] = {17,0,6,2,3,0,17,0,6,2,3,0};


////////////////////////////////////////////////////////////////////////////////

// F U N C T I O N S //////////////////////////////////////////////////////////

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


// M A I N ///////////////////////////////////////////////////////////////////

void main(void )
{
int index,
    done=0,
    vel_state=VEL_CONST;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// set sprite system size so that functions use correct sprite size

sprite_width = 32;
sprite_height = 64;

// initialize the pcx file that holds the street

PCX_Init((pcx_picture_ptr)&street_cells);

// load the pcx file that holds the cells

PCX_Load("street.pcx", (pcx_picture_ptr)&street_cells,1);

PCX_Show_Buffer((pcx_picture_ptr)&street_cells);

// use the pcx buffer for the double buffer

double_buffer = street_cells.buffer;

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

// initialize the pcx file that holds the stickman

PCX_Init((pcx_picture_ptr)&stick_cells);

// load the pcx file that holds the cells

PCX_Load("stickman.pcx", (pcx_picture_ptr)&stick_cells,1);

// grap 6 walking frames

PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,3,3,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,4,4,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,5,5,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,6, 0,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,7, 1,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,8, 2,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,9, 3,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,10,4,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&stick_cells,(sprite_ptr)&object,11,5,1);



// dont need the stickman pcx file anymore

PCX_Delete((pcx_picture_ptr)&stick_cells);


// set up stickman

object.x          = 10;
object.y          = 120;
object.curr_frame = 0;

// scan background

Behind_Sprite((sprite_ptr)&object);

// main loop

while(!done)
     {

     // erase sprite

     Erase_Sprite((sprite_ptr)&object);

     // increment current frame of stickman

     if  (++object.curr_frame > 11)
          object.curr_frame = 0;

     // move sprite using constant velocity or lookup table

     if (vel_state==VEL_CONST)
        {
        object.x+=4;
        } // end if constant velocoty mode
     else
        {
        // use current frame to index into table

        object.x += object_vel[object.curr_frame];

        } // end else use lookup table to a more realistic motion

     // test if stickman is off screen

     if (object.x > 280)
          object.x=10;

     // scan background

     Behind_Sprite((sprite_ptr)&object);

     // draw sprite

     Draw_Sprite((sprite_ptr)&object);

     // copy double buffer to screen

     Show_Double_Buffer(double_buffer);

     // wait a bit

     Timer(2);

     // test if user is hitting keyboard

     if (kbhit())
        {
        switch(getch())
              {
              case ' ': // toggle motion mode
                      {
                      vel_state = -vel_state;
                      } break;

              case 'q': // exit system
                      {
                      done=1;

                      } break;

              } // end switch

        } // end if kbhit

     } // end while

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&street_cells);

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



