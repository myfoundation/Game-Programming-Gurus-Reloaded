
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <fcntl.h>
#include <memory.h>
#include <math.h>
#include <string.h>

// D E F I N E S  ////////////////////////////////////////////////////////////

#define ROM_CHAR_SET_SEG 0xF000  // segment of 8x8 ROM character set
#define ROM_CHAR_SET_OFF 0xFA6E  // begining offset of 8x8 ROM character set

#define VGA256            0x13
#define TEXT_MODE         0x03

#define PALETTE_MASK         0x3c6
#define PALETTE_REGISTER_RD  0x3c7
#define PALETTE_REGISTER_WR  0x3c8
#define PALETTE_DATA         0x3c9

#define SCREEN_WIDTH      (unsigned int)320
#define SCREEN_HEIGHT     (unsigned int)200

#define CHAR_WIDTH        8
#define CHAR_HEIGHT       8

#define SPRITE_WIDTH      24
#define SPRITE_HEIGHT     24
#define MAX_SPRITE_FRAMES 16
#define SPRITE_DEAD       0
#define SPRITE_ALIVE      1
#define SPRITE_DYING      2

// S T R U C T U R E S ///////////////////////////////////////////////////////

// this structure holds a RGB triple in three bytes

typedef struct RGB_color_typ
        {

        unsigned char red;    // red   component of color 0-63
        unsigned char green;  // green component of color 0-63
        unsigned char blue;   // blue  component of color 0-63

        } RGB_color, *RGB_color_ptr;

typedef struct pcx_header_typ
        {
        char manufacturer;
        char version;
        char encoding;
        char bits_per_pixel;
        int x,y;
        int width,height;
        int horz_res;
        int vert_res;
        char ega_palette[48];
        char reserved;
        char num_color_planes;
        int bytes_per_line;
        int palette_type;
        char padding[58];

        } pcx_header, *pcx_header_ptr;


typedef struct pcx_picture_typ
        {
        pcx_header header;
        RGB_color palette[256];
        char far *buffer;

        } pcx_picture, *pcx_picture_ptr;


typedef struct sprite_typ
        {
        int x,y;            // position of sprite
        int x_old,y_old;    // old position of sprite
        int width,height;   // dimensions of sprite in pixels
        int anim_clock;     // the animation clock
        int anim_speed;     // the animation speed
        int motion_speed;   // the motion speed
        int motion_clock;   // the motion clock

        char far *frames[MAX_SPRITE_FRAMES]; // array of pointers to the images
        int curr_frame;                      // current frame being displayed
        int num_frames;                      // total number of frames
        int state;                           // state of sprite, alive, dead...
        char far *background;                // whats under the sprite

        } sprite, *sprite_ptr;


// E X T E R N A L S /////////////////////////////////////////////////////////


extern Set_Mode(int mode);


// P R O T O T Y P E S ///////////////////////////////////////////////////////

void Set_Palette_Register(int index, RGB_color_ptr color);

void Plot_Pixel_Fast(int x,int y,unsigned char color);

void PCX_Init(pcx_picture *image);

void PCX_Delete(pcx_picture *image);

void PCX_Load(char *filename, pcx_picture_ptr image,int enable_palette);

void PCX_Show_Buffer(pcx_picture_ptr image);

// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned char far *video_buffer = (char far *)0xA0000000L; // vram byte ptr
unsigned int far *video_buffer_w= (int far *)0xA0000000L;  // vram word ptr
unsigned char far *rom_char_set = (char far *)0xF000FA6EL; // rom characters 8x8

// F U N C T I O N S /////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

void Blit_Char(int xc,int yc,char c,int color)
{
// this function uses the rom 8x8 character set to blit a character on the
// video screen, notice the trick used to extract bits out of each character
// byte that comprises a line

int offset,x,y;
unsigned char data;
char far *work_char;
unsigned char bit_mask = 0x80;

// compute starting offset in rom character lookup table

work_char = rom_char_set + c * CHAR_HEIGHT;

// compute offset of character in video buffer

offset = (yc << 8) + (yc << 6) + xc;

for (y=0; y<CHAR_HEIGHT; y++)
    {
    // reset bit mask

    bit_mask = 0x80;

    for (x=0; x<CHAR_WIDTH; x++)
        {
        // test for transparent pixel i.e. 0, if not transparent then draw

        if ((*work_char & bit_mask))
             video_buffer[offset+x] = color;

        // shift bit mask

        bit_mask = (bit_mask>>1);

        } // end for x

    // move to next line in video buffer and in rom character data area

    offset      += SCREEN_WIDTH;
    work_char++;

    } // end for y

} // end Blit_Char

//////////////////////////////////////////////////////////////////////////////

void Blit_String(int x,int y,int color, char *string)
{
// this function blits an entire string on the screen with fixed spacing
// between each character.  it calls blit_char.

int index;

for (index=0; string[index]!=0; index++)
     {

     Blit_Char(x+(index<<3),y,string[index],color);

     } /* end while */

} /* end Blit_String */

//////////////////////////////////////////////////////////////////////////////

void Delay(int t)
{

float x = 1;

while(t-->0)
     x=cos(x);

} // end Delay

//////////////////////////////////////////////////////////////////////////////

void Set_Palette_Register(int index, RGB_color_ptr color)
{

// this function sets a single color look up table value indexed by index
// with the value in the color structure

// tell VGA card we are going to update a pallete register

_outp(PALETTE_MASK,0xff);

// tell vga card which register we will be updating

_outp(PALETTE_REGISTER_WR, index);

// now update the RGB triple, note the same port is used each time

_outp(PALETTE_DATA,color->red);
_outp(PALETTE_DATA,color->green);
_outp(PALETTE_DATA,color->blue);

} // end Set_Palette_Color

//////////////////////////////////////////////////////////////////////////////

void PCX_Init(pcx_picture_ptr image)
{
// this function allocates the buffer region needed to load a pcx file

if (!(image->buffer = (char far *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT + 1)))

   printf("\ncouldn't allocate screen buffer");

} // end PCX_Init

//////////////////////////////////////////////////////////////////////////////

void Plot_Pixel_Fast(int x,int y,unsigned char color)
{

// plots the pixel in the desired color a little quicker using binary shifting
// to accomplish the multiplications

// use the fact that 320*y = 256*y + 64*y = y<<8 + y<<6

video_buffer[((y<<8) + (y<<6)) + x] = color;

} // end Plot_Pixel_Fast

//////////////////////////////////////////////////////////////////////////////

void PCX_Delete(pcx_picture_ptr image)
{
// this function de-allocates the buffer region used for the pcx file load

_ffree(image->buffer);

} // end PCX_Delete

//////////////////////////////////////////////////////////////////////////////

void PCX_Load(char *filename, pcx_picture_ptr image,int enable_palette)
{
// this function loads a pcx file into a picture structure, the actual image
// data for the pcx file is decompressed and expanded into a secondary buffer
// within the picture structure, the separate images can be grabbed from this
// buffer later.  also the header and palette are loaded

FILE *fp, *fopen();
int num_bytes,index;
long count;
unsigned char data;
char far *temp_buffer;

// open the file

fp = fopen(filename,"rb");

// load the header

temp_buffer = (char far *)image;

for (index=0; index<128; index++)
    {
    temp_buffer[index] = getc(fp);
    } // end for index

// load the data and decompress into buffer

count=0;

while(count<=SCREEN_WIDTH * SCREEN_HEIGHT)
     {
     // get the first piece of data

     data = getc(fp);

     // is this a rle?

     if (data>=192 && data<=255)
        {
        // how many bytes in run?

        num_bytes = data-192;

        // get the actual data for the run

        data  = getc(fp);

        // replicate data in buffer num_bytes times

        while(num_bytes-->0)
             {
             image->buffer[count++] = data;

             } // end while

        } // end if rle
     else
        {
        // actual data, just copy it into buffer at next location

        image->buffer[count++] = data;

        } // end else not rle

     } // end while

// move to end of file then back up 768 bytes i.e. to begining of palette

fseek(fp,-768L,SEEK_END);

// load the pallete into the palette

for (index=0; index<256; index++)
    {
    // get the red component

    image->palette[index].red   = (getc(fp) >> 2);

    // get the green component

    image->palette[index].green = (getc(fp) >> 2);

    // get the blue component

    image->palette[index].blue  = (getc(fp) >> 2);

    } // end for index

fclose(fp);

// change the palette to newly loaded palette if commanded to do so

if (enable_palette)
   {

   for (index=0; index<256; index++)
       {

       Set_Palette_Register(index,(RGB_color_ptr)&image->palette[index]);

       } // end for index

   } // end if change palette

} // end PCX_Load

//////////////////////////////////////////////////////////////////////////////

void PCX_Show_Buffer(pcx_picture_ptr image)
{
// just copy he pcx buffer into the video buffer

_fmemcpy((char far *)video_buffer,
         (char far *)image->buffer,SCREEN_WIDTH*SCREEN_HEIGHT);

} // end PCX_Show_Picture

//////////////////////////////////////////////////////////////////////////////

void Sprite_Init(sprite_ptr sprite,int x,int y,int ac,int as,int mc,int ms)
{
// this function initializes a sprite with the sent data

int index;

sprite->x            = x;
sprite->y            = y;
sprite->x_old        = x;
sprite->y_old        = y;
sprite->width        = SPRITE_WIDTH;
sprite->height       = SPRITE_HEIGHT;
sprite->anim_clock   = ac;
sprite->anim_speed   = as;
sprite->motion_clock = mc;
sprite->motion_speed = ms;
sprite->curr_frame   = 0;
sprite->state        = SPRITE_DEAD;
sprite->num_frames   = 0;
sprite->background   = (char far *)malloc(SPRITE_WIDTH * SPRITE_HEIGHT+1);

// set all bitmap pointers to null

for (index=0; index<MAX_SPRITE_FRAMES; index++)
    sprite->frames[index] = NULL;

} // end Sprite_Init

//////////////////////////////////////////////////////////////////////////////

void Sprite_Delete(sprite_ptr sprite)
{
// this function deletes all the memory associated with a sprire

int index;

_ffree(sprite->background);

// now de-allocate all the animation frames

for (index=0; index<MAX_SPRITE_FRAMES; index++)
    _ffree(sprite->frames[index]);

} // end Sprite_Delete


//////////////////////////////////////////////////////////////////////////////

void PCX_Grap_Bitmap(pcx_picture_ptr image,
                    sprite_ptr sprite,
                    int sprite_frame,
                    int grab_x, int grab_y)

{
// this function will grap a bitmap from the pcx frame buffer. it uses the
// convention that the 320x200 pixel matrix is sub divided into a smaller
// matrix of 12x8 adjacent squares each being a 24x24 pixel bitmap
// the caller sends the pcx picture along with the sprite to save the image
// into and the frame of the sprite.  finally, the position of the bitmap
// that should be grabbed is sent

int x_off,y_off, x,y, index;
char far *sprite_data;

// first allocate the memory for the sprite in the sprite structure

sprite->frames[sprite_frame] = (char far *)malloc(SPRITE_WIDTH * SPRITE_HEIGHT);

// create an alias to the sprite frame for ease of access

sprite_data = sprite->frames[sprite_frame];

// now load the sprite data into the sprite frame array from the pcx picture

// we need to find which bitmap to scan, remember the pcx picture is really a
// 12x8 matrix of bitmaps where each bitmap is 24x24 pixels. note:0,0 is upper
// left bitmap and 11,7 is the lower right bitmap.

x_off = 25 * grab_x + 1;
y_off = 25 * grab_y + 1;

// compute starting y address

y_off = y_off * 320;

for (y=0; y<SPRITE_HEIGHT; y++)
    {

    for (x=0; x<SPRITE_WIDTH; x++)
        {

        // get the next byte of current row and place into next position in
        // sprite frame data buffer

        sprite_data[y*24 + x] = image->buffer[y_off + x_off + x];

        } // end for x

        // move to next line of picture buffer

        y_off+=320;

    } // end for y

// increment number of frames

sprite->num_frames++;

// done!, let's bail!

} // end PCX_Grap_Bitmap

//////////////////////////////////////////////////////////////////////////////

void Behind_Sprite(sprite_ptr sprite)
{

// this function scans the background behind a sprite so that when the sprite
// is draw, the background isnn'y obliterated

char far *work_back;
int work_offset=0,offset,y;

// alias a pointer to sprite background for ease of access

work_back = sprite->background;

// compute offset of background in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<SPRITE_HEIGHT; y++)
    {
    // copy the next row out off screen buffer into sprite background buffer

    _fmemcpy((char far *)&work_back[work_offset],
             (char far *)&video_buffer[offset],
             SPRITE_WIDTH);

    // move to next line in video buffer and in sprite background buffer

    offset      += SCREEN_WIDTH;
    work_offset += SPRITE_WIDTH;

    } // end for y

} // end Behind_Sprite

//////////////////////////////////////////////////////////////////////////////

void Erase_Sprite(sprite_ptr sprite)
{
// replace the background that was behind the sprite

// this function replaces the background that was saved from where a sprite
// was going to be placed

char far *work_back;
int work_offset=0,offset,y;

// alias a pointer to sprite background for ease of access

work_back = sprite->background;

// compute offset of background in video buffer

offset = (sprite->y_old << 8) + (sprite->y_old << 6) + sprite->x_old;

for (y=0; y<SPRITE_HEIGHT; y++)
    {
    // copy the next row out off screen buffer into sprite background buffer

    _fmemcpy((char far *)&video_buffer[offset],
             (char far *)&work_back[work_offset],
             SPRITE_WIDTH);

    // move to next line in video buffer and in sprite background buffer

    offset      += SCREEN_WIDTH;
    work_offset += SPRITE_WIDTH;

    } // end for y


} // end Erase_Sprite

//////////////////////////////////////////////////////////////////////////////

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
             video_buffer[offset+x] = data;

        } // end for x

    // move to next line in video buffer and in sprite bitmap buffer

    offset      += SCREEN_WIDTH;
    work_offset += SPRITE_WIDTH;

    } // end for y

} // end Draw_Sprite


// M A I N ///////////////////////////////////////////////////////////////////


void main(void)
{

long index,redraw;
RGB_color color;
int frame_dir = 1;

pcx_picture town, cowboys;

sprite cowboy;

// set video mode to 320x200 256 color mode

Set_Mode(VGA256);

// set up the global pointers to screen ram

// Set_Screen_Pointers();

// load in background

PCX_Init((pcx_picture_ptr)&town);
PCX_Load("town.pcx", (pcx_picture_ptr)&town,1);
PCX_Show_Buffer((pcx_picture_ptr)&town);

PCX_Delete((pcx_picture_ptr)&town);

// print header

Blit_String(128, 24,50, "TOMBSTONE");

// load in the players imagery

PCX_Init((pcx_picture_ptr)&cowboys);
PCX_Load("cowboys.pcx", (pcx_picture_ptr)&cowboys,0);

// grab all the images from the cowboys pcx picture

Sprite_Init((sprite_ptr)&cowboy,SPRITE_WIDTH,100,0,7,0,3);

PCX_Grap_Bitmap((pcx_picture_ptr)&cowboys,(sprite_ptr)&cowboy,0,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&cowboys,(sprite_ptr)&cowboy,1,1,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&cowboys,(sprite_ptr)&cowboy,2,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&cowboys,(sprite_ptr)&cowboy,3,1,0);

// kill the pcx memory and buffers now that were done

PCX_Delete((pcx_picture_ptr)&cowboys);

Behind_Sprite((sprite_ptr)&cowboy);
Draw_Sprite((sprite_ptr)&cowboy);

// main loop

cowboy.state = SPRITE_ALIVE;

while(!kbhit())
     {

     redraw = 0; // used to flag if we need a redraw

     if (cowboy.state==SPRITE_ALIVE)
        {
        // test if its time change frames

        if (++cowboy.anim_clock > cowboy.anim_speed)
           {
           // reset the animation clock

           cowboy.anim_clock = 0;

           if (++cowboy.curr_frame >= cowboy.num_frames)
              {
              cowboy.curr_frame = 0;

              } // end if reached last frame

           redraw=1;

           } // end if time to change frames

        // now test if its time to move the cowboy

        if (++cowboy.motion_clock > cowboy.motion_speed)
           {
           // reset the motion clock

           cowboy.motion_clock = 0;

           // save old position

           cowboy.x_old = cowboy.x;

           redraw = 1;

           // move cowboy

           if (++cowboy.x >= SCREEN_WIDTH-2*SPRITE_WIDTH)
              {

              Erase_Sprite((sprite_ptr)&cowboy);
              cowboy.state = SPRITE_DEAD;
              redraw         = 0;

              } // end if reached last frame

           } // end if time to change frames

        } // end if cowboy alive
     else
        {
        // try and start up another cowboy

        if (rand()%100 == 0 )
           {
           cowboy.state      = SPRITE_ALIVE;
           cowboy.x          = SPRITE_WIDTH;
           cowboy.curr_frame = 0;
           cowboy.anim_speed   = 3 + rand()%6;
           cowboy.motion_speed = 1 + rand()%3;
           cowboy.anim_clock   = 0;
           cowboy.motion_clock = 0;

           Behind_Sprite((sprite_ptr)&cowboy);
           }

        } // end else dead, try to bring back to life

     // now the sprite has had it's state updated

     if (redraw)
        {
        // erase sprite at old position

        Erase_Sprite((sprite_ptr)&cowboy);

        // scan the background at new postition

        Behind_Sprite((sprite_ptr)&cowboy);

        // draw sprite at new position

        Draw_Sprite((sprite_ptr)&cowboy);

        // update old position

        cowboy.x_old = cowboy.x;
        cowboy.y_old = cowboy.y;

        } // end if sprites needed to be redrawn

     Delay(1000);

     } // end while

// make a cool clear screen, disolve screen, in one line, eye might add!

for (index=0; index<=300000; index++,Plot_Pixel_Fast(rand()%320, rand()%200, 0));

// go back to text mode

Set_Mode(TEXT_MODE);

} // end main



