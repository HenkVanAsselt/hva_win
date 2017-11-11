/**************************************************************************
$Header: appl.c Sun 11-19-2000 10:50:53 pm HvA V1.00 $
Test program for HvA Windows
***************************************************************************/

#define  MAIN_MODULE

/*-------------------------------------------------------------------------
                        HEADER FILES
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <direct.h>
#include <time.h>
#include <string.h>
#include "keys.h"
#include "video/v_video.h"
#include "window.h"
#include "fortify.h"

/*-------------------------------------------------------------------------
                        PROTOTYPES
---------------------------------------------------------------------------*/
void do_dir(void);
void do_exit(void);
void initialize(void);
void exit_prog(void);
int  main(void);
void overlap_demo(void);

extern SCREEN _cursvar;

/*-------------------------------------------------------------------------
                        LOCAL VARIABLES
---------------------------------------------------------------------------*/
float  test_f = 1.0;
double test_d = 2.0;
char   test_s[80];
int    test_i = 20;
int    test_h = 0x15;
int    test_b = FALSE;


MENU M_shell[] =
{
  { "Directory ", 0, do_dir,     "", NULL, NULL, },
  { "Change dir", 0, change_dir, "", NULL, NULL, },
  { "OS shell  ", 0, OS_shell,   "", NULL, NULL, },
  { "Exit      ", 0, do_exit,    "", NULL, NULL, },
  { NULL }
};

MENU M_TEST_EDIT[] =
{
  { "Edit float   ", 5, NULL, "",  &test_f, "%.3f"  ,EDIT_STR },
  { "Edit double  ", 5, NULL, "",  &test_d, "%.6lf" ,EDIT_STR },
  { "Edit string  ", 5, NULL, "",  test_s,  "%s"    ,EDIT_STR },
  { "Edit Integer ", 5, NULL, "",  &test_i, "%d"    ,EDIT_STR },
  { "Edit Hex     ", 5, NULL, "",  &test_h, "%x"    ,EDIT_STR },
  { "Edit Boolean ", 5, NULL, "",  &test_b, "%B"    ,SELECTABLE },
  { NULL }
};

MENU M_TEST_HELP[] =
{
  { " Window functions", -1, NULL, "window functions",      NULL, NULL, HELP_ONLY },
  {NULL}
};

MENU M_WINDEMO[] =
{
  { "Demo", 0, overlap_demo, "overlap demo", NULL, NULL, NO_FLAGS},
  { NULL }
};

MENU_HEAD heads[] =
{
  { "System",  0, 4, M_shell     },
  { "Edit",    0, 5, M_TEST_EDIT },
  { "Help",    0, 7, M_TEST_HELP },
  { "Window_demo", 0, 1, M_WINDEMO },
  { NULL }
};

/*------------------
  Local variables
-------------------*/
char error_msg[132];

/*#+func-----------------------------------------------------------------------
    FUNCTION: overlap_demo()
     PURPOSE: Demo of overlapping windows
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 920217 V0.1
--#-func----------------------------------------------------------------------*/
void overlap_demo()
{
  WINDOW *w1;
  WINDOW *w2;
  WINDOW *w3;
  WINDOW *w4;

  w1 = wn_open(_SINGLE_LINE,10,10,10,10,
               16*YELLOW+GREEN,16*YELLOW+GREEN);
  wn_printf(w1,"Window 1\n");

  getch();
  w2 = wn_open(_DOUBLE_LINE,12,12,10,10,
               16*RED+GREEN,16*RED+GREEN);
  wn_printf(w2,"Window 2\n");

  getch();
  w3 = wn_open(_NO_BORDER,14,14,10,10,
               16*BLUE+GREEN,16*BLUE+GREEN);
  wn_printf(w3,"Window 3\n");

  getch();
  w4 = wn_open(_NO_BORDER,16,16,10,10,
               16*CYAN+YELLOW,16*CYAN+YELLOW);
  wn_printf(w4,"Window 4\n");


  wn_printf(w1,"Window 1\n");
  getch();
  wn_printf(w2,"Window 2\n");
  getch();
  wn_printf(w3,"Window 3\n");
  getch();
  wn_printf(w4,"Window 4\n");
  getch();

  wn_printf(w4,"Window 4\n");
  getch();
  wn_printf(w3,"Window 3\n");
  getch();
  wn_printf(w2,"Window 2\n");
  getch();
  wn_printf(w1,"Window 1\n");
  getch();

  w4 = wn_close(w4);
  getch();
  w3 = wn_close(w3);
  getch();
  w2 = wn_close(w2);
  getch();
  w1 = wn_close(w1);
}

/*#+func--------------------------------------------------------------
    FUNCTION: call_zapload()
     PURPOSE: Call the zapload utilitie.
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911211 V0.1 - Initial version
--#-func-------------------------------------------------------------*/
void call_zapload(void);
void call_zapload()
{
  char command_str[80];
  WINDOW *w;

  /*----------------------------
    Open standard DOS window
  -----------------------------*/
  w = wn_open(_NO_BORDER,0,0,_cursvar.cols,_cursvar.lines,
              16*BLACK+WHITE,16*BLACK+WHITE);

  sprintf(command_str,"zapload");
  system(command_str);
  puts("\n\nPress any key to continue ...");
  getch();

  w = wn_close(w);
}

/*#+func--------------------------------------------------------------
    FUNCTION: call_hexconv()
     PURPOSE: Call the HEX converter utilitie.
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911211 V0.1 - Initial version
--#-func-------------------------------------------------------------*/
void call_hexconv(void);
void call_hexconv()
{
  char command_str[80];
  WINDOW *w;

  /*----------------------------
    Open standard DOS window
  -----------------------------*/
  w = wn_open(_NO_BORDER,0,0,_cursvar.cols,_cursvar.lines,
              16*BLACK+WHITE,16*BLACK+WHITE);

  sprintf(command_str,"convert");
  system(command_str);
  puts("\n\nPress any key to continue ...");
  getch();

  w = wn_close(w);

}

/*#+func--------------------------------------------------------------
    FUNCTION: call_terminal()
     PURPOSE: Call the specified terminal.
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911217 V0.1 - Initial version
--#-func-------------------------------------------------------------*/
void call_terminal(void);
void call_terminal()
{
  char command_str[80];
  WINDOW *w;

  /*----------------------------
    Open standard DOS window
  -----------------------------*/
  w = wn_open(_NO_BORDER,0,0,_cursvar.cols,_cursvar.lines,
              16*BLACK+WHITE,16*BLACK+WHITE);

  sprintf(command_str,"%s","procomm");
  system(command_str);

  w = wn_close(w);

}

/*-------------------------------------------------------------------
|    FUNCTION: do_exit()
|     PURPOSE: Perform exit(0) call from pulldown menu
| DESCRIPTION: -
|     RETURNS: nothing
|     HISTORY: 911107 V0.1 - Initial version
---------------------------------------------------------------------*/
void do_exit()
{
  exit(0);
}

/*-------------------------------------------------------------------
|    FUNCTION: exit_prog()
|     PURPOSE: exit from program
| DESCRIPTION: Perform this function only once:
|              exit from frontend processor,
|              update system files,
|              close window enviroment and
|              call exit().
|     RETURNS: nothing
|     HISTORY: 911124 V0.3
---------------------------------------------------------------------*/
void exit_prog()
{
   static int done = FALSE;

  if (!done)               /* Do this only once */
  {
    wn_exit();          /* Exit from HvA window enviroment          */
    done = TRUE;        /* Set flag (this routine has been executed */
  }
}

/*-------------------------------------------------------------------
|    FUNCTION: Intro_screen()
|     PURPOSE: Intro screen with copyright message
| DESCRIPTION: -
|     RETURNS: nothing
|     HISTORY: 911124 V0.1 - Initial version
---------------------------------------------------------------------*/
void intro_screen(void);
void intro_screen()
{
  static MENU menu[]=
  {
    {"OK",   0, NULL, NULL,        NULL, NULL, EXIT      },
  };

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif

  wn_dialog(13,40,1,menu,6,
    "       WINDOW TEST        ",
    "                          ",
    "                          ",
    "  (c) H.B.J. van Asselt   ",
    "      12 OCT 1993         ",
    "");

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif
}

/*-------------------------------------------------------------------
|    FUNCTION: initialize()
|     PURPOSE: Initialize window enviroment
| DESCRIPTION: initialize variables,
|              prints introduction screen,
|     RETURNS:
|     HISTORY: 911124 V0.3
---------------------------------------------------------------------*/
void initialize()
{
  extern char *default_subject;

  /*----------------------------------------------
    Set vector to exit_prog() if exit() is called
  ------------------------------------------------*/
  atexit(exit_prog);

  /*----------------------------
    Initilize (graphic) screen
  ----------------------------*/
  wn_init();

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif

  /*-------------------------------
    Initialize help file functions
  ---------------------------------*/
  default_subject = ":general_info";
  init_help("window");

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif

  /*-------------------
    Show intro screen
  --------------------*/
  /*
  intro_screen();
  */

  /*-------------------------------------
    Initialize function key's functions
  --------------------------------------*/
  /*
  init_funkey(F2,call_zapload,"Zapload");
  init_funkey(F3,call_hexconv,"HEXconv");
  init_funkey(F4,call_terminal,"Terminal");
  init_funkey(F10,OS_shell,"SHELL");
  disp_funkeys(1);
  */
}

/*#+func--------------------------------------------------------------
    FUNCTION: do_dir()
     PURPOSE: Show directory
 DESCRIPTION: Asks for a filemask and shows directory
              If edit window was escaped with ESC, don't show the
              directory
     RETURNS: nothing
     HISTORY: 911211 V0.1 - Initial version
--#-func-------------------------------------------------------------*/
void do_dir(void)
{
  char filemask[20];
  int ret;

  strcpy(filemask,"*.*");
  ret = editstr(action_row+2,action_col+2,20,"Enter file mask",filemask,_DRAW);
  if (ret != ESC)
    get_dir(filemask,filemask);
}

/*-------------------------------------------------------------------
|    FUNCTION: main()
|     PURPOSE: main programm
| DESCRIPTION: -
|     RETURNS: nothing
|     HISTORY: 911124 V0.1 - Initial version
---------------------------------------------------------------------*/
int main()
{
  int choise;
  char s[80];
  int i;

  initialize();

#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif

  do
  {
    choise = pulldown(heads,0,":general_info");
  }
  while (choise != ESC);

  /*--------------------
    Exit this programm
  ---------------------*/
#if defined(USE_FORTIFY)
  Fortify_ListAllMemory();
  // Fortify_OutputStatistics();
#endif

  exit(0);
  return(0);
}
