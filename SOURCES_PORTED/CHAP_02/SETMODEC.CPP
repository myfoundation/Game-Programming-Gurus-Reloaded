
//------------------------------------------
// DOS EMULATION TOOLKIT
//------------------------------------------
#include "stdafx.h"
#include "DOSEmu.h"
//------------------------------------------

#include <stdio.h>

#define VGA256 0x13
#define TEXT_MODE 0x03

extern void Set_Mode(int mode);


void main2(void)
{

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// wait for keyboard to be hit

while(!kbhit()){ FAST_CPU_WAIT(10); }

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



