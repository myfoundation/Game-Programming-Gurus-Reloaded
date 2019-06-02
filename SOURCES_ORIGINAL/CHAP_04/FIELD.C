
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h> // include the basics
#include <graph.h> // include Microsofts Graphics Header
#include <math.h>  // include math stuff

// D E F I N E S   ///////////////////////////////////////////////////////////

#define NUM_ASTEROIDS 10
#define ERASE 0
#define DRAW  1

// T Y P E D E F S ///////////////////////////////////////////////////////////

// the structure for a vertex

typedef struct vertex_typ
           {
           float x,y; // a single point in the 2-D plane.
           } vertex, *vertex_ptr;


// the structure for an object

typedef struct object_typ
           {
           int num_vertices;     // number of vertices in this object
           int color;            // color of object
           float xo,yo;          // position of object
           float x_velocity;     // x velocity of object
           float y_velocity;     // y velocity of object
           float scale;          // scale factor
           float angle;          // rotation rate
           vertex vertices[16];  // 16 vertices
           } object, *object_ptr;


// G L O B A L S /////////////////////////////////////////////////////////////


object asteroids[NUM_ASTEROIDS];


// F U N C T I O N S /////////////////////////////////////////////////////////

void Delay(int t)
{

// take up some compute cycles

float x = 1;

while(t-->0)
     x=cos(x);


} // end Delay

//////////////////////////////////////////////////////////////////////////////

void Scale_Object(object_ptr object,float scale)
{
int index;

// for all vertices scale the x and y component

for (index = 0; index<object->num_vertices; index++)
      {
      object->vertices[index].x *= scale;
      object->vertices[index].y *= scale; 
      } // end for index

} // end Scale_Object

//////////////////////////////////////////////////////////////////////////////

void Rotate_Object(object_ptr object, float angle)
{
int index;
float x_new, y_new,cs,sn;

// pre-compute sin and cos
cs = cos(angle);
sn = sin(angle);

// for each vertex rotate it by angle
for (index=0; index<object->num_vertices; index++)
      {
       // rotate the vertex
      x_new  = object->vertices[index].x * cs -  object->vertices[index].y * sn;
      y_new  = object->vertices[index].y * cs + object->vertices[index].x * sn;

      // store the rotated vertex back into structure
      object->vertices[index].x = x_new;
      object->vertices[index].y = y_new;    

      } // end for index

} // end Rotate_Object

//////////////////////////////////////////////////////////////////////////////

void Create_Field(void)
{

int index;

for (index=0; index<NUM_ASTEROIDS; index++)
    {

    // fill in the fields

    asteroids[index].num_vertices = 6;
    asteroids[index].color = 1  + rand() % 14; // always visable
    asteroids[index].xo    = 41 + rand() % 599;
    asteroids[index].yo    = 41 + rand() % 439;

    asteroids[index].x_velocity = -10 + rand() % 20;
    asteroids[index].y_velocity = -10 + rand() % 20;
    asteroids[index].scale      = (float)(rand() % 30) / 10;
    asteroids[index].angle      = (float)(- 50 + (float)(rand() % 100)) / 100;

    asteroids[index].vertices[0].x = 4.0;
    asteroids[index].vertices[0].y = 3.5;
    asteroids[index].vertices[1].x = 8.5;
    asteroids[index].vertices[1].y = -3.0;
    asteroids[index].vertices[2].x = 6;
    asteroids[index].vertices[2].y = -5;
    asteroids[index].vertices[3].x = 2;
    asteroids[index].vertices[3].y = -3;
    asteroids[index].vertices[4].x = -4;
    asteroids[index].vertices[4].y = -6;
    asteroids[index].vertices[5].x = -3.5;
    asteroids[index].vertices[5].y = 5.5;

    // now scale the asteroid to proper size

    Scale_Object((object_ptr)&asteroids[index], asteroids[index].scale);

    } // end for index

} // end Create_Field

//////////////////////////////////////////////////////////////////////////////

void Draw_Asteroids(int erase)
{

int index,vertex;
float xo,yo;

for (index=0; index<NUM_ASTEROIDS; index++)
    {

    // draw the asteroid

    if (erase==ERASE)
       _setcolor(0);
    else
       _setcolor(asteroids[index].color);

    // get position of object
    xo = asteroids[index].xo;
    yo = asteroids[index].yo;


    // moveto first vertex

    _moveto((int)(xo+asteroids[index].vertices[0].x),(int)(yo+asteroids[index].vertices[0].y));

    for (vertex=1; vertex<asteroids[index].num_vertices; vertex++)
        {
        _lineto((int)(xo+asteroids[index].vertices[vertex].x),(int)(yo+asteroids[index].vertices[vertex].y));

        } // end for vertex

    // close object

    _lineto((int)(xo+asteroids[index].vertices[0].x),(int)(yo+asteroids[index].vertices[0].y));

    } // end for index

} // end Draw_Asteroids

//////////////////////////////////////////////////////////////////////////////

void Translate_Asteroids()
{

int index;

for (index=0; index<NUM_ASTEROIDS; index++)
    {
    // translate current asteroid

    asteroids[index].xo += asteroids[index].x_velocity;
    asteroids[index].yo += asteroids[index].y_velocity;

    // collision detection i.e. bounds check

    if (asteroids[index].xo > 600 || asteroids[index].xo < 40)
        {
        asteroids[index].x_velocity = -asteroids[index].x_velocity;
        asteroids[index].xo += asteroids[index].x_velocity;
        }

    if (asteroids[index].yo > 440 || asteroids[index].yo < 40)
        {
        asteroids[index].y_velocity = -asteroids[index].y_velocity;
        asteroids[index].yo += asteroids[index].y_velocity;
        }

    } // end for index

} // end Translate_Asteroids

//////////////////////////////////////////////////////////////////////////////

void Rotate_Asteroids(void)
{

int index;

for (index=0; index<NUM_ASTEROIDS; index++)
    {
    // rotate current asteroid
    Rotate_Object((object_ptr)&asteroids[index], asteroids[index].angle);

    } // end for index

} // end Rotate_Asteroids


//////////////////////////////////////////////////////////////////////////////

void main(void)
{

// put the computer into graphics mode
_setvideomode(_VRES16COLOR); //  640x480 in 16 colors

// initialize
Create_Field();

while(!kbhit())
     {
     // erase field

     Draw_Asteroids(ERASE);

     // transform field

     Rotate_Asteroids();

     Translate_Asteroids();

     // draw field

     Draw_Asteroids(DRAW);

     // wait a bit since we aren't syncing or double buffering...nuff said

     Delay(500);


     } // end while

// place the computer back into text mode

_setvideomode(_DEFAULTMODE);

} // end main


