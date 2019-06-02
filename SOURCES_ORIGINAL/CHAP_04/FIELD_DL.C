
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h> // include the basics
#include <graph.h> // include Microsofts Graphics Header
#include <math.h>  // include math stuff

// D E F I N E S   ///////////////////////////////////////////////////////////

#define NUM_ASTEROIDS 10
#define ERASE 0
#define DRAW  1

#define X_COMP 0
#define Y_COMP  1
#define N_COMP 2


// T Y P E D E F S ///////////////////////////////////////////////////////////

// new and improved vertex

typedef struct vertex_typ
           {
           float p[3];  // a single point in the 2-D plane with normalizing factor
           } vertex, *vertex_ptr;

// a general matrix structure

typedef struct matrix_typ
           {
           float elem[3][3];  // storage for a 3x3 martrix
           } matrix, *matrix_ptr;


// the structure for an object

typedef struct object_typ
           {
           int num_vertices;     // number of vertices in this object
           int color;            // color of object
           float xo,yo;          // position of object
           float x_velocity;     // x velocity of object
           float y_velocity;     // y velocity of object
           matrix scale;         // the object scaling matrix
           matrix rotation;      // the objects rotation and translation matrix

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

void Make_Identity(matrix_ptr i)
{

// makes the sent matrix into an identity matrix

i->elem[0][0] = i->elem[1][1] = i->elem[2][2] = 1;
i->elem[0][1] = i->elem[1][0] = i->elem[1][2] = 0;
i->elem[2][0] = i->elem[0][2] = i->elem[2][1] = 0;


} // end Make_Identity

//////////////////////////////////////////////////////////////////////////////

void Clear_Matrix(matrix_ptr m)
{

// zeros out the sent matrix

m->elem[0][0] = m->elem[1][1] = m->elem[2][2] = 0;
m->elem[0][1] = m->elem[1][0] = m->elem[1][2] = 0;
m->elem[2][0] = m->elem[0][2] = m->elem[2][1] = 0;


} // end Clear_Matrix

//////////////////////////////////////////////////////////////////////////////


void Mat_Mul(vertex_ptr v,matrix_ptr m)
{

// do a multiplication of a 1x3 * 3x3 the result is again a 1x3
// for speed manually do the multiplication by specifying each multiplication
// and addition manually (apprentice trick)

float x_new, y_new;

x_new = v->p[0]*m->elem[0][0] + v->p[1]*m->elem[1][0] + m->elem[2][0];
y_new = v->p[0]*m->elem[0][1] + v->p[1]*m->elem[1][1] + m->elem[2][1];

v->p[X_COMP] = x_new;
v->p[Y_COMP] = y_new;

// note we need not change N_COMP since it is always 1

} // end Mat_Mul

//////////////////////////////////////////////////////////////////////////////


void Scale_Object_Mat(object_ptr obj)
{

int index;

// scale the object, just multiply each point in the object by it's scaling
// matrix

for (index=0; index<obj->num_vertices; index++)
    {

    Mat_Mul((vertex_ptr)&obj->vertices[index],(matrix_ptr)&obj->scale);

    } // end for index

} // end Scale_Oject_Mat


//////////////////////////////////////////////////////////////////////////////

Rotate_Object_Mat(object_ptr obj)
{

int index;

// rotate the object, just multiply each point in the object by it's rotation
// matrix

for (index=0; index<obj->num_vertices; index++)
    {

    Mat_Mul((vertex_ptr)&obj->vertices[index],(matrix_ptr)&obj->rotation);

    } // end for index

} // end Rotate_Oject_Mat


//////////////////////////////////////////////////////////////////////////////

void Create_Field(void)
{

int index;
float angle,c,s;

// this function creates the asteroid field

for (index=0; index<NUM_ASTEROIDS; index++)
    {

    // fill in the fields

    asteroids[index].num_vertices = 6;
    asteroids[index].color = 1  + rand() % 14; // always visable
    asteroids[index].xo    = 41 + rand() % 599;
    asteroids[index].yo    = 41 + rand() % 439;
    asteroids[index].x_velocity = -10 + rand() % 20;
    asteroids[index].y_velocity = -10 + rand() % 20;

    // clear out matrix

    Make_Identity((matrix_ptr)&asteroids[index].rotation);

    // now setup up rotation  matrix

    angle = (float)(- 50 + (float)(rand() % 100)) / 100;

    c=cos(angle);
    s=sin(angle);

    asteroids[index].rotation.elem[0][0] = c;
    asteroids[index].rotation.elem[0][1] = -s;
    asteroids[index].rotation.elem[1][0] = s;
    asteroids[index].rotation.elem[1][1] = c;

    // set up scaling  matrix

    // clear out matrix

    Make_Identity((matrix_ptr)&asteroids[index].scale);

    asteroids[index].scale.elem[0][0] = (float)(rand() % 30) / 10;
    asteroids[index].scale.elem[1][1] = asteroids[index].scale.elem[0][0];


    asteroids[index].vertices[0].p[X_COMP] = 4.0;
    asteroids[index].vertices[0].p[Y_COMP] = 3.5;
    asteroids[index].vertices[0].p[N_COMP] = 1;

    asteroids[index].vertices[1].p[X_COMP] = 8.5;
    asteroids[index].vertices[1].p[Y_COMP] = -3.0;
    asteroids[index].vertices[1].p[N_COMP] = 1;

    asteroids[index].vertices[2].p[X_COMP] = 6;
    asteroids[index].vertices[2].p[Y_COMP] = -5;
    asteroids[index].vertices[2].p[N_COMP] = 1;

    asteroids[index].vertices[3].p[X_COMP] = 2;
    asteroids[index].vertices[3].p[Y_COMP] = -3;
    asteroids[index].vertices[3].p[N_COMP] = 1;

    asteroids[index].vertices[4].p[X_COMP] = -4;
    asteroids[index].vertices[4].p[Y_COMP] = -6;
    asteroids[index].vertices[4].p[N_COMP] = 1;

    asteroids[index].vertices[5].p[X_COMP] = -3.5;
    asteroids[index].vertices[5].p[Y_COMP] = 5.5;
    asteroids[index].vertices[5].p[N_COMP] = 1;

    // now scale the asteroid to proper size

    Scale_Object_Mat((object_ptr)&asteroids[index]);

    } // end for index

} // end Create_Field

//////////////////////////////////////////////////////////////////////////////

void Draw_Asteroids(int erase)
{

int index,vertex;
float xo,yo;

// this function draws the asteroids or erases them depending on the sent flag

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

    _moveto((int)(xo+asteroids[index].vertices[0].p[X_COMP]),
            (int)(yo+asteroids[index].vertices[0].p[Y_COMP]));

    for (vertex=1; vertex<asteroids[index].num_vertices; vertex++)
        {
        _lineto((int)(xo+asteroids[index].vertices[vertex].p[X_COMP]),
                (int)(yo+asteroids[index].vertices[vertex].p[Y_COMP]));

        } // end for vertex

    // close object

    _lineto((int)(xo+asteroids[index].vertices[0].p[X_COMP]),
            (int)(yo+asteroids[index].vertices[0].p[Y_COMP]));


    } // end for index

} // end Draw_Asteroids

//////////////////////////////////////////////////////////////////////////////

void Translate_Asteroids(void)
{

int index;

// this function moves the asteroids

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

void Rotate_Asteroids()
{

int index;

for (index=0; index<NUM_ASTEROIDS; index++)
    {
    // rotate current asteroid
    Rotate_Object_Mat((object_ptr)&asteroids[index]);

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