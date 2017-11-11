/*---------------------------------------
  WN_LOC.C
---------------------------------------*/

#include "video/v_video.h"
#include "window.h"


/*#+func-------------------------------------------------------------------
   FUNCTION: wn_locate()
    PURPOSE: Locate cursor in window
     SYNTAX: int wn_locate(WINDOW *w, int row, int col);
DESCRIPTION: Set cursor on position (row,col) relative to window's
             origin ( (0,0) = upper lefthand corner).
             N.B. Window w will become the top window.
    RETURNS: TRUE, or FALSE if error
    HISTORY: 910709 V0.1 - Initial version (always returns TRUE)
             14-Oct-1993 V0.2 - Now w->x2 and w->y2 are maximum limits.
                                0,0 is minimum limit
--#-func-------------------------------------------------------------------*/
int wn_locate(WINDOW *w, int row, int col)
{
  /*------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NO_WINDOW);

  /*------------------
    Activate window
  -------------------*/
  if (w!=top_window)
    wn_top(w);

  /*------------------------
    Check cursor position
  ------------------------*/
  if (row > w->y2)
    row = w->y2;
  if (col > w->x2)
    col = w->x2;
  if (row < 0)
    row = 0;
  if (col < 0)
    col = 0;

  /*------------------------------
    Set window cursor coordinates
  --------------------------------*/
  w->ccx = w->x1 + col;
  w->ccy = w->y1 + row;

  /*----------------------
    Set cursor in window
  -----------------------*/
  v_setcurpos(w->ccy,w->ccx);
  if (w->synflg)
    v_cursoron();
  else
    v_cursoroff();

  return(TRUE);
}
