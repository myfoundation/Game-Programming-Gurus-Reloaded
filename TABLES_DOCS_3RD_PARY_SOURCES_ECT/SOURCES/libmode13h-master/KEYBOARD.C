/*
void interrupt (*oldint9)();
void interrupt int9(void);
void InitKeys(void);
void UnInitKeys(void);
*/

#define Up             { 72, 200 }
#define Down           { 80, 208 }
#define Left           { 75, 203 }
#define Right          { 77, 205 }
#define Space          { 57, 185 }

#define NumOfKeys   5

char KeyStatus[NumOfKeys];              // 1=pressed, 0=released

typedef struct                          /* This holds a list of key scan   */
{                                       /* codes that we want to trap.     */
   unsigned char downcode;              /* Code when key is pressed        */
   unsigned char upcode;                /* Code when key is released       */
} KeyMap;

KeyMap Keys[NumOfKeys] =
{
   Up,
   Down,
   Left,
   Right,
   Space
};

/* Macro to acknowledge a keystroke */
#define AckKey  _AL=_AH = inportb( 0x61 ); \
                _AL|= 0x80;                \
                outportb( 0x61,_AL );      \
                outportb( 0x61,_AH );      \
                outportb( 0x20, 0x20 )

/*****************************************************************************
  The int 9 handler.  This routine fires everytime a key is pressed or
  released on the keyboard.  Since each key generates a unique (more or less)
  scan code both when it's pressed, and a different one when it's released,
  we can actually have multiple keys being held down simultaneously.

  This routine simple loops through the list you define above, checking the
  current scan code against the up and down codes.  If it matches one, then
  it sets or resets the appropriate byte in the vglKeyStatus[] array.
*****************************************************************************/
void interrupt int9(void)
{
    register i;
    unsigned char c;

    c=inportb(0x60);                    /* Get the scan code               */

    if (c==1)
        _chain_intr(oldint9);

    AckKey;
    for(i=0;i<NumOfKeys;i++)            /* Loop through the table          */
    {
      if(c==Keys[i].downcode)           /* Is it a downcode?               */
      {
         KeyStatus[i]=1;                /* Set the status byte to 1        */
         break;                         /* We're done!                     */
      }
      if(c==Keys[i].upcode)             /* Is it an upcode?                */
      {
         KeyStatus[i]=0;                /* Reset the status byte to 0      */
         break;                         /* We're done!                     */
      }
   }
}

void InitKeys(void)
{
    int i;

    for(i=0;i<NumOfKeys;i++)        /* Initialize the status bytes      */
        KeyStatus[i]=0;

    oldint9 = getvect(0x09);        /* Save the current int 9 handler   */
    setvect(0x09,int9);             /* Install out int 9 handler        */
}

void UnInitKeys(void)
{
    int i;

    for(i=0;i<NumOfKeys;i++)        /* Initialize the status bytes      */
        KeyStatus[i]=0;

   setvect( 0x09, oldint9 );            /* Restore the old int 9 handler    */
}
