
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>


// D E F I N E S /////////////////////////////////////////////////////////////

#define TIME_KEEPER_INT 0x1C
#define NUM_STARS 50

// S T R U C T U R E S ///////////////////////////////////////////////////////

typedef struct star_typ
        {
        int x,y;    // position of star
        int vel;    // x - component of star velocity
        int color;  // color of star

        } star, *star_ptr;


// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt _far *Old_Isr)();  // holds old com port interrupt handler

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr

int star_first=1;  // flags first time into star field

star stars[NUM_STARS]; // the star field


// F U N C T I O N S ////////////////////////////////////////////////////////

Plot_Pixel_Fast(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

video_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast

//////////////////////////////////////////////////////////////////////////////

void _interrupt _far Star_Int()
{

// this function will create a panning 3-d star field with 3-planes, like
// looking out of the Enterprise

// note: this function had better execute faster than 55.4 ms, otherwise it
// will be called again re-entrantly and kaboom!

int index;

// test if we need to initialize star field i.e. first time function is being
// called

if (star_first)
   {
   // reset first time
   star_first=0;

   // initialize all the stars

   for (index=0; index<NUM_STARS; index++)
       {
       // initialize each star to a velocity, position and color

       stars[index].x     = rand()%320;
       stars[index].y     = rand()%180;

       // decide what star plane the star is in

       switch(rand()%3)
             {
             case 0: // plane 1- the farthest star plane
                  {
                  // set velocity and color

                  stars[index].vel = 2;
                  stars[index].color = 8;

                  } break;

             case 1: // plane 2-The medium distance star plane
                  {

                  stars[index].vel = 4;
                  stars[index].color = 7;

                  } break;

             case 2: // plane 3-The nearest star plane
                  {

                  stars[index].vel = 6;
                  stars[index].color = 15;

                  } break;

             } // end switch

       } // end for index

   } // end if first time
else
   { // must be nth time in, so do the usual

   // erase, move, draw

   for (index=0; index<NUM_STARS; index++)
       {
       // erase

       Plot_Pixel_Fast(stars[index].x,stars[index].y,0);

       // move

       if ( (stars[index].x+=stars[index].vel) >=320 )
          stars[index].x = 0;

       // draw

       Plot_Pixel_Fast(stars[index].x,stars[index].y,stars[index].color);

       } // end for index

   } // end else

} // end Star_Int

// M A I N ///////////////////////////////////////////////////////////////////

main()
{
int num1, num2,c;

_setvideomode(_MRES256COLOR);

// install our ISR

Old_Isr = _dos_getvect(TIME_KEEPER_INT);

_dos_setvect(TIME_KEEPER_INT, Star_Int);

// wait for user to hit a key

_settextposition(23,0);

printf("Hit Q - to quit.");
printf("\nHit E - to see something wonderful...");

// get the character

c = getch();

// does user feel adventurous

if (c=='e')
   {
   printf("\nLook stars in DOS, how can this be ?");

   exit(0);  // exit without fixing up old ISR
   } // end if

// replace old ISR

_dos_setvect(TIME_KEEPER_INT, Old_Isr);

_setvideomode(_DEFAULTMODE);

} // end main

