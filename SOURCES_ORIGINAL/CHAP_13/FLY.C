
// I N C L U D E S////////////////////////////////////////////////////////////

#include <stdio.h>
#include <graph.h>
#include <math.h>

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal
                                                          // 18.2 clicks/sec


// the x and y components of the patterns that will be played, I just made
// them up

int patterns_x[3][20]= { 1,1,1,1,1,2,2,-1,-2,-3,-1,0,0,1,2,2,-2,-2,-1,0,
                         0,0,1,2,3,4,5,4,3,2,1,3,3,3,3,2,1,-2,-2,-1,
                         0,-1,-2,-3,-3,-2,-2,0,0,0,0,0,0,1,0,0,0,1,0,1 };



int patterns_y[3][20] = { 0,0,0,0,-1,-1,-1,-1,-1,0,0,0,0,0,2,2,2,2,2,2,
                          1,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,0,0,0,0,
                          1,1,1,2,2,-1,-1,-1,-2,-2,-1,-1,0,0,0,1,1,1,1,1 };


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

int done=0,           // exit flag
    doing_pattern=0,  // flags if a pattern is being executed
    current_pattern,  // curent pattern 0-2 that is being done by brain
    pattern_element;  // current element of pattern being executed

_setvideomode(_MRES256COLOR);

printf("        The Fly - Q to Quit");

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

     if (!doing_pattern)
        {

        if (px>ex) ex++;
        if (px<ex) ex--;
        if (py>ey) ey++;
        if (py<ey) ey--;

        // check if it's time to do a pattern i.e. is enemy within 50 pixels
        // of player


        if (sqrt(.1 + (px-ex)*(px-ex) + (py-ey)*(py-ey)) < 15)
           {
           // never ever use a SQRT in a real game!

           // get a new random pattern

           current_pattern = rand()%3;

           // set brain into pattern state

           doing_pattern = 1;

           pattern_element = 0;

           } // end if within a radius of 50

        } // end if doing a pattern
    else
       {
       // move the enemy using the next pattern element of the current pattern

       ex+=patterns_x[current_pattern][pattern_element];
       ey+=patterns_y[current_pattern][pattern_element];

       // are we done doing pattern

       if (++pattern_element==20)
           {
           pattern_element = 0;
           doing_pattern = 0;
           } // end if done doing pattern

       } // end else do pattern

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