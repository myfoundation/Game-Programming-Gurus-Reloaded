
#include <math.h>
#include <stdio.h>
#include <graph.h>


float sin_table[360], cos_table[360];

main()
{

int index,x,y,xo,yo,radius,color,ang;

char far *screen = (char far *)0xA0000000;

// use Microsofts library to go into 320x200x256 mode
_setvideomode(_MRES256COLOR);

// create look up tables

for (index=0; index<360; index++)
     {
     sin_table[index]= sin(index*3.14159/180);
     cos_table[index] = cos(index*3.14159/180);

     } // end for index

// draw 1000 circles using built in sin and cos

for (index=0; index<1000; index++)
     {
     // get a random circle
     radius = rand()%50;
     xo = rand()%320;
     yo = rand()%200;
     color = rand()%256;
     
     for (ang=0; ang<360; ang++)
          {
          x = xo + cos(ang*3.14159/180) * radius;
          y = yo + sin(ang*3.14159/180)*radius;
          // plot the point of the circle
          screen[(y<<6) + (y<<8) + x] = color;
          } // end for ang
     } // end for index

// done, halt the system and wait for user to hit a key
printf("\nHit a key to see circles drawn twith look up tables.");

getch();
_setvideomode(_MRES256COLOR);

// draw 1000 circles using look up tables

for (index=0; index<1000; index++)
     {
     // get a random circle
     radius = rand()%50;
     xo = rand()%320;
     yo = rand()%200;
     color = rand()%256;
     
     for (ang=0; ang<360; ang++)
          {
          x = xo + cos_table[ang] * radius;
          y = yo + sin_table[ang] *radius;
          // plot the point of the circle
          screen[(y<<6) + (y<<8) + x] = color;
          } // end for ang
     } // end for index

// let user hit a key to exit

printf("\nHit any key to exit.");
getch();

_setvideomode(_DEFAULTMODE);

} // end main

