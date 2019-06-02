
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>


// D E F I N E S  ////////////////////////////////////////////////////////////


#define JOYPORT      0x201  // joyport is at 201 hex

#define BUTTON_1_A   0x10   // joystick 1, button A
#define BUTTON_1_B   0x20   // joystick 1, button B
#define BUTTON_2_A   0x40   // joystick 2, button A
#define BUTTON_2_B   0x80   // joystick 2, button B

#define JOYSTICK_1_X 0x01   // joystick 1, x axis
#define JOYSTICK_1_Y 0x02   // joystick 1, y axis
#define JOYSTICK_2_X 0x04   // joystick 2, x axis
#define JOYSTICK_2_Y 0x08   // joystick 2, y axis


#define JOY_1_CAL       1   // command to calibrate joystick #1
#define JOY_2_CAL       2   // command to calibrate joystick #2



// G L O B A L S  ////////////////////////////////////////////////////////////

unsigned int joy_1_max_x,   // global joystick calibration variables
             joy_1_max_y,
             joy_1_min_x,
             joy_1_min_y,
             joy_1_cx,
             joy_1_cy,
             joy_2_max_x,
             joy_2_max_y,
             joy_2_min_x,
             joy_2_min_y,
             joy_2_cx,
             joy_2_cy;



// F U N C T I O N S /////////////////////////////////////////////////////////

unsigned char Buttons(unsigned char button)
{
// read the joystick buttons by peeking the port that the switches are attached
// to.

outp(JOYPORT,0); // clear the latch and request a sample

// invert buttons then mask with request

return( ~inp(JOYPORT) & button);

} // end Buttons

//////////////////////////////////////////////////////////////////////////////

unsigned int Joystick(unsigned char stick)
{
// reads the joystick values manually by conting how long the capacitors take
// to charge/discharge
// let's use the inline assembler.  It's Cool!

__asm
    {
    cli                    ; disable interupts

    mov ah, byte ptr stick ; get mask into ah to selct joystick to read
    xor al,al              ; zero out al, xor is a trick
    xor cx,cx              ; same with cx which we will use as a counter
    mov dx,JOYPORT         ; dx is used by inp and outp
    out dx,al              ; write 0's to the port
discharge:
    in al,dx               ; read the data back from port
    test al,ah             ; has the bit in question changed?
    loopne discharge       ; if the stick isn't ready then --cx and loop

    sti                    ; re-enable interrupts
    xor ax,ax              ; zero out ax
    sub ax,cx              ; ax now holds the position of the axis switch

    } // end asm

// since ax has the result the function will return it properly

} // end Joystick

//////////////////////////////////////////////////////////////////////////////

unsigned int Joystick_Bios(unsigned char stick)
{
// bios version of joystick read

union _REGS inregs, outregs;

inregs.h.ah = 0x84; // joystick function 84h
inregs.x.dx = 0x01; // read joysticks subfunction 1h

// call dos

_int86(0x15,&inregs, &outregs);

// return proper value depending on sent command

switch(stick)
      {
      case JOYSTICK_1_X:
           {
           return(outregs.x.ax);
           } break;

      case JOYSTICK_1_Y:
           {
           return(outregs.x.bx);
           } break;

      case JOYSTICK_2_X:
           {
           return(outregs.x.cx);
           } break;

      case JOYSTICK_2_Y:
           {
           return(outregs.x.dx);
           } break;

      default:break;

      } // end switch stick

} // end Joystick_Bios


//////////////////////////////////////////////////////////////////////////////

unsigned char Buttons_Bios(unsigned char button)
{
// bios version of buttons read

union _REGS inregs, outregs;

inregs.h.ah = 0x84; // joystick function 84h
inregs.x.dx = 0x00; // read buttons subfunction 0h

// call dos

_int86(0x15,&inregs, &outregs);

// invert buttons then mask with request

return( (~outregs.h.al) & button);

} // end Buttons_Bios

//////////////////////////////////////////////////////////////////////////////

void Joystick_Calibrate(int stick)
{
// calibrates the joystick by finding the min and max deflections in both the
// X and Y axis.  Then stores it in a global data structure for future use.

unsigned int x_new,y_new; // temp joystick positions

// set vars so that we can find there actual values

if (stick==JOY_1_CAL)
   {

   printf("\nCalibrating Joystick #1: Swirl stick then release and press fire");

   // set calibrations to impossible values

   joy_1_max_x=0;
   joy_1_max_y=0;
   joy_1_min_x=10000;
   joy_1_min_y=10000;

   // now the user should shwirl joystick let the stick fall neutral then press
   // any button

   while(!Buttons(BUTTON_1_A | BUTTON_1_B))
        {
        // get the new values and try to update calibration
        x_new = Joystick_Bios(JOYSTICK_1_X);
        y_new = Joystick_Bios(JOYSTICK_1_Y);

        // process X - axis

        if (x_new >= joy_1_max_x)
            joy_1_max_x = x_new;

        if (x_new <= joy_1_min_x)
            joy_1_min_x = x_new;

        // process Y - axis

        if (y_new >= joy_1_max_y)
            joy_1_max_y = y_new;

        if (y_new <= joy_1_min_y)
            joy_1_min_y = y_new;

        } // end while

        // user has let stick go to center so that must be the center

        joy_1_cx = x_new;
        joy_1_cy = y_new;

   } // end calibrate joystick #1
else
if (stick==JOY_2_CAL)
   {
   printf("\nCalibrating Joystick #2: Swirl stick then release and press fire");

   // set calibrations to impossible values

   joy_2_max_x=0;
   joy_2_max_y=0;
   joy_2_min_x=10000;
   joy_2_min_y=10000;

   // now the user should shwirl joystick let the stick fall neutral then press
   // any button

   while(!Buttons(BUTTON_2_A | BUTTON_2_B))
        {
        // get the new values and try to update calibration
        x_new = Joystick(JOYSTICK_2_X);
        y_new = Joystick(JOYSTICK_2_Y);

        // process X - axis

        if (x_new >= joy_2_max_x)
            joy_2_max_x = x_new;
        else
        if (x_new <= joy_2_min_x)
            joy_2_min_x = x_new;

        // process Y - axis

        if (y_new >= joy_2_max_y)
            joy_2_max_y = y_new;
        else
        if (y_new <= joy_2_min_y)
            joy_2_min_y = y_new;

        } // end while

        // user has let stick go to center so that must be the center

        joy_2_cx = x_new;
        joy_2_cy = y_new;


   } // end calibrate joystick #2

printf("\nCalibration Complete...hit any key to continue.");

getch();

} // end Joystick_Calibrate

//////////////////////////////////////////////////////////////////////////////

void main(void) // to test the joystick interface
{

// calibrate the joystick

Joystick_Calibrate(JOY_1_CAL);

_clearscreen(_GCLEARSCREEN);

// let user fiddle with the joystick

while(!kbhit())
     {

     _settextposition(2,0);

     printf("Joystick 1 = [%u,%u]    ",Joystick_Bios(JOYSTICK_1_X),Joystick_Bios(JOYSTICK_1_Y));

     if (Buttons_Bios(BUTTON_1_A))
        printf("\nButton 1 pressed   ");
     else
     if (Buttons_Bios(BUTTON_1_B))
        printf("\nButton 2 pressed   ");
     else
        printf("\nNo Button Pressed  ");
     } // end while

// let user know what the calibrations turned out to be

printf("\nmax x=%u, max y=%u,min x=%u,min y=%u,cx=%u,cy=%u",joy_1_max_x,
                                                            joy_1_max_y,
                                                            joy_1_min_x,
                                                            joy_1_min_y,
                                                            joy_1_cx,
                                                            joy_1_cy);

// later!

} // end main


