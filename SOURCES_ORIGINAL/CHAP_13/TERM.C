
// I N C L U D E S////////////////////////////////////////////////////////////

#include <stdio.h>
#include <graph.h>


// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal
                                                          // 18.2 clicks/sec

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

// M A I N ///////////////////////////////////////////////////////////////////

main()
{

int px=160,py=100, // starting position of player
    ex=0,ey=0;     // starting position of enemy

int done=0; // exit flag

_setvideomode(_MRES256COLOR);

printf("      The Terminator - Q to Quit");

// main game loop

while(!done)
     {
     // erase dots

     _setcolor(0);

     _setpixel(px,py);
     _setpixel(ex,ey);

     // move player

     if (kbhit())
        {

        // which way is player moving

        switch(getch())
              {

              case 'u': // up
                      {
                  py-=2;
                      } break;

              case 'n': // down
                      {
                  py+=2;
                      } break;

              case 'j': // right
                      {
                  px+=2;
                      } break;


              case 'h': // left
                      {
                  px-=2;
                      } break;

              case 'q':
                      {
                      done=1;
                      } break;

              } // end switch

        } // end if player hit a key

     // move enemy

     // begin brain

     if (px>ex) ex++;
     if (px<ex) ex--;
     if (py>ey) ey++;
     if (py<ey) ey--;

     // end brain

     // draw dots

     _setcolor(9);
     _setpixel(px,py);

     _setcolor(12);
     _setpixel(ex,ey);

     // wait a bit

     Timer(1);

     } // end while

_setvideomode(_DEFAULTMODE);

} // end main

