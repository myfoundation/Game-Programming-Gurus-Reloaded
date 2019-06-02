
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <graph.h>
#include "keylib.h"

// F U N C T I O N S /////////////////////////////////////////////////////////

unsigned char Get_Ascii_Key()

{

// if there is a normal ascii key waiting then return it, else return 0

if (_bios_keybrd(_KEYBRD_READY))
 return(_bios_keybrd(_KEYBRD_READ));
else return(0);

} // end Get_Ascii_Key

//////////////////////////////////////////////////////////////////////////////

unsigned int Get_Control_Keys(unsigned int mask)
{
// return the status of all the requested control key

return(mask & _bios_keybrd(_KEYBRD_SHIFTSTATUS));

} // end Get_Control_Keys

//////////////////////////////////////////////////////////////////////////////

unsigned char Get_Scan_Code()
{
// get the scan code of a key press, since we have to look at status bits
// let's use the inline assembler


// is a key ready?

__asm
    {
    mov ah,01h          ; function 1: is a key ready?
    int 16h             ; call the interrupt
    jz empty            ; there was no key so exit
    mov ah,00h          ; function 0: get the scan code please
    int 16h             ; call the interrupt
    mov al,ah           ; result was in ah so put into al
    xor ah,ah           ; zero out ah
    jmp done            ; data's in ax...let's blaze!

empty:
     xor ax,ax          ; clear out ax i.e. 0 means no key
done:

    } // end asm

// since data is in ax it will be returned properly

} // end Get_Scan_Code

//////////////////////////////////////////////////////////////////////////////
