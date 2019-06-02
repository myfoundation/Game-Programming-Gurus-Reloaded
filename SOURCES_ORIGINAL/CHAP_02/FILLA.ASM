
screen_ram EQU 0A000h           ; videoram starts at A000:0000

.MODEL MEDIUM,C                 ; use medium model with C parameter passing

.CODE                           ; this is the beginning of the code

PUBLIC Fill_Screen              ; this let's the linker have access to the function
                                ; name so that it can be exported

Fill_Screen PROC FAR C color:WORD    ; the function takes one parameter


mov ax, screen_ram              ; set es:di to screen ram
mov es, ax
xor di,di                       ; zero di out


mov cx,320*200/2                ; number of words in screen
mov al, BYTE PTR color          ; move the color into al
mov ah,al                       ; move the color into ah
rep stosw                       ; blast the video ram

ret                             ; return to caller

Fill_Screen ENDP                ; this is the end of the procedure

END                             ; this is the end of the code





