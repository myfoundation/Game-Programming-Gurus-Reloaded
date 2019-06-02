
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define KEYBOARD_INT    0x09
#define KEY_BUFFER      0x60
#define KEY_CONTROL     0x61
#define INT_CONTROL     0x20

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

// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt _far *Old_Isr)();  // holds old com port interrupt handler

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr

int raw_key;  // the global raw keyboard data

int key_table[4] = {0,0,0,0}; // the arrow key state table

// F U N C T I O N S ////////////////////////////////////////////////////////

Plot_Pixel_Fast(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

video_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast

//////////////////////////////////////////////////////////////////////////////

Fun_Back()
{
int index;
// draws a background that should jog your memory

_setcolor(1);
_rectangle(_GFILLINTERIOR, 0,0,320,200);

_setcolor(15);

for (index=0; index<10; index++)
    {
    _moveto(16+index*32,0);
    _lineto(16+index*32,199);

    } // end for index

for (index=0; index<10; index++)
    {
    _moveto(0,10+index*20);
    _lineto(319,10+index*20);

    } // end for index

} // end Fun_Back

//////////////////////////////////////////////////////////////////////////////

void _interrupt _far New_Key_Int()
{

// I'm in the mood for some inline!

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

// process the key and update the table

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

} // end New_Key_Int

// M A I N ///////////////////////////////////////////////////////////////////

main()
{
int done=0,x=160,y=100; // exit flag and dot position

// 320x200x256 color mode

_setvideomode(_MRES256COLOR);

Fun_Back(); // light cycles anyone?

printf("\nPress ESC to Exit.");

// install our ISR

Old_Isr = _dos_getvect(KEYBOARD_INT);

_dos_setvect(KEYBOARD_INT, New_Key_Int);

// main event loop

while(!done)
     {
_settextposition(24,2);

printf("raw key=%d   ",raw_key);

     // look in the table and move the little dot

     if (key_table[INDEX_RIGHT])
         x++;

     if (key_table[INDEX_LEFT])
         x--;

     if (key_table[INDEX_UP])
         y--;

     if (key_table[INDEX_DOWN])
         y++;

     // draw the cyber dot

     Plot_Pixel_Fast(x,y,10);

     // this is our exit key the make code for "esc" is 1.

     if (raw_key==1)
        done=1;

     } // end while

// replace old ISR

_dos_setvect(KEYBOARD_INT, Old_Isr);

_setvideomode(_DEFAULTMODE);

} // end main

