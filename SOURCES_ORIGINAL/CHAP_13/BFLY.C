
// I N C L U D E S////////////////////////////////////////////////////////////

#include <stdio.h>
#include <graph.h>
#include <math.h>

// D E F I N E S ////////////////////////////////////////////////////////////

#define STATE_CHASE   1
#define STATE_RANDOM  2
#define STATE_EVADE   3
#define STATE_PATTERN 4

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

int px=160,py=100,    // starting position of player
    ex=0,ey=0,        // starting position of enemy
    curr_xv,curr_yv;  // velocity of fly during random walk


int done=0,           // exit flag
    doing_pattern=0,  // flags if a pattern is being executed
    current_pattern,  // curent pattern 0-2 that is being done by brain
    pattern_element,  // current element of pattern being executed
    select_state=0,   // flags if a state transition needs to take place
    clicks=20,        // used to time the number of cycles a state stays active
    fly_state = STATE_CHASE;  // start fly off in chase state

float distance;       // used to hold distance between fly and player

_setvideomode(_MRES256COLOR);

printf("      Brainy Fly - Q to Quit");

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


     // what state is brain in let FSM sort it out


     switch(fly_state)
           {

           case STATE_CHASE:
                {
                _settextposition(24,2);
                printf("current state:chase   ");

                // make the fly chase the player

                if (px>ex) ex++;
                if (px<ex) ex--;
                if (py>ey) ey++;
                if (py<ey) ey--;

                // time to go to another state

                if (--clicks==0)
                   select_state=1;

                } break;

           case STATE_RANDOM:
                {
                _settextposition(24,2);
                printf("current state:random  ");

                // move fly in random direction

                ex+=curr_xv;
                ey+=curr_yv;

                // time to go to another state

                if (--clicks==0)
                   select_state=1;

                } break;


           case STATE_EVADE:
                {
                _settextposition(24,2);
                printf("current state:evade  ");


                // make fly run from player

                if (px>ex) ex--;
                if (px<ex) ex++;
                if (py>ey) ey--;
                if (py<ey) ey++;

                // time to go to another state

                if (--clicks==0)
                   select_state=1;

                } break;

           case STATE_PATTERN:
                {
                _settextposition(24,2);
                printf("current state:pattern  ");

                // move the enemy using the next pattern element of the current pattern

                ex+=patterns_x[current_pattern][pattern_element];
                ey+=patterns_y[current_pattern][pattern_element];

                // are we done doing pattern

                if (++pattern_element==20)
                    {
                    pattern_element = 0;
                    select_state=1;
                    } // end if done doing pattern

                } break;


           default:break;

           } // end switch fly state

           // does brain want another state ?

           if (select_state==1)
              {

              // select a state based on the envoronment and on fuzzy logic
              // uses distance from player to selct a new state

              distance =  sqrt(.5 + fabs((px-ex)*(px-ex) + (py-ey)*(py-ey)));

              if (distance > 5 && distance <15 && rand()%2==1)
                 {
                 // get a new random pattern

                 current_pattern = rand()%3;

                 // set brain into pattern state

                 fly_state = STATE_PATTERN;

                 pattern_element = 0;

                 } // end if close to player
              else
              if (distance < 10) // too close let's run!
                 {
                 clicks=20;
                 fly_state = STATE_EVADE;

                 } // else if too close
              else
              if (distance > 25 && distance <100 && rand()%3==1)  // let's chase player
                 {
                 clicks=15;
                 fly_state = STATE_CHASE;

                 }  // end if chase player
              else
              if (distance > 30 && rand()%2==1)
                 {
                 clicks=10;
                 fly_state = STATE_RANDOM;

                 curr_xv = -5 + rand()%10; // -5 to +5
                 curr_yv = -5 + rand()%10; // -5 to +5

                 } // end if random
              else
                 {
                 clicks=5;
                 fly_state = STATE_RANDOM;

                 curr_xv = -5 + rand()%10; // -5 to +5
                 curr_yv = -5 + rand()%10; // -5 to +5

                 } // end else

              // reset need another state flag

              select_state=0;

              } // end if we need to change to another state

     // make sure fly stays on paper

     if (ex>319) ex=0;
     if (ex<0)   ex=319;
     if (ey>199) ey=0;
     if (ey<0)   ey=199;

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