
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


// G L O B A L S /////////////////////////////////////////////////////////////

sprite object;
pcx_picture text_cells;

////////////////////////////////////////////////////////////////////////////////

// F U N C T I O N S //////////////////////////////////////////////////////////

void Scale_Sprite(sprite_ptr sprite,float scale)
{

// this function scale a sprite by computing the number of source pixels
// needed to satisfy the number of destination pixels

char far *work_sprite;
int work_offset=0,offset,x,y;
unsigned char data;
float y_scale_index,x_scale_step,y_scale_step,x_scale_index;

// set first source pixel

y_scale_index = 0;

// compute floating point step

y_scale_step = sprite_height/scale;
x_scale_step = sprite_width/scale;

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

// row by row scale object

for (y=0; y<(int)(scale); y++)
    {
    // copy the next row into the screen buffer using memcpy for speed

    x_scale_index=0;

    for (x=0; x<(int)scale; x++)
        {

        // test for transparent pixel i.e. 0, if not transparent then draw

        if ((data=work_sprite[work_offset+(int)x_scale_index]))
             double_buffer[offset+x] = data;

        x_scale_index+=(x_scale_step);

        } // end for x

    // using the floating scale_step, index to next source pixel

    y_scale_index+=y_scale_step;

    // move to next line in video buffer and in sprite bitmap buffer

    offset      += SCREEN_WIDTH;
    work_offset = sprite_width*(int)(y_scale_index);

    } // end for y

} // end Scale_Sprite

///////////////////////////////////////////////////////////////////////////////

void Clear_Double_Buffer()
{
// this function clears the double buffer, kinda crude, but G.E. (good enough)

_fmemset(double_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT + 1);

} // end Clear_Double_Buffer

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{
int index,
    done=0;

float scale=64;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// set sprite system size so that functions use correct sprite size

sprite_width = sprite_height = 64;

// initialize the pcx file that holds all the animation cells for net-tank

PCX_Init((pcx_picture_ptr)&text_cells);

// load the pcx file that holds the cells

PCX_Load("textures.pcx", (pcx_picture_ptr)&text_cells,1);

// PCX_Show_Buffer((pcx_picture_ptr)&text_cells);

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

// grap 4 interesting textures

PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,3,3,0);

// create some memory for the double buffer

Init_Double_Buffer();


// position object in center of screen

object.curr_frame = 0;
object.x          = 160-(sprite_width>>1);
object.y          = 100-(sprite_height>>1);

// clear the double buffer

Clear_Double_Buffer();

// show the user the scaled texture

Scale_Sprite((sprite_ptr)&object,scale);
Show_Double_Buffer(double_buffer);

_settextposition(24,0);
printf("Q - Quit, < > - Scale, Space - Toggle.");

// main loop

while(!done)
     {

     // has user hit a key?

     if (kbhit())
        {
        switch(getch())
              {
              case '.': // scale object larger
                   {
                   if (scale<180)
                      {
                      scale+=4;
                      object.x-=2;
                      object.y-=2;
                      } // end if ok to scale larger

                   } break;

              case ',': // scale object smaller
                   {
                   if (scale>4)
                      {
                      scale-=4;
                      object.x+=2;
                      object.y+=2;
                      } // end if ok to scale smaller

                   } break;

              case ' ': // go to next texture
                      {
                      // are we at the end?

                      if (++object.curr_frame==4)
                         object.curr_frame=0;

                      } break;

              case 'q': // let's go!
                      {
                      done=1;
                      } break;

              default:break;

              } // end switch


        // create a clean slate

        Clear_Double_Buffer();

        // scale the sprite and render into the double buffer

        Scale_Sprite((sprite_ptr)&object,scale);

        // show the double buffer

        Show_Double_Buffer(double_buffer);

        _settextposition(24,0);
        printf("Q - Quit, < > - Scale, Space - Toggle.");

        }// end if

     } // end while

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&text_cells);

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



