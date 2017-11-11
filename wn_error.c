/**************************************************************************
$Header: WN_ERROR.C Mon 11-06-2000 10:09:10 pm HvA V1.00 $
***************************************************************************/

#include <stdarg.h>
#include "window.h"

/*#+func-------------------------------------------------------------------
    FUNCTION: wn_error()
     PURPOSE: Display an message in a window with 1 exit button
      SYNTAX: void wn_error(char *format, ...);
 DESCRIPTION: uses wn_dialog in the middle of the screen.
     RETURNS: nothing
     HISTORY: 910717 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void wn_error(char *format, ...)
{
  va_list arguments;
  static MENU menu[] = {{"OK"}};
  int _tmp_att;
  char s[132];

  wn_log("BEGIN WN_ERROR()\n");
  wn_log_indent(+3);

  menu[0].flags |= EXIT;

  va_start(arguments,format);
  vsprintf(s,format,arguments);
  va_end(arguments);

  _tmp_att = _dialog_att;
  wn_dialog(13,40,1,menu,1,s);
  _dialog_att = _tmp_att;

  wn_log_indent(-3);
  wn_log("END WN_ERROR\n");
}
