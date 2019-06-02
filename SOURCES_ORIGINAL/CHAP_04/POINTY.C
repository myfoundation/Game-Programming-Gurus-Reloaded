
#include <stdio.h> // include the basics
#include <graph.h> // include Microsofts Graphics Header

void main(void)
{

int x,y,index,color;

// put the computer into graphics mode 

_setvideomode(_VRES16COLOR); //  640x480 in 16 colors

// let's draw 100 points randomly on the screen

for (index = 0; index<10000; index++)
     {
     // get a random position and color and plot a point there

     x = rand()%640;
     y = rand()%480; 
     color = rand()%16;
    _setcolor(color);  // set the color of the pixel to be drawn
    _setpixel(x,y);    // draw the pixel
 
     } // end for index

// wait for the user to hit a key

while(!kbhit()){}

// place the computer back into text mode

_setvideomode(_DEFAULTMODE);

} // end main


