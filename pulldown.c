/**************************************************************************
$Header: PULLDOWN.C Mon 11-06-2000 9:34:30 pm HvA V2.00 $
***************************************************************************/

/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include "video/v_video.h"
#include "keys.h"
#include "window.h"
#include "fortify.h"


extern SCREEN _cursvar;

/*------------------------------------------------------------------------------
                        PROTOTYPES
------------------------------------------------------------------------------*/
static void show_bar(void);
static void calc_pulldownbar(MENU_HEAD *head);
static void mem_pulldownbar(void);

/*------------------------------------------------------------------------------
                        LOCAL VARIABLES
------------------------------------------------------------------------------*/
static int  *columns;                  /* Ptr to array of start columns     */
static char *hotkeys;                  /* Ptr to array of hotkeys           */
static MENU_HEAD *cptr = NULL;         /* Copy of ptr to first menu header  */
static MENU_HEAD *head = NULL;         /* Copy of ptr to actual menu header */
static int  no_heads = 0;              /* Number of menu headers            */
static int  menu;                      /* Selected menu                     */
static MENU *mptr;                     /* Pointer to menu tree              */
static WINDOW *pd_bar = NULL;          /* Window with pulldown bar          */

/*==============================================================================
                        START OF FUNCTIONS
==============================================================================*/

/*#+func-------------------------------------------------------------------
    FUNCTION: show_bar()
     PURPOSE: Show the bar of the pulldown menu
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 910829 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static void show_bar()
{
  int i;
  int col;

  wn_log_indent(2);
  wn_log("SHOW_BAR *****()\n");

  /*----------------------------------------------------------
    Check if pulldownbar already openened. If not, do it here
  ------------------------------------------------------------*/
  if (!pd_bar)
  {
    pd_bar = wn_open(_NO_BORDER,0,0,_cursvar.cols,1,_menu_att,_menu_att);
    col = 1;
    for (i=0 ; i<no_heads ; i++)
    {
      /*--- show header ---*/
      hotstring(0,col,head[i].hotkey,head[i].header,_menu_att);
      col += strlen(head[i].header) + 2;
    }
    wn_save(pd_bar);
  }
  else
    wn_restore(pd_bar);

  wn_log("END SHOW_BAR *****\n");
  wn_log_indent(-2);
}

/*#+func----------------------------------------------------------------
   FUNCTION: pulldown()
    PURPOSE: Show pulldown menu bar and react on key strokes
      INPUT: MENU_HEAD *shead  : pointer to header information array
             int       ikey    : Default key to react on
             char      *help   : Pointer to help subject
 DESRIPTION: -
    RETURNS: 0 or valid key scan-code
    VERSION: 901115 V0.5
             910829 V0.6 - Using popup() now for generality
--#-func-------------------------------------------------------------------*/
int pulldown(MENU_HEAD *shead, int ikey, char *menu_help)
{
  unsigned int  key;
  char c;
  int match = FALSE;
  unsigned int choise = 0;
  int i;

  wn_log_indent(2);
  wn_log("pulldown() MENU_HEAD=%p\n",shead);

  /* SHOW MENU HEADERS
  --------------------*/
  pulldown_bar(shead);

  /*----------------------------------------
     Use initial key or wait for a keypress
  -----------------------------------------*/
  wn_log("pulldown() - Waiting for first key press\n");
  if (ikey)
    key = ikey;
  else
    key = waitkey();
  wn_log("key pressed = %x\n",key);

  /*--- If function key, react on it  */
  if (is_fkey(key)) {
    wn_log("function key pressed\n");
    switch(key) {
      case F1:  help(menu_help); return(0); break;
      default:  do_funkey(key); break;
    }
  }

  /*--- If ESC key, return  */
  if (key == ESC_KEY)
    return(ESC_KEY);

  c = (char) (key & 0x00FF);
  wn_log("character = <%c>\n",c);

  /*----------------------------------------------
    Check if key matches with one of the hotkeys
  -----------------------------------------------*/
  for (i=0 ; i<strlen(hotkeys) ; i++) {
    if (c == hotkeys[i] || toupper(c) == hotkeys[i]) {
	  wn_log("we have a hotkey match\n");
	  match = TRUE;
	  break;
	}
  }
  if (match == TRUE) {
    menu = i;
  }
  else {
    wn_log("no match found\n");
    return(key);
  }


  /* THERE IS A MATCH ...
  ----------------------*/
  v_cursoroff();
  while(TRUE)
  {
    mptr = head[menu].mptr;
	wn_log("mptr = %p\n",mptr);
    if (head[menu].number != 0)
      choise = popup(head[menu].number,mptr,1,columns[menu]);
    else
    {
	  wn_log("pulldown() - check if a function is defined\n");
      /*---------------------------------------------
        If a function was defined, perform function
      -----------------------------------------------*/
      if (mptr[0].function != NULL) {
	    wn_log("perform function \n");
        (*(PFI)(mptr[0].function))();
	  }
    }

    wn_log("choise = %x\n",choise);
    switch(choise)
    {
      case ARROW_LEFT:
        v_setattrib_area(0, columns[menu], strlen(head[menu].header), 1, _menu_att);
        menu = (--menu + no_heads) % no_heads;
        show_bar();
        break;

      case ARROW_RIGHT:
        v_setattrib_area(0, columns[menu], strlen(head[menu].header), 1, _menu_att);
        menu = ++menu % no_heads;
        show_bar();
        break;

      default:
        break;
    } /* switch(choise) */

    if (choise == ESC_KEY)
      break;

  } /* while(TRUE) */
  show_bar();
  return(0);
}

/*#+func-----------------------------------------------------------------
    FUNCTION: calc_pulldownbar()
     PURPOSE: Perform calculations for the pulldown bar
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 910829 V0.1 - Calculations moved from pulldown_bar()
                            to this function to be more readable.
--#-func-------------------------------------------------------------------*/
static void calc_pulldownbar(MENU_HEAD *head)
{
  int i,j;
  int col;
  MENU *m;         /* Pointer to menu        */


  wn_log("calc_pulldownbar()\n");

  /*-------------------------------------------
    Preprocess the entrys of each menu[header]
  ---------------------------------------------*/
  for (i=0 ; i<no_heads ; i++)
  {
    m = head[i].mptr;
    j = 0;
    while (m->header)
    {
      strcpy(m->string,m->header);   /* Copy header to string            */
      j++;                           /* Increase counter                 */
      m = m+1;                       /* Update menu pointer              */
      head[i].number = j;            /* Update cntr of number of entries */
    }
  }

  col = 1;                      /* start column of first header */
  for (i=0 ; i<no_heads ; i++)
  {
    /*--- Save header hotkey and header start column ---*/
    columns[i] = col;
    hotkeys[i] = toupper(head[i].header[head[i].hotkey]);
    col += strlen(head[i].header) + 2;
  }

  hotkeys[no_heads] = '\0';  /* Terminate the array of hotkeys   */
}

/*#+func-------------------------------------------------------------------
    FUNCTION: mem_pulldownbar()
     PURPOSE: Perform memory manupilation for the pulldown bar
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 910829 V0.1 - Mem manupilations moved from pulldown_bar()
                            to this function to be more readable.
--#-func-------------------------------------------------------------------*/
static void mem_pulldownbar()
{
  /*--------------------------------------------
    ALLOCATE OR REALLOCATE MEMORY FOR 'COLUMNS'
  ----------------------------------------------*/
  if (columns == NULL)
  { if ((columns = (int *) malloc(no_heads*sizeof(int))) == NULL)
    { printf("Out of memory\n");
      exit(1);
    }
  }
  else
  {
    if ((columns = (int *) realloc((char *)columns,no_heads*sizeof(int))) == NULL)
    {
      printf("Out of memory\n");
      exit(1);
    }
  }

  /*---------------------------------------------
    ALLOCATE OR REALLOCATE MEMORY FOR 'HOTKEYS'
  ----------------------------------------------*/
  if (hotkeys == NULL)
  {
    if ((hotkeys = malloc((no_heads+1)*sizeof(char))) == NULL)
    {
      printf("Out of memory\n");
      exit(1);
    }
  }
  else
  {
    if ((hotkeys = realloc(hotkeys,(no_heads+1)*sizeof(char))) == NULL)
    {
      printf("Out of memory\n");
      exit(1);
    }
  }
}

/*#+func-------------------------------------------------------------------
    FUNCTION: pulldown_bar()
     PURPOSE: perform initial calculations for activiting 'pulldown'
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 920223 V0.2
--#-func-------------------------------------------------------------------*/
void pulldown_bar(MENU_HEAD *shead)  /* Pointer to array of headers */
{
  MENU_HEAD *h;

  wn_log("pulldown_bar() MENU_HEAD=%p\n",shead);

  /*--------------------------------------
    If same menuline as last time, return
  ----------------------------------------*/
  if (cptr == shead)
    return;
  else
  {
    cptr = shead;
    head = shead;
  }

  /*-------------------------------
    Determine number of menu heads
    this must be done before calling
    mem_pulldownbar()
  ---------------------------------*/
  h = shead;
  while (h->header)
  {
    no_heads++;
    h++;
  }

  /*------------------------------
    Assign memory and
    Perform initial calculations
    Show the menu bar
  -------------------------------*/
  mem_pulldownbar();
  calc_pulldownbar(head);
  show_bar();

}

/*#+func-----------------------------------------------------------------------
    FUNCTION: exit_pulldown()
     PURPOSE: exit from pulldown menu
 DESCRIPTION: free's allocated memory
     RETURNS: nothing
     HISTORY: 920107 V0.1 - Initial version
--#-func----------------------------------------------------------------------*/
void exit_pulldown()
{
  wn_log("exit_pulldown()\n");
  
  if (hotkeys)
    free(hotkeys);
  if (columns)
    free((char *)columns);
}
