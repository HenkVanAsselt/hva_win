/**************************************************************************
$Header: v_scroll.c Tue 11-14-2000 2:39:19 pm HvA V1.00 $

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include "v_video.h"

extern SCREEN _cursvar;			/*+ Curses internal variables +*/
extern int _default_lines;   	/*+ default terminal height   +*/
extern int LINES;            	/*+ current terminal height   +*/
extern int COLS;         	 	/*+ current terminal width    +*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_scroll()	- low level screen scroll

Scrolls a window in the current page up or down. Urow, lcol,
lrow, rcol are the window coordinates.  Lines is the number of
lines to scroll. If 0, clears the window, if < 0 scrolls down,
if > 0 scrolls up.  Blanks areas that are left, and sets
character attributes to attr. If in a colour graphics mode,
fills them with the colour 'attr' instead.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_scroll(int urow, int lcol, int lrow, int rcol, int nlines, chtype attr)
{
  union REGS regs;

  int phys_attr=attr;

  if (nlines >= 0)
  {
      regs.h.ah = 0x06;
      regs.h.al = (unsigned char) nlines;
  }
  else
  {
      regs.h.ah = 0x07;
      regs.h.al = (unsigned char) (-nlines);
  }
  regs.h.bh = (unsigned char)(phys_attr >> 8);
  regs.h.ch = (unsigned char) urow;
  regs.h.cl = (unsigned char) lcol;
  regs.h.dh = (unsigned char) lrow;
  regs.h.dl = (unsigned char) rcol;
  int86(0x10, &regs, &regs);
  return( OK );
}



