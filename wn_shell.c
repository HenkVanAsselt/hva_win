/**************************************************************************
$Header: wn_shell.c Sun 11-19-2000 10:50:41 pm HvA V1.00 $
***************************************************************************/

#include <stdlib.h>
#include "video/v_video.h"
#include "window.h"

extern SCREEN _cursvar;

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_shell()
    PURPOSE: Calls operating system shell with a command
     SYNTAX: wn_shell(int wait, char *command);
DESCRIPTION: Creates a full-sized window and performs the given command.
    RETURNS: Nothing
    VERSION: 920224 V0.1
--#-func-------------------------------------------------------------------*/
void wn_shell(char *command)
{
  WINDOW *OS_window;

  OS_window = wn_open(_NO_BORDER,0,0,_cursvar.cols,_cursvar.lines,
                      _desktop_att,_desktop_att);
  wn_clear(OS_window);
  v_cursoron();
  printf("Return to simulation program with 'exit' ...\n\n");

  /*  MUST USE SPAWN IN THE FUTURE */

  system(command);
  OS_window = wn_close(OS_window);
}
