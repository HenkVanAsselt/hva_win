/**************************************************************************
$Header: v_video.h Fri 11-10-2000 9:47:34 pm HvA V1.00 $
***************************************************************************/

/*--- Avoid multiple inclusion  */
#ifndef _V_VIDEO_H_
#define _V_VIDEO_H_

#ifndef BYTE
#define BYTE unsigned char
#endif 

#ifndef FALSE
#  define FALSE 0
#  define TRUE  !FALSE
#endif

#ifndef	ERR
#  define	 ERR	0		/* general error flag	 */
#endif
#ifndef	OK
#  define	 OK	1		 	/* general OK flag	 */
#endif


#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

typedef unsigned char bool;		/*+ Boolean type	+*/

#define CHTYPE_LONG
#ifdef CHTYPE_LONG
typedef unsigned long chtype;	/* 16-bit attr + 16-bit char	*/
#else
typedef unsigned short chtype;	/* 8-bit attr + 8-bit char	*/
#endif


/* Monitor (terminal) type information */
#define _NONE           0x00
#define _MDA            0x01
#define _CGA            0x02
#define _EGACOLOR       0x04
#define _EGAMONO        0x05
#define _VGACOLOR       0x07
#define _VGAMONO        0x08
#define _MCGACOLOR      0x0a
#define _MCGAMONO       0x0b
#define _FLEXOS         0x20            /* A Flexos console */
#define _MDS_GENIUS     0x30
#define _UNIX_COLOR     0x40
#define _UNIX_MONO      0x41

/* Text-mode font size information */
#define _FONT8  8
#define _FONT14 14
#define _FONT15 15              /* GENIUS */
#define _FONT16 16

typedef struct
{
    bool    alive;          /* TRUE if already opened.      */
    bool    autocr;         /* if lf -> crlf            */
    bool    cbreak;         /* if terminal unbuffered       */
    bool    echo;           /* if terminal echo         */
    bool    raw_inp;        /* raw input mode (v. cooked input) */
    bool    raw_out;        /* raw output mode (7 v. 8 bits)    */
    bool    refrbrk;        /* if premature refresh brk allowed */
    bool    orgcbr;         /* original MSDOS ^-BREAK setting   */
    bool    visible_cursor; /* TRUE if cursor is visible        */
    bool    audible;        /* FALSE if the bell is visual      */
    bool    full_redraw;    /* TRUE for bad performance     */
    bool    direct_video;   /* Allow Direct Screen Memory writes*/
    bool    mono;           /* TRUE if current screen is mono.  */
    bool    sizeable;       /* TRUE if adapter is resizeable.   */
    bool    bogus_adapter;  /* TRUE if adapter has insane values*/
    bool    shell;          /* TRUE if reset_prog_mode() needs to be called */
    chtype  blank;          /* Background character         */
    chtype  orig_attr;      /* Original screen attributes       */
    int cursrow;            /* position of physical cursor      */
    int curscol;            /* position of physical cursor      */
    int cursor;             /* Current Cursor definition        */
    int visibility;         /* Visibility of cursor */
    int video_page;         /* Current PC video page        */
    int orig_emulation;     /* Original cursor emulation value  */
    int orig_cursor;        /* Original cursor size         */
    int font;               /* default font size            */
    int orig_font;          /* Original font size           */
    int lines;              /* New value for LINES          */
    int cols;               /* New value for COLS           */
    int emalloc;            /* 0x0C0C if initscr() is to reset  */
                            /*     this value to TRUE;      */
                            /* TRUE only if emalloc()/ecalloc() */
                            /*     are is to be used;       */
                            /* FALSE if malloc()/calloc() are   */
                            /*     to be used.          */
    int scrnmode;           /* default screen mode          */
    int adapter;            /* Screen type              */
    unsigned video_seg;     /* video base segment           */
    unsigned video_ofs;     /* video base offset            */
}   SCREEN;

typedef struct _win		/* definition of a window.	   */
{
	int	_cury;		/* current pseudo-cursor	   */
	int	_curx;
	int	_maxy;		/* max window coordinates	   */
	int	_maxx;
	int	_pmaxy;		/* max physical size		   */
	int	_pmaxx;
	int	_begy;		/* origin on screen		   */
	int	_begx;
	int	_lastpy;	/* last y coordinate of upper left pad display area */
	int	_lastpx;	/* last x coordinate of upper left pad display area */
	int	_lastsy1;	/* last upper y coordinate of screen window for pad */
	int	_lastsx1;	/* last upper x coordinate of screen window for pad */
	int	_lastsy2;	/* last lower y coordinate of screen window for pad */
	int	_lastsx2;	/* last lower x coordinate of screen window for pad */
	int	_flags;		/* window properties		   */
	chtype	_attrs;		/* standard A_STANDOUT attributes and colors  */
	chtype	_bkgd;		/* wrs(4/6/93) background, normally blank */
	int	_tabsize;	/* tab character size		   */
	bool	_clear;		/* causes clear at next refresh	   */
	bool	_leave;		/* leaves cursor as it happens	   */
	bool	_scroll;	/* allows window scrolling	   */
	bool	_nodelay;	/* input character wait flag	   */
	bool	_immed;	/* immediate update flag	   */
	bool	_use_keypad;	/* flags keypad key mode active	   */
	bool	_use_idl;	/* True if Ins/Del line can be used*/
	bool	_use_idc;	/* True if Ins/Del character can be used*/
	chtype**_y;		/* pointer to line pointer array   */
	int*	_firstch;	/* first changed character in line */
	int*	_lastch;	/* last changed character in line  */
	int	_tmarg;	/* top of scrolling region	   */
	int	_bmarg;	/* bottom of scrolling region	   */
	char*	_title;		/* window title			   */
	char	_title_ofs;	/* window title offset from left   */
	chtype	_title_attr;	/* window title attributes	   */
	chtype	_blank;		/* window's blank character	   */
	int	_parx, _pary;	/* coords relative to parent (0,0) */
struct	_win*	_parent;	/* subwin's pointer to parent win  */
} PDC_WINDOW;



/*+ v_adapt.c +*/
int v_query_adapter_type();
int	v_sanity_check(int adapter);
unsigned char getdosmembyte (int offset);
unsigned short getdosmemword (int offset);
void setdosmembyte (int offset, unsigned char b);
void setdosmemword (int offset, unsigned short w);

/*+ v_curs.c +*/
void   v_cursoroff(void);
void   v_cursoron(void);
int    v_get_cursor_mode(void);
int    v_getcurpos(int *row, int *col);
int    v_setcurpos(int row, int col);
int    v_get_columns(void);
int    v_get_rows(void);

/*+ v_font.c +*/
int	   v_get_font(void);
int	   v_get_rows(void);
int    v_set_font(int size);
int    v_set_80x25(void);

/*+ v_init.c +*/
PDC_WINDOW* initscr(void);
int endwin(void);
int resize_screen(int nlines);
char *v_savescreen(int xsize, int ysize);
char *v_restorescreen(char *w);

/*+ v_scroll.c +*/
int v_scroll(int urow, int lcol, int lrow, int rcol, int nlines, chtype attr);

/*+ v_print.c +*/
void v_printc(int row, int col, int c);
void v_printca(int row, int col, int c, int att);
void v_printmca(int row, int col, int c, int att, int n);
void v_gettext(int row1, int col1, int row2, int col2, void *array);
void v_puttext(int row1, int col1, int row2, int col2, void *array);
void v_prints(int row, int col, char *s);
void v_fillarea(int row, int col, int width, int height, int c, int attrib);

/*+ v_attrib.c +*/
void v_setattrib_area(int row, int col, int width, int height, int att);
void v_setattrib(int row, int col, int att);

/*+ v_box.c +*/
void v_setboxchars(int ul, int to, int ur, int ri, int lr, int bo, int ll, int le);
void v_drawbox(int row, int col, int xsize, int ysize, int att);

#endif
