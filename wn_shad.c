/**************************************************************************
$Header: wn_shad.c Sun 11-19-2000 10:50:01 pm HvA V1.00 $
***************************************************************************/

#include "video/v_video.h"
#include "window.h"

extern SCREEN _cursvar;

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_shadow()
     PURPOSE: Give a given window a shadowed border on bottom and right side
      SYNTAX: int wn_shadow(WINDOW *w);
 DESCRIPTION:
     RETURNS: FALSE if no window opened,
              TRUE if all OK
     HISTORY: 09-Oct-1993 V0.1 - Initial version
--#-func----------------------------------------------------------------------*/
int wn_shadow(WINDOW *w)
{
  #define SHADOWC 16*BLACK+DARKGRAY

  if (!w)           /* Check if window opened */
    return(FALSE);

  if(w->wx2 < _cursvar.cols && w->wy2 < _cursvar.lines) {   /* Check if shadow within screen */
    v_setattrib_area(w->wy2+1,w->wx1+1,w->xsize,1,SHADOWC);  /* bottom */
    v_setattrib_area(w->wy1+1,w->wx2+1,1,w->ysize,SHADOWC);  /* right  */
    return(TRUE);
  }
  else
    return(FALSE);

}
