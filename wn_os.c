/*---------------------------------------
  WN_OS.C
---------------------------------------*/

#include "video/v_video.h"
#include "window.h"

extern SCREEN _cursvar;

/*#+func-------------------------------------------------------------------
   FUNCTION: OS_shell()
    PURPOSE: Call operating system shell.
     SYNTAX: void OS_shell(void);
 DESRIPTION: Creates new window for the shell and creates a new
             shell, which can be left by 'exit'
    RETURNS: Nothing
    VERSION: 901107 V0.2
--#-func-------------------------------------------------------------------*/
void OS_shell()
{
  WINDOW *OS_window;

  OS_window = wn_open(_NO_BORDER,0,0,_cursvar.cols,_cursvar.lines,
                      16*BLACK+WHITE,16*BLACK+WHITE);
  wn_clear(OS_window);
  v_cursoron();
  printf("Return to simulation program with 'exit' ...\n\n");
  system(getenv("COMSPEC"));
  OS_window = wn_close(OS_window);
}
