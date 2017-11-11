/**************************************************************************
$Header: FUNKEY.C Fri 10-27-2000 9:26:44 pm HvA V2.01 $

DESC: function key handling for HVA_WIN
HIST: 19911223 V1.00
	  20001027 V2.01 - Adapted for DJGPP
***************************************************************************/

/****************************************************************************
*              Function keys handled are:       F1...F10                    *
*                                         SHIFT_F1...SHIFT_F10              *
*                                           ALT_F1...ALT_F10                *
*                                          CTRL_F1...CTRL_F10               *
****************************************************************************/

/*---------------------------------------------------------------------------
                        HEADER FILES
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <string.h>
#include <stdarg.h>
#include "keys.h"
#include "video/v_video.h"
#include "window.h"

extern SCREEN _cursvar;

/*---------------------------------------------------------------------------
                        PROTOTYPES
---------------------------------------------------------------------------*/
typedef struct
{
  void *function;	/* Function to perform */
  char *text;           /* Text on line        */
}
FUNCTION_HANDLER;

// static void init_keylabels(void);
static int func_index(int function_key);

/*---------------------------------------------------------------------------
                        CONSTANTS
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                        LOCAL VARIABLES
---------------------------------------------------------------------------*/
static WINDOW *w = NULL;
static FUNCTION_HANDLER funkey[40];
static int show_funkeys = FALSE;

/*
#define TESTING
*/

/*===========================================================================
                        START OF FUNCTIONS
===========================================================================*/

/*#+func---------------------------------------------------------------------
    FUNCTION: func_index()
     PURPOSE: Calculate index of a function key
 DESCRIPTION: -
     RETURNS: Calculated index to the array of function handlers
     HISTORY: 911224 V0.1
--#-func-------------------------------------------------------------------*/
static int func_index(int function_key)
{
  int ndx;

  switch (function_key)
  {
    case F1:
    case F2:
    case F3:
    case F4:
    case F5:
    case F6:
    case F7:
    case F8:
    case F9:
    case F10:
      ndx = function_key-F1;
      break;

    case SHIFT_F1:
    case SHIFT_F2:
    case SHIFT_F3:
    case SHIFT_F4:
    case SHIFT_F5:
    case SHIFT_F6:
    case SHIFT_F7:
    case SHIFT_F8:
    case SHIFT_F9:
    case SHIFT_F10:
      ndx = function_key - SHIFT_F1 + 10;
      break;

    case CTRL_F1:
    case CTRL_F2:
    case CTRL_F3:
    case CTRL_F4:
    case CTRL_F5:
    case CTRL_F6:
    case CTRL_F7:
    case CTRL_F8:
    case CTRL_F9:
    case CTRL_F10:
      ndx = function_key - CTRL_F1 + 20;
      break;

    case ALT_F1:
    case ALT_F2:
    case ALT_F3:
    case ALT_F4:
    case ALT_F5:
    case ALT_F6:
    case ALT_F7:
    case ALT_F8:
    case ALT_F9:
    case ALT_F10:
      ndx = function_key - ALT_F1 + 30;
      break;

    default:
      #ifdef TESTING
        wn_error("Wrong function key %d in init_funct_Fx()",function_key);
      #endif
      return(-1);
  }

  return(ndx);
}

/*#+func---------------------------------------------------------------------
    FUNCTION: init_funkey()
     PURPOSE: Initialize a function key
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911224 V0.1
--#-func-------------------------------------------------------------------*/
void init_funkey(int function_key, void *function, char *text)
{
  int ndx;

  wn_log("init_funkey(key=%d,function=%p,text=%s\n",
    function_key, function, text);

  /*-------------------------------
    Initialize funkey[] variables
  ---------------------------------*/
  ndx = func_index(function_key);       /* Calculate index to array         */
  funkey[ndx].function = function;      /* Copy pointer to funkey function  */
  if (strlen(text) > 6) text[6] = '\0'; /* Check if text is not to long     */
  funkey[ndx].text     = text;          /* Copy pointer to funkey text      */

}

/*#+func---------------------------------------------------------------------
    FUNCTION: do_funkey()
     PURPOSE: Perform function, assigned to the function key
      SYNTAX: int do_funkey(int key);
              int key : code of (function) key to perform function on
 DESCRIPTION:
     RETURNS: 0 if function performed (function available)
              function_key if no function performed
     HISTORY: 911224 V0.1
--#-func-------------------------------------------------------------------*/
int do_funkey(int key)
{
  int ndx;

  wn_log("do_funkey(key=%2d)\n");

  if (is_fkey(key)) {
    ndx = func_index(key);                /* Find array index       */
    if (funkey[ndx].function) {           /* If function defined    */
      (*(PFI)(funkey[ndx].function))();   /* then execute function  */
      return(0);
    }
    else
      return(key);
  }
  else
    return(key);

}

/*#+func---------------------------------------------------------------------
    FUNCTION: disp_funkeys()
     PURPOSE: Display function keys on screen
 DESCRIPTION: Display function keys in lower 4 lines of display
              0 = Hide function key display
              1 = normal function keys
              2 = shifted function keys
              3 = CTRL function keys
              4 = ALT function keys
     RETURNS: nothing
     HISTORY: 911224 V0.1
--#-func-------------------------------------------------------------------*/
void disp_funkeys(int i)
{
  int j;
  int row,col;

  show_funkeys = TRUE;

  if (i<0 || i>4)
  {
    wn_log("wrong index %d in disp_funkeys\n",i);
    i = 1;
  }

  /*--------------------------------------
    Check if we have to open a window for
    the function keys, or if this window
    has already been opened.
    The window will occupy the last row on
    the screen with a full length
  ----------------------------------------*/
  if (!w)
  {
    wn_log("*** START FUNKEY\n");
    w = wn_open(_NO_BORDER,_cursvar.lines,0,_cursvar.cols+1,1,_menu_att,_menu_att);
    wn_clear(w);
  }

  /*------------------------------------------------
    Check if user wants to hide function key labels
    or to display it
  --------------------------------------------------*/
  if (i==0)
  {
    show_funkeys = FALSE;
    wn_hide(w);
    return;
  }
  else {
    if (show_funkeys == FALSE) {
      show_funkeys = TRUE;
      if (w)
        wn_unhide(w);
    }
  }

  /*-------------------------
    Print the function keys
  --------------------------*/
  row = 0;
  col = 0;
  for (j = 0 ; j<10 ; j++)
  {
    wn_locate(w,row,col);
    if (funkey[j].text) {
      wn_printf(w,"%1d%-6.6s",j+1,funkey[j].text);
      wn_setone(w,row,col,_menu_hotkey);
      if (j==9)
       wn_setone(w,row,col+1,_menu_hotkey);
    }
    col+=7;
  }
}

/*#+func---------------------------------------------------------------------
    FUNCTION: exit_funkey()
     PURPOSE: Exit the funkey routine
 DESCRIPTION: Closes the function key window
     RETURNS: nothing
     HISTORY: 920107 V0.1 - Initial version
--#-func--------------------------------------------------------------------*/
void exit_funkey()
{
  if (w)
  {
    wn_log("*** END FUNKEY\n");
    w = wn_close(w);
  }
}
