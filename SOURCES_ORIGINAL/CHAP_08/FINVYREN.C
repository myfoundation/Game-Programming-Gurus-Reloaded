
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

#include <graph.h>

#include "graphics.h" // include our graphics library

// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Create_Scale_Data_X(int scale, int far *row);

void Create_Scale_Data_Y(int scale, int *row);

void Build_Scale_Table(void);

void Scale_Sprite(sprite_ptr sprite,int scale);

void Clear_Double_Buffer(void);

void Timer(int clicks);

void Init_Stars(void);

void Move_Stars(void);

void Draw_Stars(void);

// D E F I N E S /////////////////////////////////////////////////////////////

#define NUM_STARS      50       // number of stars in star field
#define MAX_SCALE      200      // largest any bitmap can be
#define SPRITE_X_SIZE  80       // the size of a sprite texture
#define SPRITE_Y_SIZE  48

// S T R U C T U R E S ///////////////////////////////////////////////////////

// this is a star

typedef struct star_typ
        {

        int x,y;        // postion of star
        int xv,yv;      // velocity of star
        int xa,ya;      // acceleration of star
        int color;      // color of star
        int clock;      // number of ticks star has been alive
        int acc_time;   // this is the number of ticks to count before accelerating
        int acc_count;  // the accleration counter

        } star, *star_ptr;

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046CL; // pointer to internal
                                                           // 18.2 clicks/sec

sprite object;                          // the generic sprite that will hold
                                        // the frames of the ship


pcx_picture text_cells;                 // the pcx file with the images

int *scale_table_y[MAX_SCALE+1];        // table with pre-computed scale indices

int far *scale_table_x[MAX_SCALE+1];    // table with pre-computed scale indices

star star_field[NUM_STARS];             // the star field

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

void Init_Stars(void)
{

// this function initializes the star field data structure when the system
// is started

int index,divisor;

for (index=0; index<NUM_STARS; index++)
    {

    star_field[index].x      = 150 + rand() % 20;
    star_field[index].y      = 90  + rand() % 20;


    if (rand()%2==1)
       star_field[index].xv     = -4 + -2 * rand() % 3;
    else
       star_field[index].xv     = 4 + 2 * rand() % 3;

    if (rand()%2==1)
       star_field[index].yv     = -4 + -2 * rand() % 3;
    else
       star_field[index].yv     = 4 + 2 * rand() % 3;


    divisor = 1 + rand()%3;

    star_field[index].xa     = star_field[index].xv/divisor;
    star_field[index].ya     = star_field[index].yv/divisor;

    star_field[index].color     = 7;
    star_field[index].clock     = 0;

    star_field[index].acc_time  = 1 + rand() % 3;
    star_field[index].acc_count = 0;


    } // end index

} // end Init_Stars

//////////////////////////////////////////////////////////////////////////////

void Move_Stars(void)
{
// this function moves the stars, and tests if a star has gone off the screen
// if so, the function starts the star over again

int index,divisor;

for (index=0; index<NUM_STARS; index++)
    {

    star_field[index].x += star_field[index].xv;
    star_field[index].y += star_field[index].yv;

    // test if star is off screen

    if (star_field[index].x >= SCREEN_WIDTH  || star_field[index].x < 0 ||
        star_field[index].y >= SCREEN_HEIGHT || star_field[index].y < 0)
       {

       // restart the star

       star_field[index].x      = 150 + rand() % 20;
       star_field[index].y      = 90  + rand() % 20;

       if (rand()%2==1)
          star_field[index].xv     = -4 + -2 * rand() % 3;
       else
          star_field[index].xv     = 4 + 2 * rand() % 3;

       if (rand()%2==1)
          star_field[index].yv     = -4 + -2 * rand() % 3;
       else
          star_field[index].yv     = 4 + 2 * rand() % 3;


       divisor = 1 + rand()%3;

       star_field[index].xa     = star_field[index].xv/divisor;
       star_field[index].ya     = star_field[index].yv/divisor;

       star_field[index].color     = 7;
       star_field[index].clock     = 0;

       star_field[index].acc_time  = 1 + rand() % 3;
       star_field[index].acc_count = 0;

       } // end if

    // test if it's time to accelerate

    if (++star_field[index].acc_count==star_field[index].acc_time)
       {
       // reset counter

       star_field[index].acc_count=0;

       // accelerate

       star_field[index].xv += star_field[index].xa;
       star_field[index].yv += star_field[index].ya;

       } // end if time to accelerate

    // test if it's time to change color

    if (++star_field[index].clock > 5)
       {
       star_field[index].color = 8;

       } // end if > 10
    else
    if (star_field[index].clock > 10)
       {
       star_field[index].color =255;

       } // end if > 20
    else
    if (star_field[index].clock > 25)
       {
       star_field[index].color = 255;

       } // end if > 25

    } // end for index

} // end Move_Stars

//////////////////////////////////////////////////////////////////////////////

void Draw_Stars(void)
{
// this function draws the stars into the double buffer

int index;

for (index=0; index<NUM_STARS; index++)
    {

    Plot_Pixel_Fast_D(star_field[index].x,star_field[index].y,(unsigned char)star_field[index].color);

    } // end for index

} // end Draw_Stars

//////////////////////////////////////////////////////////////////////////////

void Create_Scale_Data_X(int scale, int far *row)
{

// this function synthesizes the scaling of a texture sliver to all possible
// sizes and creates a huge look up table of the data.

int x;

float x_scale_index=0,
      x_scale_step;

// compute scale step or number of source pixels to map to destination/cycle

x_scale_step = (float)(sprite_width)/(float)scale;

x_scale_index+=x_scale_step;

for (x=0; x<scale; x++)
    {
    // place data into proper array position for later use

    row[x] = (int)(x_scale_index+.5);

    if  (row[x] > (SPRITE_X_SIZE-1)) row[x] = (SPRITE_X_SIZE-1);

    // next index please

    x_scale_index+=x_scale_step;

    } // end for x

} // end Create_Scale_Data_X

////////////////////////////////////////////////////////////////////////////////

void Create_Scale_Data_Y(int scale, int *row)
{

// this function synthesizes the scaling of a texture sliver to all possible
// sizes and creates a huge look up table of the data.

int y;

float y_scale_index=0,
      y_scale_step;

// compute scale step or number of source pixels to map to destination/cycle

y_scale_step = (float)(sprite_height)/(float)scale;

y_scale_index+=y_scale_step;

for (y=0; y<scale; y++)
    {
    // place data into proper array position for later use

    row[y] = ((int)(y_scale_index+.5)) * SPRITE_X_SIZE;

    if  (row[y] > (SPRITE_Y_SIZE-1)*SPRITE_X_SIZE) row[y] = (SPRITE_Y_SIZE-1)*SPRITE_X_SIZE;

    // next index please

    y_scale_index+=y_scale_step;

    } // end for y

} // end Create_Scale_Data_Y

///////////////////////////////////////////////////////////////////////////////

void Build_Scale_Table(void)
{

// this function builds the scaler tables by computing the scale indices for all
// possible scales from 1-200 pixels high

int scale;


// allocate all the memory

for (scale=1; scale<=MAX_SCALE; scale++)
    {

    scale_table_y[scale] = (int *)malloc(scale*sizeof(int)+1);
    scale_table_x[scale] = (int far *)_fmalloc(scale*sizeof(int)+1);

    } // end for scale

// create the scale tables for both the X and Y axis

for (scale=1; scale<=MAX_SCALE; scale++)
    {

    // create the indices for this scale

    Create_Scale_Data_Y(scale, (int *)scale_table_y[scale]);
    Create_Scale_Data_X(scale, (int far *)scale_table_x[scale]);

    } // end for scale

} // end Build_Scale_Table

///////////////////////////////////////////////////////////////////////////////

void Scale_Sprite(sprite_ptr sprite,int scale)
{
// this function will scale the sprite (withoot clipping).  The scaling is done
// by looking into a pre-computed table that determines how how each vertical
// strip should be.  Then another table is used to compute how many of these
// vertical strips should be drawn based on the X scale of the object

char far *work_sprite;  // the sprite texture
int *row_y;             // pointer to the Y scale data (note: it is near)
int far *row_x;         // pointer to X scale data (note: it is far)

unsigned char pixel;    // the current textel

int x,                  // work variables
    y,
    column,
    work_offset,
    video_offset,
    video_start;


// if object is too small, don't even bother rendering

if (scale<1) return;

// compute needed scaling data

row_y = scale_table_y[scale];
row_x = scale_table_x[scale];

// access the proper frame of the sprite

work_sprite = sprite->frames[sprite->curr_frame];

// compute where the starting video offset will always be

video_start = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

// the images is drawn from left to right, top to bottom

for (x=0; x<scale; x++)
    {

    // re-compute next column address

    video_offset = video_start + x;

    // compute which column should be rendered based on X scale index

    column = row_x[x];

    // now do the column as we have always

    for (y=0; y<scale; y++)
        {

        // check for transparency

        pixel = work_sprite[work_offset+column];

        if (pixel)
            double_buffer[video_offset] = pixel;


        // index to next screen row and data offset in texture memory

        video_offset += SCREEN_WIDTH;
        work_offset  =  row_y[y];

        } // end for y

    } // end for x

} // end Scale_Sprite

///////////////////////////////////////////////////////////////////////////////

void Clear_Double_Buffer(void)
{

// take a guess?

_fmemset(double_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT + 1);

} // end Clear_Double_Buffer

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

// this main places the ship into a star field and allows the user to fly around

int done=0,                   // exit flag
    scale=64,
    direction=6;              // the direction of the ship (current frame)

float scale_distance = 24000, // arbitrary constants to make the flat texture
      view_distance = 256,    // scale properly in ray casted world

      x=0,                    // position of texture or ship in 3-SPACE
      y=0,
      z=1024,
      xv=0,zv=0,              // velocity of ship in X-Z plane
      angle=180,              // angle of ship
      ship_speed=10;          // magnitude of ships speed

// set video mode to 320x200 256 color mode

_setvideomode(_MRES256COLOR);

// all sprites will have this size

sprite_width  = 80;
sprite_height = 48;

// create the look up tables for the scaler engine

Build_Scale_Table();

// initialize the pcx file that holds all the cells

PCX_Init((pcx_picture_ptr)&text_cells);

// load the pcx file that holds the cells

PCX_Load("vyrentxt.pcx", (pcx_picture_ptr)&text_cells,1);

// PCX_Show_Buffer((pcx_picture_ptr)&text_cells);

// create some memory for the double buffer

Init_Double_Buffer();

// initialize the star field

Init_Stars();

// set the ships direction and velocity up

angle=direction*30+90;

xv = (float)(ship_speed*cos(3.14159*angle/180));
zv = (float)(ship_speed*sin(3.14159*angle/180));

Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);

// load the 12 frames of the ship

PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,3,0,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,4,1,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,5,2,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,6,0,2);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,7,1,2);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,8,2,2);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,9,0,3);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,10,1,3);
PCX_Grap_Bitmap((pcx_picture_ptr)&text_cells,(sprite_ptr)&object,11,2,3);

// initialize the position of the ship

object.curr_frame = 0;
object.x          = 0;
object.y          = 0;

Clear_Double_Buffer();


// get user input and draw ship

while(!done)
     {

     // has user hit keyboard

     if (kbhit())
        {
        switch(getch())
              {
              case '4': // turn ship left
                   {

                   if (++direction==12)
                      {

                      direction=0;

                      } // end if wrap around
                   } break;

              case '6': // turn ship right
                   {

                   if (--direction < 0)
                      {

                      direction=11;

                      } // end if wrap around

                   } break;

              case '8': // speed ship up
                      {
                      if (++ship_speed > 20)
                         ship_speed=20;

                      } break;

              case '2': // slow ship down
                      {
                      if (--ship_speed < 0)
                         ship_speed=0;

                      } break;

              case 'q': // exit program
                      {
                      done=1;
                      } break;

              default:break;

              } // end switch

        // reset velocity and direction vectors

        angle=direction*30+90;

        xv = (float)(ship_speed*cos(3.14159*angle/180));
        zv = (float)(ship_speed*sin(3.14159*angle/180));

        } // end if

        // translate the ship each cycle

        x+=xv;
        z+=zv;

        // bound to hither plane

        if (z<256)
           z=256;

        // compute the size of the bitmap

        scale = (int)( scale_distance/z );

        // based on the size of the bitmap, compute the perspective X and Y

        object.x = (int)((float)x*view_distance / (float)z) + 160 - (scale>>1);
        object.y = 100 - (((int)((float)y*view_distance / (float)z) + (scale>>1)) );

        // bound to screen edges

        if (object.x < 0 )
            object.x = 0;
        else
        if (object.x+scale >= SCREEN_WIDTH)
            object.x = SCREEN_WIDTH-scale;


        if (object.y < 0 )
            object.y = 0;
        else
        if (object.y+scale >= SCREEN_HEIGHT)
            object.y = SCREEN_HEIGHT-scale;


        // set current frame

        object.curr_frame = direction;

        // blank out the double buffer

        Clear_Double_Buffer();

        Move_Stars();

        Draw_Stars();

        // scale the sprite to it's proper size

        Scale_Sprite((sprite_ptr)&object,scale);

        Show_Double_Buffer(double_buffer);

        // show user some info

        _settextposition(23,0);
        printf("Position=(%4.2f,%4.2f,%4.2f)   ",x,y,z);

       // slow things down a bit

       Timer(1);

     } // end while

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&text_cells);

// go back to text mode

_setvideomode(_DEFAULTMODE);

} // end main



