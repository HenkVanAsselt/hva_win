/**************************************************************************
$Header: wn_scrol.c Sun 11-19-2000 10:50:29 pm HvA V1.00 $

***************************************************************************/

#include "video/v_video.h"
#include "window.h"

/*#+func-------------------------------------------------------------------
    FUNCTION: wn_scroll()
     PURPOSE: Scroll a window up or down
      SYNTAX: int wn_scroll(WINDOW *w, int i);
              WINDOW *w : pointer to WCB of window to scroll
              int i     : number of lines to scroll.
                          i > 0 --> scroll up i lines
                          i < 0 --> scroll down i lines
 DESCRIPTION: Routines taken from C-Chest (CURSES) from Allen Holub, pp 360.
              The window to scroll will alway become the active window
     RETURNS: Status of window's scroll-ok flag or:
              NO_WINDOW if no window opened
     HISTORY: 910801 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_scroll(WINDOW *w, int i)
{
  char *ptr;
  int  xsize;
  int  ysize;

  wn_log("wn_scroll(window=%p,%s,lines=%1d)\n",w,w->title,i);

  /*---------------------------------------
    Check if scrolling has to be performed
  ---------------------------------------*/
  if (i == 0)
    return(-1);

  /*------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NO_WINDOW);

  /*---------------------------------------
    Check if scrolling is OK
  ---------------------------------------*/
  if (!w->scroll_ok)
    return(-2);

  /*-------------------------------------------------------------
    Check if this is the active (top) window. If not, make it so
  --------------------------------------------------------------*/
  if (w != top_window)
    wn_top(w);

  xsize = w->x2 - w->x1 + 1;
  ysize = w->y2 - w->y1 + 1;

  /*-------------------------------------------------------------
    Allocate enough memory for saving the contents of the window
  ---------------------------------------------------------------*/
  ptr = (char *) calloc(xsize*ysize*2,1);
  if (!ptr)
  {
    fprintf(stderr,"Not enough memory to save window\n");
    return(-3);
  }

  if (i > 0) {
    /*---------------------------------------
      SCROLL UP:
      Copy the part of the window to scroll
      Past the part in the window
      Clear the remainder
    ---------------------------------------*/
    v_gettext(w->x1,w->y1+i,w->x2 ,w->y2,  ptr);
    v_puttext(w->x1,w->y1,  w->x2 ,w->y2-i,ptr);
    wn_fill(w,ysize-1,0,xsize,1,' ',w->wcolor);
 }

  if ( i < 0) {
    /*---------------------------------------
      SCROLL DOWN:
      Copy the part of the window to scroll
      Past the part in the window
      Clear the remainder
    ---------------------------------------*/
    v_gettext(w->x1,w->y1,  w->x2, w->y2+i,ptr);  /* i is negative !!! */
    v_puttext(w->x1,w->y1-i,w->x2, w->y2  ,ptr);
    wn_fill(w,0,0,xsize,1,' ',w->wcolor);
  }

  free(ptr);

  /*--------------------------------
    Return status of scroll_ok flag
  ----------------------------------*/
  return(w->scroll_ok);
}
