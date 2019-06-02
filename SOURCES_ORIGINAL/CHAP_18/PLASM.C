//////////////////////////////////////////////////////////////////////////////

Plot_Pixel_Asm(int x,int y,int color)
{

// plots the pixel in the desired color a little quicker using binary shifting and assembly
// do it assembly

   _asm
      {
      les di,video_buffer   // create a pointer to the screen buffer
      mov di,y              // get the row
      shl di,6              // multiply by 64
      mov bx,di             // save result
      shl di,2              // two more times equals 256
      add di,bx             // combine result
      add di, x             // add x component
      mov al,BYTE PTR color // need to move color into a register
      mov es:[di], al       // plot the pixel
      }  // end assembly

} // end Plot_Pixel_Asm

//////////////////////////////////////////////////////////////////////////////
