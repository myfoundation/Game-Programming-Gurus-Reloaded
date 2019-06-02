

// I N C L U D E S ///////////////////////////////////////////////////////////

#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <memory.h>
#include <math.h>
#include <string.h>

// D E F I N E S  ////////////////////////////////////////////////////////////

#define ROM_CHAR_SET_SEG 0xF000  // segment of 8x8 ROM character set
#define ROM_CHAR_SET_OFF 0xFA6E  // begining offset of 8x8 ROM character set

#define VGA256            0x13
#define TEXT_MODE         0x03

#define PALETTE_MASK      0x3c6

#define PALETTE_REGISTER_RD 0x3c7
#define PALETTE_REGISTER_WR 0x3c8

#define PALETTE_DATA      0x3c9

#define SCREEN_WIDTH      (unsigned int)320
#define SCREEN_HEIGHT     (unsigned int)200

// S T R U C T U R E S ///////////////////////////////////////////////////////

// this structure holds a RGB triple in three bytes

typedef struct RGB_color_typ
        {

        unsigned char red;    // red   component of color 0-63
        unsigned char green;  // green component of color 0-63
        unsigned char blue;   // blue  component of color 0-63

        } RGB_color, *RGB_color_ptr;

// E X T E R N A L S /////////////////////////////////////////////////////////


extern Set_Mode(int mode);


// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Set_Palette_Register(int index, RGB_color_ptr color);

void Get_Palette_Register(int index, RGB_color_ptr color);

void Create_Cool_Palette();

void V_Line(int y1,int y2,int x,unsigned int color);

// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr
unsigned int far *video_buffer_w= (int far *)0xA0000000L;  // vram word ptr

// F U N C T I O N S /////////////////////////////////////////////////////////

void Set_Palette_Register(int index, RGB_color_ptr color)
{

// this function sets a single color look up table value indexed by index
// with the value in the color structure

// tell VGA card we are going to update a pallete register

_outp(PALETTE_MASK,0xff);

// tell vga card which register we will be updating

_outp(PALETTE_REGISTER_WR, index);

// now update the RGB triple, note the same port is used each time

_outp(PALETTE_DATA,color->red);
_outp(PALETTE_DATA,color->green);
_outp(PALETTE_DATA,color->blue);

} // end Set_Palette_Color

///////////////////////////////////////////////////////////////////////////////

void Get_Palette_Register(int index, RGB_color_ptr color)
{

// this function gets the data out of a color lookup regsiter and places it
// into color

// set the palette mask register

_outp(PALETTE_MASK,0xff);

// tell vga card which register we will be reading

_outp(PALETTE_REGISTER_RD, index);

// now extract the data

color->red   = _inp(PALETTE_DATA);
color->green = _inp(PALETTE_DATA);
color->blue  = _inp(PALETTE_DATA);

} // end Get_Palette_Color

//////////////////////////////////////////////////////////////////////////////

void Create_Cool_Palette(void)
{

// this function creates a cool palette. 64 shades of gray, 64 of red,
// 64 of green and finally 64 of blue.

RGB_color color;

int index;

// swip thru the color registers and create 4 banks of 64 colors

for (index=0; index < 64; index++)
    {

    // grays

    color.red   = index;
    color.green = index;
    color.blue  = index;

    Set_Palette_Register(index, (RGB_color_ptr)&color);

    // reds

    color.red   = index;
    color.green = 0;
    color.blue  = 0;

    Set_Palette_Register(index+64, (RGB_color_ptr)&color);

    // greens

    color.red   = 0;
    color.green = index;
    color.blue  = 0;

    Set_Palette_Register(index+128, (RGB_color_ptr)&color);

    // blues

    color.red   = 0;
    color.green = 0;
    color.blue  = index;

    Set_Palette_Register(index+192, (RGB_color_ptr)&color);

    } // end index


} // end Create_Cool_Palette

//////////////////////////////////////////////////////////////////////////////

void V_Line(int y1,int y2,int x,unsigned int color)
{
// draw a vertical line, note y2 > y1

unsigned int line_offset,
                    index;

// compute starting position

line_offset = ((y1<<8) + (y1<<6)) + x;

for (index=0; index<=y2-y1; index++)
    {
    video_buffer[line_offset] = color;

     line_offset+=320; // move to next line

    } // end for index

} // end V_Line

//M A I N /////////////////////////////////////////////////////////////////////

void main(void)
{
int index;
RGB_color color,color_1;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// create the color palette

Create_Cool_Palette();

// draw a bunch of vertical lines, one for each color

for (index=0; index<320; index++)
    V_Line(0,199,index,index);

// wait for user to hit a key

while(!kbhit())
     {
     Get_Palette_Register(0,(RGB_color_ptr)&color_1);
     Get_Palette_Register(0,(RGB_color_ptr)&color_1);

     for (index=0; index<=254; index++)
         {
         Get_Palette_Register(index+1,(RGB_color_ptr)&color);
         Get_Palette_Register(index+1,(RGB_color_ptr)&color);
         Set_Palette_Register(index,(RGB_color_ptr)&color);

         } // end for

         Set_Palette_Register(255,(RGB_color_ptr)&color_1);

     } // end while

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main
