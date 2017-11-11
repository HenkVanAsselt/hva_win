/**************************************************************************
$Header: wn_title.c Sun 11-19-2000 10:50:14 pm HvA V1.00 $
***************************************************************************/

/*---------------------------------------
  WN_TITLE.C
---------------------------------------*/

#include <stdarg.h>
#include <string.h>
#include "window.h"

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_title()
    PURPOSE: Draw title in topline of the window
     SYNTAX: int wn_title(WINDOW *w, char *format, ...)
DESCRIPTION: The window has to have a border.
             The window used will become the active window.
    RETURNS: TRUE  if all is well,
             FALSE if title is too big to fit on window
             NO_WINDOW if no window opened.
    HISTORY: 910907 V0.1
--#-func-------------------------------------------------------------------*/
int wn_title(WINDOWPTR w, char *format, ...)
{
  va_list arguments;
  char title[80];    /* Title string     */


  /*---------------------------------------------------
    Check if window opened and if it is the top window
  ----------------------------------------------------*/
  if (!w)
    return(NO_WINDOW);
  if (w != top_window)
    wn_top(w);

  /*------------------------------
    Check if window has a border
  -------------------------------*/
  if (w->btype == _NO_BORDER)
    return(NO_WINDOW);

  /*--------------------
    va_... housekeeping
  ----------------------*/
  va_start(arguments,format);

  /*-------------
    Build string
  ---------------*/
  vsprintf(title,format,arguments);

  /*---------------------
    va_... house-keeping
  -----------------------*/
  va_end(arguments);

  /*----------------------------------------------------------
    Store the title. If already space allocated for titel in
    the window structure, free this first
  ----------------------------------------------------------*/
  if (w->title)
    free(w->title);
  w->title = strdup(title);

  /*-----------------
    Print the title
  ------------------*/
  wn_log("wn_title(%p,%s)\n",w,title);
  wn_border(w,w->btype);
  if (strlen(title) < (w->xsize-2))
  {
    printcenter(w->y1 - 1, w->x1 + (w->xsize >> 1), title);
    return(TRUE);
  }
  else
    return(FALSE);
}
