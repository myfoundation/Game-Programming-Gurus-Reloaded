#include <conio.h>
#include <stdio.h>
#include <dos.h>

void main(void)
{
    int key;

    clrscr();
    while (key!=1)
    {
        key=inportb(0x60);
        gotoxy(1,1);
        printf("KEY: %d     ",key);
    }
}
