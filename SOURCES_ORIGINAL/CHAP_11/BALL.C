
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <graph.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define EARTH_GRAVITY 9.8

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal

// F U N C T I O N S ////////////////////////////////////////////////////////

void Timer(int clicks)
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

void main(void)
{

float ball_x   = 160,
      ball_y   = 50,
      ball_yv  = 0,
      ball_acc = EARTH_GRAVITY;

int done=0,key;

// use all MS graphics routines for a change

_setvideomode(_MRES256COLOR);

_settextposition(0,0);
printf("Q to quit, use +,- to change gravity.");

while(!done)
     {

     // has there been a keyboard press

     if (kbhit())
        {

        // test what key

        switch(getch())
              {
              case '-':
                      {
                      ball_acc-=.1;

                      } break;

              case '=':
                      {
                      ball_acc+=.1;

                      } break;

              case 'q':
                      {
                      done=1;

                      } break;

              } // end switch

        // let user know what the gravity is

        _settextposition(24,2);
        printf("Gravitational Constant = %f",ball_acc);

        } // end if keyboard hit

    // erase the ball

    _setcolor(0);

    _ellipse(_GBORDER, ball_x,ball_y,ball_x+10,ball_y+10);

    // move the ball

    ball_y+=ball_yv;

    // add acceleration to velocity

    ball_yv+=(ball_acc*.1); // the .1 is to scale it for viewing

    // test if ball has hit bottom

    if (ball_y>190)
       {
       ball_y=50;
       ball_yv=0;

       } // end if

    // draw ball

    _setcolor(1);

    _ellipse(_GBORDER, ball_x,ball_y,ball_x+10,ball_y+10);

    // wait a bit

    Timer(2);

    } // end while

// restore old videomode

_setvideomode(_DEFAULTMODE);

} // end main

