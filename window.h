/**************************************************************************
$Header: window.h Fri 10-27-2000 8:04:49 pm HvA V2.01 $

DESC: Definition file for HVA_WINDOWS
HIST: 19900818 V1.00
      20001027 V2.01 Adapting for DJGPP
***************************************************************************/

#include <stdio.h>

#ifdef __MSC__
#include <memory.h>
#else
#include <mem.h>
#endif

#include <stdlib.h>


#ifdef WN_LOG
#pragma message(">>>>>>>>>> Compiling with WN_LOG option ON <<<<<<<<")
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE  !FALSE
#endif

#define _IBM_CGA     0
#define _NONIBM_CGA  1

/*-------------------------
  Window return constants
----------------------------*/
#define NO_WINDOW   -1
#define ALLOC_ERR   -2

#define AUTO_ROW    -1
#define AUTO_COL    -1
#define LAST_ROW    -2
#define LAST_COL    -2

/*--------------------
  Color definitions
----------------------*/
#define   BLACK          0
#define   BLUE           1
#define   GREEN          2
#define   CYAN           3
#define   RED            4
#define   MAGENTA        5
#define   BROWN          6
#define   WHITE          7
#define   DARKGRAY       8
#define   LIGHTBLUE      9
#define   LIGHTGREEN     10
#define   LIGHTCYAN      11
#define   LIGHTRED       12
#define   LIGHTMAGENTA   13
#define   YELLOW         14
#define   INTENSE_WHITE  15

/*-------------------------
  Display Mode Atributes
--------------------------*/
#define B4025  0                        /* black & white 40 x 25      */
#define C4025  1                        /* color 40 x 25              */
#define B8025  2                        /* black & white 80 x 25      */
#define C8025  3                        /* color 80 x 25              */
#define C320   4                        /* color graphics 320 x 200   */
#define B320   5                        /* black & white graphics     */
#define HIRES  6                        /* B&W hi res 640 * 200       */
#define MONO   7                        /* monocrome 80 x 25          */
#define COLOR  15                       /* General COLOR display mode */

/*---------------------------------------
  Define contstants for monitor output
---------------------------------------*/
#define MONITOR_1  1
#define MONITOR_2  2

/*----------------------
  Border line constants
------------------------*/
#define _NO_BORDER    0
#define _SINGLE_LINE  1
#define _DOUBLE_LINE  2


/*----------------------------------------------------------------
  Frame constants


  …ÕÕÕÕÕÕÕÕÕª ⁄ƒƒƒƒƒƒƒƒƒø ÷ƒƒƒƒƒƒƒƒƒ∑  ’ÕÕÕÕÕÕÕÕÕ∏  ’ÕÕÕÕÕÕÕÕ∏
  ∫ FRAME 0 ∫ ≥ FRAME 1 ≥ ∫ FRAME 2 ∫  ≥ FRAME 3 ≥  ≥ FRAME 4≥
  »ÕÕÕÕÕÕÕÕÕº ¿ƒƒƒƒƒƒƒƒƒŸ ”ƒƒƒƒƒƒƒƒƒΩ  ‘ÕÕÕÕÕÕÕÕÕæ  ¿ƒƒƒƒƒƒƒƒŸ

              ‹‹‹‹‹‹‹‹‹‹  ∞∞∞∞∞∞∞∞∞∞∞  ±±±±±±±±±±±  ≤≤≤≤≤≤≤≤≤≤
    FRAME 5   › FRAME 6ﬁ  ∞ FRAME 7 ∞  ±FRAME 8  ±  ≤FRAME 9 ≤
              ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ  ∞∞∞∞∞∞∞∞∞∞∞  ±±±±±±±±±±±  ≤≤≤≤≤≤≤≤≤≤

  €€€€€€€€€€  **********
  €FRAME 10€  *FRAME 11*
  €€€€€€€€€€  **********

----------------------------------------------------------------*/

#define FRAME_0   10
#define FRAME_1   11
#define FRAME_2   12
#define FRAME_3   13
#define FRAME_4   14
#define FRAME_5   15
#define FRAME_6   16
#define FRAME_7   17
#define FRAME_8   18
#define FRAME_9   19
#define FRAME_10  20
#define FRAME_11  21



/*------------------
  Window constants
-------------------*/
#define _DRAW        1
#define _NO_DRAW     0

#define _UP          0
#define _DOWN        1
#define _LEFT        2
#define _RIGHT       3
#define _UPA         4
#define _DOWNA       5
#define _LEFTA       6
#define _RIGHTA      7

#ifndef BYTE
#define BYTE unsigned char
#endif

//#ifndef MK_FP
//#define MK_FP(seg,off) ((void far *) (((unsigned long)(seg)<<16) | (unsigned)(off)))
//#endif
#ifndef FP_OFF
#define FP_OFF(fptr) ((unsigned)(fptr))
#endif
#ifndef FP_SEG
#define FP_SEG(fptr) ((unsigned)((unsigned long)(fptr) >> 16 ))
#endif

/*----------------
  object flags
----------------*/
#define NO_FLAGS   0x0000
#define SELECTABLE 0x0001   /* Boolean, ON/OFF */
#define DEFAULT    0x0002
#define EXIT       0x0004
#define EDITABLE   0x0008
#define RBUTTON    0x0010
#define LASTOB     0x0020
#define TOUCHEXIT  0x0040
#define HIDETREE   0x0080
#define INDIRECT   0x0100
#define SELECTED   0x0200
#define CROSSED    0x0400
#define CHECKED    0x0800
#define DISABLED   0x1000
#define EDIT_STR   0x2000
#define HELP_ONLY  0x4000
#define COMMENT    0x8000

/*----------------
  window flags
----------------*/
#define HIDDEN     0x0001
#define SHADOW     0x0002

/*


  (ulx,uly)€€€€€€€€€€€€€€€€€€€€€
  €(x1,y1)                     €
  €  |                         €
  €<-|------ x-size ---------->€
  €  |                         €
  €  ysize                     €
  €  |                         €
  €  |                         €
  €  |                  (x2,y2)€
  €€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

*/

typedef struct wcb           /* Window control block                     */
{                            /* ---------------------------------------- */
  int wx1,wy1,wx2,wy2,       /* Coordinates of border of window          */
      x1,y1,x2,y2,           /* Coordinates of writable part of window   */
      xsize,                 /* width of window - inclusive border       */
      ysize,                 /* height of window -inclusive border       */
      ccx, ccy,              /* Screen cursor pos in window              */
      wcolor,                /* window color                             */
      bcolor,                /* border color                             */
      btype;                 /* _NO_BORDER,_SINGLE_LINE etc              */
  char *scrnsave;            /* pointer to screen save buffer            */
  int  synflg;               /* cursor sync flag                         */
  struct wcb *prev;          /* linked list - previous window            */
  struct wcb *next;          /* linked list - next window                */
  int  scroll_ok;            /* Scroll ok flag                           */
  int  flag;                 /* SHADOWED                                 */
  char *title;               /* Pointer to title                         */
  int  monitor;              /* FIRST, SECOND                            */
  int  page;                 /* Page window is in                        */
  int  hideflag;             /* Flag indicating if window is hidden      */
  int  level;                /* Level of window on desktop               */
}
WINDOW, *WINDOWPTR;

/*----------------------------
  Menu structure definition
-----------------------------*/
typedef struct
{
  char   *header;
  int    hotkey;
  void   (*function)();
  char   *help;
  void   *data;
  char   *format;
  int    flags;
  char   string[80];
}
MENU;

typedef int (*PFI)(void);

typedef struct
{ char *header;    /* Pulldown menu header            */
  int  hotkey;     /* Hotkey of the header            */
  int  number;     /* Number of menu's in the header  */
  MENU *mptr;      /* Pointer to actual pulldown menu */
}
MENU_HEAD;

/*-----------------------
  Variable declarations
------------------------*/

/* MACRO definitions
~~~~~~~~~~~~~~~~~~~~*/
#define clearone(row,col,att)        fillone(row,col,' ',att1)
#define clearwindow(r1,c1,r2,c2,att) fillwindow(r1,c1,r2,c2,' ',att)

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef minmax
#define minmax(a,b,c)  (max((a),( (min((b),(c))) )))
#endif

/*-----------
  Diversen
-----------*/
void   printchar(int row, int col, char c);
void   setattrib(int row1, int col1, int xsize,int ysize, int att);
void   savewindow(int col1, int row1, int col2, int row2, char *videoarray);
int    gettext(int row1, int col1, int row2, int col2, void *array);
int    puttext(int row1, int col1, int row2, int col2, void *array);
void   disp_status(char *format, ...);

/*-----------
  WINDIO.C
------------*/
void   set_display_colors(int mode);
char   *void_sprintf(char *datastr, char *format, void *data);
void   void_sscanf(char *str, char *format, void *data);
void   trimstr(char *s);
void   vb_scroll(int left, int right, int top, int bottom, int amount);
void   drawbox(int row,int col, int width, int height, int attrib);
void   printhotcenter(int row,int col, char *string, int str_att);
void   hotstring(int row, int col, int hotchar, char *s, int str_att);
void   setup_menu(MENU *menu, int i, char *header, int hotkey, void *function,
                  char *help, void *data, char *format, int flags);
void   init_menu(MENU *menu, int n);
void   printcenter(int row, int col, char *s);
void   edit_data(int row, int col, char *header, char *format, void *data);
int    editstr(int row, int tcol, int len, char *header, char *str, int w_flag);
void   show_time(void);
int    wn_desktop(int mode);

#ifdef __MSC__
void clrscr(void);
#endif

/*-----------
  WINDOW.C
-----------*/
int    wn_top(WINDOW *w);
void   wn_init(void);
void   wn_exit(void);
int    wn_monitor(int monitor_nr);
int    wn_update(void);
int    wn_border(WINDOW *w,int style);
int    wn_clear(WINDOW *w);
/*
int    wn_dborder(WINDOW *w, int ul, int ur, int tb, int sd, int ll, int lr);
*/
int    wn_fixcsr(WINDOW *w);
void   wn_restore(WINDOW *w);
int    wn_hide(WINDOW *w);
int    wn_unhide(WINDOW *w);
int    wn_move(WINDOW *w, int row, int col);
int    wn_show(WINDOW *w);
int    wn_printf(WINDOW *w, char *format, ...);
int    wn_debug(char *format, ...);
int    wn_title(WINDOW *w, char *format, ...);
void   wn_setbox(int border_linestyle);
void   wn_dma(int flag);
int    wn_locate(WINDOW *w, int row, int col);
int    wn_sync(WINDOW *w, int flag);
WINDOW *wn_close(WINDOW *w);
int    wn_closeall(void);
void   wn_fill(WINDOW *w, int row, int col, int width, int height, int c, int att);
void   wn_setattrib(WINDOW *w, int row, int col, int xsize,int ysize, int att);
int    wn_setone(WINDOW *w, int row, int col, int attrib);
int    wn_nattrib(WINDOW *w, int wattrib);
int    wn_scroll(WINDOW *w, int i);
int    wn_shadow(WINDOW *w);
WINDOW *wn_open(int page, int row, int col, int width, int height,
                int attrib, int battrib);
WINDOW *wn_save(WINDOW *w);
void   wn_error(char *format, ...);
int    wn_yesno(char *format, ...);
void   OS_shell(void);
void   wn_shell(char *command);

/*-----------
  WN_LOG.C
------------*/
void   wn_log(char *format, ...);
int    wn_log_indent(int i);

/*-----------
  DIALOG.C
------------*/
int    wn_dialog(int, int, int, MENU *, int, ...);

/*----------
  POPUP.C
-----------*/
void   do_action(MENU *menu, int i);
unsigned int popup(int, MENU *, int, int);

/*-------------
  PULLDOWN.C
-------------*/
void   pulldown_bar(MENU_HEAD *menu);
int    pulldown(MENU_HEAD *m, int i, char *help_subject);
void   exit_pulldown(void);

/*--------
  DIR.C
----------*/
void show_dir(void);
void change_dir(void);
char *get_dir(char *path, char *filename);

/*------------
  FUNKEY.C
------------*/
void init_funkey(int function_key, void *function, char *text);
void exit_funkey(void);
int  do_funkey(int function_key);
void disp_funkeys(int i);

/*---------
| HELP.C
---------*/
int  help(char *subject);
int  init_help(char *help_file_name);
void exit_help(void);

/*----------------------------------------------------------------------------
  Turbo Pascal compatible global variables
-----------------------------------------------------------------------------*/
extern unsigned char NormAttr;
extern unsigned char TextAttr;
extern unsigned int  LastMode;
extern unsigned int  CurCrtSize;
extern unsigned char CheckSnow;
extern unsigned char DirectVideo;
extern unsigned int  WindMin;
extern unsigned int  WindMax;

#ifdef MAIN_WINDOW_MODULE

/*-----------------------------------------
   Mask colors with 0x7F to avoid blinking
   when working with monochrome monitor
------------------------------------------*/
int    _menu_att       = (16*WHITE+BLACK)    & 0x7F,  /* memu item            */
       _menu_hotkey    = (16*WHITE+RED)      & 0x7F,  /* hotkey of item       */
       _menu_highlight = (16*BLACK+WHITE)    & 0x7F,  /* highlighted item     */
       _window_att     = (16*BLACK+WHITE)    & 0x7F,  /* window attribute     */
       _border_att     = (16*WHITE+BLACK)    & 0x7F,  /* border               */
       _error_att      = (16*RED+YELLOW)     & 0x7F,  /* Error window         */
       _disabled_att   = (16*WHITE+RED)      & 0x7F,  /* DISABLED item        */
       _comment_att    = (16*WHITE+BLUE)     & 0x7F,  /* COMMENT item         */
       _dialog_att     = (16*WHITE+BLACK)    & 0x7F,  /* Dialog menu          */
       _desktop_att    = (16*BLUE+YELLOW)    & 0x7F,  /* Dialog menu          */
       _help_text      = (16*WHITE+BLACK)    & 0x7F,  /* help text            */
       _help_bold      = (16*WHITE+RED)      & 0x7F,  /* bold help text       */
       _help_border    = (16*WHITE+BLACK)    & 0x7F,  /* help border          */
       _help_topiclink = (16*WHITE+LIGHTBLUE)& 0x7F,  /* topic link text      */
       _help_linkselect= (16*WHITE+RED)      & 0x7F;  /* selected topic link  */

int    display_mode;
char   display_mode_str[20];

int    shadow = FALSE;

unsigned int action_row = 0;
unsigned int action_col = 0;

/*---------------------------------------
  Define the default color setting for
  1st and 2nd monitor
---------------------------------------*/
int mon1_color = COLOR;         /* Default colors for 1st montitor       */
int mon2_color = MONO;          /* Default colors for 1st montitor       */
int monitor    = MONITOR_1;     /* Current monitor output to 1st monitor */

int direct_screen = TRUE;       /* Write directly to screen (no bios calls) */

#else

 extern char   *start_screen ;  /* Pointer to save DOS screen         */
 extern WINDOW *top_window   ;  /* Pointer to top window              */
 extern WINDOW *start_window ;  /* Start window of double linked list */
 extern WINDOW *last_window  ;  /* Last window of double linked list  */

 extern int _menu_att;          /* These attributes are defined in  */
 extern int _menu_hotkey;
 extern int _menu_highlight;
 extern int _window_att;
 extern int _border_att;
 extern int _error_att;
 extern int _disabled_att;
 extern int _comment_att;
 extern int _dialog_att;
 extern int _desktop_att;
 extern int _help_text;
 extern int _help_bold;
 extern int _help_border;
 extern int _help_topiclink;
 extern int _help_linkselect;

 extern int display_mode;
 extern char display_mode_str[];

 extern unsigned int action_row;
 extern unsigned int action_col;

 extern int mon1_color;
 extern int mon2_color;
 extern int monitor;

 extern int direct_screen;

#endif
