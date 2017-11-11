/*---------------------------------------
  WN_DEBUG.C
---------------------------------------*/

#include <stdarg.h>
#include "video/v_video.h"
#include "window.h"

extern SCREEN _cursvar;

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_debug()
     PURPOSE: send debug information to debug window
      SYNTAX: int wn_debug(WINDOWPTR w, char *format, ...);
 DESCRIPTION:
     RETURNS: NO_WINDOW: window not opened
              TRUE     : all ok
     HISTORY: 21-Nov-1992 21:43:43 V
--#-func----------------------------------------------------------------------*/
int wn_debug(char *format, ...)
{
  va_list arguments;
  static int first_time = TRUE;
  static WINDOW *debug_window;
  int old_mon;
  char s[132];

  /*---------------------------
    Switch to second monitor
  ---------------------------*/
  old_mon = monitor;
  wn_monitor(MONITOR_2);

  if (first_time) {
    debug_window = wn_open(_DOUBLE_LINE,0,0,_cursvar.cols,_cursvar.lines,
                   16*BLACK+WHITE,16*BLACK+WHITE);
    if (!debug_window)
      exit(-1);
    wn_title(debug_window,"DEBUG OUTPUT");
    first_time = FALSE;
  }

  /*----------------------------
    Make this the active window
  ------------------------------*/
  /*
  if (debug_window != top_window)
    wn_top(debug_window);
  */

  /*-----------------------
    Build and print string
  ------------------------*/
  va_start(arguments,format);
  vsprintf(s,format,arguments);
  va_end(arguments);
  wn_printf(debug_window,"%s",s);

  /*-----------------------------
    Switch to original monitor
  -----------------------------*/
  wn_monitor(old_mon);

  return(TRUE);
}
