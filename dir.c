/**************************************************************************
$Header: dir.c Fri 10-27-2000 8:49:52 pm HvA V1.00 $

DESC: Show directory and pick a filename - HVA_WIN 
HIST: 19911120 V1.00 
	  20001027 V2.01 - Adapting for DJGPP
***************************************************************************/

/*---------------------------------------------------------------------------
                        HEADER FILES
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <direct.h>
#include <unistd.h>		/*+ For getcwd() +*/
#include "keys.h"
#include "window.h"
#include "fortify.h"

/*---------------------------------------------------------------------------
                        CONSTANTS
---------------------------------------------------------------------------*/
#define MAX_FILES       100     /*+ Maximum number of files in a directory +*/
#define CELWIDTH         14     /*+ Width of 1 cell in the window          +*/
#define NO_HOR_CELLS      4     /*+ Number of names in 1 row               +*/
#define NO_VERT_CELLS     9     /*+ Number of names in 1 column            +*/
#define MAX_NAME_LENGTH  13     /*+ Maximum length of a filename           +*/

/*---------------------------------------------------------------------------
                        LOCAL VARIABLES
---------------------------------------------------------------------------*/
static int window_width  = 0;
static int window_height = 0;
static int no_cells      = 0;
static int row           = 0;
static int col           = 0;
static int filenr        = 0;
static int no_files      = 0;
static int first_file    = 0;
static int parent_dir    = FALSE;

#define NAMELEN 132

static char original_dir[NAMELEN];  /* Starting directory               */
static char full_path[NAMELEN];     /* Full path to choosen file        */
static char *file[MAX_FILES];       /* Array of pointers to filenames   */
static WINDOW *dir_window;

/*---------------------------------------------------------------------------
                        PROTOTYPES
---------------------------------------------------------------------------*/
static int  check_filenr(int nr);
static void get_files(char *mask);
static void display_dir(void);
static void calc_row_col(int nr);
static int  calc_first_file(int filenr);

/*===========================================================================
                        START OF FUNCTIONS
===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
show-dir() - Show a directory. Asks for a filemask and then shows the
directory contents, matching the mask. If the window for the mask was
escaped with ESC_KEY, then don't show the direcory

HISTORY: 911211 V0.1 - Initial version
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void show_dir(void)
{
  char filemask[40];
  int ret;

  strcpy(filemask,"*.*");
  ret = editstr(action_row+2,action_col+2,30,"File mask",filemask,_DRAW);
  if (ret != ESC)
    get_dir(filemask,filemask);
}

/*#+func--------------------------------------------------------------
    FUNCTION: change_dir()
     PURPOSE: change directory
 DESCRIPTION: shows path of current directory, and will ask for a new
              path. If this edit window is escaped by 'ESC' nothing
              will be done, otherwise we change the current directory
     RETURNS: nothing
     HISTORY: 911112 V0.1 - Initial version
--#-func-------------------------------------------------------------*/
void change_dir()
{
  char path[81];
  int  ret;

  if (getcwd(path,80) != NULL)
  {
    ret = editstr(action_row+2,action_col+2,30,"Enter directory path",path,_DRAW);
    if (ret != ESC)
      chdir(path);
  }
  else
    wn_error("Error in change_dir()");
}

/*#+func---------------------------------------------------------------------
    FUNCTION: calc_first_file()
     PURPOSE: Calculate number of first file in the window
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911213 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static int calc_first_file(int nr)
{
  if (nr >= first_file + no_cells)
  {
    first_file += NO_HOR_CELLS;
    return(first_file);
  }

  if (nr < first_file)
  {
    first_file -= NO_HOR_CELLS;
    return(first_file);
  }
  return(nr);		/*+ Default return +*/

}

/*#+func---------------------------------------------------------------------
    FUNCTION: check_filenr()
     PURPOSE: Check if filenr within valid range
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911120 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static int check_filenr(int nr)
{
  if (nr < 0)
    nr = 0;
  if (nr >= no_files)
    nr = no_files-1;
  return(nr);
}

/*#+func---------------------------------------------------------------------
    FUNCTION: calc_row_col()
     PURPOSE: Calculate row and column of a cell number in the window
 DESCRIPTION: -
     RETURNS: nothing.
              Global variables 'row' and 'column' are affected.
     HISTORY: 911120 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static void calc_row_col(int cell_nr)
{
  int i;

  /*-----------------------------
    Calculate horizontal offset
  ------------------------------*/
  i   = cell_nr % NO_HOR_CELLS;
  col = dir_window->x1+1 + (i * CELWIDTH);

  /*----------------------------
    Calculate vertical offset
  ----------------------------*/
  i = ((int) cell_nr / NO_HOR_CELLS);
  if (i >= NO_VERT_CELLS)
    i = NO_VERT_CELLS - 1;
  row = dir_window->y1 + i;
}

/*#+func---------------------------------------------------------------------
    FUNCTION: display_dir()
     PURPOSE: Display directory in a window
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911213 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static void display_dir()
{
  int j;
  int cell_nr;

  /*-----------------
    Initializations
  ------------------*/
  filenr = check_filenr(filenr);
  first_file = calc_first_file(filenr);
  j = first_file;
  cell_nr = 0;

  /*--------------------------
    Show filenames in window
  ---------------------------*/
  wn_locate(dir_window,0,0);
  while (cell_nr < no_cells && cell_nr < no_files) {
    if (!file[j][0])   /* Check if filename not empty */
      break;
    calc_row_col(cell_nr);
    hotstring(row,col,-1,file[j],_menu_att);
    cell_nr++;
    j++;
  }

  /*-----------------------------------
    Clear remainder of menu if needed
  ------------------------------------*/
  while (cell_nr < no_cells)
  {
    calc_row_col(cell_nr);
    hotstring(row,col,-1,"             ",_menu_att);
    cell_nr++;
  }

  /*----------------------------------
    Highlight filename under cursor
  ----------------------------------*/
  cell_nr = filenr - first_file;
  calc_row_col(cell_nr);
  hotstring(row,col,-1,file[filenr],_menu_highlight);
}

/*#+func---------------------------------------------------------------------
    FUNCTION: get_files()
     PURPOSE: get filenames and subdirs of current directory
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 911213 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
static void get_files(char *mask)
{
  struct find_t filedata;
  int i,j;
  char dir[81];

  getcwd(dir,80);

  /*--------------------------
    Make all filename empty
  --------------------------*/
  for (i=0 ; i<MAX_FILES ; i++)
    file[i][0] = '\0';

  window_width  = NO_HOR_CELLS * CELWIDTH;
  window_height = NO_VERT_CELLS;
  no_cells      = NO_HOR_CELLS * NO_VERT_CELLS;

  /*-----------------------------
    Get all matching file names
  ------------------------------*/
  no_files = 0;
  i = 0;
  if (_dos_findfirst(mask,_A_NORMAL,&filedata) == 0)
  {
    do
    {
      strcpy(file[i],filedata.name);
      strlwr(file[i]);
      j = strlen(file[i]);
      strncat(file[i],"            ",12-j);
      i++;
      no_files++;
    }
    while (_dos_findnext(&filedata) == 0 && no_files < MAX_FILES);
  }

  /*------------------------------------------
    Add subdirectories. Now use "*.*" as mask
  -------------------------------------------*/
  parent_dir = FALSE;
  if (_dos_findfirst("*.*",_A_SUBDIR,&filedata) == 0)
  {
    do
    {
      if (filedata.attrib & 0x10)       /* If subdirectory      */
      {
        if (filedata.name[0] == '.' && filedata.name[1] == '.')
          parent_dir = TRUE;
        if (filedata.name[0] != '.')   /* Skip current and parent directories */
        {
          strcpy(file[i],filedata.name);
          j = strlen(file[i]);
          strncat(file[i],"\\           ",12-j);
          i++;
          no_files++;
        }
      }
    }
    while (_dos_findnext(&filedata) == 0 && no_files < MAX_FILES);
  }
  if (parent_dir)
  {
    strncat(file[i++],"..\\           ",12);
    no_files++;
  }

}

/*#+func-------------------------------------------------------------------
    FUNCTION: get_dir()
     PURPOSE: Show directory in a window and pick a file
 DESCRIPTION: Searches all files in the current directory with
              the mask given, and show them in a window like TC.
              The name of the file picked will be copied in the parameter
              'filename'
     RETURNS: NULL if this function is escaped by <ESC>
              pointer to full path and filename if <ENTER> was used
     HISTORY: 911111 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
char *get_dir(char *mask, char *filename)
{
  int i;
  unsigned int  key = ESC_KEY;
  char *backslash;

  /*--------------------------------
    Allocate memory for file names
  ---------------------------------*/
  for (i=0 ; i<MAX_FILES ; i++)
    file[i] = (char *) malloc(MAX_NAME_LENGTH);

  /*------------------------
    Get current directory
  ------------------------*/
  getcwd(original_dir,80);

  get_files(mask);

  /*------------------------
    Open directory window
  ------------------------*/
  dir_window = wn_open(_SINGLE_LINE, action_row+2, action_col+2,
    window_width+2, window_height+2, _menu_att, _border_att);
  wn_title(dir_window,mask);

  /*--------------------------
    Initialize variables
  ---------------------------*/
  filenr = 0;
  first_file = 0;

  while (TRUE)
  {
    /*-----------------------------
      Display filenames in window
    ------------------------------*/
    display_dir();

    key = waitkey();
    switch(key)
    {
       case ARROW_RIGHT:
         filenr++;
         break;

       case ARROW_LEFT:
         filenr--;
         break;

       case ARROW_DOWN:
         filenr += NO_HOR_CELLS;
         break;

       case ARROW_UP:
         filenr -= NO_HOR_CELLS;
         break;

       case ESC_KEY:
         filename[0] = '\0';
         /*------------------------
           Free allocated memory
         ------------------------*/
         for (i=0 ; i<MAX_FILES ; i++)
           free(file[i]);
         chdir(original_dir);     /* Go back to original directory */
         dir_window = wn_close(dir_window);
         return(NULL);
         break;

       case ENTER:
         /*---------------------------------------------------
           Check if directory selected. If so, change to this
           directory, get filenames and show directory
         -----------------------------------------------------*/
         backslash = strchr(file[filenr],'\\');
         if (backslash)
         {
           *backslash = '\0';
           if (chdir(file[filenr]) != -1)
           {
             get_files(mask);
             first_file = 0;
             filenr = 0;
             display_dir();
           }
         }
         else
         {
           getcwd(full_path,80);
           strcat(full_path,"\\");
           /*----------------------------
             Check if filename selected
           ----------------------------*/
           if (isalnum(file[filenr][0]))
           {
             strcpy(filename,file[filenr]);
             strcat(full_path,filename);
           }

           /*------------------------
             Free allocated memory
           ------------------------*/
           for (i=0 ; i<MAX_FILES ; i++)
             free(file[i]);
           chdir(original_dir);     /* Go back to original directory */
           dir_window = wn_close(dir_window);
           return(full_path);
         }
         break;

      default:
        break;
    } /* end of switch() */
  } /* end of while() */

}
