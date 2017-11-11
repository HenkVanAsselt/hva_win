/**************************************************************************
$Header: WN_DESK.C Fri 10-27-2000 8:33:57 pm HvA V2.01 $

DESC:  Desktop utilities for HVA_WIN
HIST:  20001027 V2.01 - Adapting for DJGPP
***************************************************************************/

#include "video/v_video.h"
#include "window.h"
#include "fortify.h"

extern SCREEN _cursvar;

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_desktop()
     PURPOSE: Save or restore current screen as the desktop
      SYNTAX: int wn_desktop(int mode);
 DESCRIPTION: int mode : 0 = Save current screen as desktop
                         1 = Restore saved desktop (if any)
     RETURNS: TRUE if succesfull restorage, FALSE if not one saved
     HISTORY: 20-Oct-1993 V0.1
--#-func----------------------------------------------------------------------*/
int wn_desktop(int mode)
{
  static char *desktop = NULL;

  switch(mode) {
    case 0:               /* Save desktop                        */
      if (!desktop) {     /* Allocate memory if not already done */
        desktop = (unsigned char *) malloc(_cursvar.cols*_cursvar.lines*2);
        if (desktop == NULL) {
          fprintf(stderr,"ERROR: out of memory for saving desktop");
          exit(0);
        }
      }
      v_gettext(0,0,_cursvar.cols,_cursvar.lines,desktop);
      return(FALSE);
      break;

    case 1:                 /* Restore desktop if already saved */
      if (desktop) {
        v_puttext(0,0,_cursvar.cols,_cursvar.lines,desktop);
        return(TRUE);
      }
      return(FALSE);
      break;
  }

  return(FALSE);
}
