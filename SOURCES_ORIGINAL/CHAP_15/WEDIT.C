
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

#include "graph0.h"    // include our graphics library
#include "mouselib.h"  // include our mouse library

// D E F I N E S /////////////////////////////////////////////////////////////

#define YES                0
#define NO                 1

// the following defines deliniate the codes that will represent each type
// of object in the world, they will never change, so that the database
// can be imported to other software will a better GUI etc.

#define WALLS_START        64
#define NUM_WALLS          24

#define DOORS_START        128
#define NUM_DOORS          4

#define SCROLLS_START      144
#define NUM_SCROLLS        4

#define POTIONS_START      160
#define NUM_POTIONS        4

#define FOODS_START        176
#define NUM_FOODS          2

#define MONSTERS_START     192
#define NUM_MONSTERS       2

#define WALL_STONE_1      (WALLS_START+0)   // for now only 6
#define WALL_STONE_2      (WALLS_START+1)
#define WALL_STONE_3      (WALLS_START+2)
#define WALL_STONE_4      (WALLS_START+3)
#define WALL_STONE_5      (WALLS_START+4)
#define WALL_STONE_6      (WALLS_START+5)
#define NUM_STONE_WALLS   6

#define WALL_MELT_1       (WALLS_START+6)   // for now only 6
#define WALL_MELT_2       (WALLS_START+7)
#define WALL_MELT_3       (WALLS_START+8)
#define WALL_MELT_4       (WALLS_START+9)
#define WALL_MELT_5       (WALLS_START+10)
#define WALL_MELT_6       (WALLS_START+11)
#define NUM_MELT_WALLS    6


#define WALL_OOZ_1        (WALLS_START+12)  // for now only 6
#define WALL_OOZ_2        (WALLS_START+13)
#define WALL_OOZ_3        (WALLS_START+14)
#define WALL_OOZ_4        (WALLS_START+15)
#define WALL_OOZ_5        (WALLS_START+16)
#define WALL_OOZ_6        (WALLS_START+17)
#define NUM_OOZ_WALLS     6


#define WALL_ICE_1        (WALLS_START+18)  // for now only 6
#define WALL_ICE_2        (WALLS_START+19)
#define WALL_ICE_3        (WALLS_START+20)
#define WALL_ICE_4        (WALLS_START+21)
#define WALL_ICE_5        (WALLS_START+22)
#define WALL_ICE_6        (WALLS_START+23)
#define NUM_ICE_WALLS     6

#define DOORS_1            (DOORS_START+0)  // for now only 4
#define DOORS_2            (DOORS_START+1)
#define DOORS_3            (DOORS_START+2)
#define DOORS_4            (DOORS_START+3)

#define SCROLLS_1         (SCROLLS_START+0) // for now only 4
#define SCROLLS_2         (SCROLLS_START+1)
#define SCROLLS_3         (SCROLLS_START+2)
#define SCROLLS_4         (SCROLLS_START+3)

#define POTIONS_1         (POTIONS_START+0) // for now only 4
#define POTIONS_2         (POTIONS_START+1)
#define POTIONS_3         (POTIONS_START+2)
#define POTIONS_4         (POTIONS_START+3)

#define FOODS_1           (FOODS_START+0)   // for now only 2
#define FOODS_2           (FOODS_START+1)

#define MONSTERS_1        (MONSTERS_START+0) // for now only 2
#define MONSTERS_2        (MONSTERS_START+1)

#define GAME_START        255  // this is where the player starts the game


#define COMM_ERASER            0    // these are the commands that the user
#define COMM_LOAD              0    // can press
#define COMM_SAVE              1
#define COMM_CLEAR             2
#define COMM_EXIT              3

#define COMM_YES               1
#define COMM_NO                0

// icon positions, dimensions etc.

#define ICONS_DX                10
#define ICONS_DY                8
#define ICONS_WIDTH             6
#define ICONS_HEIGHT            6

#define BUTTONS_DY              17
#define BUTTONS_WIDTH           29
#define BUTTONS_HEIGHT          12

#define WALLS_XO               225
#define WALLS_YO               73

#define DOORS_XO               225
#define DOORS_YO               110

#define SCROLLS_XO             225
#define SCROLLS_YO             128

#define POTIONS_XO             225
#define POTIONS_YO             146

#define FOODS_XO               225
#define FOODS_YO               164

#define MONSTERS_XO            225
#define MONSTERS_YO            182

#define ERASER_XO              300
#define ERASER_YO              182

#define BUTTONS_XO             285
#define BUTTONS_YO             2

#define TEXT_XO                185
#define TEXT_YO                52

#define DIALOG_XO              110
#define DIALOG_YO              70


// window positions and dimensions

#define DIALOG_YES_XO          11
#define DIALOG_YES_YO          24

#define DIALOG_DX              56
#define DIALOG_WIDTH           28
#define DIALOG_HEIGHT          15



#define PREVIEW_XO             237
#define PREVIEW_YO             1
#define PREVIEW_WIDTH          40
#define PREVIEW_HEIGHT         40

#define ZOOM_XO                185
#define ZOOM_YO                1
#define ZOOM_WIDTH             48
#define ZOOM_HEIGHT            48

#define MAP_XO                 1
#define MAP_YO                 1
#define MAP_WIDTH              (unsigned int)180
#define MAP_HEIGHT             (unsigned int)198



// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal
                                                          // 18.2 clicks/sec

// sprites ar used to hold all the preview textures

sprite walls_stone, // the stone textures
       walls_melt,  // the red fire textures
       walls_ooz,   // the green ooz
       walls_icy,   // the ice
       doors,
       scrolls,
       potions,
       foods,
       monsters,
       dialogs;     // holds dialog box bitmaps so that they do not have to
                    // to be drawn

pcx_picture edit_view,  // the pcx file that holds the screen GUI for the map
             textures;  // used to load the preview textures

unsigned char far *data_table;  // the database storage area

unsigned char block_color=0
             ,block_value=0;   // the current active icon

char file_name[14];            // used for load and save file



// this table is a cross reference that uses the database value to index
// into the table to use the proper color on the map editor. since colors
// represent each icon this is an easy way to have any color represented
// by a specific icon value

unsigned char value_to_color[256] =

{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0-15
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 16-31
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 32-47
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 48-63
  16, 18, 20, 22, 24, 26, 32, 34, 36, 38, 40, 42, 96, 98,100,102,   // 64-79
 104,106,144,146,148,150,152,154,  0,  0,  0,  0,  0,  0,  0,  0,   // 80-95
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 96-111
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 112-127
 183,185,187,189,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 128-143
 165,167,169,171,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 144-159
 120,122,124,126,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 160-175
 208,210,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 176-191
  71, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 192-207
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 208-223
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 224-239
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  15,}; // 240-255

// F U N C T I O N S //////////////////////////////////////////////////////////

char *Get_Line(char *buffer)
{
// this function implements a crude line editor, it's used to get file names

int c,index=0;

while((c=getch())!=13)
     {

     if (c==8 && index>0)
        {

        buffer[--index] = ' ';
        printf("%c %c",8,8);

        }
     else
     if (c>=48 && c<=122 && index<8)
        {
        buffer[index++] = c;
        printf("%c",c);

        }

     } // end while

buffer[index] = 0;

return(buffer);

} // end Get_Line

//////////////////////////////////////////////////////////////////////////////

Timer(int clicks)
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

Release_Mouse()
{
// waits for user to release mouse

int x,y,buttons;

Squeeze_Mouse(MOUSE_BUTT_POS,&x,&y,&buttons);

while(buttons)
     {
     Squeeze_Mouse(MOUSE_BUTT_POS,&x,&y,&buttons);
     } // end while

} // end Release_Mouse

/////////////////////////////////////////////////////////////////////////////

int Icon_Hit(int xo, int yo, int dx, int dy,
             int width, int height,
             int num_columns, int num_rows,
             int mx, int my)
{
// given the geometry of the set of buttons or icons, this function will
// compute which one the mouse has clicked on.

int row, column, xs,ys,xe,ye;


for (row=0; row<num_rows; row++)
    {

    // compute starting and ending y of current row

    ys = row*dy + yo;
    ye = ys+height;

    for (column=0; column<num_columns; column++)
        {

        xs = column*dx + xo;
        xe = xs+width;

        // test if mouse pointer is within bonding box of current icon

        if (mx>xs && mx<xe && my>ys && my<ye)
           {

           return(column + row*num_columns);

           } // end if a hit

        } // end for column scan

    } // end for row scan

return(-1); // no hit

} // end Icon_Hit

//////////////////////////////////////////////////////////////////////////////

Load_Textures()
{

// this function load all the textures for the preview box

// set sprite system size so that functions use correct sprite size

sprite_width = 64;
sprite_height = 64;

// initialize the pcx file that holds the stone walls

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the stone walls

PCX_Load("stones.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&walls_stone,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_stone,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_stone,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_stone,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_stone,3,3,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_stone,4,0,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_stone,5,1,1);

PCX_Delete((pcx_picture_ptr)&textures);


// initialize the pcx file that holds the ooz walls

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the ooz walls

PCX_Load("ooz.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&walls_ooz,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_ooz,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_ooz,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_ooz,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_ooz,3,3,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_ooz,4,0,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_ooz,5,1,1);

PCX_Delete((pcx_picture_ptr)&textures);


// initialize the pcx file that holds the melt walls

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds melt walls

PCX_Load("bricks.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&walls_melt,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_melt,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_melt,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_melt,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_melt,3,3,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_melt,4,0,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_melt,5,1,1);

PCX_Delete((pcx_picture_ptr)&textures);


// initialize the pcx file that holds the icy walls

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the icy walls

PCX_Load("icy.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&walls_icy,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_icy,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_icy,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_icy,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_icy,3,3,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_icy,4,0,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&walls_icy,5,1,1);

PCX_Delete((pcx_picture_ptr)&textures);


// initialize the pcx file that holds the doors

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the doors

PCX_Load("doors.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&doors,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&doors,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&doors,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&doors,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&doors,3,3,0);

PCX_Delete((pcx_picture_ptr)&textures);

// set sprite system size so that functions use correct sprite size

sprite_width = 16;
sprite_height = 16;


// initialize the pcx file that hold the scrolls

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the scrolls

PCX_Load("scrolls.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&scrolls,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&scrolls,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&scrolls,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&scrolls,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&scrolls,3,3,0);

PCX_Delete((pcx_picture_ptr)&textures);


// initialize the pcx file that hold the potions

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that hold the potions

PCX_Load("potions.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);


Sprite_Init((sprite_ptr)&potions,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&potions,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&potions,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&potions,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&potions,3,3,0);

PCX_Delete((pcx_picture_ptr)&textures);


sprite_width = 32;
sprite_height = 16;

// initialize the pcx file that hold the foods

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the foods

PCX_Load("foods.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&foods,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&foods,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&foods,1,1,0);

PCX_Delete((pcx_picture_ptr)&textures);

sprite_width = 48;
sprite_height = 48;

// initialize the pcx file that hold the monsters

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the monsters

PCX_Load("monsters.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&monsters,PREVIEW_XO,PREVIEW_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&monsters,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&monsters,1,1,0);


PCX_Delete((pcx_picture_ptr)&textures);



sprite_width = 108;
sprite_height = 44;

// initialize the pcx file that hold the monsters

PCX_Init((pcx_picture_ptr)&textures);

// load the pcx file that holds the monsters

PCX_Load("yesno.pcx", (pcx_picture_ptr)&textures,1);

PCX_Show_Buffer((pcx_picture_ptr)&textures);

Sprite_Init((sprite_ptr)&dialogs,DIALOG_XO,DIALOG_YO,0,0,0,0);

PCX_Grap_Bitmap((pcx_picture_ptr)&textures,(sprite_ptr)&dialogs,0,0,0);


PCX_Delete((pcx_picture_ptr)&textures);

} // end Load_Textures

//////////////////////////////////////////////////////////////////////////////

Clear_Preview()
{

// this function clears the preview window

int y;
unsigned int offset;

// this function clear the preview window


for (y=0; y<PREVIEW_HEIGHT; y++)
    {
    offset = (((PREVIEW_YO+y)<<8) + ((PREVIEW_YO+y)<<6));
    _fmemset((char far *)&video_buffer[PREVIEW_XO + offset], 0, PREVIEW_WIDTH);
    } // end for y

} // end Clear_Preview

//////////////////////////////////////////////////////////////////////////////

Clear_Zoom()
{

// this function clears the zoom window

int y;
unsigned int offset;

for (y=0; y<ZOOM_HEIGHT; y++)
    {
    offset = (((ZOOM_YO+y)<<8) + ((ZOOM_YO+y)<<6));
    _fmemset((char far *)&video_buffer[ZOOM_XO + offset], 0, ZOOM_WIDTH);
    } // end for y

} // end Clear_Zoom

///////////////////////////////////////////////////////////////////////////////

Scale_Sprite(sprite_ptr sprite,float scale)
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
             video_buffer[offset+x] = data;

        x_scale_index+=(x_scale_step);

        } // end for x

    // using the floating scale_step, index to next source pixel

    y_scale_index+=y_scale_step;

    // move to next line in video buffer and in sprite bitmap buffer

    offset      += SCREEN_WIDTH;
    work_offset = sprite_width*(int)(y_scale_index);

    } // end for y

} // end Scale_Sprite

//////////////////////////////////////////////////////////////////////////////

Save_File()
{
// this function saves the current map to a file

FILE *fp, *fopen();

unsigned int index;

_settextposition(8,25);
printf("S:");

Get_Line(file_name);

_settextposition(8,25);

printf("          ");

// open file

fp=fopen(file_name,"wb");

// save data

for (index=0; index<MAP_WIDTH*MAP_HEIGHT; index++)

putc(data_table[index],fp);

// close file

fclose(fp);

} // end Save_File

//////////////////////////////////////////////////////////////////////////////

int Load_File()
{

// this function loads a file

// this function loades a file and displays it

FILE *fp, *fopen();

unsigned int index,x,y;

_settextposition(8,25);
printf("L:");

Get_Line(file_name);

_settextposition(8,25);

printf("          ");

// open file

if (!(fp=fopen(file_name,"rb")) )
   {

   // couldn't find file

   return(0);

   } // end if file opened


// load data

for (index=0; index<MAP_WIDTH*MAP_HEIGHT; index++)
    data_table[index] = getc(fp);

// close file

fclose(fp);

// display map on screen

for (y=0; y<MAP_HEIGHT; y++)
    {
    for (x=0; x<MAP_WIDTH; x++)
        {
        Plot_Pixel_Fast(x+MAP_XO, y+MAP_YO,
                        value_to_color[data_table[x+y*MAP_WIDTH] ]);

        } // end for x

    } // end for y


} // end Load_File

//////////////////////////////////////////////////////////////////////////////

Clear()
{
// this functions clears the screen, data structures etc.

unsigned int index,x,y;

// clear the data structure

for (index=0; index<=MAP_WIDTH*MAP_HEIGHT; index++)
    data_table[index]=0;

// clear the screen

for (y=0; y<MAP_HEIGHT; y++)
    {
    for (x=0; x<MAP_WIDTH; x++)
        {
        Plot_Pixel_Fast(x+MAP_XO,y+MAP_YO,0);
        } // end for x
    } // end for y

// clear the preview window

Clear_Preview();

// clear the zoom window

Clear_Zoom();

// show the mouse

Squeeze_Mouse(MOUSE_SHOW,NULL,NULL,NULL);

} // end Clear

//////////////////////////////////////////////////////////////////////////////


int Yes_No()
{
// displays a yes no box and returns answer

int gadget,mx,my,no_answer=1,buttons;

// put up dialog box

sprite_width = 108;
sprite_height = 44;


Behind_Sprite_VB((sprite_ptr)&dialogs);
Draw_Sprite_VB((sprite_ptr)&dialogs);

// wait for user to answer

while(no_answer)
     {

     // get the mouse

     Squeeze_Mouse(MOUSE_BUTT_POS,&mx,&my,&buttons);

     mx=(mx>>1) + 1;
     my-=1;

     // has button been depressed

     if (buttons==MOUSE_LEFT_BUTTON)
        {

        gadget = Icon_Hit(DIALOG_XO+DIALOG_YES_XO,
                          DIALOG_YO+DIALOG_YES_YO,
                          DIALOG_DX,            0,
                          DIALOG_WIDTH, DIALOG_HEIGHT,
                          2,1,
                          mx,my);

        // test which button was hit

        if (gadget==0 || gadget==1)
           {
           // replace background under dialog

           Squeeze_Mouse(MOUSE_HIDE,NULL,NULL,NULL);
           Erase_Sprite_VB((sprite_ptr)&dialogs);
           Squeeze_Mouse(MOUSE_SHOW,NULL,NULL,NULL);
           // return button

           return(gadget);

           } // end if yes or no was pressed

        } // end if mouse was pressed

       } // end while

} // end Yes_No

//////////////////////////////////////////////////////////////////////////////

Plot_Zoom_Pixel(int x,int y, int color)
{
// this function draws a zoom pixel in the zoom box. Each zoom pixel is
// 3x3 with a single pixel boundary around it

int xs,ys,index;

// comput starting position of zoom pixel

xs = x*4+ZOOM_XO + 1;
ys = y*4+ZOOM_YO + 1;

// plot the nine pixels

Plot_Pixel_Fast(xs  ,ys,(unsigned char)color);
Plot_Pixel_Fast(xs+1,ys,(unsigned char)color);
Plot_Pixel_Fast(xs+2,ys,(unsigned char)color);
Plot_Pixel_Fast(xs  ,ys+1,(unsigned char)color);
Plot_Pixel_Fast(xs+1,ys+1,(unsigned char)color);
Plot_Pixel_Fast(xs+2,ys+1,(unsigned char)color);
Plot_Pixel_Fast(xs  ,ys+2,(unsigned char)color);
Plot_Pixel_Fast(xs+1,ys+2,(unsigned char)color);
Plot_Pixel_Fast(xs+2,ys+2,(unsigned char)color);

// test if this is the highlighted pixel

if (x==6 && y==6)
   {
   // draw a white rectanlge around pixel

   for (index=0; index<=4; index++)
       {
       Plot_Pixel_Fast(xs-1,ys+index-1,(unsigned char)15);
       Plot_Pixel_Fast(xs+3,ys+index-1,(unsigned char)15);
       Plot_Pixel_Fast(xs-1+index,ys-1,(unsigned char)15);
       Plot_Pixel_Fast(xs-1+index,ys+3,(unsigned char)15);
       } // end for index

   } // end if need to highlight

} // end Plot_Zoom_Pixel

//////////////////////////////////////////////////////////////////////////////

Draw_Zoom_Box(int curr_x, int curr_y)
{
// this function draws the zoom box and highlights the cursor position

int x,y;

for (y=curr_y-6; y<curr_y+5; y++)
    {
    for (x=curr_x-6; x<curr_x+5; x++)
        {
        // test if the pixel is valid

        if (x>0 && y>0)
           {
           // plot the zoomed pixel

           Plot_Zoom_Pixel(x-(curr_x-6), y-(curr_y-6),
                           value_to_color[data_table[y*MAP_WIDTH+x]]);

           } // end if on screen
        else
           {
           // zooming off map so plot blanks

           Plot_Zoom_Pixel(x-(curr_x-6), y-(curr_y-6),0);

           } // end else

        } // end for x

    } // end for y

} // end Draw_Zoom_Box

// M A I N ///////////////////////////////////////////////////////////////////

main()
{

int done=0,
    mx,my,buttons,num_buttons,gadget;

unsigned int bc,x,y,index;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// Let user know whats happening

printf("Loading Textures...");

Load_Textures();

// load up the editor view

// initialize the pcx file that holds the edit view

PCX_Init((pcx_picture_ptr)&edit_view);

// load the pcx file that holds the cells

PCX_Load("waredit.pcx", (pcx_picture_ptr)&edit_view,1);

PCX_Show_Buffer((pcx_picture_ptr)&edit_view);

// delete the pcx file

PCX_Delete((pcx_picture_ptr)&edit_view);

// initialize mouse

Squeeze_Mouse(MOUSE_RESET,NULL,NULL,&num_buttons);

// allocate data table

data_table = (char far*)_fmalloc(MAP_WIDTH*MAP_HEIGHT+1);

Clear();

// show the mouse

Squeeze_Mouse(MOUSE_SHOW,NULL,NULL,NULL);


// main loop

while(!done)
     {

// get mouse input

Squeeze_Mouse(MOUSE_BUTT_POS,&mx,&my,&buttons);

// mouse is in 640x200 so divide

mx=(mx>>1) + 1;
my-=1;

// test if user in in map, if so render zoom box

if (mx>=MAP_XO && mx<MAP_XO+MAP_WIDTH &&
    my>=MAP_YO && my<MAP_YO+MAP_HEIGHT)
    {

    Draw_Zoom_Box(mx-MAP_XO,my-MAP_YO);

    } // end if in map


// is user trying to draw or select an icon?

     if (buttons)
        {

        // test for drawing region

        if (mx>=MAP_XO && mx<MAP_XO+MAP_WIDTH &&
            my>=MAP_YO && my<MAP_YO+MAP_HEIGHT)
            {
            // test for which button was down

            if (buttons==MOUSE_LEFT_BUTTON)
               {

               // plot pixel on screen in proper color

               Plot_Pixel_Fast(mx-MAP_XO,my-MAP_YO,value_to_color[block_value]);

               // update data table

               data_table[(my-MAP_YO)*MAP_WIDTH + (mx-MAP_XO)] = block_value;

               } // end if normal mode
            else
            if (buttons==MOUSE_RIGHT_BUTTON)
               {

               Plot_Pixel_Fast(mx-MAP_XO,my-MAP_YO,0);

               // update data table

               data_table[(my-MAP_YO)*MAP_WIDTH + (mx-MAP_XO)] = 0;

               } // end else if eraser mode)

            } // end if in drawing region
        else
        if (buttons==MOUSE_LEFT_BUTTON)
           {
           // test for all control buttons

           // walls

           if (-1!=(gadget = Icon_Hit(WALLS_XO, WALLS_YO,
                                      ICONS_DX, ICONS_DY,
                                      ICONS_WIDTH, ICONS_HEIGHT,
                                      6,4,
                                      mx,my)))
           {

          // _settextposition(8,25);
          // printf("walls #%d       ",gadget);


           // change current block value and color

           block_value = WALLS_START + gadget;

           // update preview window

           Clear_Preview();

           // resize sprite

           sprite_width = sprite_height = 64;

           // test what type of walls

           switch(gadget / 6)
                 {
                 case 0:
                       {
                       walls_stone.curr_frame = gadget % 6;
                       Scale_Sprite((sprite_ptr)&walls_stone,PREVIEW_HEIGHT);
                       } break;
                 case 1:
                       {
                       walls_melt.curr_frame = gadget % 6;
                       Scale_Sprite((sprite_ptr)&walls_melt  ,PREVIEW_HEIGHT);
                       } break;

                 case 2:
                       {
                       walls_ooz.curr_frame = gadget % 6;
                       Scale_Sprite((sprite_ptr)&walls_ooz ,PREVIEW_HEIGHT);
                       } break;
                 case 3:
                       {
                       walls_icy.curr_frame = gadget % 6;
                       Scale_Sprite((sprite_ptr)&walls_icy  ,PREVIEW_HEIGHT);

                       } break;

                 default:break;

                 } // end switch

           Release_Mouse();

           } // end if clicked a walls

           // doors
           else
           if (-1!=(gadget = Icon_Hit(DOORS_XO, DOORS_YO,
                                      ICONS_DX, ICONS_DY,
                                      ICONS_WIDTH, ICONS_HEIGHT,
                                      4,1,
                                      mx,my)))
           {


           // _settextposition(8,25);
           // printf("doors #%d       ",gadget);

           // change current block value and color

           block_value = DOORS_START + gadget;


           // update preview window

           Clear_Preview();



           // resize sprite

           sprite_width = sprite_height = 64;

           doors.curr_frame = gadget;
           Scale_Sprite((sprite_ptr)&doors,PREVIEW_HEIGHT);

           Release_Mouse();

           } // end if clicked a door

           // scrolls
           else

           if (-1!=(gadget = Icon_Hit(SCROLLS_XO, SCROLLS_YO,
                                      ICONS_DX, ICONS_DY,
                                      ICONS_WIDTH, ICONS_HEIGHT,
                                      4,1,
                                      mx,my)))
           {


           // _settextposition(8,25);
           // printf("scrolls #%d       ",gadget);

           // change current block value and color

           block_value = SCROLLS_START + gadget;



           // update preview window

           Clear_Preview();


           // resize sprite

           sprite_width =  16;
           sprite_height = 16;

           scrolls.curr_frame = gadget;
           Scale_Sprite((sprite_ptr)&scrolls,PREVIEW_HEIGHT);

           Release_Mouse();

           } // end if clicked a scroll

           // potions
           else

           if (-1!=(gadget = Icon_Hit(POTIONS_XO, POTIONS_YO,
                                      ICONS_DX, ICONS_DY,
                                      ICONS_WIDTH, ICONS_HEIGHT,
                                      4,1,
                                      mx,my)))
           {

           // _settextposition(8,25);
           // printf("potions #%d       ",gadget);

           // change current block value and color

           block_value = POTIONS_START + gadget;

           // update preview window

           Clear_Preview();


           // resize sprite

           sprite_width =  16;
           sprite_height = 16;

           potions.curr_frame = gadget;
           Scale_Sprite((sprite_ptr)&potions,PREVIEW_HEIGHT);

           Release_Mouse();

           } // end if clicked a potion


           // foods
           else
           if (-1!=(gadget = Icon_Hit(FOODS_XO, FOODS_YO,
                                      ICONS_DX, ICONS_DY,
                                      ICONS_WIDTH, ICONS_HEIGHT,
                                      2,1,
                                      mx,my)))
           {

           // _settextposition(8,25);
           // printf("foods #%d       ",gadget);

           // change current block value and color

           block_value = FOODS_START + gadget;

           // update preview window

           Clear_Preview();


           // resize sprite

           sprite_width =  32;
           sprite_height = 16;

           foods.curr_frame = gadget;
           Scale_Sprite((sprite_ptr)&foods,PREVIEW_HEIGHT);

           Release_Mouse();

           } // end if food

           // monsters
           else
           if (-1!=(gadget = Icon_Hit(MONSTERS_XO, MONSTERS_YO,
                                      ICONS_DX, ICONS_DY,
                                      ICONS_WIDTH, ICONS_HEIGHT,
                                      2,1,
                                      mx,my)))
           {

           // _settextposition(8,25);
           // printf("monsters #%d       ",gadget);

           // change current block value and color

           block_value = MONSTERS_START + gadget;


           // update preview window

           Clear_Preview();

           // resize sprite

           sprite_width =  48;
           sprite_height = 48;

           monsters.curr_frame = gadget;
           Scale_Sprite((sprite_ptr)&monsters,PREVIEW_HEIGHT);

           Release_Mouse();

           } // end if clicked a monster


           // eraser
           else
           if (-1!=(gadget = Icon_Hit(ERASER_XO, ERASER_YO,
                                      ICONS_DX, ICONS_DY,
                                      ICONS_WIDTH, ICONS_HEIGHT,
                                      1,1,
                                      mx,my)))
           {

           // _settextposition(8,25);
           // printf("start         ",gadget);


           // change current block value and color

           block_value = GAME_START;

           // update preview window

           Clear_Preview();


           } // end if clicked the eraser

           // controls
           else

           if (-1!=(gadget = Icon_Hit(BUTTONS_XO, BUTTONS_YO,
                                      0         , BUTTONS_DY,
                                      BUTTONS_WIDTH, BUTTONS_HEIGHT,
                                      1,4,
                                      mx,my)))
           {

           // _settextposition(8,25);
           // printf("controls #%d       ",gadget);


           // which command is user to do

           switch(gadget)
                 {

                 case COMM_LOAD:
                      {

                      // load a file, but let user know that this will
                      // destroy the current file

                      if (Yes_No()==YES)
                         {
                         Clear();
                         Load_File();
                         } // end if yes

                      } break;

                 case COMM_SAVE:
                      {
                      // save the data to disk

                      Save_File();

                      } break;

                 case COMM_CLEAR:
                      {
                      // clear everything

                      if (Yes_No()==YES)
                         {

                         Clear();

                         // reset the current icon

                         block_value=0;

                         } // end if

                      } break;

                 case COMM_EXIT:
                      {
                      // lets bail

                      if (Yes_No()==YES)
                         {
                         done=1;
                         } // end if yes

                      } break;

                 default:break;

                 // make sure mouse doesn't disapear

                 Squeeze_Mouse(MOUSE_SHOW,NULL,NULL,NULL);

                 } // end switch

           } // end if clicked the main controls

           } // end else icon hits

        } // end if a button was hit

     } // end while

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



