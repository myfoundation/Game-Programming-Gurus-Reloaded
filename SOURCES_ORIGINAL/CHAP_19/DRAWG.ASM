
;//////////////////////////////////////////////////////////////////////////////
;
;This function draws the ground and sky.  It does this using the 32 bit
;registers and using the "store double word" instruction.  Note: there are
;two separate parts to the fill.  One fills the sky and the other fills the
;ground
;
;//////////////////////////////////////////////////////////////////////////////

.MODEL MEDIUM,C                 ; use medium memory model and C names

.CODE                           ; begin code segment

EXTRN double_buffer:DWORD       ; the double buffer is elsewhere

PUBLIC Draw_Ground_32           ; export function name to linker

Draw_Ground_32 PROC FAR C       ; this function is C callable and far

.386                            ; use 80386 instruction i.e. 32 bit stuff

push di                         ; save di

cld                             ; set direction to foward

les di, double_buffer           ; point es:di to double buffer

xor eax,eax                     ; put a zero into eax
mov cx,320*76/4                 ; need to fill 76 lines of the screen
rep stosd                       ; do it


mov eax,01E1E1E1Eh              ; now put a grey into eax
mov cx,320*76/4                 ; fill the remaining 76 lines i.e. the ground
rep stosd                       ; do it

pop di                          ; restore di

ret                             ; blaze!

Draw_Ground_32 ENDP

END





