
// M O U S E  L I B R A R Y  H E A D E R //////////////////////////////////////

// D E F I N E S  ////////////////////////////////////////////////////////////

// mouse sub-function calls

#define MOUSE_INT                0x33 //mouse interrupt number
#define MOUSE_RESET              0x00 // reset the mouse
#define MOUSE_SHOW               0x01 // show the mouse
#define MOUSE_HIDE               0x02 // hide the mouse
#define MOUSE_BUTT_POS           0x03 // get buttons and postion
#define MOUSE_SET_SENSITIVITY    0x1A // set the sensitivity of mouse 0-100
#define MOUSE_MOTION_REL         0x0B // query motion counters to compute
                                      // relative motion

// defines to make reading buttons easier

#define MOUSE_LEFT_BUTTON        0x01 // left button mask
#define MOUSE_RIGHT_BUTTON       0x02 // right button mask
#define MOUSE_CENTER_BUTTON      0x04 // center button mask

// P R O T O T Y P E S ///////////////////////////////////////////////////////

int Squeeze_Mouse(int command, int *x, int *y,int *buttons);


