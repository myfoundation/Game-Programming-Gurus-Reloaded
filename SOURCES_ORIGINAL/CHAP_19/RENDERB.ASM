
;//////////////////////////////////////////////////////////////////////////////
;
;This function simply copies the double buffer into the video buffer.  Note
;it uses the 32 bit instructions and registers for a little bit more speed
;eventhough the video ram access will ultimately be the bottleneck
;
;//////////////////////////////////////////////////////////////////////////////


.MODEL MEDIUM,C                 ; use medium memory model C function names

.CODE                           ; begin the code segment

EXTRN double_buffer:DWORD       ; the double buffer is elsewhere
EXTRN video_buffer:DWORD        ; the video buffer is elsewhere


PUBLIC Render_Buffer_32         ; export function name to linker


Render_Buffer_32 PROC FAR C     ; this function is C callable and far

.386                            ; use 80386 stuff

push ds                         ; save the data segment register

cld                             ; set direction to foward
lds si, double_buffer           ; make source for move the double buffer
les di, video_buffer            ; make destination the video buffer
mov cx,320*152/4                ; move the whole 3-D portion of the screen

rep movsd                       ; do it

pop ds                          ; restore registers

ret                             ; blaze!!

Render_Buffer_32 ENDP

END





