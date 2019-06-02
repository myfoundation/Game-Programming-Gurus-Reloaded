
// I N C L U D E S////////////////////////////////////////////////////////////

#include <stdio.h>
#include <graph.h>
#include <math.h>

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

int ex=160,ey=100; // starting position of fly

int curr_xv=1,curr_yv=0, // current translation factors
    clicks=0;            // times when the fly is done moving in the random
                         // direction

_setvideomode(_MRES256COLOR);

printf("  The Dumb Fly - Any key to Quit");

// main game loop

while(!kbhit())
     {
     // erase dots

     _setcolor(0);

     _setpixel(ex,ey);

     // move the fly

     // begin brain

     // are we done with this direction

     if (++clicks==20)
        {
        curr_xv = -5 + rand()%10; // -5 to +5
        curr_yv = -5 + rand()%10; // -5 to +5
        clicks=0;
        } // end if time for a new direction

     // move the fly

     ex+=curr_xv;
     ey+=curr_yv;

     // make sure fly stays on paper

     if (ex>319) ex=0;
     if (ex<0)   ex=319;
     if (ey>199) ey=0;
     if (ey<0)   ey=199;

     // end brain

     // draw fly

     _setcolor(12);
     _setpixel(ex,ey);

     // wait a bit

     Timer(1);

     } // end while

_setvideomode(_DEFAULTMODE);

} // end main

