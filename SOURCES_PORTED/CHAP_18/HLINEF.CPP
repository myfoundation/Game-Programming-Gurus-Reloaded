//////////////////////////////////////////////////////////////////////////////

H_Line_Fast(int x1,int x2,int y,unsigned int color)
{
// a fast horizontal line renderer uses word writes instead of byte writes
// the only problem is the endpoints of the h line must be taken into account.
// test if the endpoints of the horizontal line are on word boundaries i.e.
// they are envenly divisible by 2
// basically, we must consider the two end points of the line separately
// if we want to write words at a time or in other words two pixels at a time
// note x2 > x1

unsigned int first_word,
            middle_word,
              last_word,
            line_offset,
                   index;


// test the 1's bit of the starting x

if ( (x1 & 0x0001) )
   {

   first_word = (color<<8);

   } // end if starting point is on a word boundary
else
   {
   // replicate color in to both bytes
   first_word = ((color<<8) | color);

   } // end else

// test the 1's bit of the ending x

if ( (x2 & 0x0001) )
   {

   last_word = ((color<<8) | color);

   } // end if ending point is on a word boundary
else
   {
   // place color in high byte of word only

   last_word = color;

   } // end else

// now we can draw the horizontal line two pixels at a time

line_offset = ((y<<7) + (y<<5));  // y*160, since there are 160 words/line

// compute middle color

middle_word = ((color<<8) | color);

// left endpoint

video_buffer_w[line_offset + (x1>>1)] = first_word;

// the middle of the line

for (index=(x1>>1)+1; index<(x2>>1); index++)
    video_buffer_w[line_offset+index] = middle_word;

// right endpoint

video_buffer_w[line_offset + (x2>>1)] = last_word;

} // end H_Line_Fast

//////////////////////////////////////////////////////////////////////////////
