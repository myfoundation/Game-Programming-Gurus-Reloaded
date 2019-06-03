
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>

// D E F I N E S /////////////////////////////////////////////////////////////

#define NUM_ATOMS 300

// S T R U C T U R E S ///////////////////////////////////////////////////////

// atom structure

typedef struct ant_typ
        {
        int x,y;            // position of atom
        int xv,yv;          // velocity of atom

        } atom, *atom_ptr;

// G L O B A L S /////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr

unsigned int far *clock = (unsigned int far *)0x0000046C; // pointer to internal

// our atoms

atom atoms[NUM_ATOMS];


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

//////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Fast(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

video_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast

////////////////////////////////////////////////////////////////////////////

void Initialize_Atoms(void)
{
int index;

for (index=0; index<NUM_ATOMS; index++)
    {
    // select a random position and trajectory for each atom
    // their background

    atoms[index].x     = 5 + rand()%300;
    atoms[index].y     = 20 + rand()%160;

    atoms[index].xv    = -5 + rand()%10;
    atoms[index].yv    = -5 + rand()%10;

    } // end for index

} // end Initialize_Atoms

////////////////////////////////////////////////////////////////////////////

void Erase_Atoms(void)
{
int index;

// loop through the atoms and erase them

for (index=0; index<NUM_ATOMS; index++)
    {
    Plot_Pixel_Fast(atoms[index].x, atoms[index].y, 0);
    } // end for index

} // end Erase_Atoms

////////////////////////////////////////////////////////////////////////////

void Move_Atoms(void)
{

int index;;

// loop through the atom array and move each atom also check collsions
// with the walls of the container

for (index=0; index<NUM_ATOMS; index++)
    {

    // move the atoms

    atoms[index].x+=atoms[index].xv;
    atoms[index].y+=atoms[index].yv;

    // did the atom hit a wall, if so reflect the velocity vector

    if (atoms[index].x > 310 || atoms[index].x <10)
        {
        atoms[index].xv=-atoms[index].xv;
        atoms[index].x+=atoms[index].xv;
        } // end if hit a vertical wall


    if (atoms[index].y > 190 || atoms[index].y <30)
       {
       atoms[index].yv=-atoms[index].yv;
       atoms[index].y+=atoms[index].yv;
       } // end if hit a horizontal wall

    } // end for index

} // end Move_Atoms

////////////////////////////////////////////////////////////////////////////

void Draw_Atoms(void)
{
int index;

// loop through the atoms and draw them

for (index=0; index<NUM_ATOMS; index++)
    {
    Plot_Pixel_Fast(atoms[index].x, atoms[index].y, 10);
    } // end for index

} // end Draw_Atoms

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

// 320x200x256 color mode

_setvideomode(_MRES256COLOR);

_settextposition(2,0);
printf("Hit any key to exit.");

// draw the container

_setcolor(9);

_rectangle(_GBORDER,0,16,319,199);

// set all the ants up

Initialize_Atoms();

while(!kbhit())
     {
     // erase all the atoms

     Erase_Atoms();

     // move all the atoms

     Move_Atoms();

     // now draw the atoms

     Draw_Atoms();

     // wait a little

     Timer(1);

     } // end while

// restore the old video mode

_setvideomode(_DEFAULTMODE);

} // end main

