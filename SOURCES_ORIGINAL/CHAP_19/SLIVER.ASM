
;/////////////////////////////////////////////////////////////////////////////
;
;This function is an assembly language version of the C function that renders
;a single textured sliver into the double buffer.  It uses the pre-computed
;look up table for the scale indices to speed things up, moreover it uses
;global variables instead of variables passed on the stack.  I highly suggest
;you understand the the C version first before trying this one!
;
;////////////////////////////////////////////////////////////////////////////

.MODEL MEDIUM,C             ; use medium memory model and C function names

.CODE                       ; begin code segment

EXTRN double_buffer:DWORD   ; the external double buffer
EXTRN sliver_texture:DWORD  ; a pointer to the texture memory
EXTRN sliver_column:WORD    ; the current texture column
EXTRN sliver_top:WORD       ; the starting Y of the sliver
EXTRN sliver_scale:WORD     ; the over all height of the sliver
EXTRN sliver_ray:WORD       ; the current video column
EXTRN sliver_clip:WORD      ; how much of the texture is being clipped
EXTRN scale_row:WORD        ; the pointer to the proper row of pre-computed scale
                            ; indices


PUBLIC Render_Sliver_32     ; export the function to the linker

Render_Sliver_32 PROC FAR C ; this is a C function and is far

.386                             ; use 80386 instructions, we need them for the
                                 ; extra segments fs,gs

        push si                  ; save registers we will obliterate
        push di

        les di, double_buffer    ; point es:di to double buffer

        mov dx,sliver_column     ; hold the column in dx

        lfs si, sliver_texture   ; fs:si points to texture memory

        ; offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x

        mov bx,sliver_top        ; multiply Y by 320 to get proper offset
        shl bx,8
        mov ax,bx
        shr bx,2
        add bx,ax
        add bx,sliver_ray        ; add X
        add di,bx


        mov bx,sliver_clip       ; move important constants into registers
        mov ax,sliver_scale
        add ax,bx

Sliver_Loop:                     ; main loop


        ; double_buffer[offset] = work_sprite[work_offset+column]

        xchg dx,bx               ; exchange dx and bx since only bx can be used
                                 ; as a index
        mov cl, BYTE PTR fs:[si+bx] ; get texture pixel

        mov es:[di], cl             ; move it to screen

        xchg dx,bx                  ; restore dx and bx

        mov cx,bx                   ; get ready to access proper scale index

        ; row = scale_table[scale]

        mov dx, scale_row

        shl bx,1

        add bx,dx

        mov dx, WORD PTR [bx]       ; get scale index out of array

        add dx,sliver_column

        mov bx,cx

        ; offset      += SCREEN_WIDTH;

        add di,320                  ; move down to next video line

        inc bx                      ; increment counter
        cmp bx, ax
        jne Sliver_Loop             ; are we done?

        pop di                      ; restore registers
        pop si

        ret                         ; blaze

Render_Sliver_32 ENDP

END



