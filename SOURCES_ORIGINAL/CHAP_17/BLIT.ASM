    ideal
    model compact,c
    p286
    dataseg

    VIEW_WIDTH  equ 320
    VIEW_HEIGHT equ 100
    TRANSPARENT equ 0
    global MemBuf:dword

    codeseg

    public OpaqueBlt
    public TransparentBlt

;
;  This routine copies a bitmap to MemBuf.  It also scrolls
;  the bitmap left and right depending on the status of ScrollSplit.
;
    proc OpaqueBlt
    ARG Bitmap:dword,StartY:word,Height:word,ScrollSplit:word
    USES si,di

      les   di,[MemBuf]             ; get pointer to memory buffer
      mov   ax,[StartY]             ; get starting y coordinate
      mov   bx,ax                   ; make a copy
      sal   ax,6                    ; mult by 64
      sal   bx,8                    ; mult by 256
      add   ax,bx                   ; result is same as mult by 320
      add   di,ax                   ; calc offset into MemBuf

      mov   bx,[Height]             ; get height of bitmap
      mov   dx,[ScrollSplit]        ; get bitmap right half length
      push  ds                      ; save data segment
      lds   si,[Bitmap]             ; get full pointer to bitmap

      mov   ax,VIEW_WIDTH           ; get screen width
      sub   ax,dx                   ; calc left half length
      cld                           ; set direction to forward
@@loop01:
      add   di,dx                   ; calc screen starting pos
      mov   cx,ax                   ; get left half length
      shr   cx,1                    ; turn into words
      rep   movsw                   ; draw left half of bitmap
      jnc   short @@skip01          ; skip if done
      movsb                         ; move last pixel
@@skip01:

      sub   di,VIEW_WIDTH           ; adjust screen pointer offset
      mov   cx,dx                   ; get right half length
      shr   cx,1                    ; turn cx into words
      rep   movsw                   ; draw right half of bitmap
      jnc   short @@skip02          ; skip if done
      movsb                         ; move last pixel
@@skip02:

      add   di,ax                   ; add right width to get next row
      dec   bx                      ; decrement row counter
      jnz   short @@loop01          ; do it again

      pop   ds                      ; restore data segment
      ret                           ; back to the caller
    endp OpaqueBlt

;
;  This routine copies a bitmap to MemBuf.  It also scrolls the bitmap
;  left and right depending on the status of ScrollSplit.  This routine
;  does not draw pixels if the color index is equal to TRANSPARENT.
;
    proc TransparentBlt
    ARG Bitmap:dword,StartY:word,Height:word,ScrollSplit:word
    USES si,di

      les   di,[MemBuf]          ; get pointer to memory buffer
      mov   ax,[StartY]          ; get starting y coordinate
      mov   cx,ax                ; make a copy
      sal   ax,6                 ; mult by 64
      sal   cx,8                 ; mult by 256
      add   ax,cx                ; result is same as mult by 320
      add   di,ax                ; add offset to MemBuf

      mov   dx,[ScrollSplit]     ; get bitmap left half length
      mov   bx,VIEW_WIDTH        ; get view width
      sub   bx,dx                ; calc right half length

      push  ds                   ; save data segment
      lds   si,[Bitmap]          ; get full pointer to bitmap
@@loop01:
      add   di,dx                ; calc screen starting pos
      mov   cx,bx                ; get right half length

;
;  Draw the right half
;
@@loop02:
      mov   al,[si]               ; get bitmap pixel
      inc   si                    ; point to next pixel
      cmp   al,TRANSPARENT        ; is pixel transparent?
      je    short @@skip01        ; ...yes so skip next instruction
      mov   [es:di],al            ; otherwise draw it
@@skip01:
      inc   di                    ; point to next byte of MemBuf
      dec   cx                    ; decrement counter
      jnz   short @@loop02

      sub   di,VIEW_WIDTH         ; adjust screen pointer offset
      mov   cx,dx                 ; get left half length

;
;  Draw the left half
;
@@loop03:
      mov   al,[si]               ; get bitmap pixel
      inc   si                    ; point to next pixel
      cmp   al,TRANSPARENT        ; is pixel transparent?
      je    short @@skip02        ; ...yes so skip next instruction
      mov   [es:di],al            ; otherwise draw it
@@skip02:
      inc   di                    ; point to next byte of MemBuf
      dec   cx                    ; decrement counter
      jnz   short @@loop03        ; loop till done

      add   di,bx                 ; add right width to get next row
      dec   [Height]              ; decrement row counter
      jnz   short @@loop01        ; do it again

      pop   ds                    ; restore data segment
      ret                         ; back to the caller
    endp TransparentBlt

    end
