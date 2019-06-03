
// I N C L U D E S ////////////////////////////////////////////////////////////

#include <stdio.h>
#include <conio.h>

// D E F I N E S //////////////////////////////////////////////////////////////

#define CONTROL_8253  0x43  // the 8253's control register
#define CONTROL_WORD  0x3C  // the control word to set mode 2,binary least/most
#define COUNTER_0     0x40  // counter 0

#define TIMER_60HZ    0x4DAE // 60 hz
#define TIMER_30HZ    0x965C // 30 hz
#define TIMER_20HZ    0xE90B // 20 hz
#define TIMER_18HZ    0xFFFF // 18.2 hz (the standard count)

// M A C R O S ///////////////////////////////////////////////////////////////

#define LOW_BYTE(n) (n & 0x00ff)
#define HI_BYTE(n) ((n>>8) & 0x00ff)

// F U N C T I O N S //////////////////////////////////////////////////////////

Change_Time(unsigned int new_count)
{

// send the control word, mode 2, binary, least/most

_outp(CONTROL_8253, CONTROL_WORD);

// now write the least significant byte to the counter register

_outp(COUNTER_0,LOW_BYTE(new_count));

// and now the hi byte

_outp(COUNTER_0,HI_BYTE(new_count));


} // end Change_Time

// M A I N ////////////////////////////////////////////////////////////////////

main()
{

// reprogram the timer to 60 hz instead of 18.2 hz

Change_Time(TIMER_60HZ);

} // end main

