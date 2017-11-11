/**************************************************************************
$Header: windio.c Fri 10-27-2000 8:08:55 pm HvA V2.01 $

DESC: Low-level I/O routines for HVA_WINDOWS
HIST: 920223 V1.02
      20001027 V2.01 - Adapting for DJGPP
***************************************************************************/
				  
/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <bios.h>
#include <dos.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "video/v_video.h"
#include "window.h"

extern SCREEN _cursvar;

/*#+func-------------------------------------------------------------
   FUNCTION: setup_menu()
    PURPOSE: Setup a menu item
 DESRIPTION: This way a menu can be defined as an auto variable
             within a function. The variables are in the same order
             as in the WINDOW type definition
    RETURNS: nothing
    VERSION: 920117 V0.1 - Initial version
--#-func-------------------------------------------------------------*/
void setup_menu(MENU *menu, int i, char *header, int hotkey, void *function,
                char *help, void *data, char *format, int flags)
{
  menu[i].header   = header;
  menu[i].hotkey   = hotkey;
  menu[i].function = function;
  menu[i].help     = help;
  menu[i].data     = data;
  menu[i].format   = format;
  menu[i].flags    = flags;
}

/*#+func-------------------------------------------------------------------
   FUNCTION: init_menu()
    PURPOSE: Initialze a menu structure
 DESRIPTION: -
    RETURNS: nothing
    VERSION: 910901 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void init_menu(MENU *menu,  /* Pointer to menu structure to initialize */
               int n)       /* Number of menu items to initialize      */
{
  int i;

  for (i=0 ; i<n ; i++)
  {
    menu[i].function = NULL;
    menu[i].format = "";
    menu[i].hotkey = -1;
    menu[i].header = "";
    menu[i].help = "";
    menu[i].flags = 0;
    menu[i].data  = NULL;
  }
}

/*#+func-------------------------------------------------------------------
   FUNCTION: set_display_colors()
    PURPOSE: Set display colors for COLOR or MONO monitor
 DESRIPTION: Sets the global variables according to monitor type
             defined. This can be MONO or COLOR.
    RETURNS: nothing
    VERSION: 911126 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void set_display_colors(int mode)
{
  switch(mode)
  {
    case MONO:
      display_mode = MONO;
      strcpy(display_mode_str,"MONO");
      _menu_att       = (16*BLACK+WHITE) & 0x7F;          /* memu item        */
      _menu_hotkey    = (16*BLACK+INTENSE_WHITE) & 0x7F;  /* hotkey of item   */
      _menu_highlight = (16*WHITE+BLACK) & 0x7F;          /* highlighted item */
      _window_att     = (16*BLACK+WHITE) & 0x7F;          /* window attribute */
      _border_att     = (16*BLACK+WHITE) & 0x7F;          /* border           */
      _error_att      = (16*BLACK+WHITE) & 0x7F;          /* Error window     */
      _disabled_att   = (16*BLACK+WHITE) & 0x7F;          /* DISABLED item    */
      _comment_att    = (16*BLACK+WHITE) & 0x7F;          /* COMMENT item     */
      _dialog_att     = (16*BLACK+WHITE) & 0x7F;          /* Dialog menu      */
      _desktop_att    = (16*BLACK+WHITE) & 0x7F;          /* Desktop attrib   */
      break;

    default:
      display_mode = COLOR;
      strcpy(display_mode_str,"COLOR");
      _menu_att       = (16*WHITE+BLACK) & 0x7F;      /* memu item        */
      _menu_hotkey    = (16*WHITE+RED)   & 0x7F;      /* hotkey of item   */
      _menu_highlight = (16*BLACK+WHITE) & 0x7F;      /* highlighted item */
      _window_att     = (16*BLACK+WHITE) & 0x7F;      /* window attribute */
      _border_att     = (16*WHITE+BLACK) & 0x7F;      /* border           */
      _error_att      = (16*RED+YELLOW)  & 0x7F;      /* Error window     */
      _disabled_att   = (16*WHITE+RED)   & 0x7F;      /* DISABLED item    */
      _comment_att    = (16*WHITE+BLUE)  & 0x7F;      /* COMMENT item     */
      _dialog_att     = (16*WHITE+BLACK) & 0x7F;      /* Dialog menu      */
      _desktop_att    = (16*BLUE+YELLOW) & 0x7F;      /* Desktop attr     */
      break;
  }
}

/*#+func-------------------------------------------------------------------
    FUNCTION: clrscr()
     PURPOSE: clear screen
 DESCRIPTION: This function is only to be used with MSC.
              This is a standard function in TC. We will clear the
              entire screen by filling a 'window' with spaces with
              BLACK background and WHITE foreground.
     RETURNS: Nothing
     HISTORY: 910901 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
#ifdef __MSC__
void clrscr()
{
  v_fillarea(0,0,_cursvar.cols,_cursvar.lines,' ',_desktop_att);
}
#endif

/*#+func-------------------------------------------------------------------
    FUNCTION: printcenter()
     PURPOSE: Print a string centered around (row,col).
 DESCRIPTION: Calculates center and calls v_prints()
     RETURNS: nothing
     HISTORY: 910710 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void printcenter(int row,int col, char *string)
{
  v_prints(row, col - (strlen(string) >> 1), string);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: printhotcenter()
     PURPOSE: Print a hotstring centered around (row,col)
 DESCRIPTION: Calculates center and calls hotstring() with no
              hotkey (i.e. -1)
     RETURNS: nothing
     HISTORY: 911219 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void printhotcenter(int row,int col, char *string, int str_att)
{
  hotstring(row, col - (strlen(string) >> 1), -1, string, str_att);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: hotstring()
     PURPOSE: print a string with the 'hotkey' set
 DESCRIPTION: Print the string on the screen, set the attribute of
              the entire string and set the attribute of the 'hotkey'
     RETURNS: nothing
     HISTORY: 920213 V0.1
--#-func-------------------------------------------------------------------*/
void hotstring(int row, int col, int hotkey, char *string, int str_att)
{
  wn_log("hotstring() (%d,%d), hotkey='%d' <%s>\n",row,col,hotkey,string);
  v_prints(row,col,string);
  v_setattrib_area(row,col,strlen(string),1,str_att);
  if (hotkey > -1)
    v_setattrib(row,col+hotkey,_menu_hotkey);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: edit()
     PURPOSE: Edit data with editstr()
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 910829 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void edit_data(int row, int col, char *header, char *format, void *data)
{
  char s[80];

  void_sprintf(s,format,data);
  editstr(row,col,10,header,s,_DRAW);
  void_sscanf(s,format,data);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: void_sprintf()
     PURPOSE: My own printf() function, including Boolean type
 DESCRIPTION: -
     RETURNS: Pointer to string, in which is printed
     HISTORY: 900624 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
char *void_sprintf(char *str, char *format, void *data)
{
  char *s;
  int long_format = FALSE;

  if (!format) /* || !data) */
    return(NULL);

  s = strchr(format,'%'); /* Check for format identifier   */
  if (!s)                 /* If no identifier, return NULL */
    return(NULL);

  s = strpbrk(format,"duouxXfegEGcsB");
  if (*(s-1) == 'l')
    long_format = TRUE;

  switch (*s)
  {
    case 'd':
    case 'i':
      if (long_format)
        sprintf(str,format, *((long *) data));
      else
        sprintf(str,format, *((unsigned int *) data));
      break;
    case 'o':
    case 'u':
    case 'x':
      sprintf(str,format, *((unsigned int *) data));
      break;
    case 'X':
      if (long_format)
        sprintf(str,format,*((unsigned long *) data));
      else
        sprintf(str,format, *((unsigned int *) data));
      break;
    case 'f':
    case 'e':
    case 'g':
    case 'E':
    case 'G':
      if (long_format)
        sprintf(str,format,*((double *)data));
      else
        sprintf(str,format, *((float *) data));
      break;
    case 'c':
      sprintf(str,format,*(char *)data);
      break;
    case 's':
      sprintf(str,format,(char *) data);
      break;
    case 'B':
      if ((int *)data)
        strcpy(str,"ON");
      else
        strcpy(str,"OFF");
      break;
    default:
      break;
  }

  return(str);
}

/*#+func-------------------------------------------------------------------
    FUNCTION: void_sscanf()
     PURPOSE: Scan data from a string, typecasting the void pointer
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 900624 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void void_sscanf(char *str, char *format, void *data)
{
  char *s;
  char *dummy;
  int long_format = FALSE;

  s = strchr(format,'%'); /* Check for format identifier   */
  if (!s)                 /* If no identifier, return NULL */
    return;

  if (format == NULL)
    return;

  s = strpbrk(format,"duouxXfegEGcsB");
  if (*(s-1) == 'l')
    long_format = TRUE;

  switch (*s)
  {
    case 'd':
    case 'i':
      if (long_format)
        *(long *)data = atol(str);
      else
        *(int *)data = atoi(str);
      break;

    case 'o':
      *(unsigned int *)data = (unsigned int) strtol(str,&dummy,8);
      break;

    case 'O':
      *(long *)data = strtoul(str,&dummy,8);
      break;

    case 'u':
      *(unsigned int *)data = (unsigned int) strtoul(str,&dummy,10);
      break;

    case 'U':
      *(long *)data = strtoul(str,&dummy,10);
      break;

    case 'x':
      *(unsigned int *) data = (unsigned int) strtoul(str,&dummy,16);
      break;

    case 'X':
      *(long *) data = strtoul(str,&dummy,16);
      break;

    case 'f':
    case 'e':
    case 'g':
    case 'E':
    case 'G':
      if (long_format)
        *(double *)data = atof(str);
      else
        *(float *)data = (float) atof(str);
      break;

    case 'c':
      sscanf(str,"%c",(char *)data);
      break;

    case 's':
      strcpy((char *)data,str);
      break;

    default:
      break;
  }
}

/*#+func-------------------------------------------------------------------
    FUNCTION: disp_status()
     PURPOSE: display a message on line 23
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911106 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void disp_status(char *format, ...)
{
  va_list arguments;
  static unsigned int i = 0;
  int len;
  char s[132];

  /*------------------------------------
    Generate the string
  -------------------------------------*/
  va_start(arguments,format);
  vsprintf(s,format,arguments);
  va_end(arguments);

  /*------------------
    Trim the string
  ------------------*/
  len = strlen(s);
  for (i=len ; i<_cursvar.cols ; i++)
    s[i] = ' ';
  s[_cursvar.cols] = '\0';

  /*-----------------------------------
    Print string on bottom of display
  ------------------------------------*/
  v_prints(_cursvar.lines,1,s);

}

/*#+func---------------------------------------------------------------------
    FUNCTION: show_time()
     PURPOSE: Display time in upper right corner of display
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 920212 V0.1
--#-func-------------------------------------------------------------------*/
void show_time()
{
  char timestr[9];
  time_t     tnow;
  struct tm  *tmnow;

  /*--------------------------
  | Get and print actual time
  ---------------------------*/
  time(&tnow);
  tmnow = localtime(&tnow);
  sprintf(timestr,"%02d:%02d:%02d",tmnow->tm_hour,tmnow->tm_min,tmnow->tm_sec);
  v_prints(0,_cursvar.cols-7,timestr);
}

