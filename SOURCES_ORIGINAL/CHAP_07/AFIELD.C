
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

#define NUM_STARS 30

// S T R U C T U R E S ///////////////////////////////////////////////////////

typedef struct star_typ
        {
        int x,y;    // position of star
        int vel;    // x - component of star velocity
        int color;  // color of star

        } star, *star_ptr;

// G L O B A L S /////////////////////////////////////////////////////////////

star stars[NUM_STARS]; // the star field
                                                          // 18.2 clicks/sec
sprite object;
pcx_picture ast_cells;

////////////////////////////////////////////////////////////////////////////////

// F U N C T I O N S //////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

void Star_Field(void)
{

static int star_first=1;

// this function will create a panning 3-d star field with 3-planes, like
// looking out of the Enterprise

int index;

// test if we need to initialize star field i.e. first time function is being
// called

if (star_first)
   {
   // reset first time
   star_first=0;

   // initialize all the stars

   for (index=0; index<NUM_STARS; index++)
       {
       // initialize each star to a velocity, position and color

       stars[index].x     = rand()%320;
       stars[index].y     = rand()%180;

       // decide what star plane the star is in

       switch(rand()%3)
             {
             case 0: // plane 1- the farthest star plane
                  {
                  // set velocity and color

                  stars[index].vel = 2;
                  stars[index].color = 8;

                  } break;

             case 1: // plane 2-The medium distance star plane
                  {

                  stars[index].vel = 4;
                  stars[index].color = 7;

                  } break;

             case 2: // plane 3-The nearest star plane
                  {

                  stars[index].vel = 6;
                  stars[index].color = 15;

                  } break;

             } // end switch

       } // end for index

   } // end if first time
else
   { // must be nth time in, so do the usual

   // erase, move, draw

   for (index=0; index<NUM_STARS; index++)
       {

       if ( (stars[index].x+=stars[index].vel) >=320 )
          stars[index].x = 0;

       // draw

       Plot_Pixel_Fast_D(stars[index].x,stars[index].y,stars[index].color);

       } // end for index

   } // end else

} // end Star_Field

///////////////////////////////////////////////////////////////////////////////

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

void Clear_Double_Buffer(void)
{
// this function clears the double buffer, kinda crude, but G.E. (good enough)

_fmemset(double_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT + 1);

} // end Clear_Double_Buffer

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{
int index,
    done=0,dx=5,dy=4,ds=4;

float scale=5;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// set sprite system size so that functions use correct sprite size

sprite_width = sprite_height = 47;

// initialize the pcx file that holds all the animation cells for net-tank

PCX_Init((pcx_picture_ptr)&ast_cells);

// load the pcx file that holds the cells

PCX_Load("asteroid.pcx", (pcx_picture_ptr)&ast_cells,1);

// create some memory for the double buffer

Init_Double_Buffer();

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

// load in frames of rotating asteroid

PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,3,3,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,4,4,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,5,5,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,6,0,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&ast_cells,(sprite_ptr)&object,7,1,1);

// position object in center of screen

object.curr_frame = 0;
object.x          = 160-(sprite_width>>1);
object.y          = 100-(sprite_height>>1);

// clear the double buffer

Clear_Double_Buffer();

// show the user the scaled texture

Scale_Sprite((sprite_ptr)&object,scale);
Show_Double_Buffer(double_buffer);

// main loop

while(!kbhit())
     {

     // scale asteroid

     scale+=ds;

     // test if asteroid is too big or too small

     if (scale>100 || scale < 5)
        {
        ds=-ds;
        scale+=ds;
        } //  end if we need to scale in other direction

     // move asteroid

     object.x+=dx;
     object.y+=dy;

     // test if object needs to bounch off wall

     if ((object.x + scale) > 310 || object.x < 10)
        {
        dx=-dx;
        object.x+=dx;
        } // end if hit a vertical boundary

     if ((object.y + scale) > 190 || object.y < 10)
        {
        dy=-dy;
        object.y+=dy;
        } // end if hit a horizontal boundary

     // rotate asteroid by 45

     if (++object.curr_frame==8)
         object.curr_frame=0;

     // create a clean slate

     Clear_Double_Buffer();

     // draw stars

     Star_Field();

     // scale the sprite and render into the double buffer

     Scale_Sprite((sprite_ptr)&object,scale);

     // show the double buffer

     Show_Double_Buffer(double_buffer);

     } // end while

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&ast_cells);

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



