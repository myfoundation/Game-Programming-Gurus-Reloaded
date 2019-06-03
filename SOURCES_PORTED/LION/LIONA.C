#include <assert.h>

#define SPRITE_WIDTH      62
#define SPRITE_HEIGHT     40
#define MAX_SPRITE_FRAMES 18

#define SPRITE_STAY_L       0
#define SPRITE_STAY_R       1
#define SPRITE_GO_L      2
#define SPRITE_GO_R      3

extern char* MemBuf;

typedef struct sprite_typ
	{
	int x,y;            // position of sprite
	int x_old,y_old;    // old position of sprite
	int width,height;   // dimensions of sprite in pixels
/*
	int anim_clock;     // the animation clock
	int anim_speed;     // the animation speed
	int motion_speed;   // the motion speed
	int motion_clock;   // the motion clock
*/
	char far *frames[MAX_SPRITE_FRAMES]; // array of pointers to the images
	int curr_frame;                      // current frame being displayed
	int num_frames;                      // total number of frames
	int state;                           // state of sprite, alive, dead...
//	char far *background;                // whats under the sprite

	int pcxOffsetX[MAX_SPRITE_FRAMES];
	int pcxOffsetY[MAX_SPRITE_FRAMES];

	} sprite, *sprite_ptr;


void Sprite_Init(sprite_ptr sprite,int x,int y/*,int ac,int as,int mc,int ms*/)
{
// this function initializes a sprite with the sent data

int index;

sprite->x            = x;
sprite->y            = y;
sprite->x_old        = x;
sprite->y_old        = y;
sprite->width        = SPRITE_WIDTH;
sprite->height       = SPRITE_HEIGHT;
/*
sprite->anim_clock   = ac;
sprite->anim_speed   = as;
sprite->motion_clock = mc;
sprite->motion_speed = ms;
*/
sprite->curr_frame   = 0;
sprite->state        = SPRITE_STAY_L;
sprite->num_frames   = 0;
//sprite->background   = (char far *)malloc(SPRITE_WIDTH * SPRITE_HEIGHT+1);

// set all bitmap pointers to null

for (index=0; index<MAX_SPRITE_FRAMES; index++)
{
    sprite->frames[index] = NULL;
    sprite->pcxOffsetX[index] = 0;
    sprite->pcxOffsetY[index] = 0;
}

} // end Sprite_Init


void Sprite_Delete(sprite_ptr sprite)
{
// this function deletes all the memory associated with a sprire

int index;

// _ffree(sprite->background);

// now de-allocate all the animation frames

for (index=0; index<MAX_SPRITE_FRAMES; index++)
    free(sprite->frames[index]);

} // end Sprite_Delete


void PCX_Grap_Bitmap(char* image,
		    sprite_ptr sprite,
		    int sprite_frame,
		    int x_off, int y_off)

{
int x,y, index;
char far *sprite_data;

// first allocate the memory for the sprite in the sprite structure

sprite->frames[sprite_frame] = (char far *)malloc(SPRITE_WIDTH * SPRITE_HEIGHT);
assert(sprite->frames[sprite_frame]);

// create an alias to the sprite frame for ease of access

sprite_data = sprite->frames[sprite_frame];

// now load the sprite data into the sprite frame array from the pcx picture

y_off*=320;

for (y=0; y<SPRITE_HEIGHT; y++)
    {

    for (x=0; x<SPRITE_WIDTH; x++)
	{

	// get the next byte of current row and place into next position in
	// sprite frame data buffer

	sprite_data[y*62 + x] = image[y_off + x_off + x];

	} // end for x

	// move to next line of picture buffer

	y_off+=320;

    } // end for y

// increment number of frames

sprite->num_frames++;

// done!, let's bail!

} // end PCX_Grap_Bitmap



void Draw_Sprite(sprite_ptr sprite)
{

// this function draws a sprite on the screen row by row very quickly
// note the use of shifting to implement multplication

char far *work_sprite;
int work_offset=0,offset,x,y;
unsigned char data;

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<SPRITE_HEIGHT; y++)
    {
    // copy the next row into the screen buffer using memcpy for speed

    for (x=0; x<SPRITE_WIDTH; x++)
	{

        // test for transparent pixel i.e. 0, if not transparent then draw

	if ((data=work_sprite[work_offset+x]))
	     MemBuf[offset+x] = data;

	} // end for x

    // move to next line in video buffer and in sprite bitmap buffer

    offset      += VIEW_WIDTH;
    work_offset += SPRITE_WIDTH;

    } // end for y

} // end Draw_Sprite


int lionStayPcxOffset[12][2]={ //x    y
			       62*0,  0,
			       62*1,  0,
			       62*2,  0,
			       62*3,  0,
			       62*4,  0,
			       62*0,  40,

			       62*0,  80,
			       62*1,  80,
			       62*2,  80,
			       62*3,  80,
			       62*4,  80,
			       62*0,  120
			      };

int lionGoPcxOffset[18][2]=  { //x    y
			       62*0,  0,
			       62*1,  0,
			       62*2,  0,
			       62*3,  0,
			       62*4,  0,
			       62*0,  40,
			       62*1,  40,
			       62*2,  40,
			       62*3,  40,

			       62*0,  80,
			       62*1,  80,
			       62*2,  80,
			       62*3,  80,
			       62*4,  80,
			       62*0,  120,
			       62*1,  120,
			       62*2,  120,
			       62*3,  120
			      };

void MakePcxOffset(sprite_ptr sprite,int PcxOffset[][2],int frames)
{
	int i;
	for(i=0;i<frames;i++)
	{
		sprite->pcxOffsetX[i]=PcxOffset[i][0];
		sprite->pcxOffsetY[i]=PcxOffset[i][1];
	}
}
