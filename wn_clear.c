/**************************************************************************
$Header: WN_CLEAR.C Fri 10-27-2000 9:43:56 pm HvA V2.01 $

DESC: clear window function for HVA_WIN
HIST: 20001027 V2.01 - Adapted for DJGPP
***************************************************************************/

/*---------------------------------------
  WN_CLEAR.C
---------------------------------------*/

#include "video/v_video.h"
#include "window.h"


/*#+func-------------------------------------------------------------------
   FUNCTION: wn_clear()
    PURPOSE: Clear contents of a window
     SYNTAX: int wn_clear(WINDOW *w);
DESCRIPTION: -
    RETURNS: NO_WINDOW if no window opened
             TRUE      if all ok
    HISTORY: 910718 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_clear(WINDOW *w)
{
  int l,h;

  wn_log("wn_clear(%p,%s) (%d,%d) x2=%d  y2=%d\n",w,w->title,w->y1,w->x1,w->x2,w->y2);

  /*----------------------------------------
    Check if window opened and top window.
  -----------------------------------------*/
  if (!w)
    return(NO_WINDOW);
  if (w != top_window)
    wn_top(w);

  /*-----------------------------------------------------
    Clear window by filling in attribute and SPACE's and
    Put cursor in upper-right corner of window
  -------------------------------------------------------*/
  l = w->x2 - w->x1 + 1;
  h = w->y2 - w->y1 + 1;
  wn_log("l=%d   h=%d   color=%d   \n",l,h,w->wcolor);
  v_fillarea(w->y1,w->x1,l,h,' ',w->wcolor);
  wn_locate(w,0,0);

  return(TRUE);
}
