/**************************************************************************
$Header: v_init.c Mon 11-13-2000 10:30:13 pm HvA V1.00 $

Video Initialization

Note: This file comprises part of PDCurses. PDCurses is Public Domain software.

***************************************************************************/

#define CURSES_LIBRARY  1
#define LOCAL_VAR

#include <stdio.h>
#include <dos.h>
#include <bios.h>
#include <memory.h>
#include <sys\movedata.h>
#include "v_video.h"

#include "fortify.h"

SCREEN _cursvar = {0};      /* curses variables     */

PDC_WINDOW *curscr = NULL;            /* the current screen image */
PDC_WINDOW *stdscr = NULL;            /* the default screen window    */

int _default_lines = 25;    /* default terminal height  */
int LINES=0;            /* current terminal height  */
int COLS=0;         /* current terminal width   */

/*+ Local prototypes +*/
static int v_scr_open(SCREEN *internal, bool echo);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
initscr()
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
PDC_WINDOW* initscr(void)
{
  register int i=0;

    /*--- Return if this function is already called before  */
    if  (_cursvar.alive)
        return( NULL);

    memset(&_cursvar, 0, sizeof(SCREEN));

    v_scr_open(&_cursvar, 0);
    _cursvar.orig_cursor = _cursvar.cursor;
/*  _cursvar.orig_font = PDC_get_font();*/
    _cursvar.orig_font = _cursvar.font;
    _cursvar.blank = ' ';

/*  LINES = v_get_rows();*/
/*  COLS = v_get_columns(); */
    LINES = _cursvar.lines;
    COLS = _cursvar.cols;
    if (LINES < 2 || COLS < 2)
    {
        fprintf( stderr, "initscr(): LINES=%d COLS=%d: too small.\n",LINES,COLS );
        exit( 4 );
    }

#ifdef SKIP_THIS_FOR_NOW
    if ((curscr = newwin(LINES, COLS, 0, 0)) == (PDC_WINDOW *) NULL)
    {
        fprintf( stderr, "initscr(): Unable to create curscr.\n" );
        exit( 2 );
    }
    if ((stdscr = newwin(LINES, COLS, 0, 0)) == (PDC_WINDOW *) NULL)
    {
        fprintf( stderr, "initscr(): Unable to create stdscr.\n" );
        exit( 1 );
    }
    curscr->_clear = FALSE;
#endif

#ifdef SKIP_THIS_FOR_NOW
    v_init_atrtab(); /* set up default (BLACK on WHITE colours */
#endif

    _cursvar.alive = TRUE;

    printf("alive            %d  ",_cursvar.alive          );
    printf("autocr           %d  ",_cursvar.autocr         );
    printf("cbreak           %d  ",_cursvar.cbreak         );
    printf("echo             %d  ",_cursvar.echo           );
    printf("raw_inp          %d  ",_cursvar.raw_inp        );
    printf("raw_out          %d  ",_cursvar.raw_out        );
    printf("refrbrk          %d  ",_cursvar.refrbrk        );
    printf("orgcbr           %d  ",_cursvar.orgcbr         );
    printf("visible_cursor   %d  ",_cursvar.visible_cursor );
    printf("audible          %d  ",_cursvar.audible        );
    printf("full_redraw      %d  ",_cursvar.full_redraw    );
    printf("direct_video     %d  ",_cursvar.direct_video   );
    printf("mono             %d  ",_cursvar.mono           );
    printf("sizeable         %d  ",_cursvar.sizeable       );
    printf("bogus_adapter    %d  ",_cursvar.bogus_adapter  );
    printf("shell            %d  ",_cursvar.shell          );
    printf("blank            %d  ",_cursvar.blank          );
    printf("orig_attr        %d  ",_cursvar.orig_attr      );
    printf("cursrow          %d  ",_cursvar.cursrow        );
    printf("curscol          %d  ",_cursvar.curscol        );
    printf("cursor           %d  ",_cursvar.cursor         );
    printf("visibility       %d  ",_cursvar.visibility     );
    printf("video_page       %d  ",_cursvar.video_page     );
    printf("orig_emulation   %d  ",_cursvar.orig_emulation );
    printf("orig_cursor      %d  ",_cursvar.orig_cursor    );
    printf("font             %d  ",_cursvar.font           );
    printf("orig_font        %d  ",_cursvar.orig_font      );
    printf("lines            %d  ",_cursvar.lines          );
    printf("cols             %d  ",_cursvar.cols           );
    printf("emalloc          %d  ",_cursvar.emalloc        );

    return( stdscr );

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int endwin(void)
{
#ifdef SKIP_THIS_FOR_NOW
    PDC_scr_close();
#endif

    if (_cursvar.orig_font != _cursvar.font)  /* screen has not been resized */
        {
        v_set_font(_cursvar.orig_font);
        resize_screen(v_get_rows());
        }

    _cursvar.visible_cursor = FALSE;    /* Force the visible cursor */
    _cursvar.cursor = _cursvar.orig_cursor;
    v_cursoron();
    /*
     * Position cursor so that the screen will not scroll until they hit
     * a carriage return. Do this BEFORE delwin(curscr) as PDC_gotoxy() uses
     * curscr.
     */
    v_setcurpos(v_get_rows() - 2, 0);
	/*
    delwin(stdscr);
    delwin(curscr);
	*/
    stdscr = (PDC_WINDOW *)NULL;
    curscr = (PDC_WINDOW *)NULL;
    _cursvar.alive = FALSE;

#ifdef SKIP_THIS_FOR_NOW
    if (_cursvar.orig_font != _cursvar.font)  /* screen has not been resized */
        reset_shell_mode();
#endif

    return( OK );
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int resize_screen(int nlines)
{
    PDC_WINDOW* tmp=NULL;

    switch (_cursvar.adapter)
    {
    case _EGACOLOR:
        if (nlines >= 43)   v_set_font(_FONT8);
        else                v_set_80x25();
        break;

    case _VGACOLOR:
        if  (nlines > 28)       v_set_font(_FONT8);
        else    if (nlines > 25)    v_set_font(_FONT14);
        else                v_set_80x25();
        break;

    default:
        break;
    }

    _cursvar.lines = LINES = v_get_rows();
    _cursvar.cols  = COLS  = v_get_columns();

    if (curscr->_pmaxy > LINES)
    {
        v_scroll(0, 0, curscr->_pmaxy - 1, COLS - 1, 0, _cursvar.orig_attr);
    }
    else
    {
        v_scroll(0, 0, LINES - 1, COLS - 1, 0, _cursvar.orig_attr);
    }
#ifdef SKIP_THIS_FOR_NOW
    if ((tmp = resize_window(curscr, LINES, COLS)) != (PDC_WINDOW *) NULL)
    {
        curscr = tmp;
    }
    else
    {
        return (ERR);
    }
    if ((tmp = resize_window(stdscr, LINES, COLS)) != (PDC_WINDOW *) NULL)
    {
        stdscr = tmp;
        touchwin(stdscr);
        wnoutrefresh(stdscr);
    }
    else
    {
        return (ERR);
    }
#endif
    return (OK);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_scr_open()	- Internal low-level binding to open the physical screen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static int v_scr_open(SCREEN *internal, bool echo)
{

	internal->orig_attr	 	 = 0;
	internal->orig_emulation = getdosmembyte (0x487);
	v_getcurpos(&internal->cursrow, &internal->curscol);
	internal->direct_video	 = TRUE;	/* Assume that we can	      */
	internal->autocr		 = TRUE;	/* lf -> crlf by default      */
	internal->raw_out		 = FALSE;	/* tty I/O modes	      */
	internal->raw_inp		 = FALSE;	/* tty I/O modes	      */
	internal->cbreak		 = TRUE;
	internal->echo			 = echo;
	internal->refrbrk		 = TRUE;	/* allow premature end of refresh*/
	internal->video_seg		 = 0xb000;	/* Base screen segment addr   */
	internal->video_ofs		 = 0x0;		/* Base screen segment ofs    */
	internal->video_page	 = 0;		/* Current Video Page	      */
	internal->visible_cursor = TRUE;	/* Assume that it is visible  */
	internal->cursor		 = v_get_cursor_mode();
	internal->adapter		 = v_query_adapter_type();
	internal->font			 = v_get_font();
	internal->lines			 = v_get_rows();
	internal->cols			 = v_get_columns();
	internal->audible		 = TRUE;
	internal->visibility	 = 1;
	return( OK );
}

static int stored_xsize;
static int stored_ysize;
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
savescreen() - Save complete original DOS screen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *v_savescreen(int xsize, int ysize)
{
  char *w = NULL;

  wn_log("v_savescreen(%d,%d)\n",xsize,ysize);
  w = (unsigned char *) malloc((xsize+1) * (ysize+1) * 2);
  if (w == NULL)
  {
    fprintf(stderr,"ERROR: out of memory for screensave");
    exit(0);
  }
  v_gettext(0,0,xsize,ysize,w);
  stored_xsize = xsize;
  stored_ysize = ysize;
  return(w);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
restorescreen() - Restore original screen
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *v_restorescreen(char *w)
{
  wn_log("v_restorescreen()\n");
  if (w)
  {
    v_puttext(0,0,stored_xsize,stored_ysize,w);
    free(w);
    w = NULL;
  }
  return(NULL);
}


