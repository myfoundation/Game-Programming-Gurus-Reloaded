
// NET-TANK - 1994, Andre' Lamothe

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
#include "sndlib.h" // include our sound library
#include "keylib.h" // include our keyboard I/O library
#include "serlib.h" // include our serial library

// D E F I N E S /////////////////////////////////////////////////////////////

#define NET1           // used to create each version of the game, use "NET2" to
                       // build the other version


#define LOCAL_MISSILE  0 // this missile was fired by the local player
#define REMOTE_MISSILE 1 // this missile was fired by remote player
#define MISS_ALIVE     1 // flags that a missile is alive
#define MISS_DEAD      0 // flags that a missile is dead
#define NUM_MISSILES  20 // number of total missiles allowed in world


#define LINKED        1  // flags used to dentote if game is linked or solo
#define UNLINKED      0

#define GAME_STOPPED  0  // flags for the main loop body of the game
#define GAME_PAUSED   1
#define GAME_RUNNING  2

#define CELL_ROWS    11  // size of game grid
#define CELL_COLUMNS 20
#define NUM_CELLS    17  // number of cells in game grid
#define CELL_WIDTH   16  // dimensions of each cell
#define CELL_HEIGHT  16

#define GAUGE_X      (unsigned int)0    // position and size of instrumentation
#define GAUGE_Y      (unsigned int)176  // panel
#define GAUGE_WIDTH  (unsigned int)320
#define GAUGE_HEIGHT (unsigned int)24

#define SOUND_FIRE     0   // the sound made when a missile is fired
#define SOUND_EXPLODE  1   // the sound made when you explode
#define SOUND_HIT      2   // the sound made missile hits wall
#define SOUND_CRASH    3   // the sound made when you hit a wall
#define SOUND_FX       4   // special fx sounds, not in yet
#define SOUND_DIE      5   // the sound you send to the remote to scare hiim
#define SOUND_LINK     6   // the sound made when a link is established

#define NUM_PARTICLES  100 // number of particles in explosion

// S T R U C T U R E S ///////////////////////////////////////////////////////

// typedef for a explosion particle and for a missile

typedef struct particle_typ
        {
        int x;                   // x position
        int y;                   // y position
        int xv;                  // x velocity
        int yv;                  // y velocity
        unsigned char color;     // the color of the particle
        unsigned char back;      // the color behind the particle
        int state;               // the state of the particle
        int tag;                 // if the particle is a missile then who
                                 // does it belong to?

        } particle, *particle_ptr;



// G L O B A L S /////////////////////////////////////////////////////////////


// G A M E  G R I D

// current game grid

char **game_grid=NULL;  // this will point to tha active game grid i.e.
                        // game1_grid, game2_grid ...


// game grid 1

char *game1_grid[CELL_ROWS]  ={"####################",
                               "#               #  #",
                               "# # ### #  #### #  #",
                               "# #     # #     #  #",
                               "# ####       ####  #",
                               "#       # #        #",
                               "# ## #    #      # #",
                               "# ## ## # ###  # # #",
                               "# #   #        ### #",
                               "#         #        #",
                               "####################"};


// another game grid compliments of Rock Berry!

char *game2_grid[CELL_ROWS]  ={"####################",
                               "#        #         #",
                               "# #  ### #####     #",
                               "# #   #  #   #  #  #",
                               "# #      #      #  #",
                               "# #  #          #  #",
                               "# #  ######     #  #",
                               "# #  #      #####  #",
                               "#    #      #      #",
                               "#                  #",
                               "####################"};


particle shrap[NUM_PARTICLES]; // the array of sharpnel particles for explosion

particle missiles[NUM_MISSILES]; // the array of missiles in the world


char far *sounds[10];               // pointers to loaded sounds
unsigned char sound_lengths[10];    // lengths of each of the sounds

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal
                                                          // 18.2 clicks/sec

sprite back_cells,       // the background cells, stored as a sprite
     local_player,       // the local player
     remote_player;      // the remote player

int local_score=0,       // score of local player
   remote_score=0,       // score of remote player
   local_missiles=100,   // number of shells the local has
   remote_missiles=100;  // number of shells the remote has

int game_state = GAME_RUNNING,   // state of game when system starts
    link_state = UNLINKED;       // at start up there is no link


int remote_hit=0, local_hit=0;  // flags for explosion engine to track who is
                                // dieing

// direction and missle velocity looks ups used to compute translations of
// tanks without using sin and cosine

int dir_look_up_x[16] = {0,1,2,2,3,2,2,1,0,-1,-2,-2,-3,-2,-2,-1};

int dir_look_up_y[16] = {-3,-2,-2,-1,0,1,2,2,3,2,2,1,0,-1,-2,-2};


// F U N C T I O N S//////////////////////////////////////////////////////////

Sound_Init()
{

// this funntion initializes the sound card and loads the sound.  if you have
// trouble with sounds on your system try changing the interruot and or the
// i/o port

Voc_Load_Driver();    // load the driver
Voc_Init_Driver();    // start it up
Voc_Set_Port(0x220);  // I/O port 220h
Voc_Set_DMA(5);       // interrupt 5

// get the version and set up the status variable

Voc_Get_Version();
Voc_Set_Status_Addr((char __far *)&ct_voice_status);

// load in the sounds for this game, note: the use of conditional compilation

sounds[SOUND_FIRE]    = Voc_Load_Sound("fire.voc" ,    &sound_lengths[SOUND_FIRE]);
sounds[SOUND_EXPLODE] = Voc_Load_Sound("explode.voc" , &sound_lengths[SOUND_EXPLODE]);
sounds[SOUND_HIT]     = Voc_Load_Sound("hit.voc" ,     &sound_lengths[SOUND_HIT]);
sounds[SOUND_CRASH]   = Voc_Load_Sound("crash.voc" ,   &sound_lengths[SOUND_CRASH]);
sounds[SOUND_FX]      = Voc_Load_Sound("fx.voc" ,      &sound_lengths[SOUND_FX]);
sounds[SOUND_LINK]    = Voc_Load_Sound("link.voc" ,    &sound_lengths[SOUND_LINK]);

#ifdef NET1
sounds[SOUND_DIE]      = Voc_Load_Sound("die1.voc" ,      &sound_lengths[SOUND_DIE]);
#endif

#ifdef NET2
sounds[SOUND_DIE]      = Voc_Load_Sound("die2.voc" ,      &sound_lengths[SOUND_DIE]);
#endif

Voc_Set_Speaker(1);

} // end Sound_Init

//////////////////////////////////////////////////////////////////////////////

Extract_Back_cells(pcx_picture_ptr image)
{

// extract the cells for the background imagery of the circuits from the loaded
// pcx file and store them in a sprite

PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,0,0,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,1,1,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,2,2,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,3,3,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,4,4,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,5,5,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,6,6,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,7,7,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,8,8,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,9,9,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,10,10,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,11,11,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,12,12,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,13,13,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,14,14,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,15,15,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,16,16,2);
PCX_Grap_Bitmap(image,(sprite_ptr)&back_cells,17,17,2);

} // Extract_Back_Cells

//////////////////////////////////////////////////////////////////////////////

#ifdef NET1

Build_Players(pcx_picture_ptr image, sprite_ptr local, sprite_ptr remote)
{

// this function builds the bitmaps of the players and initializes them

int index;

// intialize both players internal variables

Sprite_Init(local,0,0,0,0,0,0);

Sprite_Init(remote,0,0,0,0,0,0);

// now scan in all 16 frames of each players rotatation sequence form pcx file

for (index=0; index<16; index++)
    {

    // next local player bitmap
    PCX_Grap_Bitmap(image,local,index,index,0);

    // next remote player bitmap
    PCX_Grap_Bitmap(image,remote,index,index,1);

    } // end index

} // end Build_Players

//////////////////////////////////////////////////////////////////////////////


Start_Local(sprite_ptr local)
{

// sets the local player in the top left corner if NET1

local->curr_frame = 4;   // point to the right
local->x          = 16;
local->y          = 16;

} // end Start_Local

//////////////////////////////////////////////////////////////////////////////

Start_Remote(sprite_ptr remote)
{
// sets the remote player in the bottom right corner if NET1

remote->curr_frame = 12;  // point to the left
remote->x          = 286;
remote->y          = 144;

} // end Start_Remote

#endif


//////////////////////////////////////////////////////////////////////////////


#ifdef NET2

Build_Players(pcx_picture_ptr image, sprite_ptr local, sprite_ptr remote)
{
// this function builds the bitmaps of the players and initializes them

int index;

// intialize both players internal variables

Sprite_Init(local,0,0,0,0,0,0);

Sprite_Init(remote,0,0,0,0,0,0);

// now scan in all 16 frames of each players rotatation sequence form pcx file

for (index=0; index<16; index++)
    {

    // next local player bitmap
    PCX_Grap_Bitmap(image,local,index,index,1);

    // next remote player bitmap
    PCX_Grap_Bitmap(image,remote,index,index,0);

    } // end index

} // end Build_Players

//////////////////////////////////////////////////////////////////////////////

Start_Local(sprite_ptr local)
{
// sets the locallayer in the bottom right corner if NET1

local->curr_frame = 12;   // point to the right
local->x          = 286;
local->y          = 144;

} // end Start_Local

//////////////////////////////////////////////////////////////////////////////

Start_Remote(sprite_ptr remote)
{
// sets the remote player in the top left corner if NET1

remote->curr_frame = 4;  // point to the left
remote->x          = 16;
remote->y          = 16;

} // end Start_Remote

#endif

//////////////////////////////////////////////////////////////////////////////

Timer(int clicks)
{
// this function uses the internal time keeper timer i.e. the one that goes
// at 18.2 clicks/sec to to a time delay.  You can find a 322 bit value of
// this timer at 0000:046Ch

unsigned int now;

// get current time

now = *clock;

// wait till time has gone past current time plus the amount we eanted to
// wait.  Note each click is approx. 55 milliseconds.

while(abs(*clock - now) < clicks){}

} // end Timer

//////////////////////////////////////////////////////////////////////////////

Draw_Game_Grid(char **game_grid)
{

// this function draws the game grid in the double buffer, by scanning the
// the game_grid 2-d array and placing the bitmaps that are represented by
// each one of the characters in the array.  note the use of look up tables
// to translate the character in the game_grid array into the proper bitmap
// index in the sprite

char *curr_row;

int index_x, index_y, cell_number;

// translation table for screen database

static char back_cell_lookup[] =

   {17,0,10,16,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,9,6,0,
// SP  !  " #  $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?

    0,4,0,0,0,0,0,0,0,11,3,0,0,0,15,0,0,0,0,0,1,14,5,0,0,0,0,12,0,13,0,0};
//  @ A B C D E F G H I  J K L M  N O P Q R S T U  V W X Y Z [  \  ] ^ _

// now draw the game grid row by row

for (index_y = 0; index_y<CELL_ROWS; index_y++)
    {

    // get the current row for speed

    curr_row = game_grid[index_y];

    // do the row

    for (index_x = 0; index_x<CELL_COLUMNS; index_x++)
        {
        // extract cell out of data structure and blit it onto screen

        cell_number = back_cell_lookup[curr_row[index_x]-32];

        // compute the screen x and y

        back_cells.x = index_x * SPRITE_WIDTH;
        back_cells.y = index_y * SPRITE_HEIGHT;

        // figure out which bitmap to draw

        back_cells.curr_frame = cell_number;

        // draw the bitmap

        Draw_Sprite((sprite_ptr)&back_cells);

        } // end for index_x

    } // end for index_y

} // Draw_Game_Grid

/////////////////////////////////////////////////////////////////////////////

Draw_Gauges(pcx_picture_ptr image)
{
// draws the gauges into the video buffer

_fmemcpy((char far *)&video_buffer[ GAUGE_Y * 320],
         (char far *)&image->buffer[GAUGE_Y * 320],
         SCREEN_WIDTH*GAUGE_HEIGHT);

} // end Draw_Gauges

////////////////////////////////////////////////////////////////////////////

Draw_Instruments()
{
// this function draws the data on the instrument panel

char buffer[32];

// draw local player

sprintf(buffer,"Local:%d ",local_score);
Blit_String(8,180,9, buffer,0);

sprintf(buffer,"Shells %d ",local_missiles);
Blit_String(8,189,9, buffer,0);

// draw remote player

sprintf(buffer,"Remote:%d ",remote_score);
Blit_String(215,180,9, buffer,0);

sprintf(buffer,"Shells %d ",remote_missiles);
Blit_String(215,189,9, buffer,0);

// draw remote player

} // end Draw_Instruments

//////////////////////////////////////////////////////////////////////////////

int Get_Remote_Input()
{
// this function just does a serial read, no real need for it

int ch;

ch = Serial_Read();

return(ch);

} // end Get_Remote_Input

/////////////////////////////////////////////////////////////////////////////

Erase_Missiles()
{

// this function indexes through all the missiles and if they are active
// erases them by replacing the background color that was under them

int index;

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       Plot_Pixel_Fast_D(missiles[index].x,missiles[index].y,missiles[index].back);

       } // end if alive

    } // end for index

} // end Erase_Missiles

/////////////////////////////////////////////////////////////////////////////

Behind_Missiles()
{

// this function indexes through all the missiles and if they are active
// scans the background color that is behind them so it can be replaced later

int index;

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       missiles[index].back = Get_Pixel_D(missiles[index].x,missiles[index].y);

       } // end if alive

    } // end for index

} // end Behind_Missiles

/////////////////////////////////////////////////////////////////////////////

Draw_Missiles()
{

// this function indexes through all the missiles and if they are active
// draws the missile as a bright white pixel on the screen

int index;

for (index=0; index<NUM_MISSILES; index++)
    {

    // is this missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       Plot_Pixel_Fast_D(missiles[index].x,missiles[index].y,missiles[index].color);

       } // end if alive

    } // end for index

} // end Draw_Missiles

/////////////////////////////////////////////////////////////////////////////


Blink_Lights()
{

// this function is cool, it does a small color rotatiob on palette registers
// 3,11,14 so if 3, 11, and 14 happen to be next to each other it will look
// like little blinking lights.  Note how this function is completely self
// sustaining, it has it's own static locals and clock and it knows that the
// first time it is called it has to initialize it self, but the caller doesnt
// need to know this, he can just call it blindly and it works all the time

static int clock=0,entered_yet=0;// used for timing, note: they are static!
RGB_color color,color_1,color_2,color_3;

// this function blinks the little circuitry lights

if (!entered_yet)
   {

   // reset the palette registers 3,11,14, they are used for lights to red,
   // black, black

   color.red   = 255;
   color.green = color.blue = 0;

   Set_Palette_Register(3,(RGB_color_ptr)&color);

   color.red = color.green = color.blue = 0;

   Set_Palette_Register(11,(RGB_color_ptr)&color);
   Set_Palette_Register(14,(RGB_color_ptr)&color);

   // system has initialized, so flag it

   entered_yet=1;

   } // end if first time into function

// try and rotate the light colors i.e. color rotation

   if (++clock==6)  // is it time to rotate
      {
      // get the colors

      Get_Palette_Register(3,(RGB_color_ptr)&color_1);
      Get_Palette_Register(3,(RGB_color_ptr)&color_1);
      Get_Palette_Register(11,(RGB_color_ptr)&color_2);
      Get_Palette_Register(11,(RGB_color_ptr)&color_2);
      Get_Palette_Register(14,(RGB_color_ptr)&color_3);
      Get_Palette_Register(14,(RGB_color_ptr)&color_3);

      // set the colors

      Set_Palette_Register(11,(RGB_color_ptr)&color_1);
      Set_Palette_Register(14,(RGB_color_ptr)&color_2);
      Set_Palette_Register(3,(RGB_color_ptr)&color_3);

      // reset the clock

      clock=0;

      } // end if time to rotate

} // end Blink_Lights

/////////////////////////////////////////////////////////////////////////////

Fade_Lights()
{
int index,pal_reg;
RGB_color color,color_1,color_2,color_3;

for (index=0; index<30; index++)
    {

    for (pal_reg=1; pal_reg<33; pal_reg++)
        {
        // get the color to fade

        Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);
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

/////////////////////////////////////////////////////////////////////////////

Move_Missiles()
{
// this function scans through all the missiles and moves them if they are
// active, then the function does all the collision detection with the
// missiles.  it tests if the have hit a wall or if they have hit the other
// player, if they have then a flag is set so that an explosion will occur
// at the end of the cycle when the background can be corrupted

int index;
int miss_x,miss_y;      // speed aliases
int loc_x,loc_y, rem_x,rem_y;

for (index=0; index<NUM_MISSILES; index++)
    {

    // is missile active

    if (missiles[index].state == MISS_ALIVE)
       {

       // move the missile

       miss_x = (missiles[index].x += missiles[index].xv);
       miss_y = (missiles[index].y += missiles[index].yv);

       // first test if missile has hit either tank (only if linked)

       if (link_state == LINKED)
          {
          if (missiles[index].tag==LOCAL_MISSILE)
             {

             // alias vars for speed

             rem_x = remote_player.x;
             rem_y = remote_player.y;

             // test if missile is within bounding box of tank

             if ( (miss_x > rem_x + 2) &&
                  (miss_x < rem_x + 14) &&
                  (miss_y > rem_y + 2) &&
                  (miss_y < rem_y + 14) )
                  {

                  // send message
                  remote_hit=1;

                  } // end if local hit remote

             }
          else
             {

             // alias vars for speed

             loc_x = local_player.x;
             loc_y = local_player.y;

             // test if missile is within bounding box of tank

             if ( (miss_x > loc_x + 2) &&
                  (miss_x < loc_x + 14) &&
                  (miss_y > loc_y + 2) &&
                  (miss_y < loc_y + 14) )
                  {
                  local_hit=1;

                  } // end if remote hit local

             } // end else

          } // end if linked

       // test if it's hit the edge of the screen or a wall

       if ( (miss_x >= SCREEN_WIDTH) || (miss_x <= 0) ||
           (miss_y > (SCREEN_HEIGHT-25)) ||( miss_y <=0) )
          {
          missiles[index].state = MISS_DEAD;

          } // end if off edge of screen

       // now test if a piece of background was hit

       if (game_grid[(miss_y >> 4)][(miss_x >> 4)] !=' ')
          missiles[index].state = MISS_DEAD;

       } // end if alive

    } // end for index

} // end Move_Missiles

/////////////////////////////////////////////////////////////////////////////

Start_Missile(sprite_ptr who, int tag)
{

// this function scans through the missile array and tries to find one that
// isn't being used.  this function could be more efficient.

int index;

// scan for a useable missle

for (index=0; index<NUM_MISSILES; index++)
    {
    // is this missile free?

    if (missiles[index].state == MISS_DEAD)
       {

       // set up fields

       missiles[index].state = MISS_ALIVE;
       missiles[index].x     = who->x+8;
       missiles[index].y     = who->y+8;
       missiles[index].xv    = (dir_look_up_x[who->curr_frame]*3);
       missiles[index].yv    = (dir_look_up_y[who->curr_frame]*3);
       missiles[index].color = 15;
       missiles[index].back  = Get_Pixel_D(who->x,who->y);
       missiles[index].tag   = tag;

       // make a fire sound

       Voc_Stop_Sound();
       Voc_Play_Sound(sounds[SOUND_FIRE] , sound_lengths[SOUND_FIRE]);

       break; // exit loop

       } // end if found a good one

    } // end for index

} // end Start_Missile

/////////////////////////////////////////////////////////////////////////////

Init_Missiles()
{
// this function just makes sure all the "state" fields of the missiles are
// dead so that we don't get any strays on start up.  Remember never assume
// that variables are zeroed on instantiation!

int index;

for (index=0; index<NUM_MISSILES; index++)
    missiles[index].state = MISS_DEAD;

} // Init_Missiles

////////////////////////////////////////////////////////////////////////////

Link_Up()
{
// this function is kinds neat, it tries to see if there is a nother player
// linked to the serial port.  it work as follows: 1 - is the remote sending
// and 'l', if so then send and 'l' and we are linked up.  If not then send an
// 'l' and try again in .5 secs.  do this for a few seconds if no response then
// go solo

char ch;
int tries=0,done=0;

// main link up loop do it until we have a link or we've tried 10 times

Blit_String(128,180,10,"Trying..",0);
Voc_Stop_Sound();

while(!done & tries++<10)
     {
     // tell other we want to link

     Serial_Write('l');

     // see if he is trying to link

     ch = Serial_Read();

     if (ch=='l')
        {
        // we have a link, let's bail

        done=1;
        link_state = LINKED;

        } // end if

     // wait 1 sec and try again

     Timer(20);

     } // end while

// test link state

if (link_state==LINKED)
   {

   // send message to display

   Blit_String(128,180,10,"Linked  ",0);
   Voc_Stop_Sound();
   Voc_Play_Sound(sounds[SOUND_LINK] , sound_lengths[SOUND_LINK]);
   }
else
   {
   Blit_String(128,180,10," Solo   ",0);
   } // end else no link

} // end Link_Up

/////////////////////////////////////////////////////////////////////////////

Explosion(int x,int y)
{
unsigned int now;
int index;

// does a non-interactive explosion i.e. the game stops and the explosion
// executes without letting the players move.
// create a bunch of little particles with random velocities and colors

for (index=0; index<NUM_PARTICLES; index++)
    {
    shrap[index].x      = x + -8  + rand() % 16;
    shrap[index].y      = y + -8  + rand() % 16;
    shrap[index].xv     = -10 + rand() % 20;
    shrap[index].yv     = -10 + rand() % 20;
    shrap[index].color  = rand() % 256;
    shrap[index].back   = Get_Pixel(shrap[index].x,shrap[index].y);
    shrap[index].state  = 1;

    } // end for index

// make explosion sound

Voc_Stop_Sound();
Voc_Play_Sound(sounds[SOUND_EXPLODE] , sound_lengths[SOUND_EXPLODE]);

// do explosion for 3 seconds

// get current time

now = *clock;

while(abs(*clock - now) < 50)
     {

     // replace all the backgrounds and move

     for (index=0; index<NUM_PARTICLES; index++)
         {
         // replace background

         if (shrap[index].state==1)
            {

            Plot_Pixel_Fast(shrap[index].x,shrap[index].y,shrap[index].back);

            // move particles if off screen then kill

            if ( (shrap[index].x+=shrap[index].xv) >= SCREEN_WIDTH ||
                 (shrap[index].x<0) )
                 shrap[index].state = 0;

            if ( ((shrap[index].y+=shrap[index].yv) >= SCREEN_HEIGHT - 25) ||
                 (shrap[index].y<0) )
                 shrap[index].state = 0;

            } // end if this particle is alive

         } // end for index

     // get all the backgrounds

     for (index=0; index<NUM_PARTICLES; index++)
         {
         // scan background

         if (shrap[index].state==1)
            {

            shrap[index].back = Get_Pixel(shrap[index].x,shrap[index].y);

            } // end if this particle is alive

         } // end for index


     // draw all the paritcles

     for (index=0; index<NUM_PARTICLES; index++)
         {
         // plot particle background

         if (shrap[index].state==1)
            {

            Plot_Pixel_Fast(shrap[index].x,shrap[index].y,shrap[index].color);

            } // end if this particle is alive

         } // end for index

     Delay(1000);

     } // end while

// restore

// replace all the backgrounds and move

for (index=0; index<NUM_PARTICLES; index++)
    {
    // replace background

    if (shrap[index].state==1)
       {

       Plot_Pixel_Fast(shrap[index].x,shrap[index].y,shrap[index].back);

       } // end if this particle is alive

    } // end for index

} // end Explosion

/////////////////////////////////////////////////////////////////////////////

Do_Explosion()
{
// controls an explosion be listening for a "hit" message from the "move" tanks
// function

if (local_hit)
   {
   // blow up the local

   Explosion(local_player.x + 8, local_player.y + 8);
   remote_score+=100;

   } // end if local was hit
else
   {
   // blow up the remote

   Explosion(remote_player.x + 8, remote_player.y + 8);
   local_score+=100;

   } // end else remote was hit

// clean up playfield

if (link_state==LINKED)
    {
    Erase_Sprite((sprite_ptr)&local_player);
    Erase_Sprite((sprite_ptr)&remote_player);
    Erase_Missiles();
    Start_Local((sprite_ptr)&local_player);
    Start_Remote((sprite_ptr)&remote_player);
    Behind_Sprite((sprite_ptr)&local_player);
    Behind_Sprite((sprite_ptr)&remote_player);

    }
else
   {
   Erase_Sprite((sprite_ptr)&local_player);
   Erase_Missiles();
   Start_Local((sprite_ptr)&local_player);
   Behind_Sprite((sprite_ptr)&local_player);
   }

// reset hits

local_hit = remote_hit = 0;

// reset missiles

Init_Missiles();

} // end Do_Explosion


// M A I N ///////////////////////////////////////////////////////////////////

main()
{
int index,
    done=0,
    local_key,
    remote_key,
    input_clock=0;

    pcx_picture tank_cells,
                intro;

int cell_x,
    cell_y,
    dx=0,
    dy=0;

// load some sounds

Sound_Init();

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// open comm port 1 at 1200 baud N81 9600 will work fine, but I've been
// string 100 ft of cable to play at my next store neihbors house and 9600
// flakes at that length

Open_Serial(COM_1,SER_BAUD_1200,SER_PARITY_NONE | SER_BITS_8 | SER_STOP_1);

// intialize the introduction screen

PCX_Init((pcx_picture_ptr)&intro);

// load the intro

PCX_Load("netintro.pcx", (pcx_picture_ptr)&intro,1);

// show the user the intro

PCX_Show_Buffer((pcx_picture_ptr)&intro);

// let user go ohhhh ahhhh

Timer(50);

// delete the intro screen from memory

PCX_Delete((pcx_picture_ptr)&intro);

// initialize the pcx file that holds all the animation cells for net-tank

PCX_Init((pcx_picture_ptr)&tank_cells);

// load the pcx file that holds the cells

PCX_Load("netimage.pcx", (pcx_picture_ptr)&tank_cells,1);

// extract the playground cells from image

Extract_Back_Cells((pcx_picture_ptr)&tank_cells);

// create some memory for the double buffer

Init_Double_Buffer();

// clear out the missiles and get them ready for lanch!

Init_Missiles();

// build the players animation cells up for the rotation cycle

Build_Players((pcx_picture_ptr)&tank_cells,
              (sprite_ptr)&local_player,
              (sprite_ptr)&remote_player);


// set the game grid to level 1

game_grid = (char **)game1_grid;

// draw the game grid in the double buffer

Draw_Game_Grid((char **)game_grid);

// draw the gauges to the video screen (only do once)

Draw_Gauges((pcx_picture_ptr)&tank_cells);

// draw the text for the gauges

Draw_Instruments();

// try and link up

Link_Up();

// main game loop
// explaination

// 1 - erase all imagery
// 2 - get input from local and remote and send local's input to remote
// 3 - move players and do collision detection
// 4 - draw players
// 5 goto 1

// look pretty easy!

while(!done)
     {

     // initialize players depending on status of linked play or solo

     if (link_state==LINKED)
         {
         Start_Local((sprite_ptr)&local_player);
         Start_Remote((sprite_ptr)&remote_player);
         Behind_Sprite((sprite_ptr)&local_player);
         Behind_Sprite((sprite_ptr)&remote_player);

         }
     else
        {
        Start_Local((sprite_ptr)&local_player);
        Behind_Sprite((sprite_ptr)&local_player);
        }



     // inner game loop

     while(game_state==GAME_RUNNING)
          {

          // erase old tanks

          if (link_state==LINKED)
             {
             Erase_Sprite((sprite_ptr)&local_player);
             Erase_Sprite((sprite_ptr)&remote_player);
             }
          else
             {
             Erase_Sprite((sprite_ptr)&local_player);
             }

          // erase missiles

          Erase_Missiles();

          // get input if time to

          if (++input_clock==2)
          {
             input_clock=0;  // reset the input clock, just used to slow
                             // things down

          // get the input from local

          if (local_key=Get_Scan_Code())
             {

             // what is local player trying to do

             switch(local_key)
                   {

                   case SCAN_UP: // going foward
                        {
                        // use look up tables to compute translation factors

                        dx=dir_look_up_x[local_player.curr_frame];
                        dy=dir_look_up_y[local_player.curr_frame];

                        } break;

                   case SCAN_DOWN: // backing up
                        {
                        // use look up tables to compute translation factors
                        dx=-dir_look_up_x[local_player.curr_frame];
                        dy=-dir_look_up_y[local_player.curr_frame];

                        } break;

                   case SCAN_RIGHT: // tryin to rotate right
                        {
                        // change frames

                        if (++local_player.curr_frame > 15)
                            local_player.curr_frame = 0;

                        } break;

                   case SCAN_LEFT:  // trying to rotate left
                        {
                        // back up one frame

                        if (--local_player.curr_frame < 0)
                            local_player.curr_frame = 15;

                        } break;

                   case SCAN_E: // cheater key, but I took it out, I had it
                        {       // so it would send a meesage that would blow
                                // up the other player!!!!!

                        } break;


                   case SCAN_SPACE: // fire a missile
                        {

                        // if the local has a missile to fire then try and
                        // fire

                        if (local_missiles>0)
                           {
                           Start_Missile((sprite_ptr)&local_player, LOCAL_MISSILE);
                           local_missiles--;
                           } // end if any missiles left
                        else
                           local_key = 0;

                        } break;

                   case SCAN_T: // send a nasty audio-gram to other player
                        {
                        // talk to other player

                        Serial_Write('t');

                        } break;

                   case SCAN_ESC: // trying to escape...fine then go!!!!!
                        {

                        game_state = GAME_STOPPED;
                        done=1;

                        } break;

                   default: break;

                   } // end switch local input

             // move player

             local_player.x+=dx;
             local_player.y+=dy;

             // do collision detection with background

             // compute if tank body is within a cell if so moveback

             // take into consideration the direction of movement to make the
             // collision a little more realistic

             if (dx>0)
                 cell_x = ((local_player.x+10) >> 4);
             else
                 cell_x = ((local_player.x+4) >> 4);

             if (dy>0)
                cell_y = ((local_player.y+10) >> 4);
             else
                cell_y = ((local_player.y+4) >> 4);

             // have we tried to merge with solid matter?

             if (game_grid[cell_y][cell_x]!=' ')
                {
                local_player.x-=dx;
                local_player.y-=dy;

                // play a crash sound

                Voc_Stop_Sound();
                Voc_Play_Sound(sounds[SOUND_CRASH] , sound_lengths[SOUND_CRASH]);

                } // end if a background collision has occured

             // reset translation vars

             dx=dy=0;

               // send data to remote

             if (link_state==LINKED)
                    Serial_Write((char)local_key);

             } // end if player moved

          } // end if movement timer expired

          // no process the remote tank, exact same as above

          if (link_state==LINKED)
          {
          // now move remote tank

          if (remote_key=Get_Remote_Input())
             {

             // what key did remote hit

             dx=dy=0;

             switch(remote_key)
                   {

                   case SCAN_UP: // going foward
                        {
                        dx=dir_look_up_x[remote_player.curr_frame];
                        dy=dir_look_up_y[remote_player.curr_frame];

                        } break;

                   case SCAN_DOWN: // backing up
                        {
                        dx=-dir_look_up_x[remote_player.curr_frame];
                        dy=-dir_look_up_y[remote_player.curr_frame];

                        } break;

                   case SCAN_RIGHT: // rotating right
                        {
                        if (++remote_player.curr_frame > 15)
                            remote_player.curr_frame = 0;

                        } break;

                   case SCAN_LEFT: // rotating left
                        {

                        if (--remote_player.curr_frame < 0)
                            remote_player.curr_frame = 15;

                        } break;

                   case SCAN_SPACE: // remote missile fire
                        {

                        Start_Missile((sprite_ptr)&remote_player, REMOTE_MISSILE);
                        remote_missiles--;

                        } break;

                   case SCAN_T: // other player is talking
                        {

                        Voc_Stop_Sound();
                        Voc_Play_Sound(sounds[SOUND_DIE] , sound_lengths[SOUND_DIE]);

                        } break;

                   case SCAN_ESC: // trying to bail
                        {

                        game_state = GAME_STOPPED;
                        done=1;

                        } break;

                   default: break;

                   } // end switch remote input

             // move player

             remote_player.x+=dx;
             remote_player.y+=dy;

             // do collision detection with background

             // compute if tank body is within a cell if so moveback

             // take into consideration the direction of movement to make the
             // collision a little more realistic

             if (dx>0)
                 cell_x = ((remote_player.x+10) >> 4);
             else
                 cell_x = ((remote_player.x+4) >> 4);

             if (dy>0)
                cell_y = ((remote_player.y+10) >> 4);
             else
                cell_y = ((remote_player.y+4) >> 4);

             // have we tried to merge with solid matter?

             if (game_grid[cell_y][cell_x]!=' ')
                {
                remote_player.x-=dx;
                remote_player.y-=dy;

                Voc_Stop_Sound();
                Voc_Play_Sound(sounds[SOUND_CRASH] , sound_lengths[SOUND_CRASH]);

                } // end if a background collision has occured

             // reset translation vars

             dx=dy=0;

             } // end if player moved

          } // end if linked

          // now that we have erased and moved both players let's deal with
          // missiles and everything else

          // move missiles

          Move_Missiles();

          // draw tanks in rendering buffer

          if (link_state==LINKED)
             {
             Behind_Missiles();
             Behind_Sprite((sprite_ptr)&local_player);
             Behind_Sprite((sprite_ptr)&remote_player);

             if (!local_hit)
                Draw_Sprite((sprite_ptr)&local_player);

             if (!remote_hit)
                Draw_Sprite((sprite_ptr)&remote_player);

             Draw_Missiles();
             }
          else
             {
             Behind_Missiles();
             Behind_Sprite((sprite_ptr)&local_player);
             Draw_Sprite((sprite_ptr)&local_player);
             Draw_Missiles();
             }

          // copy double buffer to screen

          Show_Double_Buffer(double_buffer);

          // update the status of the instruments

          Draw_Instruments();

          // blink the lights on the background cells

          Blink_Lights();

          // test if a hit has occured i.e. a message was sent, note: this is
          // the only place we can start an explosion, otherwise all kinds
          // of rendering problems can occur

          if (local_hit || remote_hit)
             {

             Do_Explosion();

             } // end if either hit

          } // end while game_state

     } // end main game loop

// exit nicely

Fade_Lights();

// close the connection and blaze

Close_Serial(COM_1);

// shut down the sound

Voc_Set_Speaker(0);

Voc_Unload_Sound(sounds[SOUND_FIRE]);
Voc_Unload_Sound(sounds[SOUND_EXPLODE]);
Voc_Unload_Sound(sounds[SOUND_HIT]);
Voc_Unload_Sound(sounds[SOUND_CRASH]);
Voc_Unload_Sound(sounds[SOUND_FX]);
Voc_Unload_Sound(sounds[SOUND_DIE]);
Voc_Unload_Sound(sounds[SOUND_LINK]);

// kill the sound driver

Voc_Terminate_Driver();

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&tank_cells);

// go back to text mode

Set_Mode(TEXT_MODE);

printf("\nWimp...\n");

} // end main



