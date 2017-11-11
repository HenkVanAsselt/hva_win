/**************************************************************************
$Header: WN_YESNO.C Mon 11-06-2000 10:10:25 pm HvA V1.00 $
***************************************************************************/

#include <stdarg.h>
#include "keys.h"
#include "window.h"

/*#+func-------------------------------------------------------------------
    FUNCTION: wn_yesno()
     PURPOSE: Display an message in a window with 2 exit buttons: YES and NO
      SYNTAX: int wn_yesno(char *format, ...);
 DESCRIPTION: uses wn_dialog in the middle of the screen.
     RETURNS: Choise to end this dialog: 'Y', 'N' or ESC.
     HISTORY: 920213 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
int wn_yesno(char *format, ...)
{
  va_list arguments;
  int _tmp_att;
  char s[132];

  static MENU menu[] =
  {
    {"Yes", 0 },
    {"No",  0 },
    { NULL }
  };
  int choise = ESC;

  menu[0].flags |= EXIT;
  menu[1].flags |= EXIT;

  va_start(arguments,format);
  vsprintf(s,format,arguments);
  va_end(arguments);

  _tmp_att = _dialog_att;
  choise = wn_dialog(13,40,2,menu,1,s);
  _dialog_att = _tmp_att;

  return(choise);
}
