
#include <stdio.h> // include the basics
#include <graph.h>// include Microsofts Graphics Header

void main(void)
{

int x1,y1,x2,y2,color,index;

// put the computer into graphics mode 

_setvideomode(_VRES16COLOR); //  640x480 in 16 colors

// let's draw 1,000 lines randomly on the screen

for (index = 0; index<1000; index++)
     {
     // get a random positions and color and draw a line there

     x1 = rand()%640;  //  x of starting point
     y1 = rand()%480;  //  y of starting point
     x2 = rand()%640;  //  x of ending point
     y2 = rand()%480;  //  y of ending point
     color = rand()%16;
    _setcolor(color);  // set the color of the pixel to be drawn
    _moveto(x1,y1);    // move to the start of the line
    _lineto(x2,y2);    //  draw the line
  
     } // end for index

// wait for the user to hit a key

while(!kbhit()){}

// place the computer back into text mode

_setvideomode(_DEFAULTMODE);

} // end main



