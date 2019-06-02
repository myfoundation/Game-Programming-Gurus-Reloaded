
#include <stdio.h> // include the basics
#include <graph.h>// include Microsofts Graphics Header

void main(void)
{

// put the computer into graphics mode 
_setvideomode(_VRES16COLOR); //  640x480 in 16 colors

// draw a simple polygon
_setcolor(1);  // blue
_moveto(100,100); // vertex 1
_lineto(120,120);   // vertex 2
_lineto(150,200);   // vertex 3
_lineto(80,190);    // vertex 4
_lineto(80,60);      // vertex 5
_lineto(100,100);   // back to vertex 1 to close up the polygon

// now highlight each vertex in white

_setcolor(15);  // white
_setpixel(100,100); // vertex 1
_setpixel(120,120); // vertex 2
_setpixel(150,200); // vertex 3
_setpixel(80,190);  // vertex 4
_setpixel(80,60);   // vertex 5


// wait for the user to hit a key
while(!kbhit()){}

// place the computer back into text mode

_setvideomode(_DEFAULTMODE);

} // end main

