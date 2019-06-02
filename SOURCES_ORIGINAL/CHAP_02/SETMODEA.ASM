
.MODEL MEDIUM,C                 ; use medium model with C parameter passing
	
.CODE 				; this is the beginning of the code

PUBLIC Set_Mode                 ; this let's the linker have access to the function
                                ; name so that it can be exported

Set_Mode PROC FAR C vmode:WORD    ; the function takes one parameter


mov ah,0                        ; function 0:set video mode
mov al, BYTE PTR vmode          ; set the mode we want to change to
int 10h                         ; use BIOS to set the mode

ret                             ; return to caller

Set_Mode ENDP                   ; this is the end of the procedure

END                             ; this is the end of the code





