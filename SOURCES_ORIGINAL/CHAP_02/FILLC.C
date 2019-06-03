
#include <stdio.h>

#define VGA256 0x13
#define TEXT_MODE 0x03

extern void Set_Mode(int mode);
extern void Fill_Screen(int color);


void main(void)
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