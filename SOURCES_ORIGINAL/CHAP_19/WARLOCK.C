
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
#include "graphics.h"  // load our graphics library
#include "sndlib.h"  // load our sound library

// P R O T O T Y P E S //////////////////////////////////////////////////////

void Blit_Char_D(int xc,int yc,char c,int color);

void Blit_String_D(int x,int y,int color, char *string);

void _interrupt _far New_Key_Int(void);

void Melt(void);

void Render_Sliver(sprite_ptr sprite,long scale, int column);

void Timer(int clicks);

void Create_Scale_Data(int scale, int *row);

void Build_Tables(void);

void Allocate_World(void);

int Load_World(char *file);

void Demo_Setup(void);

void Ray_Caster(long x,long y,long view_angle);

void Wait_For_Vsync(void);

void Draw_Ground(void);

int Get_Input(void);

void Destroy_Door(int x_cell, int y_cell, int command);

// A S S E M B L Y  E X T E R N S ///////////////////////////////////////////

extern far Draw_Ground_32();   // renders the sky and ground
extern far Render_Buffer_32(); // copies the double buffer to the video buffer
extern far Render_Sliver_32(); // draws a textured "sliver"

// T Y P E S ////////////////////////////////////////////////////////////////

typedef long fixed;    // fixed point is 32 bits

// S T R U C T U R E S //////////////////////////////////////////////////////

// this structure is used for the worm or melt effect

typedef struct worm_typ
        {
        int y;       // current y position of worm
        int color;   // color of worm
        int speed;   // speed of worm
        int counter; // counts time until movement

        } worm, *worm_ptr;


// D E F I N E S /////////////////////////////////////////////////////////////

// #define MAKING_DEMO 1     // this flag is used to turn on the demo record
                             // option.  this is for developers only
// keyboard stuff

#define KEYBOARD_INT    0x09         // the keyboard interrupt vector
#define KEY_BUFFER      0x60         // keyboard buffer area
#define KEY_CONTROL     0x61         // keyboard control register
#define INT_CONTROL     0x20         // interrupt control register

// make and break codes for the arrow keys

#define MAKE_RIGHT      77
#define MAKE_LEFT       75
#define MAKE_UP         72
#define MAKE_DOWN       80

#define BREAK_RIGHT     205
#define BREAK_LEFT      203
#define BREAK_UP        200
#define BREAK_DOWN      208

// indices into arrow key state table

#define INDEX_UP        0
#define INDEX_DOWN      1
#define INDEX_RIGHT     2
#define INDEX_LEFT      3


// these dataums are used as the records for the demo data

#define DEMO_RIGHT      1
#define DEMO_LEFT       2
#define DEMO_UP         4
#define DEMO_DOWN       8

// these are for the door system

#define DOOR_DEAD            0     // the door is gone
#define DOOR_DYING           1     // the door is phasing
#define PROCESS_DOOR_DESTROY 0     // tells the door engine to continue processing
#define START_DOOR_DESTROY   1     // telsl the door engine to begin


#define MAX_LENGTH_DEMO      2048  // maximum length a demo sequence can be
#define END_OF_DEMO          255   // used in the demo file to flag EOF

#define VGA_INPUT_STATUS_1   0x3DA // vga status reg 1, bit 3 is the vsync
                                   // when 1 - retrace in progress
                                   // when 0 - no retrace

#define VGA_VSYNC_MASK       0x08  // masks off unwanted bits of status reg

// #define DEBUG                1  // used to toggle debugging

#define OVERBOARD              52  // the closest a player can get to a wall

#define INTERSECTION_FOUND      1  // used by ray caster to flag an intersection

#define MAX_SCALE             201  // maximum size and wall "sliver" can be

#define WINDOW_HEIGHT         152  // height of the game view window

#define WINDOW_MIDDLE          76  // the center or horizon of the view window

#define VERTICAL_SCALE      13312  // used to scale the "slivers" to get proper
                                   // perspective and aspect ratio


// these are for the sound FX's

#define NUM_SOUNDS     5           // maximum number of loaded sounds

#define BACK_SOUNDS    4           // larget index to backgrounbd sound

#define SOUND_LAUGH    0
#define SOUND_MOAN     1
#define SOUND_WIND     2
#define SOUND_GROWL    3
#define SOUND_DOOR     4


// constants used to represent angles for the ray caster

#define ANGLE_0     0
#define ANGLE_1     5
#define ANGLE_2     10
#define ANGLE_4     20
#define ANGLE_5     25
#define ANGLE_6     30
#define ANGLE_15    80
#define ANGLE_30    160
#define ANGLE_45    240
#define ANGLE_60    320
#define ANGLE_90    480
#define ANGLE_135   720
#define ANGLE_180   960
#define ANGLE_225   1200
#define ANGLE_270   1440
#define ANGLE_315   1680
#define ANGLE_360   1920     // note: the circle has been broken up into 1920
                             // sub-arcs

#define STEP_LENGTH 15       // number of units player moves foward or backward


#define WORLD_ROWS    64     // number of rows in the game world
#define WORLD_COLUMNS 64     // number of columns in the game world

#define CELL_X_SIZE   64     // size of a cell in the gamw world
#define CELL_Y_SIZE   64


#define CELL_X_SIZE_FP   6   // log base 2 of 64 (used for quick division)
#define CELL_Y_SIZE_FP   6

// size of overall game world

#define WORLD_X_SIZE  (WORLD_COLUMNS * CELL_X_SIZE)
#define WORLD_Y_SIZE  (WORLD_ROWS    * CELL_Y_SIZE)

// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt _far *Old_Key_Isr)(); // holds old keyboard interrupt handler


unsigned int far *clock = (unsigned int far *)0x0000046CL; // pointer to internal
                                                           // 18.2 clicks/sec


// world map of nxn cells, each cell is 64x64 pixels

char far *world[WORLD_ROWS];       // pointer to matrix of cells that make up
                                   // world

float far *tan_table;              // tangent tables used to compute initial
float far *inv_tan_table;          // intersections with ray


float far *y_step;                 // x and y steps, used to find intersections
float far *x_step;                 // after initial one is found


float far *cos_table;              // used to cacell out fishbowl effect

float far *inv_cos_table;          // used to compute distances by calculating
float far *inv_sin_table;          // the hypontenuse

int *scale_table[MAX_SCALE+1];     // table with pre-computed scale indices

worm worms[320];                   // used to make the screen melt

sprite object;                     // general sprite object used by everyone

pcx_picture walls_pcx,             // holds the wall textures
         controls_pcx,             // holds the control panel at bottom of screen
         intro_pcx;                // holds the intro screen


int demo_mode=1;                   // toogles demo mode on and off.  Note: this
                                   // must be 0 to record a demo


// parmeter block used by assembly language sliver engine

char far *sliver_texture; // pointer to texture being rendered
int sliver_column;        // index into texture i.e. which column of texture
int sliver_top;           // starting Y position to render at
int sliver_scale;         // overall height of sliver
int sliver_ray;           // current ray being cast
int sliver_clip;          // index into texture after clipping
int *scale_row;           // row of scale value look up table to use


// keyboard stuff

int raw_key;                  // the global raw keyboard data aquired from the ISR

int key_table[4] = {0,0,0,0}; // the key state table for the motion keys

// the player

int player_x,                 // the players X position
    player_y,                 // the players Y position
    player_view_angle;        // the current view angle of the player

unsigned char far *demo;      // table of data for demo mode


// if the code gets enabled it allocates various data to create a demo file

#if MAKING_DEMO

unsigned char demo_out[MAX_LENGTH_DEMO];  // digitized output file

unsigned char demo_word=0;                // packed demo packet

int demo_out_index=0;                     // number of motions in file

FILE *fp, *fopen();                       // general file stuff

#endif


// used for color FX

RGB_color red_glow;                       // red glowing objects

int red_glow_index = 254;                 // index of color register to glow


// variables to track status of a door

int door_state = DOOR_DEAD;               // state of door
int door_clock = 0;                       // global door clock, counts
                                          // number of frames to do door
                                          // animation

// sound system stuff

char far *sounds[NUM_SOUNDS];             // pointers to sound files

unsigned char sound_lengths[NUM_SOUNDS];  // length of each sound


// F U N C T I O N S /////////////////////////////////////////////////////////

void Blit_Char_D(int xc,int yc,char c,int color)
{
// this function uses the rom 8x8 character set to blit a character to the
// double buffer, also it blits the character in two colors

int offset,x,y;
char far *work_char;
unsigned char bit_mask = 0x80;

// compute starting offset in rom character lookup table

work_char = (char far *)(rom_char_set + c * CHAR_HEIGHT);

// compute offset of character in video buffer

offset = (yc << 8) + (yc << 6) + xc;

for (y=0; y<CHAR_HEIGHT; y++)
    {
    // reset bit mask

    bit_mask = 0x80;

    // test if it's time to change colors

    if (y==(CHAR_HEIGHT/2))
       color-=8; // change to lower intensity

    for (x=0; x<CHAR_WIDTH; x++)
        {
        // test for transparent pixel i.e. 0, if not transparent then draw

        if ((*work_char & bit_mask))
             double_buffer[offset+x] = (char)color;

        // shift bit mask

        bit_mask = (bit_mask>>1);

        } // end for x

    // move to next line in video buffer and in rom character data area

    offset      += SCREEN_WIDTH;
    work_char++;

    } // end for y


} // end Blit_Char_D

//////////////////////////////////////////////////////////////////////////////

void Blit_String_D(int x,int y,int color, char *string)
{
// this function blits an entire string to the double buffer
// It calls blit_char_d

int index;

for (index=0; string[index]!=0; index++)
     {

     Blit_Char_D(x+(index<<3),y,string[index],color);

     } /* end while */

} /* end Blit_String_D */

//////////////////////////////////////////////////////////////////////////////

void _interrupt _far New_Key_Int(void)
{
// this function links into the keyboard interrupt and takes over.  it is called
// when a key is pressed.  Note: how it differs from the one were saw in the
// chapter on I/O.  It has been modified to take into consideration the demo
// mode of the system

_asm
   {
   sti                    ; re-enable interrupts
   in al, KEY_BUFFER      ; get the key that was pressed
   xor ah,ah              ; zero out upper 8 bits of AX
   mov raw_key, ax        ; store the key in global
   in al, KEY_CONTROL     ; set the control register
   or al, 82h             ; set the proper bits to reset the FF
   out KEY_CONTROL,al     ; send the new data back to the control register
   and al,7fh
   out KEY_CONTROL,al     ; complete the reset
   mov al,20h
   out INT_CONTROL,al     ; re-enable interrupts
                          ; when this baby hits 88 mph, your gonna see
                          ; some serious @#@#$%

   } // end inline assembly

// now for some C to update the arrow state table

// process the key and update the table (only if not in demo mode)

if (!demo_mode)
{
switch(raw_key)
      {
      case MAKE_UP:    // pressing up
           {
           key_table[INDEX_UP]    = 1;
           } break;

      case MAKE_DOWN:  // pressing down
           {
           key_table[INDEX_DOWN]  = 1;
           } break;

      case MAKE_RIGHT: // pressing right
           {
           key_table[INDEX_RIGHT] = 1;
           } break;

      case MAKE_LEFT:  // pressing left
           {
           key_table[INDEX_LEFT]  = 1;
           } break;

      case BREAK_UP:    // releasing up
           {
           key_table[INDEX_UP]    = 0;
           } break;

      case BREAK_DOWN:  // releasing down
           {
           key_table[INDEX_DOWN]  = 0;
           } break;

      case BREAK_RIGHT: // releasing right
           {
           key_table[INDEX_RIGHT] = 0;
           } break;

      case BREAK_LEFT:  // releasing left
           {
           key_table[INDEX_LEFT]  = 0;
           } break;

      default: break;


      } // end switch

} // end if in demo mode

} // end New_Key_Int

//////////////////////////////////////////////////////////////////////////////

void Melt(void)
{

// this function "melts" the screen by moving little worms at different speeds
// down the screen.  These worms change to the color they are eating

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

     Wait_For_Vsync();

     } // end while

} // end Melt

///////////////////////////////////////////////////////////////////////////////

#if 0

void Render_Sliver(sprite_ptr sprite,long scale, int column)
{

// this function will scale a single sliver of texture data.  it uses fixed point
// numbers.

char far *work_sprite;

int work_offset=0,offset,x,y,scale_int;

unsigned char data;

fixed scale_index,scale_step;

scale_int = scale;
scale = (scale<<8);

scale_index = 0;
scale_step = (fixed)(((fixed)64) << 16) / scale;

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<scale_int; y++)
    {

    double_buffer[offset] = work_sprite[work_offset+column];

    scale_index+=scale_step;

    offset      += SCREEN_WIDTH;
    work_offset =  ((scale_index & 0xff00)>>2);

    } // end for y

} // end Draw_Sliver

#endif

///////////////////////////////////////////////////////////////////////////////

#if 0

Render_Sliver(sprite_ptr sprite,int scale, int column)
{
// this is yet another version of the sliver scaler, however it uses look up
// tables with pre-computed scale indices.  in the end I converted this to
// assembly for speed

char far *work_sprite;
int far *row;

int work_offset=0,offset,y,scale_off;

unsigned char data;

// alias proper data row

row = scale_table[scale];

if (scale>(WINDOW_HEIGHT-1))
   {
   scale_off = (scale-(WINDOW_HEIGHT-1)) >> 1;
   scale=(WINDOW_HEIGHT-1);
   sprite->y = 0;
   }

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<scale; y++)
    {

    double_buffer[offset] = work_sprite[work_offset+column];

    offset      += SCREEN_WIDTH;
    work_offset =  row[y+scale_off];

    } // end for y

} // end Draw_Sliver

#endif


///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

void Create_Scale_Data(int scale, int *row)
{

// this function synthesizes the scaling of a texture sliver to all possible
// sizes and creates a huge look up table of the data.

int y;

float y_scale_index=0,
      y_scale_step;

// compute scale step or number of source pixels to map to destination/cycle

y_scale_step = (float)64/(float)scale;

y_scale_index+=y_scale_step;

for (y=0; y<scale; y++)
    {
    // place data into proper array position for later use

    row[y] = ((int)(y_scale_index+.5)) * CELL_X_SIZE;

    // test if we slightly went overboard

    if  (row[y] > 63*CELL_X_SIZE) row[y] = 63*CELL_X_SIZE;

    // next index please

    y_scale_index+=y_scale_step;

    } // end for y

} // end Create_Scale_Data

///////////////////////////////////////////////////////////////////////////////

void Build_Tables(void)
{

// this function builds all the look up tables for the system

int ang,scale;
float rad_angle;

// allocate memory for all look up tables

// tangent tables equivalent to slopes

tan_table     = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );
inv_tan_table = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );

// step tables used to find next intersections, equivalent to slopes
// times width and height of cell

y_step        = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );
x_step        = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );


// cos table used to fix view distortion caused by caused by radial projection

cos_table     = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );


// 1/cos and 1/sin tables used to compute distance of intersection very
// quickly

inv_cos_table = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );
inv_sin_table = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );

// create the lookup tables for the scaler
// there have the form of an array of pointers, where each pointer points
// another another array of data where the 'data' are the scale indices

for (scale=0; scale<=MAX_SCALE; scale++)
    {

    scale_table[scale] = (int *)malloc(scale*sizeof(int)+1);

    } // end for scale

// create tables, sit back for a sec!

for (ang=ANGLE_0; ang<=ANGLE_360; ang++)
    {

    rad_angle = (float)((3.272e-4) + ang * 2*3.141592654/ANGLE_360);

    tan_table[ang]     = (float)tan(rad_angle);
    inv_tan_table[ang] = (float)(1/tan_table[ang]);


    // tangent has the incorrect signs in all quadrants except 1, so
    // manually fix the signs of each quadrant since the tangent is
    // equivalent to the slope of a line and if the tangent is wrong
    // then the ray that is case will be wrong

    if (ang>=ANGLE_0 && ang<ANGLE_180)
       {
       y_step[ang]        = (float)(fabs(tan_table[ang]  * CELL_Y_SIZE));
       }
    else
       y_step[ang]        = (float)(-fabs(tan_table[ang] * CELL_Y_SIZE));

    if (ang>=ANGLE_90 && ang<ANGLE_270)
       {
       x_step[ang]        = (float)(-fabs(inv_tan_table[ang] * CELL_X_SIZE));
       }
    else
       {
       x_step[ang]        = (float)(fabs(inv_tan_table[ang]  * CELL_X_SIZE));
       }



    // create the sin and cosine tables to copute distances

    inv_cos_table[ang] = (float)(1/cos(rad_angle));
    inv_sin_table[ang] = (float)(1/sin(rad_angle));

    } // end for ang

// create view filter table.  There is a cosine wave modulated on top of
// the view distance as a side effect of casting from a fixed point.
// to cancell this effect out, we multiple by the inverse of the cosine
// and the result is the proper scale.  Without this we would see a
// fishbowl effect, which might be desired in some cases?

for (ang=-ANGLE_30; ang<=ANGLE_30; ang++)
    {

    rad_angle = (float)((3.272e-4) + ang * 2*3.141592654/ANGLE_360);

    cos_table[ang+ANGLE_30] = (float)(VERTICAL_SCALE/cos(rad_angle));

    } // end for

// build the scaler table.  This table holds MAX_SCALE different arrays.  Each
// array consists of the pre-computed indices for an object to be scaled

for (scale=1; scale<=MAX_SCALE; scale++)
    {

    // create the indices for this scale

    Create_Scale_Data(scale, (int *)scale_table[scale]);

    } // end for scale

} // end Build_Tables

/////////////////////////////////////////////////////////////////////////////

void Allocate_World(void)
{
// this function allocates the memory for the world

int index;

// allocate each row

for (index=0; index<WORLD_ROWS; index++)
    {
    world[index] = (char far *)_fmalloc(WORLD_COLUMNS+1);

    } // end for index

} // end Allocate_World


////////////////////////////////////////////////////////////////////////////////

int Load_World(char *file)
{
// this function opens the input file and loads the world data from it

FILE  *fp, *fopen();
int row,column;
char ch;

// open the file

if (!(fp = fopen(file,"r")))
   return(0);

// load in the data

for (row=0; row<WORLD_ROWS; row++)
    {
    // load in the next row

    for (column=0; column<WORLD_COLUMNS; column++)
        {

        while((ch = getc(fp))==10){} // filter out CR

        // translate character to integer

        if (ch == ' ')
           ch=0;
        else
           ch = ch - '0';

        // insert data into world

        world[(WORLD_ROWS-1) - row][column] = ch;

        } // end for column

    // process the row

    } // end for row

// close the file

fclose(fp);

return(1);

} // end Load_World

////////////////////////////////////////////////////////////////////////////////

void Demo_Setup(void)
{
// this function allocates the demo mode storage area and loads the demo mode
// data

FILE *fp_demo, *fopen();
int index=0;
unsigned char data;

// allocate storage for demo mode

demo = (unsigned char far*)_fmalloc(MAX_LENGTH_DEMO);

// open up demo file

fp_demo = fopen("demo.dat","rb");

// load data

while((data=getc(fp_demo))!=END_OF_DEMO)
     {

     demo[index++] = data;

     } // end while

// place end of demo flag in data

demo[index] = END_OF_DEMO;

// close file

fclose(fp_demo);

} // end Demo_Setup

/////////////////////////////////////////////////////////////////////////////

void Ray_Caster(long x,long y,long view_angle)
{

// This is the heart of the system.  it casts out 320 rays and builds the
// 3-D image from their intersections with the walls.  It was derived from
// the previous version used in "RAY.C", however, it has been extremely
// optimized for speed by the use of many more lookup tables and fixed
// point math

int

cell_x,       // the current cell that the ray is in
cell_y,
ray,          // the current ray being cast 0-320
casting=2,    // tracks the progress of the X and Y component of the ray
x_hit_type,   // records the block that was intersected, used to figure
y_hit_type,   // out which texture to use
x_bound,      // the next vertical and horizontal intersection point
y_bound,
next_y_cell,  // used to figure out the quadrant of the ray
next_x_cell,
xray=0,       // tracks the progress of a ray looking for Y interesctions
yray=0,       // tracks the progress of a ray looking for X interesctions

x_delta,      // the amount needed to move to get to the next cell
y_delta,      // position
xb_save,
yb_save,
xi_save,      // used to save exact x and y intersection points
yi_save,
scale;


long
     cast=0,
     dist_x,  // the distance of the x and y ray intersections from
     dist_y;  // the viewpoint

float xi,     // used to track the x and y intersections
      yi;

// S E C T I O N  1 /////////////////////////////////////////////////////////v

// initialization

// compute starting angle from player.  Field of view is 60 degrees, so
// subtract half of that current view angle

if ( (view_angle-=ANGLE_30) < 0)
   {
   view_angle=ANGLE_360 + view_angle;
   } // end if

// loop through all 320 rays

for (ray=319; ray>=0; ray--)
    {

// S E C T I O N  2 /////////////////////////////////////////////////////////

    // compute first x intersection

    // need to know which half plane we are casting from relative to Y axis

    if (view_angle >= ANGLE_0 && view_angle < ANGLE_180)
       {

       // compute first horizontal line that could be intersected with ray
       // note: it will be above player

       y_bound = (CELL_Y_SIZE + (y & 0xffc0));

       // compute delta to get to next horizontal line

       y_delta = CELL_Y_SIZE;

       // based on first possible horizontal intersection line, compute X
       // intercept, so that casting can begin

       xi = inv_tan_table[view_angle] * (y_bound - y) + x;

       // set cell delta

       next_y_cell = 0;

       } // end if upper half plane
    else
       {
       // compute first horizontal line that could be intersected with ray
       // note: it will be below player

       y_bound = (int)(y & 0xffc0);

       // compute delta to get to next horizontal line

       y_delta = -CELL_Y_SIZE;

       // based on first possible horizontal intersection line, compute X
       // intercept, so that casting can begin

       xi = inv_tan_table[view_angle] * (y_bound - y) + x;

       // set cell delta

       next_y_cell = -1;

       } // end else lower half plane

// S E C T I O N  3 /////////////////////////////////////////////////////////

    // compute first y intersection

    // need to know which half plane we are casting from relative to X axis

    if (view_angle < ANGLE_90 || view_angle >= ANGLE_270)
       {

       // compute first vertical line that could be intersected with ray
       // note: it will be to the right of player

       x_bound = (int)(CELL_X_SIZE + (x & 0xffc0));

       // compute delta to get to next vertical line

       x_delta = CELL_X_SIZE;

       // based on first possible vertical intersection line, compute Y
       // intercept, so that casting can begin

       yi = tan_table[view_angle] * (x_bound - x) + y;

       // set cell delta

       next_x_cell = 0;

       } // end if right half plane
    else
       {

       // compute first vertical line that could be intersected with ray
       // note: it will be to the left of player

       x_bound = (int)(x & 0xffc0);

       // compute delta to get to next vertical line

       x_delta = -CELL_X_SIZE;

       // based on first possible vertical intersection line, compute Y
       // intercept, so that casting can begin

       yi = tan_table[view_angle] * (x_bound - x) + y;

       // set cell delta

       next_x_cell = -1;

       } // end else right half plane


// begin cast

    casting       = 2;                // two rays to cast simultaneously
    xray=yray     = 0;                // reset intersection flags


// S E C T I O N  4 /////////////////////////////////////////////////////////

    while(casting)
         {

         // continue casting each ray in parallel

         if (xray!=INTERSECTION_FOUND)
            {

            // compute current map position to inspect

            cell_x = ( (x_bound+next_x_cell) >> CELL_X_SIZE_FP);

            cell_y = (int)yi;
            cell_y>>=CELL_Y_SIZE_FP;

            // test if there is a block where the current x ray is intersecting

            if ((x_hit_type = world[cell_y][cell_x])!=0)
               {
               // compute distance

               dist_x  = (long)((yi - y) * inv_sin_table[view_angle]);
               yi_save = (int)yi;
               xb_save = x_bound;

               // terminate X casting

               xray = INTERSECTION_FOUND;
               casting--;

               } // end if a hit
            else
               {

               // compute next Y intercept

               yi += y_step[view_angle];

               // find next possible x intercept point

               x_bound += x_delta;

               } // end else

            } // end if x ray has intersected

// S E C T I O N  5 /////////////////////////////////////////////////////////

         if (yray!=INTERSECTION_FOUND)
            {

            // compute current map position to inspect

            cell_x = xi;
            cell_x>>=CELL_X_SIZE_FP;

            cell_y = ( (y_bound + next_y_cell) >> CELL_Y_SIZE_FP);

            // test if there is a block where the current y ray is intersecting

            if ((y_hit_type = world[cell_y][cell_x])!=0)
               {

               // compute distance

               dist_y  = (long)((xi - x) * inv_cos_table[view_angle]);
               xi_save = (int)xi;
               yb_save = y_bound;

               yray = INTERSECTION_FOUND;
               casting--;

               } // end if a hit
            else
               {

               // terminate Y casting

               xi += x_step[view_angle];

               // compute next possible y intercept

               y_bound += y_delta;

               } // end else

            } // end if y ray has intersected

         } // end while not done

// S E C T I O N  6 /////////////////////////////////////////////////////////

    // at this point, we know that the ray has succesfully hit both a
    // vertical wall and a horizontal wall, so we need to see which one
    // was closer and then render it

    if (dist_x < dist_y)
       {

       // there was a vertical wall closer than the horizontal

       // compute actual scale and multiply by view filter so that spherical
       // distortion is cancelled

       scale = (int)(cos_table[ray]/dist_x);

       // clip wall sliver against view port

       if (scale>(MAX_SCALE-1)) scale=(MAX_SCALE-1);

       scale_row      = scale_table[scale-1];

       if (scale>(WINDOW_HEIGHT-1))
          {
          sliver_clip = (scale-(WINDOW_HEIGHT-1)) >> 1;
          scale=(WINDOW_HEIGHT-1);
          }
       else
          sliver_clip = 0;

       sliver_scale   = scale-1;

       // set up parameters for assembly language

       sliver_texture = object.frames[x_hit_type];
       sliver_column  = (yi_save & 0x003f);
       sliver_top     = WINDOW_MIDDLE - (scale >> 1);
       sliver_ray     = ray;

       // render the sliver in assembly

       Render_Sliver_32();

       } // end if

    else // must of hit a horizontal wall first
       {

       // there was a vertical wall closer than the horizontal

       // compute actual scale and multiply by view filter so that spherical
       // distortion is cancelled

       scale = (int)(cos_table[ray]/dist_y);

       // do clipping again

       if (scale>(MAX_SCALE-1)) scale=(MAX_SCALE-1);

       scale_row      = scale_table[scale-1];

       if (scale>(WINDOW_HEIGHT-1))
          {
          sliver_clip = (scale-(WINDOW_HEIGHT-1)) >> 1;
          scale=(WINDOW_HEIGHT-1);
          }
       else
          sliver_clip = 0;

       sliver_scale   = scale-1;

       // set up parameters for assembly language

       sliver_texture = object.frames[y_hit_type+1];
       sliver_column  = (xi_save & 0x003f);
       sliver_top     = WINDOW_MIDDLE - (scale >> 1);
       sliver_ray     = ray;

       // render the sliver

       Render_Sliver_32();

       } // end else

// S E C T I O N  7 /////////////////////////////////////////////////////////

    // cast next ray

    // test if view angle need to wrap around

    if (++view_angle>=ANGLE_360)
       {

       view_angle=0;

       } // end if

    } // end for ray

} // end Ray_Caster

/////////////////////////////////////////////////////////////////////////////

void Wait_For_Vsync(void)
{
// this function waits for the start of a vertical retrace, if a vertical
// retrace is in progress then it waits until the next one

while(!_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK)
     {
     // do nothing, vga is in retrace
     } // end while

// now wait for vysnc and exit

while((_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK))
     {
     // do nothing, wait for start of retrace
     } // end while

// at this point a vertical retrace is occuring, so return back to caller

} // Wait_For_Vsync

//////////////////////////////////////////////////////////////////////////////

void Draw_Ground(void)
{
// clear the screen and draw the ground using 16 bit instructions.
// the sky is black and the ground is grey using the standard Dpaint pallete

_asm
   {
   push di                      ; save di
   cld                          ; set direction to foward
   les di, double_buffer        ; get address of double buffer
   xor ax,ax                    ; zero out ax
   mov cx,320*WINDOW_MIDDLE/2   ; lets fill the top with black
   rep stosw                    ; do it

   mov ax,0x1E1E                ; lets fill the bottom with grey
   mov cx,320*WINDOW_MIDDLE/2
   rep stosw                    ; do it

   pop di                       ; restore registers and blaze
   } // end asm

} // end Draw_Ground

//////////////////////////////////////////////////////////////////////////////

int Get_Input(void)
{
// this function returns true if there is a motion key being pressed or if
// the demo file is being ran. if the demo file is being ran then the function
// changes the global keytable array to simulate that the input is comming
// from a source other than the keyboard

static int demo_index=0;

unsigned char demo_data;

if (demo_mode)
   {

   // test if user is trying to get out of demo

   if (raw_key==1)
      {
      // reset everything

      raw_key=0;
      demo_mode=0;

      // send player back to home position

      player_x=53*64+25;
      player_y=14*64+25;
      player_view_angle=ANGLE_60;

      // reset the motion table

      key_table[INDEX_RIGHT] = 0;
      key_table[INDEX_LEFT]  = 0;
      key_table[INDEX_UP]    = 0;
      key_table[INDEX_DOWN]  = 0;

      return(0);

      } // end if user trying to get out of demo

   // read raw key from file

   demo_data = demo[demo_index++];

   // test for end of demo, if at end start demo over

   if (demo_data==END_OF_DEMO)
      {

      // reset data index

      demo_index=0;

      // move player to starting position again

      player_x=53*64+25;
      player_y=14*64+25;
      player_view_angle=ANGLE_60;

      return(0);

      } // end if at end of demo

   // else, process key and alter demo file to make it seem as if it
   // came from keyboard.

   key_table[INDEX_RIGHT] = (demo_data & DEMO_RIGHT);
   key_table[INDEX_LEFT]  = (demo_data & DEMO_LEFT);
   key_table[INDEX_UP]    = (demo_data & DEMO_UP);
   key_table[INDEX_DOWN]  = (demo_data & DEMO_DOWN);

   if (key_table[0] || key_table[1] || key_table[2] || key_table[3])
      return(1);
   else
      return(0);

   } // end if in demo mode
else
   {
   // not in demo mode, so see if user is pressing a motion key

   if (key_table[0] || key_table[1] || key_table[2] || key_table[3])
       return(1);
   else
       return(0);

   } // end else

} // end Get_Input

///////////////////////////////////////////////////////////////////////////////

void Destroy_Door(int x_cell, int y_cell, int command)
{
// this function is called every frame when a door is being destroyed.
// Basically it cycles a color on the door and makes it glow red as if it
// were energizing.  the function does this a specific number of times and
// then turns itself off and takes the door out of the world

static int door_x_cell,   // used to hold the position of the door
           door_y_cell;

// test what is happening i.e. door starting destruction or
// continuing

if (command==START_DOOR_DESTROY)
   {

   // play spell

   Voc_Stop_Sound();
   Voc_Play_Sound(sounds[SOUND_DOOR] , sound_lengths[SOUND_DOOR]);

   // reset glow color

   red_glow.red   = 0;
   red_glow.green = 0;
   red_glow.blue  = 0;

   Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

   // set door sequence in motion and number of clicks

   door_state = DOOR_DYING;
   door_clock = 30;

   // remember where door is so we can make it disapear

   door_x_cell = x_cell;
   door_y_cell = y_cell;

   } // end start up door
else
   {

   // increase intensity of glow

   red_glow.red+=2;

   Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

   // test if we are done with door

   if (--door_clock < 0)
      {

      // reset state of door

      door_state = DOOR_DEAD;

      // say bye-bye to door

      world[door_y_cell][door_x_cell] = 0;

      // reset pallete register

      red_glow.red=0;

      Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

      } // end if

   } // end else door must be dying

} // end Destroy_Door

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

// S E C T I O N   1 /////////////////////////////////////////////////////////



int done=0,            // flag to exit game
    play_sound,        // sound sound system will play next
    x_cell,            // celluar position of player in game grid
    y_cell,
    x_sub_cell,        // position of player within a single cell
    y_sub_cell,
    door_x,            // cell position of door player is trying to open
    door_y;

float dx,dy;


// S E C T I O N   2 /////////////////////////////////////////////////////////

// seed random number genrerator

srand(13);

// set videomode to 320x256

_setvideomode(_MRES256COLOR);

// initialize sound system

Voc_Load_Driver();
Voc_Init_Driver();
Voc_Set_Port(0x220);
Voc_Set_DMA(5);
Voc_Get_Version();
Voc_Set_Status_Addr((char __far *)&ct_voice_status);

// load the intro screen for a second and bore everyone

PCX_Init((pcx_picture_ptr)&intro_pcx);
PCX_Load("warintr2.pcx", (pcx_picture_ptr)&intro_pcx,1);
PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);


// load in sounds

sounds[SOUND_LAUGH] = Voc_Load_Sound("laugh.voc", &sound_lengths[SOUND_LAUGH]);
sounds[SOUND_MOAN]  = Voc_Load_Sound("moan.voc",  &sound_lengths[SOUND_MOAN]);
sounds[SOUND_WIND]  = Voc_Load_Sound("wind.voc",  &sound_lengths[SOUND_WIND]);
sounds[SOUND_GROWL] = Voc_Load_Sound("growl.voc", &sound_lengths[SOUND_GROWL]);
sounds[SOUND_DOOR]  = Voc_Load_Sound("door.voc", &sound_lengths[SOUND_DOOR]);

// laugh at player a little

Voc_Play_Sound(sounds[SOUND_LAUGH] , sound_lengths[SOUND_LAUGH]);
Voc_Set_Speaker(1);

// install new isr's

Old_Key_Isr = _dos_getvect(KEYBOARD_INT);

_dos_setvect(KEYBOARD_INT, New_Key_Int);

// load the demo information

Demo_Setup();

Allocate_World();

// build all the lookup tables

Build_Tables();

Load_World("raymap.dat");

// initialize the double buffer

Init_Double_Buffer();

sprite_width  = 64;
sprite_height = 64;

// load up the textures

PCX_Init((pcx_picture_ptr)&walls_pcx);
PCX_Load("walltext.pcx", (pcx_picture_ptr)&walls_pcx,1);
Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);


// grab a blank

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,0,0,0);

// grab first wall

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,1,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,2,1,0);

// grab second wall

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,3,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,4,3,0);

// grab third wall

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,5,2,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,6,3,1);


// grab doors

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,7,0,2);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,8,1,2);

// dont need textures anymore

PCX_Delete((pcx_picture_ptr)&walls_pcx);


// load up the control panel

PCX_Init((pcx_picture_ptr)&controls_pcx);

PCX_Load("controls.pcx", (pcx_picture_ptr)&controls_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&controls_pcx);

PCX_Delete((pcx_picture_ptr)&controls_pcx);

PCX_Delete((pcx_picture_ptr)&intro_pcx);

// initialize the generic sprite we will use to access the textures with

object.curr_frame = 0;
object.x          = 0;
object.y          = 0;

// position the player somewhere interseting

player_x=53*64+25;
player_y=14*64+25;
player_view_angle=ANGLE_60;

// render initial view

// clear the double buffer

_fmemset(double_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT + 1);

red_glow.red   = 0;
red_glow.green = 0;
red_glow.blue  = 0;

Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

// S E C T I O N   3 /////////////////////////////////////////////////////////

// render the initial view

Draw_Ground_32();

Ray_Caster(player_x,player_y,player_view_angle);

Render_Buffer_32();

// main event loop, wait for user to press ESC to quit

while(!done)
     {

#if MAKING_DEMO

demo_word=0;

#endif

// S E C T I O N   4 /////////////////////////////////////////////////////////

     // is player moving due to input or demo mode ?

     if (Get_Input())
        {

        // reset deltas

        dx=dy=0;

        // what is user doing

        if (key_table[INDEX_RIGHT])
           {
           // rotate player right

           if ((player_view_angle-=ANGLE_6)<ANGLE_0)
              player_view_angle=ANGLE_360;

           #if MAKING_DEMO

           demo_word|=DEMO_RIGHT;

           #endif

           } // end if right
        else
        if (key_table[INDEX_LEFT])
           {
           // rotate player to left

           if ((player_view_angle+=ANGLE_6)>=ANGLE_360)
              player_view_angle=ANGLE_0;

           #if MAKING_DEMO

           demo_word|=DEMO_LEFT;

           #endif

           } // end if left

        if (key_table[INDEX_UP])
           {
           // move player along view vector foward

           dx=(float)(cos(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);
           dy=(float)(sin(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);

           #if MAKING_DEMO

           demo_word|=DEMO_UP;

           #endif

           } // end if up
        else
        if (key_table[INDEX_DOWN])
           {
           // move player along view vector backward

           dx=(float)(-cos(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);
           dy=(float)(-sin(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);

           #if MAKING_DEMO

           demo_word|=DEMO_DOWN;

           #endif

           } // end if down

// S E C T I O N   5 /////////////////////////////////////////////////////////

        // move player

        player_x= (int)((float)player_x+dx);
        player_y= (int)((float)player_y+dy);

        // test if user has bumped into a wall i.e. test if there
        // is a cell within the direction of motion, if so back up !

        // compute cell position

        x_cell = player_x/CELL_X_SIZE;
        y_cell = player_y/CELL_Y_SIZE;

        // compute position relative to cell

        x_sub_cell = player_x % CELL_X_SIZE;
        y_sub_cell = player_y % CELL_Y_SIZE;


        // resolve motion into it's x and y components

        if (dx>0 )
           {
           // moving right

           if ( (world[y_cell][x_cell+1] != 0)  &&
                (x_sub_cell > (CELL_X_SIZE-OVERBOARD ) ) )
                {
                // back player up amount he steped over the line

                player_x-= (x_sub_cell-(CELL_X_SIZE-OVERBOARD ));

                } // end if need to back up
           }
        else
           {
           // moving left

           if ( (world[y_cell][x_cell-1] != 0)  &&
                (x_sub_cell < (OVERBOARD) ) )
                {
                // back player up amount he steped over the line

                player_x+= (OVERBOARD-x_sub_cell) ;

                } // end if need to back up

           } // end else

        if (dy>0 )
           {
           // moving up

           if ( (world[(y_cell+1)][x_cell] != 0)  &&
                (y_sub_cell > (CELL_Y_SIZE-OVERBOARD ) ) )
                {
                // back player up amount he steped over the line

                player_y-= (y_sub_cell-(CELL_Y_SIZE-OVERBOARD ));

                } // end if need to back up
           }
        else
           {
           // moving down

           if ( (world[(y_cell-1)][x_cell] != 0)  &&
                (y_sub_cell < (OVERBOARD) ) )
                {
                // back player up amount he steped over the line

                player_y+= (OVERBOARD-y_sub_cell);

                } // end if need to back up

           } // end else

        }  // end if player in motion

        #if MAKING_DEMO

        demo_out[demo_out_index++] = demo_word;
        printf("\ndemo out = %d",demo_word);

        #endif

// S E C T I O N   6 /////////////////////////////////////////////////////////

        // test non-motion keys

        if (raw_key==1)
           {
           done=1;

           } // end if user is exiting
        else
        if (raw_key==57) // trying to open a door
           {
           // test if there is a door in front of player

           // project a "feeler" 3 steps in front of player and test for a door

           door_x = (int)(player_x + cos(6.28*player_view_angle/ANGLE_360)*6*STEP_LENGTH);
           door_y = (int)(player_y + sin(6.28*player_view_angle/ANGLE_360)*6*STEP_LENGTH);

           // compute cell position

           x_cell = door_x/CELL_X_SIZE;
           y_cell = door_y/CELL_Y_SIZE;

           // test for door

           if (world[y_cell][x_cell] == 7 || world[y_cell][x_cell] == 8)
              {

              // make door disapear by starting process

              Destroy_Door(x_cell,y_cell, START_DOOR_DESTROY);

              } // end if a door was found

           } // end if trying to open a door

        // call all responder and temporal functions that occur each frame

        Destroy_Door(0,0,PROCESS_DOOR_DESTROY);

// S E C T I O N   7 /////////////////////////////////////////////////////////

        // clear the double buffer and render the ground and ceiling

        Draw_Ground_32();

        // render the view

        Ray_Caster(player_x,player_y,player_view_angle);

        // do all rendering that goes on top of 3-D view here

        if (demo_mode)
           Blit_String_D(100,16,10,"D e m o   M o d e");

        // show the double buffer

        Render_Buffer_32();

        // test if it's time for a sound

           if (rand()%150==1 && ct_voice_status==0)
              {
              play_sound = rand()%BACK_SOUNDS;

              Voc_Stop_Sound();
              Voc_Play_Sound(sounds[play_sound] , sound_lengths[play_sound]);

              } // end if time to play a sound

     } //end while

// S E C T I O N   8 /////////////////////////////////////////////////////////

// let's blaze with some style

Melt();

// replace old ISR's

_dos_setvect(KEYBOARD_INT, Old_Key_Isr);

// restore original mode

_setvideomode(_DEFAULTMODE);


#if MAKING_DEMO

// save the digitized demo data to a file

fp = fopen("demo.dat","wb");
for (t=0; t<demo_out_index-1; t++)
    {
    putc(demo_out[t],fp);
    }
    putc(END_OF_DEMO,fp);
    putc(END_OF_DEMO,fp);
    fclose(fp);

#endif

Voc_Set_Speaker(0);

// unload sounds

Voc_Unload_Sound(sounds[SOUND_LAUGH]);
Voc_Unload_Sound(sounds[SOUND_MOAN]);
Voc_Unload_Sound(sounds[SOUND_WIND]);
Voc_Unload_Sound(sounds[SOUND_GROWL]);

Voc_Terminate_Driver();


} // end main


