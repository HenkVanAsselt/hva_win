/**************************************************************************
$Header: v_curs.c Fri 10-27-2000 8:08:55 pm HvA V2.01 $

DESC: Low-level cursor routines for HVA_WINDOWS
HIST: 920223 V1.02
      20001027 V2.01 - Adapting for DJGPP
***************************************************************************/
				  
/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include "v_video.h"

/*+ Local prototypes +*/
static void v_get_cursor_shape(void);

/*+ Local varialbles +*/
static int cursorstart = -1,
           cursorend   = -1;

extern SCREEN _cursvar;			/*+ Curses internal variables +*/
extern int _default_lines;   	/*+ default terminal height   +*/
extern int LINES;            	/*+ current terminal height   +*/
extern int COLS;         	 	/*+ current terminal width    +*/


/*==============================================================================
                        CURSOR FUNCTIONS
==============================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    FUNCTION: v_get_cursor_shape()
     PURPOSE: initialize cursor
 DESCRIPTION: Get cursor shape by calling int 10h, function 03h
              The shape will be stored int the local variables
              'cursorstart' and 'cursorend'
     RETURNS: nothing
     HISTORY: 911230 V0.1
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void v_get_cursor_shape()
{
  union REGS regs;

  if (cursorstart == -1 && cursorend == -1)
  {
    regs.h.ah = 3;              /* Get cursor position   */
    regs.h.bh = 0;              /* Number of screen page */
    int86(0x10,&regs,&regs);
    cursorstart = regs.h.ch;
    cursorend   = regs.h.cl;
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    FUNCTION: set_cursor_shape()
     PURPOSE: Set the shape of the cursor
 DESCRIPTION: Set the shape by setting the local variables 'cursorstart'
              and 'cursorend' and calling cursoron()
     RETURNS: nothing
     HISTORY: 910710 V0.1 - Intial version
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_set_cursor_shape(int cstart, int cend)
{
  cursorstart = cstart;
  cursorend   = cend;
  v_cursoron();
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    FUNCTION: v_cursoroff()
     PURPOSE: Turn cursor off
 DESCRIPTION: Hide cursor by calling int 10h function 1.
              The first and last screen line of the cursor will
              be set to 20h. The values are out of the valid range
              so the cursor will disappear.
              First we will save the current cursor shape by calling
              get_cursor_shape()
     RETURNS: nothing
     HISTORY: 911230 V0.1

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_cursoroff()
{
  union REGS regs;

  v_get_cursor_shape();
  regs.h.ah = 1;
  regs.x.cx = 0x2020;
  int86(0x10,&regs,&regs);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    FUNCTION: v_cursoron()
     PURPOSE: Turn cursor on
 DESCRIPTION: Show the cursor by calling int 10h function 01h.
              The first and last screenline of the cursor have
              previously been saved by get_cursor_shape().
     RETURNS: nothing
     HISTORY: 911230 V0.1

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_cursoron()
{
  union REGS regs;

  /*
  get_cursor_shape();
  */
  regs.h.ah = 1;
  regs.h.ch = (BYTE) cursorstart;
  regs.h.cl = (BYTE) cursorend;
  int86(0x10,&regs,&regs);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    FUNCTION: v_setcurpos()
     PURPOSE: Set cursor on (row,col).
 DESCRIPTION: Uses BIOS interrupt 10h, function 02h.
              The origin of the screen is (0,0).
     RETURNS: nothing
     HISTORY: 910710 V0.2 - Changed origin (1,1) to (0,0)

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_setcurpos(int row, int col)
{
  union REGS regs;

  regs.h.ah = 2;
  regs.h.bh = _cursvar.video_page;
  regs.h.dh = (BYTE) row;
  regs.h.dl = (BYTE) col;
  int86(0x10,&regs,&regs);
  return(OK);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: v_getcurpos()
     PURPOSE: Get the position of the cursor on the screen.
 DESCRIPTION: Uses BIOS interrupt 10h, function 03h
              The origin of the screen is (0,0)
     RETURNS: nothing
     HISTORY: 910710 V0.2 - Changed origin (1,1) in (0,0)
--#-func-------------------------------------------------------------------*/
int v_getcurpos(int *row, int *col)
{
  union REGS regs;

  regs.h.ah = 3;
  regs.h.bh = _cursvar.video_page;
  int86(0x10,&regs,&regs);
  *row    = regs.h.dh;
  *col    = regs.h.dl;
  return(OK);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_get_cursor_mode() -
Gets the cursor type to begin in scan line startrow and end in
scan line endrow.  Both values should be 0-31.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_get_cursor_mode(void)
{
  short cmode=0;

  cmode = getdosmemword (0x460);
  return (cmode);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_get_rows()    - Return number of screen rows.
Returns the maximum number of rows supported by the display.
e.g.  25, 28, 43, 50, 60, 66...

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_get_rows(void)
{
    char *env_rows=NULL;
    int rows=0;

/* use the value from LINES environment variable, if set. MH 10-Jun-92 */
/* and use the minimum of LINES and *ROWS.                MH 18-Jun-92 */
    rows = getdosmembyte(0x484) + 1;
    env_rows = (char *)getenv("LINES");
    if (env_rows != (char *)NULL)
        rows = min(atoi(env_rows),rows);

    if ((rows == 1) && (_cursvar.adapter == _MDS_GENIUS))
        rows = 66;
    if ((rows == 1) && (_cursvar.adapter == _MDA))
        rows = 25;  /* new test MH 10-Jun-92 */
    if (rows == 1)
    {
        rows = _default_lines;  /* Allow pre-setting LINES   */
        _cursvar.direct_video = FALSE;
    }
    switch (_cursvar.adapter)
    {
    case _EGACOLOR:
    case _EGAMONO:
        switch (rows)
        {
        case 25:
        case 43:
            break;
        default:
            rows = 25;
        }
        break;

    case _VGACOLOR:
    case _VGAMONO:
/* lets be reasonably flexible with VGAs - they could be Super VGAs */
/* capable of displaying any number of lines. MH 10-Jun-92          */
/*
        switch (rows)
        {
        case 25:
        case 28:
        case 50:
            break;
        default:
            rows = 25;
        }
*/
        break;

    default:
        rows = 25;
        break;
    }
    return (rows);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_get_columns()	- return width of screen/viewport.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_get_columns(void)
{
  union REGS regs;
  int cols=0;
  char *env_cols=NULL;

  /* use the value from COLS environment variable, if set. MH 10-Jun-92 */
  /* and use the minimum of COLS and return from int10h    MH 18-Jun-92 */
  regs.h.ah = 0x0f;
  int86(0x10, &regs, &regs);
  cols = (int)regs.h.ah;
  env_cols = (char *)getenv("COLS");
  if (env_cols != (char *)NULL)
  {
      cols = min(atoi(env_cols),cols);
  }
  return(cols);
}


