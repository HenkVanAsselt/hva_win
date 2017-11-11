/**************************************************************************
$Header: window.c Fri 10-27-2000 8:02:06 pm HvA V2.02 $

DESC: HVA_WINDOWS
	  Dynamic window routines for HVA_WIN Toolbox
HIST: 19900818 V1.00
	  20001027 V2.01 Adapted for DJGPP
***************************************************************************/

#define MAIN_WINDOW_MODULE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include "video/v_video.h"
#include "window.h"

#ifdef __DJGPP
#include <sys/nearptr.h>
#include <crt0.h>
#endif

#ifdef __MSC__
#include <memory.h>
#else
#include <mem.h>
#endif

/* Dynamic memory handling shell */
/*
#include <mshell.h>
*/

/*----- Local variables -----*/
static int startrow;
static int startcol;

/* GLOBAL VARIABLES
-------------------*/
/* VP vptr; */                      /* Pointer to first byte in video RAM */
char   *start_screen = NULL;   /* Pointer to save DOS screen         */
WINDOW *top_window   = NULL;   /* Pointer to top window              */
WINDOW *start_window = NULL;   /* Start window of double linked list */
WINDOW *last_window  = NULL;   /* Last window of double linked list  */

unsigned int non_ibm = 0x0001;

extern SCREEN _cursvar;

/*--------------------------
  Local function prototypes
----------------------------*/
static WINDOW *add_wn_list(WINDOW *w);
static WINDOW *del_wn_list(WINDOW *w);
static void level_wnds(WINDOW *startw);


/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_closeall()
     PURPOSE: Close all opened windows.
      SYNTAX: void wn_closeall(void);
 DESCRIPTION: Goes through the linked window list and closes all windows
              in this list.
     RETURNS: Always returns TRUE
     HISTORY: 920224 V0.1
--#-func----------------------------------------------------------------------*/
int wn_closeall()
{
  WINDOW *w, *nextw;

  /*------------------
    Close all windows
  --------------------*/
  if (start_window != NULL) {
    w = start_window;
    while (w) {
      nextw = w->next;
      wn_close(w);
      w = nextw;
    }
  }
  return(TRUE);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_update()
     PURPOSE: window update
      SYNTAX: int wn_update(void);
 DESCRIPTION: Update entire screen by redrawing all window's from the window
              list in the order as they are in this list on a virtual screen,
              after which this virtual screen will be copied to the physical
              screen.
     RETURNS: TRUE if all OK, FALSE if not
     HISTORY: 920212 V0.1
              920919 V0.2 Now also fills virtual window with _desktop_att
              920920 V0.3 Hidden windows will not be drawn.
--#-func----------------------------------------------------------------------*/
int wn_update()
{
  WINDOW *w;

  wn_log("wn_update() START **** \n");
  wn_log_indent(+2);

  /*-------------------------------
    Save window currently active
  -------------------------------*/
  if (top_window)
    wn_save(top_window);

  /*---------------------------------------------
    Fill virtual screen with desktop attribute if
    no desktop saved
  ---------------------------------------------*/
  /*
  if (!wn_desktop(1))
    v_fillarea(0,0,_cursvar.cols,_cursvar.lines,' ',_desktop_att);
  */
  clrscr();

  /*------------------------------------
    Draw all windows on virtual screen
  -------------------------------------*/
  if (start_window != NULL) {
    w = start_window;
    while (w) {
      if (! (w->flag & HIDDEN))     /* Restore window if not hidden */
        wn_restore(w);
      w = w->next;
    }
  }

  wn_log_indent(-2);
  wn_log("wn_update() END ****\n");

  /*---------------------
    Return all OK flag
  ----------------------*/
  return(TRUE);
}


/*#+func-------------------------------------------------------------------
   FUNCTION: wn_dmode()
    PURPOSE: Set window display mode (PAINT or FLASH)
DESCRIPTION: This a dummy for compatibility with WindowBoss
    RETURNS: nothing
    HISTORY: 910709 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void wn_dmode(int mode)
{
  mode++;
}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_save()
    PURPOSE: Save a window, including the border (if any)
     SYNTAX: WINDOW *wn_save(WINDOWPTR w);
DESCRIPTION: If no space was allocated, this function will allocate
             memory to save the window. If space was allocated, we
             assume that the size of the window has not been changed.
    RETURNS: nothing
    HISTORY: 910709 V0.1 - Initial version
             920106 V0.2 - Now also saves the border (if any)
             920916 V0.3 - Now also saves the area for the shadow effect
             931011 V0.4 - Now call v_gettext (origin 0,0)
--#-func-------------------------------------------------------------------*/
WINDOW *wn_save(WINDOWPTR w)
{
  int row,col;

  if (!w)              /* Check if window opened */
    return(NULL);
  wn_log("wn_save(%p,%s) (%d,%d)-(%d,%d) \n",w,w->title,w->wx1,w->wy1,w->wx2,w->wy2);

  /*---------------------------------------
    Allocate memory for saving window
    (inclusive border and shadow)
  ---------------------------------------*/
  if (!w->scrnsave) {
    w->scrnsave = (char *) calloc((w->xsize)*(w->ysize)*2,1);
    if (!w->scrnsave) {
      fprintf(stderr,"Not enough memory to save window\n");
      exit(1);
    }
  }

  /*---------------------------------------
    Save contents of the window
  ---------------------------------------*/
  v_gettext(w->wx1,w->wy1,w->wx2,w->wy2,w->scrnsave);

  /*-------------------------------
    Save cursor info of the window
  ---------------------------------*/
  v_getcurpos(&row,&col);
  w->ccy = row;
  w->ccx = col;

  return(w);        /* Return pointer to last saved window */

}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_hide()
    PURPOSE: Hide a window
     SYNTAX: int wn_hide(WINDOWPTR w);
DESCRIPTION: Hide a window by settting the window flag to HIDDEN and
             update the complete screen. Hidden windows will not be drawn
    RETURNS: NO_WINDOW if window 'w' not opened.
             TRUE if all OK.
    HISTORY: 920130 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_hide(WINDOWPTR w)
{
  wn_log("wn_hide(%p,%s)\n",w,w->title);

  if (!w)                   /* Check if window opened */
    return(NO_WINDOW);
  w->flag = TRUE;           /* Set hidden flag        */
  wn_update();              /* Update all windows     */
  return(TRUE);
}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_unhide()
    PURPOSE: Unhide a window
     SYNTAX: int wn_unhide(WINDOWPTR w);
DESCRIPTION: Unhide a window by resettting the hideflag
             update the complete screen.
    RETURNS: NO_WINDOW if window 'w' not opened.
             TRUE if all OK.
    HISTORY: 920130 V0.1 - Initial version
             931021 V0.2 - Now uses w->hideflag (was w->flag)
--#-func-------------------------------------------------------------------*/
int wn_unhide(WINDOWPTR w)
{
  wn_log("wn_unhide(%p,%s)\n",w,w->title);

  if (!w)                       /* Check if window opened */
    return(NO_WINDOW);
  w->hideflag = 0;              /* Reset hidden flag */
  wn_update();                  /* Update all windows */
  return(TRUE);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_move()
     PURPOSE: Move a window on the screen
      SYNTAX: int wn_move(WINDOW *w , int row, int col);
 DESCRIPTION: Move window 'w' to 'row' and 'col'
     RETURNS: NO_WINDOW if window 'w' not opened.
              TRUE if all OK.
     HISTORY: 920920 V0.1 - Initial version
--#-func----------------------------------------------------------------------*/
int wn_move(WINDOW *w , int row, int col)
{
  int dx = 0;
  int dy = 0;

  wn_log("wn_move(%p,%s,%2d,%2d)\n",w,w->title,row,col);
  wn_log_indent(+2);

  /*--------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NO_WINDOW);

  /*--------------------------
    Check parameters
  --------------------------*/
  row = minmax(0,row,_cursvar.lines);
  col = minmax(0,col,_cursvar.cols);

  /*----------------------------------------------
    Calculate in which direction we have to move
  -----------------------------------------------*/
  if (row > w->wy1) dy=1; else dy=-1 ;
  if (col > w->wx1) dx=1; else dx=-1 ;

  /*---------------------------------
    Move the window over the screen
  -----------------------------------*/
  do {
    /* Update column pointers */
    if (((dx>0 && w->wx2<_cursvar.cols) || (dx<0 && w->wx1 > 0)) && w->wx1 != col) {
      w->wx1 += dx;
      w->wx2 += dx;
      w->x1  += dx;
      w->x2  += dx;
    }
    /* Update row pointers */
    if (((dy>0 && w->wy2 < _cursvar.lines) || (dy<0 && w->wy1 > 0)) && w->wy1 != row) {
      w->wy1 += dy;
      w->wy2 += dy;
      w->y1  += dy;
      w->y2  += dy;
    }
    wn_restore(w);
    wn_update();
    wn_log("target row=%2d, target col=%2d\n",row,col);
    wn_log("moved to (%02d,%02d)\n",w->wx1, w->wy1);
  }
  while ((w->wx1 != col) || (w->wy1 != row));

  wn_log_indent(-2);
  wn_log("end of wn_move()\n");

  return(0);

}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_restore()
    PURPOSE: Restore window border, contents and cursor on the screen.
     SYNTAX: void wn_restore(WINDOWPTR w);
DESCRIPTION: -
    RETURNS: nothing
    HISTORY: 910709 V0.1 - Initial version

--#-func-------------------------------------------------------------------*/
void wn_restore(WINDOWPTR w)
{

  wn_log("wn_restore(%p,%s) (%d,%d)-(%d,%d)\n",w,w->title,w->wx1,w->wy1,w->wx2,w->wy2);

  if (!w)             /* Check if window opened */
    return;
  if (w->hideflag)    /* Check if window not hidden */
    return;

  if (w->scrnsave)    /* Restore contents of window */
    v_puttext(w->wx1,w->wy1,w->wx2,w->wy2,w->scrnsave);

  wn_fixcsr(w);     /* Initialize cursor */

}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_top()
    PURPOSE: Make a window the top window
     SYNTAX: int wn_top(WINDOW *w);
DESCRIPTION: Make window 'w' the top window by:
             Transfering it to the end of the list of windows
             Saving the contents of the current top window
             Restoring the contents of window 'w'
    RETURNS: NO_WINDOW: window not opened
             TRUE     : all ok
    HISTORY: 910709 V0.1
--#-func-------------------------------------------------------------------*/
int wn_top(WINDOW *w)
{
  if (!w)                    /* Check if the window is opened */
    return(NO_WINDOW);
  wn_log("wn_top(%p,%s)\n",w,w->title);

  if (w != last_window) {    /* If the window is not the last window */
    del_wn_list(w);          /* Of the double linked list, put it in */
    add_wn_list(w);          /* the last position */
  }

  if (w == top_window)       /* Check if window not already top window */
    wn_fixcsr(w);
  else
  {
    if (top_window != NULL)
      wn_save(top_window);   /* Save contents of active window */
    top_window = w;          /* Set pointer to top window */
    wn_restore(w);           /* Restore contents of new active window */
  }

  return(TRUE);
}


/*#+func-------------------------------------------------------------------
   FUNCTION: wn_dma()
    PURPOSE: Write flag (TRUE or FALSE) to video RAM
     SYNTAX: void wn_dma(int flag);
DESCRIPTION: Dummy function to be compatible with WindowBoss
    RETURNS: nothing
    HISTORY: 910709 V0.1 - Intitial version
--#-func-------------------------------------------------------------------*/
void wn_dma(int flag)
{
  flag++;
}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_sync()
    PURPOSE: set/clear window's cursor synchronisation flag
     SYNTAX: wn_sync(WINDOW *w, int flag);
DESCRIPTION: If flag is TRUE, subsequent text output will be
             followed by a flashing (normal) cursor. If FALSE
             the cursor will not be physically advanced
    RETURNS: A copy of the flag
    HISTORY: 910709 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_sync(WINDOW *w, int flag)
{
  /*------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NO_WINDOW);

  w->synflg = flag;
  return(flag);
}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_fixcsr()
    PURPOSE: Update window's cursor position
     SYNTAX: int wn_fixcsr(WINDOW *w);
DESCRIPTION: Place the physical cursor at the logical window cursor
             position. Does not alter the state of window's syncflag.
             Is mostly used after a wn_sync() to disable the cursor.
    RETURNS: TRUE, or FALSE if error
    HISTORY: 910709 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_fixcsr(WINDOW *w)
{
  /*------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NO_WINDOW);

  /*--------------------------------
    Check if this is the top window
  ----------------------------------*/
  if (w != top_window)
    return(NO_WINDOW);

  /*--------------------------------------
    Get physical cursor coordinates and
    show cursor (if wanted
  --------------------------------------*/
  v_setcurpos(w->ccy,w->ccx);
  if (w->synflg)
    v_cursoron();
  else
    v_cursoroff();

  return(TRUE);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_fill()
     PURPOSE: Fill part of a opened window with character and attribute
      SYNTAX: void wn_fill(WINDOW *w, int row, int col, int width, int height,
              int c, int att);
 DESCRIPTION:
     RETURNS: Nothing
     HISTORY: 14-Oct-1993 V0.1
--#-func----------------------------------------------------------------------*/
void wn_fill(WINDOW *w, int row, int col, int width, int height, int c, int att)
{
  v_fillarea(w->y1+row,w->x1+col,width,height,c,att);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_setattrib()
     PURPOSE: Set attribute of a field within a window
      SYNTAX: void wn_setattrib(WINDOW *w, int row1, int col1, int xsize,int ysize, int att);
 DESCRIPTION: -
     RETURNS: Nothing
     HISTORY: 14-Oct-1993 V0.1
--#-func----------------------------------------------------------------------*/
void wn_setattrib(WINDOW *w, int row1, int col1, int xsize,int ysize, int att)
{
  v_setattrib_area(w->y1+row1, w->x1+col1, xsize, ysize, att);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_setone()
     PURPOSE: Set attribute of 1 character in a window
      SYNTAX: int wn_setone(WINDOW *w, int row, int col, int attrib);
              WINDOW *w : pointer to window to perform action on
              int row   : row (y) of position to set
              int col   : col (x) of position to set
              int attrib: attribute to set on (row,col) in window 'w'
 DESCRIPTION: -
     RETURNS: 0         if all OK
              NO_WINDOW if window not opened
     HISTORY: 920926 V0.1
--#-func----------------------------------------------------------------------*/
int wn_setone(WINDOW *w, int row, int col, int attrib)
{
  /*----------------------------------
    Check if window opened and topped
  -----------------------------------*/
  if (!w)
    return(NO_WINDOW);

  /*---------------------------
    Call the routine setone()
  ----------------------------*/
  v_setattrib(w->y1+row,w->x1+col,attrib);
  return(0);
}


/*#+func-------------------------------------------------------------------
   FUNCTION: wn_nattrib()
    PURPOSE: Set and change window attribute
     SYNTAX: int wn_nattrib(WINDOW *w, int wattrib);
DESCRIPTION: The attribute of window 'w'is changed immediately.
             The border will not be altered.
    RETURNS: NO_WINDOW if no window opened.
             TRUE if all ok.
    HISTORY: 910907 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_nattrib(WINDOW *w, int wcolor)
{
  /*----------------------------------
    Check if window opened and topped
  -----------------------------------*/
  if (!w)
    return(NO_WINDOW);
  if (w != top_window)
    wn_top(w);

  w->wcolor = wcolor;     /* Save attribute */
  v_setattrib_area(w->y1, w->x1, w->xsize, w->ysize, wcolor);
  return(TRUE);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: add_wn_list()
     PURPOSE: Add window to double linked list
      SYNTAX: WINDOw *add_wn_list(WINDOW *w);
 DESCRIPTION: -
     RETURNS: pointer to window at start of list
     HISTORY: 910730 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static WINDOW *add_wn_list(WINDOW *w)
{
  WINDOW *tmp_w;

  wn_log("add_wn_list(%p,%s)\n",w,w->title);

  /*-----------------------
    Check if window opened
  -------------------------*/
  if (!w)
    return(NULL);

  /*-----------------------------------
    Add window to double linked list
  ------------------------------------*/
  if (start_window == NULL)
  {
    /*--- This is the first window in the list ---*/
    last_window  = w;
    start_window = w;
    w->next   = NULL;
    w->prev   = NULL;
  }
  else
  {
    /*--- Add this window to end of the list ---*/
    last_window->next = w;
    w->next  = NULL;
    w->prev  = last_window;
    last_window = w;
  }

  tmp_w = start_window;
  while(tmp_w)
    tmp_w = tmp_w->next;

  return(start_window);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: del_wn_list()
     PURPOSE: delete window control block from double linked list
      SYNTAX: WINDOW *del_wn_list(WINDOW *w);
 DESCRIPTION: -
     RETURNS: pointer to start of list
     HISTORY: 910730 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static WINDOW *del_wn_list(WINDOW *w)
{
  wn_log("del_wn_list(%p,%s)\n",w,w->title);

  /*------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NULL);

  if (w == start_window)
  {
    /*--- We are going to delete the first window in the
          double linked list ---*/
    start_window = start_window->next;
    if (start_window)
      start_window->prev = NULL;
    if (!start_window)   /* If no window opened any more ... */
      last_window = NULL;
  }
  else {
    /*--- We are going to delete a window in the middle or at the
          end of the double linked list ---*/
    if (!w->next) {
      /*--- This window is the last one of the list ---*/
      last_window = w->prev;
      w->prev->next = NULL;
    }
    else {
      /*--- This window is in the middle of the list ---*/
      w->prev->next = w->next;
      w->next->prev = w->prev;
    }
  }

  level_wnds(start_window);
  return(start_window);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: level_windows()
     PURPOSE: This funtion runs thru the list from the beginning and
              reassignes level values to each window after and inser-
              tion or deletion.
      SYNTAX: static void level_wnds(WINDOW *startw);
 DESCRIPTION:
     RETURNS: nothing
     HISTORY: 21-Oct-1993 V0.1
--#-func----------------------------------------------------------------------*/
static void level_wnds(WINDOW *startw)
{
  int  i = 1;
  WINDOW *w;

  w = startw;
  while (w) {               /* While window pointer not NULL */
    w->level = i++;         /* Assign a new level number */
    wn_log("  --- %p (%s) level:%2d\n",w,w->title,w->level);
    w = w->next;            /* Get next window */
  }
}


/*#+func-------------------------------------------------------------------
   FUNCTION: wn_open()
    PURPOSE: open and use a new text window. Origin is (0,0)
     SYNTAX: WINDOW *wn_open(int btype, int row, int col, int xsize,
                             int ysize, int wcolor, int bcolor);
DESCRIPTION: btype   = border type (_NO_BORDER,_SINGLE_LINE,_DOUBLE_LINE).
             row     = top left row of window
             col     = top left column of window
             xsize   = width of contents of window  (inclusive border)
             ysize   = height of contents of window (inclusive border)
             wcolor  = color of window contents
             bcolor  = color of border
    RETURNS: Pointer to the window control block of opened window
    HISTORY: 910709 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
WINDOW *wn_open( int btype,         /* Window type      */
                 int row,           /* Top left row     */
                 int col,           /* Top left corner  */
                 int xsize,         /* External width   */
                 int ysize,         /* External height  */
                 int wcolor,        /* Window attribute */
                 int bcolor)        /* Border attribute */
{
  WINDOWPTR w;

  /*----------------------------------
    Reserve memory for the window
  -----------------------------------*/
  w = (WINDOWPTR) calloc(sizeof(WINDOW),1);
  if (!w) {
    fprintf(stderr,"Not enough memory to open window\n");
    exit(1);
  }

  wn_log("wn_open() %p: (%2d,%2d) w=%2d h=%2d\n",w,row,col,xsize,ysize);

  /*------------------
    Check parameters
  -------------------*/
  if (btype == _NO_BORDER) {
    wn_log("wn_open(1) NO_BORDER\n");
    /*---------------------------------------
      Window has no border
    ---------------------------------------*/
    ysize  = minmax(0,ysize,_cursvar.lines);
    xsize  = minmax(0,xsize,_cursvar.cols);
	wn_log("wn_open(2) xsize = %d   ysize = %d\n",xsize,ysize);

    row = minmax(0,row,_cursvar.lines);
    col = minmax(0,col,_cursvar.cols);
	wn_log("wn_open(3) row = %d   col = %d\n",row,col);

    if (row+ysize > _cursvar.lines)
      ysize = _cursvar.lines-row;
    if (col+xsize > _cursvar.cols)
      xsize = _cursvar.cols-col;
	wn_log("wn_open(4) xsize = %d   ysize = %d\n",xsize,ysize);
  }
  else {

    wn_log("wn_open() WITH BORDER");
    /*---------------------------------------
      Window has a border
    ---------------------------------------*/
    ysize  = minmax(0,ysize,_cursvar.lines);
    xsize  = minmax(0,xsize,_cursvar.cols);
	wn_log("wn_open() xsize = %d   ysize = %d\n",xsize,ysize);

    row = minmax(0,row,_cursvar.lines-4);
    col = minmax(0,col,_cursvar.cols-4);
	wn_log("wn_open() row = %d   col = %d\n",row,col);

    if (row+ysize > _cursvar.lines-2)
      ysize  = (_cursvar.lines-row);
    if (col+xsize > _cursvar.cols-2)
      xsize = _cursvar.cols-col;
	wn_log("wn_open() xsize = %d   ysize = %d\n",xsize,ysize);
  }

  /*---------------------------------------
    Save coordinates of complete window
  ---------------------------------------*/
  w->wx1 = col;
  w->wy1 = row;
  w->wx2 = w->wx1 + xsize - 1;
  w->wy2 = w->wy1 + ysize - 1;

  /*---------------------------------------
    Save coordinates of 'text' window
  ---------------------------------------*/
  if (btype == _NO_BORDER) {
    w->x1 = w->wx1;
    w->y1 = w->wy1;
    w->x2 = w->wx2;
    w->y2 = w->wy2;
  }
  else {   /* Window has a border */
    w->x1 = w->wx1 + 1;
    w->y1 = w->wy1 + 1;
    w->x2 = w->wx2 - 1;
    w->y2 = w->wy2 - 1;
  }

  wn_log("wn_open(5) (%d,%d) x2 = %d  y2=%d\n",w->x1,w->y1,w->x2,w->y2);

  /*-----------------------------------
    Save variables in window structure
  -------------------------------------*/
  w->btype     = btype;           /* Border type         */
  w->xsize     = xsize;
  w->ysize     = ysize;
  w->wcolor    = wcolor;
  w->bcolor    = bcolor;
  w->ccy       = row;            /* Initialize screen cursor row  */
  w->ccx       = col;            /* Initialize screen cursor col  */
  w->synflg    = FALSE;          /* Standard cursor off           */
  w->scroll_ok = TRUE;           /* Standard scrolling window     */
  w->scrnsave  = NULL;
  w->monitor   = monitor;
  w->title     = NULL;
  w->page      = 0;

  wn_log("Variables are initialized\n");

  /*---------------------------------------
    Save contents of current active window
    Add window to double linked list
  --------------------------------------*/
  if (top_window != NULL) {
    wn_save(top_window);
  }
  add_wn_list(w);
  top_window = w;

  /*--------------------------------
    Draw window border (if wanted)
  --------------------------------*/
  if (btype != _NO_BORDER)
  {
    wn_border(w,btype);
  }
  /*-----------------------
    Add the shadow effect
  ------------------------*/
  if ( btype != _NO_BORDER && shadow != FALSE)
    wn_shadow(w);

  /*------------------------------
    Clear the inside of the window
  --------------------------------*/
  wn_clear(w);

  return(w);
}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_close()
    PURPOSE: close a window
     SYNTAX: WINDOW *wn_close(WINDOW *w);
DESCRIPTION: Checks if window defined,
             Deletes it from the linked window list
             Free's the memory allocated to it.
             Makes the last window in the linked list the top window
             Updates the screen.
    RETURNS: Alway's returns NULL pointer
    HISTORY: 910709 V0.1 - Initial version
             920919 V0.2 - If the window closed was the top window,
                           a NULL pointer will be assigned to 'top_window'
--#-func-------------------------------------------------------------------*/
WINDOW *wn_close(WINDOW *w)
{

  wn_log("wn_close(%p,%s)\n",w,w->title);

  /*---------------------------
    Check if window was opened
  -----------------------------*/
  if (w) {

    /*-------------------------------------------
      Check if window to close is the top window
    ---------------------------------------------*/
    if (w == top_window) {
      top_window = NULL;
      last_window = NULL;
    }

    /*-----------------------------------------------
      Delete the window from the double linked list
      and free memory of window's save area and
      control block
    -------------------------------------------------*/
    del_wn_list(w);
    if (w->scrnsave)
      free(w->scrnsave);
    if (w->title)
      free(w->title);
    free((char *) w);

    /*-----------------------------------------------
      Make last window in linked list the top window
      and update the screen
    -------------------------------------------------*/
    wn_top(last_window);
    wn_update();
  }

  return(NULL);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: wn_init()
     PURPOSE: initialize window and video enviroment
      SYNTAX: void wn_init(void);
 DESCRIPTION: Sets atexit() pointer.
              Initializes global variables _NormAttr, _TextAttr, _LastMode,
              _CurCrtSize, _CheckSnow, _DirectVideo, _WindMin and _WindMax.
              by calling the assembler function crtinit().
              Sets the segmentaddress 'displayseg' of the physical videoscreen.
              Gets the current cursor size and position and saves the contents
              of the current screen and clears it
     RETURNS: nothing
     HISTORY: 910710 V0.2 - Adjusted for window enviroment
--#-func-------------------------------------------------------------------*/
void wn_init()
{
  /*------------------
    Initializations
  ------------------*/
  wn_log("wn_init()\n");
  atexit(wn_exit);

  /*-----------------------------------------------------------------
     Initializes global variables _NormAttr, _TextAttr, _LastMode,
     _CurCrtSize, _CheckSnow, _DirectVideo, _WindMin and _WindMax.
  -----------------------------------------------------------------*/
  initscr();

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif

  /*-----------------------------
    Get original cursor position
  --------------------------------*/
  v_getcurpos(&startrow,&startcol);
  v_cursoroff();

  set_display_colors(COLOR);

  /*---------------------
    Open the top window
  ----------------------*/
  start_screen = v_savescreen(_cursvar.cols,_cursvar.lines);

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif

  clrscr();

}

/*#+func-------------------------------------------------------------------
    FUNCTION: wn_exit()
     PURPOSE: Exit from window enviroment
      SYNTAX: void wn_exit(void);
 DESCRIPTION: Calls several routines to clean up the HvA window enviroment.
              Closes all open windows.
              Restores the screen which was there when starting the window
              enviroment, restores the cursor position and shows it.
              Close the logfile 'window.log' (if opened).
     RETURNS: nothing
     HISTORY: 910717 V0.1 - Initial version
              920107 V0.1 - The functions will be performed only once
--#-func-------------------------------------------------------------------*/
void wn_exit()
{
  static int done = FALSE;

  if (!done)               /* Do this only once */
  {
    wn_log("wn_exit()\n");

    exit_help();
    exit_pulldown();
    exit_funkey();
    wn_closeall();
    if (start_screen)
      start_screen = v_restorescreen(start_screen);
    v_setcurpos(startrow,startcol);
    v_cursoron();

    done = TRUE;        /* Set flag (this routine has been executed */
  }
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: wnd_overlap()
     PURPOSE: Determines if two windows overlap the each other
      SYNTAX: int wn_overlap(WINDOW *w1, WINDOW *w2);
 DESCRIPTION:
     RETURNS: FALSE if no overlap, TRUE if there is an overlap
     HISTORY: 21-Oct-1993 V0.1
--#-func----------------------------------------------------------------------*/
int wn_overlap(WINDOW *w1, WINDOW *w2)
{
  if (w1->page != w2->page)          /* If not in same page */
     return(FALSE);                  /* they can't overlap  */

  if (((w2->x1 >= w1->x1) && (w2->x1 <= w1->x2)) &&
     (((w2->y1 >= w1->y1) && (w2->y1 <= w1->y2)) ||
      ((w2->y2 >= w1->y1) && (w2->y2 <= w1->y2))))
          return(TRUE);

  if (((w2->x2 >= w1->x1) && (w2->x2 <= w1->x2)) &&
     (((w2->y1 >= w1->y1) && (w2->y1 <= w1->y2)) ||
      ((w2->y2 >= w1->y1) && (w2->y2 <= w1->y2))))
          return(TRUE);

  if (((w1->x1 >= w2->x1) && (w1->x2 <= w2->x2)) &&
     (((w1->y1 >= w2->y1) && (w1->y1 <= w2->y2)) ||
      ((w1->y2 >= w2->y1) && (w1->y2 <= w2->y2))))
          return(TRUE);

  if (((w1->y1 >= w2->y1) && (w1->y2 <= w2->y2)) &&
     (((w1->x1 >= w2->x1) && (w1->x1 <= w2->x2)) ||
      ((w1->x2 >= w2->x1) && (w1->x2 <= w2->x2))))
          return(TRUE);

  if ((w1->x1 > w2->x1 && w1->x2 < w2->x2) &&
      (w1->y1 > w2->y1 && w1->y2 < w2->y2))
          return(TRUE);

  return(FALSE);
}











