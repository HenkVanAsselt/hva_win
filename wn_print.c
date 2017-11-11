/**************************************************************************
$Header: wn_print.c Sun 11-19-2000 10:49:43 pm HvA V1.00 $
***************************************************************************/

#include <stdarg.h>
#include <string.h>
#include "video\v_video.h"
#include "window.h"

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_printf()
    PURPOSE: printf function for windows
     SYNTAX: int wn_printf(WINDOW *w, char *format, ...);
DESCRIPTION: -
    RETURNS: NO_WINDOW: window not opened
             TRUE     : all ok
    HISTORY: 910708 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_printf(WINDOWPTR w, char *format, ...)
{
  va_list arguments;
  int i;
  int row,col;
  int len;
  char s[132];

  /*------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NO_WINDOW);

  /*----------------------------
    Make this the active window
  ------------------------------*/
  if (w != top_window)
    wn_top(w);

  /*-------------
    Build string
  ---------------*/
  va_start(arguments,format);
  vsprintf(s,format,arguments);
  va_end(arguments);

  /*------------------------------------------
    Print string in window with own routines
  -------------------------------------------*/
  i = 0;                      /* Reset counter */
  col = w->ccx;               /* Get window cursor coordinates   */
  row = w->ccy;

  len = strlen(s);
  wn_log("(%02d,%02d)/%-d wn_printf(%p,%s,'%s')\n",row,col,len,w,w->title,s);
  if (s[len-1] != '\n') wn_log("\n");

  while (s[i])
  {
    switch (s[i])
    {
      case 0x0A:     /* LF */
        col = w->x1;
        row++;
        break;

      case 0x0D:     /* CR */
        col = w->x1;
        break;

      case 0x09:     /* HT */
        col += 4;
        break;

      case 0x0B:     /* VT */
        row++;
        break;

      case 0x0C:     /* FF */
        row++;
        col = w->x1;
        break;

      case 0x08:     /* BS */
        if (col <= w->x1)
          col = w->x1;
        else
          col--;
        v_setcurpos(row,col);
        v_printc(row,col,' ');
        break;

      default:
        v_printc(row,col,s[i]);
        col++;
    }

    /*----------------------------------------------
      Test position of cursor row and column, and
      adjust these if neccessary
    -----------------------------------------------*/
    if (col > w->x2) {
      col = w->x1;
      row++;
    }
    if (row > w->y2) {
      wn_scroll(w,1);
      col = w->x1;
      row = w->y2;
    }

    i++;        /* point to next character */

  }


  /*------------------------------------------------
    Update cursor position in window control block
    and on screen
  -------------------------------------------------*/
  w->ccx = col;
  w->ccy = row;
  v_setcurpos(row,col);

  return(TRUE);
}
