/**************************************************************************
$Header: wn_getsc.c Tue 11-07-2000 9:34:49 pm HvA V1.00 $

This file contains parts of PDCurses. PDCurses is Public Domain software.
***************************************************************************/

#define	CURSES_LIBRARY	1
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCgetsc  = "$Id$";
#endif

/*man-start*********************************************************************

  PDC_get_cur_col()	- get current column position of cursor

  PDCurses Description:
 	This is a private PDCurses function

 	This routine returns the current column position of the cursor on
 	screen.

  PDCurses Return Value:
 	This routine returns the current column position of the cursor. No
 	error is returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_cur_col( void );

**man-end**********************************************************************/

int	PDC_get_cur_col(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_col() - called\n");
#endif

	regs.x.ax = 0x0003;
	regs.h.bh = _cursvar.video_page;
	int86(0x10, &regs, &regs);
	return((int) regs.h.dl);

}

/*man-start*********************************************************************

  PDC_get_cur_row()	- get current row position of cursor

  PDCurses Description:
 	This is a private PDCurses function

 	This routine returns the current row position of the cursor on
 	screen.

  PDCurses Return Value:
 	This routine returns the current row position of the cursor. No
 	error is returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_cur_row( void );

**man-end**********************************************************************/

int	PDC_get_cur_row(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_cur_row() - called\n");
#endif

	regs.x.ax = 0x0003;
	regs.h.bh = _cursvar.video_page;
	int86(0x10, &regs, &regs);
	return ((int) regs.h.dh);
}

/*man-start*********************************************************************

  PDC_get_attribute()	- Get attribute at current cursor

  PDCurses Description:
 	This is a private PDCurses function

 	Return the current attr at current cursor position on the screen.

  PDCurses Return Value:
 	This routine will return OK upon success and otherwise ERR will be
 	returned.

  PDCurses Errors:
 	There are no defined errors for this routine.

  Portability:
 	PDCurses	int	PDC_get_attribute( void );

**man-end**********************************************************************/

int	PDC_get_attribute(void)
{
#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_get_attribute() - called\n");
#endif

	regs.x.ax = 0x0800;
	regs.h.bh = _cursvar.video_page;
	int86(0x10, &regs, &regs);
	return ((int) regs.h.ah);

}

/*man-start*********************************************************************

  PDC_get_scrn_mode()	- Return the current BIOS video mode

  PDCurses Description:
 	This is a private PDCurses routine.


  PDCurses Return Value:
 	Returns the current BIOS Video Mode Number.

  PDCurses Errors:
 	The FLEXOS version of this routine returns an ERR.
 	The UNIX version of this routine returns an ERR.
 	The EMXVIDEO version of this routine returns an ERR.

  Portability:
 	PDCurses	int PDC_get_scrn_mode( void );

**man-end**********************************************************************/

int	PDC_get_scrn_mode(void)
{
	regs.h.ah = 0x0f;
	int86(0x10, &regs, &regs);
	return ((int) regs.h.al);

}

