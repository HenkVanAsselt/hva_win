;=============================================================
; VIDEO.ASM
;
; Low level screen routines for use with Turbo C
;
; Assemble with:  'tasm /ml video'
;                 'masm /Mx /Zd /Zi /L video,;'
;
; History:   910710 V1.1 - Adjusted for screen origin (0,0)
;                          Originally (1,1) was used
;            910810 V1.2 - Added gettext() and puttext() for MSC
;            920112 V1.3 - Deleted _settext80 and replaced it
;                          by crtinit(). This handles all sizes
;                          of screens (also 132x43 etc)
;============================================================

[BITS 32]

bios_data    equ 0x40
crt_mode_set equ 0x65

[SECTION .data]

[EXTERN _displayseg]
[EXTERN _non_ibm]

;------------------------
; Box drawing characters
;------------------------
ul_char     db    218         ; upper left corner
le_char     db    179         ; left side
ri_char     db    179         ; right side
ll_char     db    192         ; lower left corner
to_char     db    196         ; top
bo_char     db    196         ; bottom
lr_char     db    217         ; lower right corner
ur_char     db    191         ; upper right corner



;VIDEO_TEXT  segment para public 'CODE'
;            assume cs:VIDEO_TEXT

;ifdef MSC
;.GLOBAL _puttext,_gettext
;endif

[GLOBAL _drawbox]
[GLOBAL _printstring]
[GLOBAL _printchar]
[GLOBAL _setboxchars]
[GLOBAL _fillwindow]
[GLOBAL _setattrib]
[GLOBAL _crtinit]

;------------------------------------------------------------------------------
; Turbo Pascal compatible global variables
;------------------------------------------------------------------------------
[GLOBAL _NormAttr]
[GLOBAL _TextAttr]
[GLOBAL _LastMode]
[GLOBAL _CurCrtSize]
[GLOBAL _CheckSnow]
[GLOBAL _DirectVideo]
[GLOBAL _WindMin]
[GLOBAL _WindMax]

_NormAttr    db  0
_TextAttr    db  0
_LastMode    dw  0
_CurCrtSize  dw  0
_CheckSnow   db  0
_DirectVideo db  0
_WindMin     dw  0
_WindMax     dw  0


[SECTION .text]

;------------------------------------------------------------------------------
;    FUNCTION: _crtinit
;      C CALL: crtinit(void);
;     PURPOSE: initialize video (CRT) for HvA window enviroment
; DESCRIPTION: Uses routines from the turbo-pascal CRT unit,
;          but modified by Tillman Reh (C'T Magazine, dec. 1991)
;          to be able to use high resolution text windows.
;     RETURNS: Nothing
;     HISTORY: 911227 V0.1
;------------------------------------------------------------------------------
_crtinit:	;   proc far

        ;-----------------------------
        ; Select Alternate PrtScr
        ;----------------------------
        ;mov  ax,1200h
        ;mov  bl,20h
        ;int  10h

        ;-----------------------------
        ; Get video state
        ;-----------------------------
        mov  ah,0Fh
        int  10h
        push ax     ; Save 'mode' and 'no. of columns'

        ;----------------------------------------------
        ; Get current character generator information
            ;----------------------------------------------
            push bp
        mov  ax,1130h
        mov  bh,0
        mov  dl,0
            int  10h            ; dl <-- rows-1
            pop  bp

        pop  ax     ; al=mode, ah=columns
        mov  ch,0       ; Reset CheckSnow flag
        or   dl,dl      ;
        jne  @@1        ; rows==0 ?
        mov  dl,24      ; then assume 25 rows
        cmp  al,3       ; mode <= 3 ?
        ja   @@1        ;   no, ...
        mov  ch,1       ;   yes, CheckSnow !
@@1:        mov  dh,dl      ; dh <-- rows-1
        mov  dl,ah      ; dl <-- cols
        dec  dl     ; cols--
        mov  ah,0       ; Reset font flag
        cmp  cl,8       ; 8x8 font ?
        jne  @@2        ;   no, ah stay's 0
        mov  ah,1       ;   no, ah becomes 1

        ;------------------------------------------
        ; Save crt information in global variables
        ;------------------------------------------
@@2:        mov  [_LastMode],ax   ; Mode and Font
            mov  [_CurCrtSize],dx ; (screensize, both -1)
            mov  [_CheckSnow],ch  ; CheckSnow flag
            mov byte [_DirectVideo],1 ; Set Direct Video flag
        xor  ax,ax      ; ax <-- 0
        mov  [_WindMin],ax    ; Screen origin (upper left corner) = 0,0
        mov  [_WindMax],dx    ; Screen end (lower right corner)

        ;---------------------------------
        ; Get and save current attribute
        ;---------------------------------
        mov  ah,8       ; Get Char & Attribute
        xor  bh,bh      ; (page 0)
        int  10h        ;
        mov  al,ah      ; we are not interested in char
        and  al,7Fh     ; Mask bit 7 (blinking)
        mov  [_NormAttr],al   ; Save as Standard and
        mov  [_TextAttr],al   ; Text attribute.

        ret         ; Return from this function
;_crtinit    endp


;; ifdef MSC
;; ;--------------------------------------------------------------------
;; ;    FUNCTION: get_text
;; ;      C CALL: void gettext(int col1 , int row1, int col2, int row2,
;; ;                            char *array);
;; ;              Conform TC: (left,top,right,bottom,array);
;; ;     PURPOSE: Save a text window in the array
;; ; DESCRIPTION:
;; ;     RETURNS: nothing
;; ;     HISTORY: 910810 V0.1 - Initial version
;; ;---------------------------------------------------------------------
;; _gettext        proc far
;; 
;; ;
;; ; Parameters
;; ;
;; col1            equ  <6[bp]>     ; 1..80
;; row1            equ  <8[bp]>     ; 1..25
;; col2            equ  <10[bp]>    ; 1..80
;; row2            equ  <12[bp]>    ; 1..25
;; array           equ  <14[bp]>
;; 
;; ;
;; ; Local variables
;; ;
;; rows            equ  <-2[bp]>
;; cols            equ  <-4[bp]>
;; 
;;                 ;--------------------------------------------
;;                 ; Reserve space on stack for local variables
;;                 ;--------------------------------------------
;;                 push bp
;;                 mov  bp,sp
;;                 sub  sp,4
;; 
;;                 push di
;;                 push si
;;                 mov  ax,row1        ; Figure video offset
;;                 dec  ax
;;                 mov  bx,col1
;;                 dec  bx
;;                 call fig_vid_off
;;                 mov  si,ax          ; si = video offset
;; 
;;                 mov  ax,row2        ; Calc no. of rows
;;                 sub  ax,row1
;;                 inc  ax
;;                 mov  rows,ax
;; 
;;                 mov  ax,col2        ; Calc no. of colums
;;                 sub  ax,col1
;;                 inc  ax
;;                 mov  cols,ax
;; 
;;                 cld                 ; Flag increment
;;                 call disable_cga    ; Disable CGA if necessary
;;                 push ds
;;                 les  di,array       ; ES:DI = array pointer
;;                 mov  ds,_displayseg ; DS:SI = video pointer
;; get_text1:      push si             ; Save video offset
;;                 mov  cx,cols        ; CX = no. of colums
;;                 rep  movsw          ; Save row
;;                 pop  si             ; Restore video offset
;;                 push cx
;;                 mov  cx,_WindMax     ; e.g. 79
;;                 and  cx,00FFh
;;                 inc  cx              ; e.g. 80
;;                 shl  cx,1            ; e.g. 160
;;                 add  si,cx           ; Point to next row
;;                 pop  cx
;;                 dec  word ptr rows  ; Loop
;;                 jnz  get_text1      ; Till done
;; 
;;                 pop  ds             ; Restore DS
;;                 call enable_cga     ; Enable CGA if necessary
;;                 pop  si             ; Restore registers
;;                 pop  di
;; 
;;                 ;--------------------------
;;                 ; Restore stack and return
;;                 ;--------------------------
;;                 mov  sp,bp
;;                 pop  bp
;;                 ret
;; 
;; ;_gettext        endp
;; 
;; ;--------------------------------------------------------------------
;; ;    FUNCTION: put_text
;; ;      C CALL: void put_text(int col1 , int row1, int col2, int row2,
;; ;                            char *array);
;; ;              Conform TC: (left,top,right,bottom,array);
;; ;     PURPOSE: Restore a text window from the array
;; ; DESCRIPTION:
;; ;     RETURNS: nothing
;; ;     HISTORY: 910810 V0.1 - Initial version
;; ;---------------------------------------------------------------------
;; _puttext       proc far
;; 
;; ;
;; ; Parameters
;; ;
;; col1           equ  <6[bp]>      ; 1..80
;; row1           equ  <8[bp]>      ; 1..25
;; col2           equ  <10[bp]>     ; 1..80
;; row2           equ  <12[bp]>     ; 1..25
;; array          equ  <14[bp]>
;; 
;; ;
;; ; Local variables
;; ;
;; rows            equ  <-2[bp]>
;; cols            equ  <-4[bp]>
;; 
;;                 ;--------------------------------------------
;;                 ; Reserve space on stack for local variables
;;                 ;--------------------------------------------
;;                 push bp
;;                 mov  bp,sp
;;                 sub  sp,4
;; 
;;                 push di
;;                 push si
;; 
;;                 mov  ax,row1        ; Figure video offset
;;                 dec  ax
;;                 mov  bx,col1
;;                 dec  bx
;;                 call fig_vid_off
;;                 mov  di,ax          ; di = video offset
;;                 mov  es,_displayseg ; es = video segment
;; 
;;                 mov  ax,row2        ; Calc no. of rows
;;                 sub  ax,row1
;;                 inc  ax
;;                 mov  rows,ax
;; 
;;                 mov  ax,col2        ; Calc no. of colums
;;                 sub  ax,col1
;;                 inc  ax
;;                 mov  cols,ax
;; 
;;                 cld                 ; Flag increment
;;                 call disable_cga    ; Disable CGA if necessary
;;                 push ds
;;                 lds  si,array       ; DS:SI = array pointer
;; put_text1:      push di             ; Save video offset
;;                 mov  cx,cols        ; CX = no. of colums
;;                 rep  movsw          ; Save row
;;                 pop  di             ; Restore video offset
;;                 push cx
;;                 mov  cx,_WindMax     ; e.g. 79
;;                 and  cx,00FFh
;;                 inc  cx              ; e.g. 80
;;                 shl  cx,1            ; e.g. 160
;;                 add  di,cx           ; Point to next row
;;                 pop  cx
;;                 dec  word ptr rows  ; Loop
;;                 jnz  put_text1      ; Till done
;; 
;;                 pop  ds             ; Restore DS
;;                 call enable_cga     ; Enable CGA if necessary
;;                 pop  si             ; Restore registers
;;                 pop  di
;; 
;;                 ;--------------------------
;;                 ; Restore stack and return
;;                 ;--------------------------
;;                 mov  sp,bp
;;                 pop  bp
;;                 ret
;; 
;;                 mov  sp,bp
;;                 pop  bp
;;                 ret
;; ;_puttext        endp
;; 
;; endif

;--------------------------------------------------------------------
;    FUNCTION: set_boxchars()
;      C CALL: void setboxchars(int ul, int to, int ur, int ri, int lr, int bo, int ll, int le);
;     PURPOSE: Set characters for drawbox()
; DESCRIPTION: Read parameters and store them locally
;     RETURNS: nothing
;     HISTORY: 910805 V0.1 - Initial version
;---------------------------------------------------------------------

%define ul_c bp+6
%define to_c bp+8
%define ur_c bp+10
%define ri_c bp+12
%define lr_c bp+14
%define bo_c bp+16
%define ll_c bp+18
%define le_c bp+20

_setboxchars:		;   proc far

    push bp
    mov  bp,sp

    mov  ax,[ul_c]
    mov  [ul_char],al
    mov  ax,[to_c]
    mov  [to_char],al
    mov  ax,[ur_c]
    mov  [ur_char],al
    mov  ax,[ri_c]
    mov  [ri_char],al
    mov  ax,[lr_c]
    mov  [lr_char],al
    mov  ax,[bo_c]
    mov  [bo_char],al
    mov  ax,[ll_c]
    mov  [ll_char],al
    mov  ax,[le_c]
    mov  [le_char],al

    ;--------------------------
    ; Restore stack and return
    ;--------------------------
    mov  sp,bp
    pop  bp
    ret

;_setboxchars   endp

;-----------------------------------------------------
; Fill text window
; C call: fillwindow(row,col,xsize,ysize,char,att)
; Origin is (0,0)
;-----------------------------------------------------

_fillwindow:	; proc far

%define row     bp+6
%define col     bp+8
%define xsize   bp+10
%define ysize   bp+12
%define char    bp+14
%define att     bp+16

; Local variables

%define rows    bp-2
%define cols    bp-4

               ;--------------------------------------------
               ; Reserve space on stack for local variables
               ;--------------------------------------------
               push bp
               mov  bp,sp
               sub  sp,4

               ;----------------------
               ; Initial calculations
               ;----------------------
               push di
               mov  ax,[row]
               mov  bx,[col]
               call fig_vid_off
               mov  di,ax
               mov  es,[_displayseg]
               mov  ax,[ysize]
               mov  [rows],ax
               mov  ax,[xsize]
               mov  [cols],ax

               ;---------------
               ; Fill the area
               ;---------------
               cld
               mov  al,[char] 		; function variable 'char'
               mov  ah,[att]  		; function variable 'att'
               call disable_cga
fillwindow1:   push di
               mov  cx,[cols]
               rep  stosw
               pop  di
               push cx
               mov  cx,[_WindMax]     ; e.g. 79
               and  cx,00FFh
               inc  cx              ; e.g. 80
               shl  cx,1            ; e.g. 160
               add  di,cx
               pop  cx
               dec  word [rows]			; local variable 'rows'
               jnz  fillwindow1
               call enable_cga

               ;--------------------------
               ; Restore stack and return
               ;--------------------------
               pop  di
               mov  sp,bp
               pop  bp
               ret
;_fillwindow    endp

;---------------------------------------------
; Set attributes
; C call: setattrib(row,col,xsize,ysize,att)
; Origin is (0,0)
;---------------------------------------------

_setattrib:		; proc far

; Parameters
%define row             bp+6
%define col             bp+8
%define xsize           bp+10
%define ysize           bp+12
%define att             bp+14

; Local variables
%define rows            bp-2
%define cols            bp-4

               ;-------------------------------------------
               ; Reserve space on stack for local variables
               ;--------------------------------------------
               push bp
               mov  bp,sp
               sub  sp,4

               ;----------------------
               ; Initial calculations
               ;----------------------
               push di
               mov  ax,[row]
               mov  bx,[col]
               call fig_vid_off
               mov  di,ax
               inc  di
               mov  es,[_displayseg]
               mov  ax,[ysize]
               mov  [rows],ax
               mov  ax,[xsize]
               mov  [cols],ax
               cld
               mov  al,[att]

               ;-------------
               ; Disable CGA
               ;-------------
               call disable_cga

               ;----------------------------------------
               ; Set the attributes in the defined area
               ;----------------------------------------
setattrib1:    push di
               mov  cx,[cols]
setattrib2:    stosb
               inc  di
               loop setattrib2
               pop  di
               push cx
               mov  cx,[_WindMax]     ; e.g. 79
               and  cx,00FFh
               inc  cx              ; e.g. 80
               shl  cx,1            ; e.g. 160
               add  di,cx
               pop  cx
               dec  word [rows]
               jnz  setattrib1

               ;------------
               ; Enable CGA
               ;------------
               call enable_cga

               ;--------------------------
               ; Restore stack and return
               ;--------------------------
               pop  di
               mov  sp,bp
               pop  bp
               ret
;_setattrib     endp


;-------------------------------------------------------------------
; Drawbox
; C call: drawbox(int row, int col, int xsize, int ysize, int att)
; Origin is (0,0)
;-------------------------------------------------------------------
_drawbox:		;  proc far

; Parameters
%define row            bp+6
%define col            bp+8
%define xsize          bp+10
%define ysize          bp+12
%define att            bp+14

; Local variables
%define rows           bp-2
%define cols           bp-4

               ;--------------------------------------------
               ; Reserve space on stack for local variables
               ;--------------------------------------------
               push bp
               mov  bp,sp
               sub  sp,4

               ;----------------------
               ; Initial calculations
               ;----------------------
               push di
               mov  ax,[row]
               mov  bx,[col]
               call fig_vid_off
               mov  di,ax
			   mov  ax,[_displayseg]
               mov  es,ax
               mov  ax,[ysize]
               dec  ax
               dec  ax
               mov  [rows],ax
               mov  ax,[xsize]
               dec  ax
               dec  ax
               mov  [cols],ax
               mov  ah,[att]

               ;-------------
               ; Disable CGA
               ;-------------
               call disable_cga

               ;--------------------------------
               ; Draw upper left corner
               ;--------------------------------
               push di            ; di = video offset
               mov  al,[ul_char]
               stosw

               ;-----------------
               ; Draw upper line
               ;-----------------
               mov  al,[to_char]
               mov  cx,[cols]
               rep  stosw

               ;---------------------------
               ; Draw right corner
               ;--------------------------
               mov  al,[ur_char]
               stosw

               ;------------------------------------
               ; Draw left- and right lines
               ;------------------------------------
               pop  di
               push cx
               mov  cx,[_WindMax]     ; e.g. 79
               and  cx,00FFh
               inc  cx              ; e.g. 80
               shl  cx,1            ; e.g. 160
               add  di,cx
               pop  cx
drawbox4:      push di
               mov  al,[le_char]
               stosw
               add  di,[cols]
               add  di,[cols]
               mov  al,[ri_char]
               stosw
               pop  di
               push cx
               mov  cx,[_WindMax]     ; e.g. 79
               and  cx,00FFh
               inc  cx              ; e.g. 80
               shl  cx,1            ; e.g. 160
               add  di,cx
               pop  cx
               dec  word [rows]
               jnz  drawbox4

               ;---------------------------------
               ; Draw bottom left corner
               ;---------------------------------
               mov  al,[ll_char]
               stosw

               ;------------------
               ; Draw  bottom line
               ;-------------------
               mov  al,[bo_char]
               mov  cx,[cols]
               rep  stosw

               ;----------------------------------
               ; Draw bottom right corner
               ;----------------------------------
               mov  al,[lr_char]
               stosw

               ;------------
               ; Enable CGA
               ;------------
               call enable_cga

               ;--------------------------
               ; Restore stack and return
               ;--------------------------
               pop  di
               mov  sp,bp
               pop  bp
               ret
;_drawbox       endp

;------------------------------------------------------
; Display string
; C call: printstring(int row, int col, char *string)
; Origin is (0,0)
;------------------------------------------------------
_printstring:	;  proc far

; Parameters
%define row            bp+6   
%define col            bp+8   
%define string         bp+10  

               ;----------------
               ; Make new stack
               ;----------------
               push bp
               mov  bp,sp

               ;----------------------
               ; Save registers
               ;----------------------
               push si
               push di

               ;-----------------------
               ; Initial calculations
               ;-----------------------
               mov  ax,[row]
               mov  bx,[col]
               call fig_vid_off
               mov  di,ax
               mov  es,[_displayseg]
               cld
               cmp  byte [_non_ibm],0            ; IBM CGA ?
               push ds                      ; Save DS
               lds  si,[string]               ; DS:SI = string pointer
               je   print_string2           ; Jump if IBM CGA

               ;-------------------
               ; Print the string
               ;-------------------
print_string1: lodsb                        ; Get next character
               or   al,al                   ; Jump
               jz   print_string6           ;   if done
               stosb                        ; Display the charcter
               inc  di                      ; Bump video pointer
               jmp  print_string1           ; Loop till done
print_string2: mov  dx,03dah                ; DX = video status register
print_string3: lodsb                        ; Get the next character
               or   al,al                   ; Jump
               jz   print_string6           ;  if done
               mov  ah,al                   ; Put it in AH
               cli                          ; Disable the interrupts
print_string4: in   al,dx                   ; Loop
               and  al,1                    ;  if in
               jnz  print_string4           ;    horizontal retrace
print_string5: in   al,dx                   ; Loop
               and  al,1                    ;  if not in
               jz   print_string5           ;    horizontal retrace
               mov  [es:di],ah              ; Display the character
               sti                          ; Reenable the interrupts
               inc  di                      ; Bump the
               inc  di                      ;  video pointer
               jmp  print_string3           ; Loop till done

               ;----------------------------------
               ; Restore the registers and return
               ;----------------------------------
print_string6: pop  ds
               pop  di
               pop  si
               pop  bp
               ret
;_printstring   endp

;------------------------------------------------------
; Display character
; C call: printchar(int row, int col, char c)
; Origin is (0,0)
;------------------------------------------------------
_printchar:		;  proc far

; Parameters
%define row            bp+6   
%define col            bp+8
%define c              bp+10  

               ;----------------
               ; Make new stack
               ;----------------
               push bp
               mov  bp,sp

               ;----------------------
               ; Save registers
               ;----------------------
               push si
               push di

               ;-----------------------
               ; Figure video offset
               ;-----------------------
               mov  ax,[row]
               mov  bx,[col]
               call fig_vid_off
               mov  di,ax                   ; DI = video offset
               mov  es,[_displayseg]          ; ES = video segment

               mov  al,[c]
               cld                          ; Flag increment

               cmp  byte [_non_ibm],0            ; IBM CGA ?
               push ds                      ; Save DS
               je   print_char2             ; Jump if IBM CGA

               ;---------------------
               ; Print the character
               ;---------------------
print_char1:   mov  al,[c]                    ; Get character
               stosb                        ; Display the charcter
               jmp  print_char6             ; Done

               ;---------------------
               ; IBM CGA has to wait
               ;---------------------
print_char2:   mov  dx,03dah                ; DX = video status register
print_char3:   mov  ah,[c]                  ; Get character
               cli                          ; Disable the interrupts
print_char4:   in   al,dx                   ; Loop
               and  al,1                    ;  if in
               jnz  print_char4             ;    horizontal retrace
print_char5:   in   al,dx                   ; Loop
               and  al,1                    ;  if not in
               jz   print_char5             ;    horizontal retrace
               mov  [es:di],ah              ; Display the character
               sti                          ; Reenable the interrupts

               ;----------------------------------
               ; Restore the registers and return
               ;----------------------------------
print_char6:   pop  ds
               pop  di
               pop  si
               pop  bp
               ret
;_printchar     endp

; --------------------------------
; Figure video offset
; Origin (0,0)
;
; Input parameters:
;   AX = row
;   BX = col
;---------------------------------

fig_vid_off:	; proc near
               push dx
               push bx
               push cx
               mov  cx,[_WindMax]     ; e.g. 79
               and  cx,00FFh
               inc  cx              ; e.g. 80
               shl  cx,1            ; e.g. 160
               mov  bx,cx
               pop  cx
               mul  bx
               pop  bx
               sal  bx,1
               add  ax,bx
               pop  dx
               ret
;fig_vid_off    endp

;
; Disable CGA
;
disable_cga:	;   proc near
               cmp  byte [_non_ibm],0
               jne  disable_cga2
               push ax
               push dx
               mov  dx,3dah
disable_cga1:  in   al,dx
               and  al,8
               jz   disable_cga1
               mov  dl,0d8h
               mov  al,25h
               out  dx,al
               pop  dx
               pop  ax
disable_cga2:  ret
;disable_cga    endp

;
; Enable CGA
;
enable_cga:		;     proc near
               cmp  byte [_non_ibm],0
               jne  enable_cga1
               push ax
               push bx
               push dx
               push ds
               mov  ax,bios_data
               mov  ds,ax
               mov  bx,crt_mode_set
               mov  al,[bx]
               mov  dx,03d8h
               out  dx,al
               pop  ds
               pop  dx
               pop  bx
               pop  ax
enable_cga1:   ret
;enable_cga     endp

