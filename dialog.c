/**************************************************************************
$Header: DIALOG.C Fri 10-27-2000 8:47:18 pm HvA V2.01 $

DESC: Dialog routines for HVA_WIN
HIST: 19900818 V1.00
	  20001027 V2.01 - Adapting for DJGPP
***************************************************************************/

/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "keys.h"
#include "video/v_video.h"
#include "window.h"

/*--------------------
  Dialog attributes
---------------------*/
int m_att = (16*RED+YELLOW) & 0x7F;
int b_att = (16*RED+YELLOW) & 0x7F;

/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_dialog()
     PURPOSE: Perform a dialog with user
 DESCRIPTION: -
     RETURNS: choise made by user
     HISTORY: 922024 V0.3
--#-func----------------------------------------------------------------------*/
int wn_dialog(int row, int col, int nchoices, MENU *menu, int ntitles, ...)
{
  int     i,j,height,width=0,choiselen=0,titlelen=0;
  unsigned int key;
  int     choise_row;
  int     tabs[25],select;
  char    *titles[25];
  int     xsize;
  WINDOW  *window;
  va_list arg_marker;

  /*------------------------------------------
    Get the titles by va_start and calculate
    the width, needed for the titles. Store
    this value in 'titlelen'
  -------------------------------------------*/
  va_start(arg_marker,ntitles);
  for (i=0 ; i<ntitles ; i++)
  {
    titles[i] = va_arg(arg_marker,char *);
    titlelen = max(titlelen,strlen(titles[i]));
  }

  /*------------------------------------------------
    Calculate the width, needed for
    the options (choises). Store it in 'choiselen'
  -------------------------------------------------*/
  choiselen = nchoices - 1;
  for (i=0 ; i<nchoices ; i++)
    choiselen += (strlen(menu[i].header) + 2);

  width  =  max(titlelen,choiselen)+4;
  height =  ntitles + 6;
  row    =  row - height/2;
  choise_row = row + height - 4;

  wn_log("*** START DIALOG\n");
  wn_log_indent(+2);

  /*------------------------
    Open the dialog window
  -------------------------*/
  wn_log("open window");
  window = wn_open(_DOUBLE_LINE, row, col-width/2, width, height,m_att,b_att);

  /*---------------------------------------
    Print test of dialog menu (titles)
  ---------------------------------------*/
  wn_log("print titles");
  for (i=0 ; i<ntitles ; i++)
    printcenter(row+i+1, col, titles[i]);

  /*---------------------------------
    Draw the 'choise' boxes
    The default box is double lined
  ----------------------------------*/
  wn_log("print boxes");
  j = col - choiselen/2;
  for (i=0 ; i<nchoices ; i++)
  {
    tabs[i] = j-2;
    xsize = strlen(menu[i].header);
    if (!i)
    {
      wn_setbox(_DOUBLE_LINE);
      v_drawbox(choise_row, tabs[i], xsize+2, 3, m_att);
    }
    else
    {
      wn_setbox(_SINGLE_LINE);
      v_drawbox(choise_row, tabs[i], xsize+2, 3,  b_att);
    }
    v_setattrib_area(choise_row,   tabs[i]+1, xsize, 1, m_att);
    hotstring(choise_row+1, tabs[i]+1, menu[i].hotkey, menu[i].header, m_att);
    j += strlen(menu[i].header) + 5;
  }

  select = 0;
  while (TRUE)
  {
    key = waitkey();
    wn_log("key pressed = %d",key);

    /*-----------------------------------------------
      If hotkey defined, make this the new key value
    ------------------------------------------------*/
    switch(key)
    {
      case ENTER:
        if (menu[select].hotkey != -1)
          key = menu[select].header[menu[select].hotkey];
        break;
      default:
        break;
    }

    switch (key)
    {
      case ESC:
        {
          wn_setbox(_SINGLE_LINE);     /* Restore default */
          window = wn_close(window);
          wn_log_indent(-2);
          wn_log("*** END DIALOG\n");
          return(ESC);
        }
        break;

      case ARROW_LEFT:
        if (nchoices != 1)
        {
          wn_setbox(_SINGLE_LINE);
          v_drawbox(choise_row, tabs[select],
                  strlen(menu[select].header)+2, 3, b_att);
          select = (--select + nchoices) % nchoices;
          wn_setbox(_DOUBLE_LINE);
          v_drawbox(choise_row, tabs[select],
                  strlen(menu[select].header)+2, 3, m_att);
        }
        continue;

      case ARROW_RIGHT:
        if (nchoices != 1)
        {
          wn_setbox(_SINGLE_LINE);
          v_drawbox(choise_row, tabs[select],
                  strlen(menu[select].header)+2, 3, b_att);
          select = ++select % nchoices;
          wn_setbox(_DOUBLE_LINE);
          v_drawbox(choise_row, tabs[select],
                  strlen(menu[select].header)+2, 3, m_att);
        }
        continue;

      case F1:
        help(menu[select].help);
        continue;

      /*--------------------------------------
      | No hotkey defined for this popup menu
      ----------------------------------------*/
      case ENTER:
        wn_hide(window);       /* Hide it before actions */
        do_action(menu,select);
        if (menu[select].flags & EXIT)
        {
          window = wn_close(window);
          wn_log_indent(-2);
          wn_log("*** END DIALOG\n");
          return(select);
        }
        else
        {
          wn_unhide(window);
          continue;
        }
        break;

      /*--------------------
      | React on hotkey
      --------------------*/
      default:
        if (key > 31 && key < 128)
        {
          for (i=0 ; i<nchoices ; i++)
          {
            if (toupper(key) == toupper(menu[i].header[menu[i].hotkey]))
            {
              wn_hide(window);
              do_action(menu,i);
              if (menu[i].flags & EXIT)
              {
                window = wn_close(window);
                wn_log_indent(-2);
                wn_log("*** END DIALOG\n");
                return(toupper(key));
              }
              else
              {
                wn_unhide(window);
                wn_setbox(_SINGLE_LINE);    /* Restore default */
                break;
              }
            }
          }
        } /* for (i=0 ; ... */
    } /* switch(key) */
  } /* while (TRUE) */

  wn_setbox(_SINGLE_LINE);     /* Restore default */
  return(ESC);
}

