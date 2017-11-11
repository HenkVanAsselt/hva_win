/**************************************************************************
$Header: EDITSTR.C Fri 10-27-2000 8:53:26 pm HvA V2.01 $

DESC: String edit routines for HVA_WIN
HIST: 19900818 V1.00
	  20001027 V2.01 - Adapted for DJGPP
***************************************************************************/

/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include "video/v_video.h"
#include "keys.h"
#include "window.h"

/*------------------------------------------------------------------------------
                        PROTOTYPES
------------------------------------------------------------------------------*/
static void display_line(int l);

/*------------------------------------------------------------------------------
                        CONSTANTS
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
                        LOCAL VARIABLES
------------------------------------------------------------------------------*/
static int maxcol = 0;
static int len = 0;             /* Current string length                */
static char disp_str[132];       /* String to display                    */
static WINDOW *w;               /* Ptr to window structure (if wanted)  */
static int startrow,startcol;
static int row,col;             /* Actual row and colum                 */
static int pos;                 /* pointer to next char to edit         */
static int sstart;              /* First character to display ( >= 0    */
static char empty_str[132];  /* Dummy string to clear window         */
static char s[80];              /* Temporary edit string                */

extern SCREEN _cursvar;

/*==============================================================================
                        START OF FUNCTIONS
==============================================================================*/

/*#+func-------------------------------------------------------------------
    FUNCTION: editstr()
     PURPOSE: Edit string
 DESCRIPTION: Feels like the TURBO integrated enviroment string editing
     RETURNS: Last character typed
     HISTORY: 900902 V0.3 - x
              910716 V0.4 - Adapted for new window enviroment
              910826 V0.5 - Now returns last character typed
              930101 V0.6 - 3rd parameter is now the length of the window.
              931012 V0.7 - Adjust wn_open() for borders including...
--#-func-------------------------------------------------------------------*/
int editstr(int srow, int scol, int l, char *header, char *str, int w_flag)
{
  int c_flag = TRUE;     /* Cursor OFF flag              */
  int i;
  unsigned int key;      /* Scancode of key pressed      */
  int key_stat;          /* Key status                   */
  int firstkey = TRUE;   /* Flag for for first key press */

  /*----------------
    Save variables
  ----------------*/
  startrow = srow;
  startcol = scol;

  if (startrow == -1) startrow = action_row + 2;
  if (startcol == -1) startcol = action_col + 2;

  maxcol = startcol + l -1;  /* Maximum column position of cursor     */

  /* CHECK IF WINDOW WITHIN BOUNDARIES
  ------------------------------------*/
  startcol = minmax(0,startcol,_cursvar.cols-l);
  startrow = minmax(0,startrow,_cursvar.lines);

  /* DRAW THE EDIT WINDOW
  -----------------------*/
  if (w_flag)
  {
    /*-------------------------------------------------------
      l is the number of characters to display,
      but use l+2 to reserve space for the '<' and '>'
      931012 V0.7: add another 2 for xsize and ysize of window
                   to include borders
    ------------------------------------------------------*/
    w = wn_open(_SINGLE_LINE, startrow-1, startcol-1,
                l+4,3,_window_att,_border_att);
    wn_sync(w,TRUE);          /* Set cursor on flag      */
    wn_fixcsr(w);             /* Update cursor in window */
    wn_title(w,header);
  }
  else
    w = wn_open(_NO_BORDER, startrow, startcol, l+2, 1,
                _window_att,_border_att);

  /*-----------------------------
    Clear string s and empty_str
  ------------------------------*/
  memset(s,'\0',80);
  memset(empty_str,' ',l);
  empty_str[l] = '\0';

  /*---------------------
    Initialize variables
  ----------------------*/
  len = strlen(str);           /* Determine string length */
  strcpy(s,str);               /* Original str will be used with ESCaping */
  row = startrow;
  col = startcol;
  pos = len;                      /* Set character pointer after last char */
  col += pos;                     /* Set cursor column                     */

  display_line(l);

  /*-------------------------------------------------------------------
    After each process the following rules have to be true:

    - pos will point to the next position in the string to process.
    - col (cursorcolumn) on the screen will indicate the next position.
    - len is the actual lenght of the string
  ---------------------------------------------------------------------*/

  do
  {
    /* GET A KEY AND KEYBOARD STATUS
    --------------------------------*/
    key = waitkey();
    key_stat = shift_status();

    /* THREAT FIRST KEYPRESS SPECIAL
    --------------------------------*/
    if (firstkey)
    {
      firstkey = FALSE;
      if (isalnum(key))               /* Alpha numberic char ?  */
      {                               /* Start with new string  */
        v_prints(startrow,startcol,empty_str);
        s[0] = (char) key;            /* Store key in string    */
        for (i=1 ; i<80 ; i++)        /* Fill string with NIL's */
          s[i] = '\0';
        pos = 1;                      /* Reset string pointer   */
        col = startcol+1;             /* Reset column           */
        len = 1;                      /* Reset string length    */
        display_line(l);
        continue;
      }
    }

    switch (key) {
      case ARROW_LEFT:
        if (key_stat & (RIGHT_SHIFT|LEFT_SHIFT)) {
          col = startcol;
          pos = 0;          /* Put cursor on first character */
        }
        else {
          if (pos > 0)      /* Cursor one character to left  */
            pos--;
          if (!sstart)      /* If not truncated to left ...  */
            col--;
        }
        break;

/*    --------------  startcol 0 ; l 10 ; len 10 ; maxcol 10
      |<012345678_>|
      --------------
*/
      case ARROW_RIGHT:
        if (key_stat & (RIGHT_SHIFT|LEFT_SHIFT)) {
          pos = len;                 /* Cursor after last character     */
          col = min(startcol+len,maxcol);
        }
        else {                         /* Cursor 1 char to the right      */
          if (pos < len) {             /* Stop at end of string           */
            pos++;
            col++;
          }
        }
        break;

      case RETURN:
        strcpy(str,s);
        break;

      case BACKSPACE:     /* Delete character left of the cursor */
        if (pos > 0)
        { if (!sstart)             /* If no left-truncation      */
            col--;
          pos--;                   /* Generate new string        */
          strcpy(s+pos,s+pos+1);
          len--;                   /* Adjust length              */
          i = startcol+len+1;
          if (i <= maxcol)
            v_printc(row,i,' ');   /* Delete last char on screen */
        }
        break;

      case DELETE:                 /* Delete character under the cursor */
        if (s[pos] && pos >= 0)
        {
          len--;
          i = startcol+len;
          if (i <= maxcol)
            v_printc(row,i,' ');   /* Delete last char on screen */
          strcpy(s+pos,s+pos+1);   /* Generate new string        */
        }
        break;

      case HOME:         /* Cursor on first character */
        pos = 0;
        col = startcol;
        break;

      case END_LINE:     /* Cursor after last character  */
        pos = len;
        col = min(startcol+len,maxcol);
        break;

      default:
        if (isprint(key & 0x00FF))
        {
          for (i=len ; i>=pos ; i--)
            s[i] = s[i-1];
          s[pos] = (char) (key & 0x00FF);
          pos++;                 /* Adjust pointer           */
          len++;                 /* Adjust string length     */
          col++;
        }
    }

    display_line(l);

  }
  while (key != ENTER && key != ESC);

  /* DELETE TRAILING BLANKS
  -------------------------*/
  i = len - 1;
  while (isspace(s[i]) && i>=0)
  {
    s[i] = '\0';
    i--;
  }

  /* RESTORE CURSOR AND WINDOW
  ----------------------------*/
  if (!c_flag) v_cursoroff();
  if (w_flag)
    w = wn_close(w);

  return(key);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: display_line()
     PURPOSE: Display line for editstr()
      SYNTAX: static void display_line(int l);
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 24-Feb-1992 10:02:00 V0.1 - Initial version
              01-Jan-1993 20:30:11 V0.2 - added parameter 'l' for length
--#-func----------------------------------------------------------------------*/
static void display_line(int l)
{
  int i;
  int left_flag,right_flag;

  /* SET VARIBLES WITHIN BOUNDARIES
  ---------------------------------*/
  col = minmax(startcol,col,maxcol);
  pos = minmax(0,pos,80);
  len = minmax(0,len,80);

/*    --------------
      |<012345678_>|
      --------------
*/
  /* DETERMINE START OF STRING
  ----------------------------*/
  sstart = max(0,pos-l+1);

 /* CHECK ON TRUNCATION OF STRING
  --------------------------------*/
  i = max(pos,l-1);
  left_flag  = sstart ? TRUE : FALSE;  /* Check left truncation   */
  right_flag = s[i] ? TRUE : FALSE;    /* Check right truncation  */

/*  --------------
    |<012345678_>|
    --------------
*/

  /*-----------------------------------
    Initialize the string to display
  ------------------------------------*/
  s[len] = '\0';                        /* Alway's terminate string  */
  memset(disp_str,' ',l);               /* Initialize display string */
  strncpy(disp_str,s+sstart,l);         /* sstart is minimal 0       */
  strcat(disp_str," ");
  disp_str[l] = '\0';                   /* Alway's terminate display string */

  /*------------------------------------------------
    Show the string.
    Use startcol+1 to leave space for the '<' sign
  -------------------------------------------------*/
  v_prints(startrow,startcol+1,disp_str);

  /*------------------------
    If truncation, show it
  -------------------------*/
  v_printc(row, maxcol+2, right_flag ? 0x10 : ' ');
  v_printc(row, startcol, left_flag ? 0x11 : ' ');

  /*---------------------------------------------------------
    If cursor just after last character of the string, and
    this would be just before the '>' sign, then print a
    space on this place. This is a fucking exception !
  ---------------------------------------------------------*/
  if (col == maxcol && !right_flag)
    v_printc(row,col+1,' ');

  /*-------------------------------------
    Set cursor on the character to edit
  ---------------------------------------*/
  v_setcurpos(row,col+1);
}

