
//------------------------------------------
// DOS DEVELOPMENT ENVIRONMENT EMULATION TOOLKIT
//------------------------------------------
#include "stdafx.h"
#include "DOSEmu.h"
//------------------------------------------

#include <stdio.h>

#define VGA256 0x13
#define TEXT_MODE 0x03

extern void Set_Mode(int mode);

void Fill_Screen(int color)
{
	char far * screen_ram = (char far *)MEMORY_0xA0000000; // vram byte ptr
	memset(screen_ram,color,320*200);                // clear buffer

	_redraw_screen();
}


void main2(void)
{
int t;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// fill the screen with 1's which in the defualt pallete will be blue


for (t=0; t<1000; t++)
Fill_Screen(t);

// wait for keyboard to be hit

// while(!kbhit()){}

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



