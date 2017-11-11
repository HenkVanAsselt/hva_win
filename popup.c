/**************************************************************************
$Header: POPUP.C Fri 10-27-2000 9:40:58 pm HvA V2.01 $

DESC: Popup menu routines for HVA_WIN

HIST: 19900818 V0.01 - Initial version                         
	  19910727 V0.02 - Adapted for new window routines (wn_...)
	  19911119 V0.03 - Added directory pick routine            
	  20001027 V2.01 - Adapted for DJGPP
***************************************************************************/

/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include "video/v_video.h"
#include "keys.h"
#include "window.h"

/* Dynamic memory handling shell */
/*
#include <mshell.h>
*/

/*------------------------------------------------------------------------------
                        PROTOTYPES
------------------------------------------------------------------------------*/
char *void_sprintf(char *datastr, char *format, void *data);
void void_sscanf(char *str, char *format, void *data);

/*==============================================================================
                        START OF FUNCTIONS
==============================================================================*/

/*#+func-------------------------------------------------------------
    FUNCTION: do_action()
     PURPOSE: Perform action if a item is selected by a hotkey or
              by <ENTER>
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 28-Aug-1992 10:00:00 V0.1 - Initial version
              02-Jan-1993 16:13:09 V0.2 - New flag: EDIT_STR.
                                          Deleted NO_EDIT flag.
--#-func-------------------------------------------------------------*/
void do_action(MENU *menu, int i)
{
  char datastr[80], *s;

  /*---------------------------------------
    Toggle a BOOLEAN value
  ---------------------------------------*/
  if (menu[i].flags & SELECTABLE) {
    if (menu[i].data == FALSE)
      menu[i].data = (void *) TRUE;
    else
      menu[i].data = (void *) FALSE;
    return;
  }

  /*---------------------------------------
    Edit a string
  ---------------------------------------*/
  if (menu[i].data != NULL
  && (menu[i].format != NULL)
  && (menu[i].flags & EDIT_STR)) {
    s = void_sprintf(datastr,menu[i].format,menu[i].data);
    if (s) {
      editstr(action_row+2,action_col+2,30,menu[i].header,datastr,_DRAW);
      void_sscanf(datastr,menu[i].format,menu[i].data);
      return;
    }
  }

  /*---------------------------------------
    Enter help functions if HELP_ONLY
  ---------------------------------------*/
  if (menu[i].flags & HELP_ONLY)
  {
    help(menu[i].help);
    return;
  }

  /*---------------------------------------------
    If a function was defined, perform function
  -----------------------------------------------*/
  if (menu[i].function != NULL)
  {
    (*(PFI)(menu[i].function))();
    return;
  }

}

/*#+func-------------------------------------------------------------
   FUNCTION: popup()
    PURPOSE: Make and react on a popup menu
 DESRIPTION: This function creates an popup menu with text and
             values of referenced data. The upperleft corner of
             the popup menu is (row,col)
    RETURNS: Hot-key of selected item if available, else
             the number (0...number-1) of the item selected,
             If ESC is pressed, ESC is returned.
    VERSION: 910828 V0.6
             12-Oct-1993 V0.7 - wn_open() now uses size of external window
                                (incl. borders) -> adapted size of
                                popupwindow.
--#-func-------------------------------------------------------------*/
unsigned int popup(int  number,     /* Number of menu items                   */
          MENU *menu,      /* Pointer to an array of MENU structures */
          int row,         /* Upper row for the menu                 */
          int col )        /* Left column for the menu               */
{
  int i,mlen;
  unsigned int key = ARROW_DOWN;    /* Default key */
  WINDOW *popup_window;
  char datastr[132], *s;
  static int select = 0, old_select = 0;
  static MENU *oldmenu = NULL;
  static int autorow = FALSE;
  static int last_row = 0;
  static int autocol = FALSE;
  static int last_col = 0;

  wn_log("START POPUP()\n");
  wn_log_indent(+2);

  /*---------------------------------------
    Determine row
  ---------------------------------------*/
  if (row == AUTO_ROW) {
    if (!autorow) {
      row = action_row;
      last_row = action_row;
      autorow = TRUE;
    }
    else {
      row = last_row;
    }
  }
  else {
    autorow = FALSE;
    last_row = row;
  }

  /*---------------------------------------
    Determine column
  ---------------------------------------*/
  if (col == AUTO_COL) {
    if (!autocol) {
      col = action_col;
      last_col = action_col;
      autocol = TRUE;
    }
    else {
      col = last_col;
    }
  }
  else {
    autocol = FALSE;
    last_col = col;
  }


  /*---------------------------------------
    Fill menu strings with data
  ---------------------------------------*/
  for (i=0 ; i<number ; i++)
  {
    menu[i].string[0] = '\0';
    strcpy(menu[i].string,menu[i].header);  /* Copy 'header' to 'string' */
    s = void_sprintf(datastr,menu[i].format,menu[i].data);
    if (s)
    {
      strcat(menu[i].string,"  ");      /* Add blanks  */
      strcat(menu[i].string,datastr);   /* Add 'datastr' to 'string' */
    }
  }

  /*-----------------------------------
    Determine the maximum line length
  ------------------------------------*/
  mlen = 0;
  for (i=0 ; i<number ; i++)
    mlen = max(mlen,strlen(menu[i].string));

  /*------------------------------------------------------------
    Open the window and print the items
    12-Oct-1993 V0.7: Add 2 for xsize to include window borders
  -------------------------------------------------------------*/
  popup_window = wn_open(_SINGLE_LINE, row, col, mlen+2, number+2,
                    _menu_att, _border_att);
  row = popup_window->y1;
  col = popup_window->x1;

  wn_log("printing popup menu items:\n");
  for (i=0 ; i<number ; i++)
  {
    if (menu[i].flags & COMMENT)
      hotstring(row+i,col,-1,menu[i].string,_comment_att);
    else if (menu[i].flags & DISABLED)
      hotstring(row+i,col,-1,menu[i].string,_disabled_att);
    else
      hotstring(row+i,col,menu[i].hotkey,menu[i].string,_menu_att);
  }

  /*-------------------------------------------------------
    If it is the same menu as last time, we use the last
    selected item to highlight
  -------------------------------------------------------*/
  if (menu == oldmenu)
  {
    select = ++select % number;
    old_select = select;
  }
  else
  {
    select = 0;
    old_select = 0;
    oldmenu = menu;
  }

  while(TRUE)
  {
    /*-------------------------------------------------------------------
      Reset attribute of old selected line.
      Don't do this when the item is a COMMENT or when it is DISABLED
    --------------------------------------------------------------------*/
    if (!(menu[old_select].flags & (COMMENT|DISABLED))) {
      v_setattrib_area(row+old_select,col,mlen,1,_menu_att);
      hotstring(row+old_select,col,
        menu[old_select].hotkey,menu[old_select].string,_menu_att);
    }

    /*-------------------------------------------------------------------
      Set attribute of new selected line.
      Don't do this when the item is a COMMENT or when it is DISABLED
    --------------------------------------------------------------------*/
    if (!(menu[select].flags & (COMMENT|DISABLED))) {
      action_row = row+select;
      action_col = col;
      v_setattrib_area(action_row,action_col,mlen,1,_menu_highlight);
    }

    if (!(menu[select].flags & (COMMENT|DISABLED)))
      key = waitkey();		  /*--- Scancode is returned here  */

    if (is_fkey(key)) {
      switch(key) {
        case F1:  help(menu[select].help); break;
        default:  do_funkey(key); break;
      }
    }

    /*-----------------------------------------------
      If hotkey defined, make this the new key value
    ------------------------------------------------*/
    switch(key) {
      case ENTER:
        if (menu[select].hotkey != -1)
          key = menu[select].header[menu[select].hotkey];
        break;
      default:
        break;
    }

    switch(key) {
      /*----------------------
      | Up- and down arrow
      -----------------------*/
      case ARROW_UP:
	    wn_log("ARROW_UP\n");
        old_select = select;
        select = (--select+number) % number;
        break;

      case ARROW_DOWN:
	    wn_log("ARROW_DOWN\n");
        old_select = select;
        select = ++select % number;
        break;

      /*--------------------------------------
      | No hotkey defined for this popup menu
      ----------------------------------------*/
      case ENTER:
	    wn_log("ENTER\n");
        if (menu[select].flags & EXIT) {
          wn_log_indent(-2);
          wn_log("END POPUP()\n");
          popup_window = wn_close(popup_window);   /* Close it before actions */
        }
        do_action(menu,select);
        wn_log_indent(-2);
        wn_log("END POPUP()\n");
        popup_window = wn_close(popup_window);
        return(select);           /* Return number of selected item [0,...> */

      /*----------------------
      | Escape from popup()
      ----------------------*/
      case ESC_KEY:
      case ARROW_LEFT:
      case ARROW_RIGHT:
	    wn_log("ESC or ARROW_LEFT or ARROW_RIGHT\n");
        wn_log_indent(-2);
        wn_log("END POPUP()\n");
        popup_window = wn_close(popup_window);
        return(key);

      default:
	    wn_log("OTHER KEY\n");
		key = key & 0x00FF;	
        if (key > 31 && key < 128) {
          for (i=0 ; i<number ; i++) {
            if (toupper(key) == toupper(menu[i].header[menu[i].hotkey])) {
              if (menu[i].flags & EXIT) {
                wn_log_indent(-2);
                wn_log("END POPUP()\n");
                popup_window = wn_close(popup_window);
              }
              do_action(menu,i);
              wn_log_indent(-2);
              wn_log("END POPUP()\n");
              popup_window = wn_close(popup_window);
              return(toupper(key));
            }
          }
        } /* if (key ...) */
    } /* switch(key) */
  } /* while (TRUE) */

  if (key == ENTER)
    return(select);
  else if (key == ESC_KEY)
    return(ESC);
  else
    return(toupper(key));
}

