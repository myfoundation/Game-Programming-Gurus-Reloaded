
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>
#include "serlib.h"

// G L O B A L S /////////////////////////////////////////////////////////////

void (_interrupt _far *Old_Isr)();  // holds old com port interrupt handler


char ser_buffer[SERIAL_BUFF_SIZE];  // the receive buffer

int ser_end = -1,ser_start=-1;      // indexes into receive buffer
int ser_ch, char_ready=0;           // current character and ready flag
int old_int_mask;                   // the old interrupt mask on the PIC
int open_port;                      // the currently open port
int serial_lock = 0;                // serial ISR semaphore so the buffer
                                    // isn't altered will it is being written
                                    // to by the ISR

//////////////////////////////////////////////////////////////////////////////

void _interrupt _far Serial_Isr(void)
{

// this is the ISR (Interrupt Service Routine) for the com port.  It is very
// simple.  When it gets called, it gets the next character out of the receive
// buffer register 0 and places it into the software buffer. Note: C takes care
// of all the register saving and house work.  Cool huh!

// lock out any other functions so the buffer doesn't get corrupted

serial_lock = 1;

// place character into next position in buffer

ser_ch = _inp(open_port + SER_RBF);

// wrap buffer index around

if (++ser_end > SERIAL_BUFF_SIZE-1)
    ser_end = 0;

// move character into buffer

ser_buffer[ser_end] = ser_ch;

++char_ready;

// restore PIC

_outp(PIC_ICR,0x20);

// undo lock

serial_lock = 0;

} // end Serial_Isr

//////////////////////////////////////////////////////////////////////////////

int Ready_Serial()
{

// this functions returns true if there are any characters waiting and 0 if
// the buffer is empty

return(char_ready);

} // end Ready_Serial

//////////////////////////////////////////////////////////////////////////////

int Serial_Read()
{

// this function reads a character from the circulating buffer and returns it
// to the caller

int ch;

// wait for isr to end

while(serial_lock){}

// test if there is a character(s) ready in buffer

if (ser_end != ser_start)
   {

   // wrap buffer index if needed

   if (++ser_start > SERIAL_BUFF_SIZE-1)
       ser_start = 0;

   // get the character out of buffer

   ch = ser_buffer[ser_start];

   // one less character in buffer now

   if (char_ready > 0)
       --char_ready;

   // send data back to caller

   return(ch);

   } // end if a character is in buffer
else
   // buffer was empty return a NULL i.e. 0
   return(0);

} // end Serial_read


//////////////////////////////////////////////////////////////////////////////

Serial_Write(char ch)
{

// this function writes a character to the transmit buffer, but first it
// waits for the transmit buffer to be empty.  note: it is not interrupt
// driven and it turns of interrupts while it's working

// wait for transmit buffer to be empty

while(!(_inp(open_port + SER_LSR) & 0x20)){}

// turn off interrupts for a bit

_asm cli

// send the character

_outp(open_port + SER_THR, ch);

// turn interrupts back on

_asm sti

} // end Serial_Write

//////////////////////////////////////////////////////////////////////////////

Open_Serial(int port_base, int baud, int configuration)
{

// this function will open up the serial port, set it's configuration, turn
// on all the little flags and bits to make interrupts happen and load the
// ISR

// save the port for other functions

open_port = port_base;

// first set the baud rate

// turn on divisor latch registers

_outp(port_base + SER_LCR, SER_DIV_LATCH_ON);

// send low and high bytes to divsor latches

_outp(port_base + SER_DLL, baud);
_outp(port_base + SER_DLH, 0);

// set the configuration for the port

_outp(port_base + SER_LCR, configuration);

// enable the interrupts

_outp(port_base + SER_MCR, SER_GP02);

_outp(port_base + SER_IER, 1);

// hold off on enabling PIC until we have the ISR installed

if (port_base == COM_1)
   {
   Old_Isr = _dos_getvect(INT_SER_PORT_0);
   _dos_setvect(INT_SER_PORT_0, Serial_Isr);
   printf("\nOpening Communications Channel Com Port #1...\n");

   }
else
   {
   Old_Isr = _dos_getvect(INT_SER_PORT_1);
   _dos_setvect(INT_SER_PORT_1, Serial_Isr);
   printf("\nOpening Communications Channel Com Port #2...\n");
   }


// enable interrupt on PIC

old_int_mask = _inp(PIC_IMR);

_outp(PIC_IMR, (port_base==COM_1) ? (old_int_mask & 0xEF) : (old_int_mask & 0xF7 ));


} // Open_Serial

//////////////////////////////////////////////////////////////////////////////


Close_Serial(int port_base)
{

// this function closes the port which entails turning off interrupts and
// restoring the old interrupt vector

// disable the interrupts

_outp(port_base + SER_MCR, 0);

_outp(port_base + SER_IER, 0);

_outp(PIC_IMR, old_int_mask );

// reset old isr handler

if (port_base == COM_1)
   {
   _dos_setvect(INT_SER_PORT_0, Old_Isr);
   printf("\nClosing Communications Channel Com Port #1.\n");
   }
else
   {
   _dos_setvect(INT_SER_PORT_1, Old_Isr);
   printf("\nClosing Communications Channel Com Port #2.\n");
   }

} // end Close_Serial

//////////////////////////////////////////////////////////////////////////////

