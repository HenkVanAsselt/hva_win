/*
***************************************************************************
* This file comprises part of PDCurses. PDCurses is Public Domain software.
* You may use this code for whatever purposes you desire. This software
* is provided AS IS with NO WARRANTY whatsoever.
* Should this software be used in another application, an acknowledgement
* that PDCurses code is used would be appreciated, but is not mandatory.
*
* Any changes which you make to this software which may improve or enhance
* it, should be forwarded to the current maintainer for the benefit of 
* other users.
*
* The only restriction placed on this code is that no distribution of
* modified PDCurses code be made under the PDCurses name, by anyone
* other than the current maintainer.
* 
* See the file maintain.er for details of the current maintainer.
***************************************************************************
*/
#define	CURSES_LIBRARY	1
#include <curses.h>

#ifdef PDCDEBUG
char *rcsid_PDCsetsc  = "$Id$";
#endif

/*man-start*********************************************************************

  PDC_set_cursor_mode()	- Set the cursor start and stop scan lines.

  PDCurses Description:
 	Sets the cursor type to begin in scan line startrow and end in
 	scan line endrow.  Both values should be 0-31.

  PDCurses Return Value:
 	This function returns OK on success and ERR on error.

  PDCurses Errors:
 	No errors are defined for this function.

  Portability:
 	PDCurses	int PDC_set_cursor_mode( int startrow, int endrow );

**man-end**********************************************************************/

int	PDC_set_cursor_mode( int startrow, int endrow )
{
#ifdef	OS2
# ifndef EMXVIDEO
	VIOCURSORINFO cursorInfo={0};
# endif
#endif

#ifdef	FLEXOS
	unsigned short mybuff = 0;
#endif

#ifdef PDCDEBUG
	if (trace_on) PDC_debug("PDC_set_cursor_mode() - called: startrow %d endrow %d\n",startrow,endrow);
#endif

#ifdef	FLEXOS
	/*
	 * Under FLEXOS, this routine translates the input parameters in the
	 * following way:
	 *
	 * startrow --> visible_cursor endrow	 -->	cursor type:
	 * underline = 0; block = 1;
	 *
	 * VCWM_CURSOR	   0x0100	   bit - 8 Cursor off VCWM_BLOCK	
	 * 0x0200	   bit - 9 Block Cursor	
	 *
	 */
	retcode = s_getfield(T_VIRCON, VC_MODE, 1L, (void far *) &mybuff, 2L);
	if (retcode < 0L)
		return( ERR );
	if (startrow)
		mybuff &= ~VCWM_CURSOR;
	else
		mybuff |= VCWM_CURSOR;

	if (endrow)
		mybuff |= VCWM_BLOCK;
	else
		mybuff &= ~VCWM_BLOCK;

	retcode = s_setfield(T_VIRCON, VC_MODE, 1L, (void far *) &mybuff, 2L);
	return( (retcode < 0L) ? ERR : OK );
#endif

#ifdef	DOS
	regs.h.ah = 0x01;
	regs.h.ch = (unsigned char) startrow;
	regs.h.cl = (unsigned char) endrow;
	int86(0x10, &regs, &regs);
	return( OK );
#endif

#ifdef	OS2
# ifdef EMXVIDEO
	if (endrow <= startrow)
		v_hidecursor();
	else
		v_ctype (startrow, endrow);
	return( OK );
# else
	cursorInfo.yStart = startrow;
	cursorInfo.cEnd = endrow;
	cursorInfo.cx = 1;
	cursorInfo.attr = 0;
	return (VioSetCurType (&cursorInfo, 0) == 0);
# endif
#endif

#ifdef UNIX
	return(0); /* this is N/A */
#endif
}

/*man-start*********************************************************************

  PDC_set_rows()    - sets the physical number of rows on screen

  PDCurses Description:
    This is a private PDCurses function.

    This routine attempts to set the number of rows on the physical
    screen to the passed value.

  PDCurses Return Value:
    This function returns OK upon success otherwise ERR is returned.

  PDCurses Errors:
    It is an error to attempt to change the screen size on a "bogus"
    adapter.  The reason for this is that we have a known video
    adapter identity problem.  e.g. Two adapters report the same
    identifying characteristics.

    It is also an error to attempt to change the size of the Flexos
    console (as there is currently no support for that).

  Portability:
    PDCurses    int PDC_set_rows( int rows );

**man-end**********************************************************************/

int PDC_set_rows(int rows)
{
#ifdef  OS2
# ifndef EMXVIDEO
    VIOMODEINFO modeInfo={0};
    USHORT result=0;
# endif
#endif

#ifdef PDCDEBUG
    if (trace_on) PDC_debug("PDC_set_rows() - called\n");
#endif

#ifdef  FLEXOS
    return( ERR );
#endif

#ifdef  DOS
    if (_cursvar.bogus_adapter)
        return( ERR );

    switch (_cursvar.adapter)
    {
    case _EGACOLOR:
    case _EGAMONO:
        if (rows < 43)
            PDC_set_font(_FONT14);
        else
            PDC_set_font(_FONT8);
        break;

    case _VGACOLOR:
    case _VGAMONO:
        if (rows < 28)
            PDC_set_font(_FONT16);
        else
        if (rows < 50)
            PDC_set_font(_FONT14);
        else
            PDC_set_font(_FONT8);
        break;

    case _MCGACOLOR:
    case _MCGAMONO:
    case _MDA:
    case _CGA:
    case _MDS_GENIUS:
    default:
        break;
    }
    _cursvar.font = PDC_get_font();
    LINES = PDC_get_rows();
    COLS = PDC_get_columns();
    return( OK );
#endif

#ifdef  OS2
# ifdef EMXVIDEO
    return (ERR);
# else
    modeInfo.cb = sizeof(modeInfo);
    /* set most parameters of modeInfo */
    VioGetMode(&modeInfo, 0);
    modeInfo.fbType = 1;
    modeInfo.row = rows;
    result = VioSetMode(&modeInfo, 0);
    _cursvar.font = PDC_get_font();
    LINES = PDC_get_rows();
    COLS = PDC_get_columns();
    return ((result == 0) ? OK : ERR);
# endif
#endif

#ifdef UNIX
    return(0); /* this is N/A */
#endif
}

/*man-start*********************************************************************

  PDC_set_scrn_mode()   - Set BIOS Video Mode

  PDCurses Description:
    Sets the BIOS Video Mode Number ONLY if it is different from
    the current video mode.  This routine is for DOS systems only.

  PDCurses Return Value:
    This function returns OK on success and ERR on error.

  PDCurses Errors:
    No errors are defined for this function.

  Portability:
    PDCurses    int PDC_set_scrn_mode( int new_mode );

**man-end**********************************************************************/

#if defined( OS2 ) && !defined( EMXVIDEO )
int PDC_set_scrn_mode(VIOMODEINFO new_mode)
#else
int PDC_set_scrn_mode(int new_mode)
#endif
{

#ifdef  DOS
    int cur=0;
#endif

#ifdef PDCDEBUG
    if (trace_on) PDC_debug("PDC_set_scrn_mode() - called\n");
#endif

#ifdef  FLEXOS
    return( OK );
#endif

#ifdef  DOS
    cur = (int) PDC_get_scrn_mode();
    if (cur != new_mode)
    {
        regs.h.ah = 0x00;
        regs.h.al = (char) new_mode;
        int86(0x10, &regs, &regs);
    }
    _cursvar.font = PDC_get_font();
    _cursvar.scrnmode = new_mode;
    LINES = PDC_get_rows();
    COLS = PDC_get_columns();
    return( OK );
#endif

#ifdef  OS2
# ifdef EMXVIDEO
    return ( OK );
# else
    if (VioSetMode (&new_mode, 0) != 0)
        {
        _cursvar.font = PDC_get_font();
        _cursvar.scrnmode = new_mode;
        LINES = PDC_get_rows();
        COLS = PDC_get_columns();
        return( OK );
        }
    else
        return (ERR);
# endif
#endif

#ifdef UNIX
    return(OK); /* this is N/A */
#endif
}
