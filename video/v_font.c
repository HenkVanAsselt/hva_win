/**************************************************************************
$Header: v_font.c Mon 11-13-2000 1:54:48 pm HvA V1.00 $

Low level video font routines
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <bios.h>
#include <sys\movedata.h>
#include "v_video.h"

/*+ External varialbes +*/
extern SCREEN _cursvar;
extern int _default_lines;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_get_font() - Get the current font size
This function returns the current font size.  This function only
works if the #define FAST_VIDEO is true.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_get_font(void)
{
#if defined (FAST_VIDEO)
    int retval=0;
#endif

#if defined (FAST_VIDEO)
    retval = getdosmemword (0x485);
    if ((retval == 0) && (_cursvar.adapter == _MDS_GENIUS))
    {
        retval = _FONT15; /* Assume the MDS Genius is in 66 line mode. */
    }
    switch (_cursvar.adapter)
    {
    case _MDA:
            retval = 10; /* POINTS is not certain on MDA/Hercules */
            break;
    case _EGACOLOR:
    case _EGAMONO:
        switch (retval)
        {
        case _FONT8:
        case _FONT14:
            break;
        default:
            retval = _FONT14;
        }
        break;

    case _VGACOLOR:
    case _VGAMONO:
        switch (retval)
        {
        case _FONT8:
        case _FONT14:
        case _FONT16:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return( retval );

#endif

    return(0);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_set_font()
This routine sets the current font size, if the adapter allows
such a change.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_set_font(int size)
{
  union REGS regs;

  if (_cursvar.bogus_adapter)
      return( ERR );

  switch (_cursvar.adapter)
  {
    case _CGA:
    case _MDA:
    case _MCGACOLOR:
    case _MCGAMONO:
    case _MDS_GENIUS:
        break;

    case _EGACOLOR:
    case _EGAMONO:
        if (_cursvar.sizeable && (_cursvar.font != size))
        {
            switch (size)
            {
            case _FONT8:
                regs.h.ah = 0x11;
                regs.h.al = 0x12;
                regs.h.bl = 0x00;
                int86(0x10, &regs, &regs);
                break;
            case _FONT14:
                regs.h.ah = 0x11;
                regs.h.al = 0x11;
                regs.h.bl = 0x00;
                int86(0x10, &regs, &regs);
                break;
            default:
                break;
            }
        }
        break;

    case _VGACOLOR:
    case _VGAMONO:
        if (_cursvar.sizeable && (_cursvar.font != size))
        {
            switch (size)
            {
            case _FONT8:
                regs.h.ah = 0x11;
                regs.h.al = 0x12;
                regs.h.bl = 0x00;
                int86(0x10, &regs, &regs);
                break;
            case _FONT14:
                regs.h.ah = 0x11;
                regs.h.al = 0x11;
                regs.h.bl = 0x00;
                int86(0x10, &regs, &regs);
                break;
            case _FONT16:
                regs.h.ah = 0x11;
                regs.h.al = 0x14;
                regs.h.bl = 0x00;
                int86(0x10, &regs, &regs);
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
  }

  if (_cursvar.visible_cursor)
      v_cursoron();
  else
      v_cursoroff();

  _cursvar.font = v_get_font();

  return( OK );
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_set_80x25()   - force a known screen state: 80x25 text mode.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_set_80x25(void)
{
  union REGS regs;

  switch (_cursvar.adapter)
  {
    case _CGA:
    case _EGACOLOR:
    case _EGAMONO:
    case _VGACOLOR:
    case _VGAMONO:
    case _MCGACOLOR:
    case _MCGAMONO:
        regs.h.ah = 0x00;
        regs.h.al = 0x03;
        int86(0x10, &regs, &regs);
        break;
    case _MDA:
        regs.h.ah = 0x00;
        regs.h.al = 0x07;
        int86(0x10, &regs, &regs);
    default:
        break;
  }
  return( OK );
}




