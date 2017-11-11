/**************************************************************************
$Header: Fri 10-27-2000 9:30:09 pm HvA V2.01 $

DESC: Help driver for HVA_WIN
      Use GENINDEX to create the index file
HIST: 19911127 V00.2 - Generalized page break and end of topic character sequences
      19920211 V00.3 - uses LIFO stack to switch between display's   
	  20001027 V2.01 - Adapted for HVA_WIN
***************************************************************************/

/*===========================================================================

  HelpPC text files are simple ASCII files that contain control codes
  in column one.   Each file must contain a menu title in the first
  line.  The remainder of the file consists of keyed lines and help
  text.  Each line must end with a CR/LF pair (standard DOS format) and
  shouldn't be longer than 79 characters.   Tabs position the text at
  8 character tab positions.  The following is a list of keys and
  special characters (keys are found in column 1, special characters
  appear in columns 1-80):

   '@'  in column 1 indicates a file title which will appear in
        the main topic menu.  This must be the very first line
        in the file and has a maximum length of 40 characters
        (excluding the '@').
   ':'  in column 1 indicates a topic key.  Multiple keys separated
        by colons ':' can be entered on the same line.  Single spaces
        are allowed in a key, multiple spaces are compressed to single.
   '%'  in column 1 indicates a highlighted title line.
   '^'  in column 1 indicates a centered highlighted title.
   ' '  (space) in column 1 indicates normal text.
   '~'  Tilde is used to mark text as a topic link.  Use two
        tilde characters to represent an actual tilde in the data.
        A word or phrase enclosed between tilde's will become a
        topic link for the current topic.
   TAB  in column 1 starts text in column 9
   any other character in column 1 is truncated

   Limits of the genindex() and help()

   Max items in topic menu:              512
   Max topics in index:                 1800
   Max size of topic text:             16384 bytes
   Max lines of text per topic:          512
   Max topic key length:                  20
   Max file title length:                 40
   Max topic links:                      120
   No limit on text file size

=============================================================================*/

/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <conio.h>        /* For color definitions */
#include "video/v_video.h"
#include "keys.h"
#include "window.h"

/*------------------------------------------------------------------------------
                        PROTOTYPES
------------------------------------------------------------------------------*/
static void show_line(WINDOW *w, int linenr, char *s);
static void show_topic(long offset);
static int  filbuf(long offset, int flag);
static int  find_topiclink(int linenr, int n);
static int  push_subject(char *s);
static char *pop_subject(void);
static void display_stack(void);

extern SCREEN _cursvar;

/*------------------------------------------------------------------------------
                        CONSTANTS
------------------------------------------------------------------------------*/

#define KEY_LEN        25    /* Maximum lenght of a key      */
#define MAX_LINES     512    /* Max no lines in text buffer  */
#define MAXKEY        512    /* Maximum number of topics     */
#define HELP_LINE_LEN  90    /* Maximum lenght of a line     */

typedef struct    /* topic list structure         */
{
  char *key;      /* pointer to :info string      */
  long loc;       /* file offset                  */
}
TOPIC;

typedef struct    /* TOPIC_LINK structure         */
{
  char *key;      /* pointer to topic string                              */
  int  line;      /* Line of topic link.     Set to -1 for unused entries */
  int  pos;       /* Position in line        Set to -1 for unused entries */
  int  len;       /* Length of topic string  Set to -1 for unused entries */
}
TOPIC_LINK;

/*------------------------------------------------------------------------------
                        LOCAL VARIABLES
------------------------------------------------------------------------------*/
TOPIC       topic[MAXKEY];          /* topic list                   */
TOPIC_LINK  topic_link[MAXKEY];     /* topic link list              */
char        *topic_txt[MAXKEY];     /* Topic text buffer pointers   */

static FILE *fp;                    /* stream file pointer          */

static WINDOW   *wn;                /* window pointer                     */

static int  initflg = FALSE;        /* init complete flag                 */
static int  startline = 0;          /* first line of textbuffer in window */
static int  height = 0;             /* Hight of the window (no of lines)  */
static int  endline   = 0;          /* Last line on the screen            */
static int  link_selected = 0;      /* Selected topic link                */
static int  no_topics;
static char current_subject[KEY_LEN];
static char *subject = NULL;
static char *topic_key_buffer = NULL;
static char *topic_txt_buffer = NULL;
static char *link_key_buffer  = NULL;

#define MAX_TOS 25                   /* Dimension of stack        */
static char stack[MAX_TOS][KEY_LEN]; /* stack of MAX_TOS keys     */
static int tos = 0;                  /* top of stack              */

/*---------------------------------
  Subjects to push on the stack:

  startline,
  endline,
  link_selected,
  no_topics

---------------------------------*/



/*------------------------------------------------------------------------------
                        GLOBAL VARIABLES
------------------------------------------------------------------------------*/
char *default_subject = NULL;

/*==============================================================================
                        START OF FUNCTIONS
==============================================================================*/

/*#+func-----------------------------------------------------------------------
    FUNCTION: push_subject()
     PURPOSE: push an item on the stack
 DESCRIPTION: LIFO buffer
     RETURNS: New top of stack
     HISTORY: 920211 V0.1
--#-func----------------------------------------------------------------------*/
static int push_subject(char *s)
{
  /*---------------------------------------------
    If stack is not full, push data on the stack
  -----------------------------------------------*/
  if (tos < MAX_TOS)
  {
    strcpy(stack[tos],s);
    tos++;
  }
  return(tos);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: pop_subject()
     PURPOSE: pop an item from the stack
 DESCRIPTION: LIFO buffer
     RETURNS: item number
     HISTORY: 920211 V0.1
--#-func----------------------------------------------------------------------*/
static char *pop_subject()
{
  tos--;
  if (tos < 0)
  {
    /*----------------
      Stack is empty
    -----------------*/
    tos++;
    return(NULL);
  }
  else
  {
    /*---------------------------------
      return contents of top of stack
    ----------------------------------*/
    return(stack[tos]);
  }
}

/*#+func-------------------------------------------------------------------
   FUNCTION: init_help(char *help_file_name)
    PURPOSE: Will initialize help functions and parameters
 DESRIPTION: Initializing will be done by help() function and
             includes allocation of memory and building the
             topic index table.
    RETURNS: TRUE if successfull, FALSE if not
    VERSION: 901115 V0.2
--#-func-------------------------------------------------------------------*/
int init_help(char *help_file_name)
{
  int i;
  char tmpname[30];                   /* temporary filename */
  char *ptr;
  char line[HELP_LINE_LEN];           /* line buffer                     */

  if (initflg)
  {
    wn_error("Help system for %s already initialized",*help_file_name);
    return(initflg);
  }

  /*-----------------------------------------------
  | allocate memory for strings in topic key index
  | and allocate pointers to it.
  ------------------------------------------------*/
  topic_key_buffer = calloc(MAXKEY,KEY_LEN);
  if (!topic_key_buffer)
  {
    wn_error("ERROR in allocating TOPIC_KEY_BUFFER");
    return(initflg);
  }
  for(i=0; i<MAXKEY; i++)
    topic[i].key = topic_key_buffer + i*KEY_LEN;

  /*--------------------------------------------------
    Call exit_help when exiting from the main program
  ----------------------------------------------------*/
  atexit(exit_help);

  /*-----------------------------------------------------
  | Build the topic index table will be build by using
  | file 'subject.ndx'. After this the helpfile
  | 'subject.hlp' will be opened and left open to use
  -------------------------------------------------------*/
  strcpy(tmpname,help_file_name);   /* form filename from root */
  strcat(tmpname,".ndx");
  fp = fopen(tmpname,"r");
  if (!fp)                          /* index file cant be found ! */
  {
    wn_error("Can't find indexfile %s for help functions",tmpname);
    return(initflg);
  }
  i = 0;                                /* read and process key file */
  while(fgets(line,HELP_LINE_LEN,fp))   /* build the topic index table */
  {
    ptr = strchr(line,'\n');        /* Find '\n'        */
    if (ptr) *ptr = '\0';           /* And make it '\0' */
    strcpy(topic[i].key, line);

    #ifdef DONT_SKIP_THIS

    if(line[0] != ':')              /* ignore all but subjects */
      continue;

    #endif

    fgets(line,HELP_LINE_LEN,fp);
    topic[i].loc = atol(line);
    i++;
  }
  fclose(fp);                       /* close index file */
  strcpy(tmpname, help_file_name);  /* form help file name from root */
  strcat(tmpname, ".hlp");          /* open and leave open ! */
  fp = fopen(tmpname, "r");
  if (!fp)
    return(initflg);

  initflg = TRUE;
  return(initflg);

}

/*#+func-------------------------------------------------------------------
   FUNCTION: help()
    PURPOSE: Show help text about 'subject'
 DESRIPTION: When the first character of 'subject' is a ':' then
             the help text will be displayed.
    RETURNS: TRUE if successfull, FALSE if not
    VERSION: 901114 V0.1
--#-func-------------------------------------------------------------------*/
int help(char *in_subject)
{
  int i;                                  /* scratch integers */
  int  topic_found = FALSE;

  if (!initflg)
  {
    wn_error("HELP system not initialized");
    return(FALSE);
  }

  wn_log_indent(2);
  wn_log("HELP() START ****\n");

  /*------------------------------------------
    Allocate memory for topic text buffer
    and assign pointer to it.
  ------------------------------------------*/
  topic_txt_buffer = calloc(MAX_LINES,HELP_LINE_LEN);
  if (!topic_txt_buffer)
  {
    wn_error("ERROR in allocating TOPIC_TXT_BUFFER");
    return(FALSE);
  }
  for(i=0; i<MAX_LINES; i++) /* Assign pointers to each text line*/
    topic_txt[i] =  topic_txt_buffer + i*HELP_LINE_LEN;

  /*--------------------------------------------
  | allocate memory for topic link key buffer
  | and assign pointer to it.
  --------------------------------------------*/
  link_key_buffer = calloc(MAXKEY,KEY_LEN);
  if (!link_key_buffer)
  {
    wn_error("ERROR in allocating memory for LINK_KEY_BUFFER");
    return(FALSE);
  }
  for(i=0; i<MAXKEY; i++)
    topic_link[i].key =  link_key_buffer + i*KEY_LEN;

  /*--------------------------------
    Remove leading ':' from subject
  ----------------------------------*/
  if (in_subject && in_subject[0] == ':')
  {
    in_subject[0] = ' ';
    trimstr(in_subject);
  }

  /*--------------------------------------------------------
    If no in_subject given, set subject to default subject
  ---------------------------------------------------------*/
  if (in_subject == NULL || in_subject[0] == '\0')
  {
    if (default_subject)
      in_subject = default_subject;
    else {
      wn_error("No default subject given");
      return(FALSE);
    }
  }

  strcpy(current_subject,in_subject);

  subject = in_subject;

  wn = wn_open(_SINGLE_LINE,0,0,_cursvar.cols,_cursvar.lines,
               _help_text,_help_border);
  if (!wn)
    return(FALSE);

  height = (wn->y2 - wn->y1) + 1;

  while (TRUE)
  {
    /*----------------------------
      Check if a subject is given
    ------------------------------*/
    if (subject == NULL)
      break;

    /*-----------------------------
      Search for key in topic list
    -------------------------------*/
    i=0;
    topic_found = FALSE;
    while(i < MAXKEY)
    {
      trimstr(subject);
      trimstr(topic[i].key);
      if(!strcmp(subject,topic[i].key))
      {
        topic_found = TRUE;
        break;
      }
      else i++;
    }

    /*---------------------------------------
      If key was found, display text of topic
    -----------------------------------------*/
    if (topic_found)
    {
      show_topic(topic[i].loc);
    }
    else
    {
      /*---------------------------------------
        Show that topic text is not available
      ----------------------------------------*/
      wn_error("Sorry... No help available on subject %s",subject);

      /*-------------------------------------------------
        Retrieve last subject from stack. If no subject
        is available, then stop this loop
      -------------------------------------------------*/
      subject = pop_subject();
      if (subject)
      {
        strcpy(current_subject,subject);
        subject = current_subject;
      }
      else      /* No subjects available any more */
        break;
    }

  }

  /*--------------------
    Close help window
  ---------------------*/
  wn_close(wn);

  /*-------------------------------------
    Free allocated memory
  ---------------------------------------*/
  free(topic_txt_buffer);
  free(link_key_buffer);

  wn_log_indent(-2);
  wn_log("HELP() END *****\n");

  return(FALSE);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: find_topiclink()
     PURPOSE: Find the link in the topic link list
      SYNTAX: static int find_topiclink(int linenr, int n);
 DESCRIPTION: int n : return the index of the n'th topic in this line.
     RETURNS: the index of the n'th topic in the line.
              If no link is found, -1 will be returned.
     HISTORY: 920205 V0.1
              29-Sep-1992 20:52:42 V0.2 -
--#-func----------------------------------------------------------------------*/
static int find_topiclink(int linenr, int n)
{
  int i;
  int line;

  /*---------------------------------------
    Find the 1st topic in this line
  ---------------------------------------*/
  i = -1;               /* Reset index */
  while (TRUE) {
    i++;
    line = topic_link[i].line;
    if (line > linenr)
      return(-1);      /* We are past the wanted line in the list    */
    if (line < 0)
      return(-1);      /* We ran out of the actual number of topics  */
    else if (i >= MAXKEY)
      return(-1);      /* We ran out of the maximum number of topics */
    else if (line == linenr) {
      if (n <= 0)
        return(i);     /* We found the n'th topic on the line        */
      else
        n--;
    }
  }
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: check_screen()
     PURPOSE: Check variables for drawing the help screen
      SYNTAX: static int check_screen(int *sl, int *el, int no_lines);
              int *sl      : pointer to startline
              int *el      : pointner to endline
              int no_lines : number of help lines for the current topic
 DESCRIPTION:
     RETURNS: TRUE if screen has to be redrawn,
              FALSE if not
     HISTORY: 30-Sep-1992 22:19:39 V0.1
--#-func----------------------------------------------------------------------*/
static int check_screen(int *sl, int *el, int no_lines)
{
  int redraw = FALSE;

  /*-------------------------------------------------------------
    Check on valid start line:
    (1) The startline must be at least 0 and
        at most MAX_LINES - window height
    (2) If there are less textlines then the window height,
        the startline number is 0.
  ------------------------------------------------------------*/
  if (*sl < 0) {
    redraw = TRUE;
    *sl = 0;
  }
  if (*sl > (MAX_LINES-height+1)) {
    redraw = TRUE;
    *sl = MAX_LINES-height+1;
  }
  if (no_lines > height) {
    redraw = TRUE;
    *sl = min(*sl,no_lines-height+11);
  }
  if (no_lines < height) {
    redraw = TRUE;
    *sl = 0;
  }

  /*---------------------------------------------------------
    Check on valid end line:
    (1) The last line in the window is at
        most the number of the startline + the height of
        the window.
    (2) If there are less textlines then the window height,
        the last textline is 'no_lines
  ----------------------------------------------------------*/
  if (no_lines > height) {
    *el = *sl + height - 1;
    redraw = TRUE;
  }
  if (no_lines < height) {
    *el = no_lines - 1;
    redraw = TRUE;
  }

  return(redraw);

}


/*#+func-----------------------------------------------------------------------
    FUNCTION: prev_link()
     PURPOSE: Find previous link
      SYNTAX: int prev_link(int curlink, int maxlink);
              int curlink : current link
              int maxlink : maximum number of links
     RETURNS: previous link found
 DESCRIPTION:
     HISTORY: 06-Oct-1992 21:26:45 V0.1
--#-func----------------------------------------------------------------------*/
int prev_link(int curlink, int maxlink)
{
  maxlink=0;

  if (curlink > 0)
    return(curlink-1);
  else
    return(curlink);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: next_link()
     PURPOSE: Find next link
      SYNTAX: int next_link(int curlink, int maxlink);
              int curlink : current link
              int maxlink : maximum number of links
     RETURNS: next link found
     HISTORY: 06-Oct-1992 21:26:45 V
--#-func----------------------------------------------------------------------*/
int next_link(int curlink, int maxlink)
{
  if (curlink < (maxlink-1))
    return(curlink+1);
  else
    return(curlink);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: redraw_helpscreen()
     PURPOSE: redraw complete help screen
      SYNTAX: void redraw_helpscreen(WINDOW *w, int startline, int no_lines);
              WINDOW *w    : Pointer to opened help window
              int startline: first textbuffer line in window
              int no_lines:  number of text lines
 DESCRIPTION: -
     RETURNS: nothing
     HISTORY: 07-Oct-1992 19:24:53 V
--#-func----------------------------------------------------------------------*/
void redraw_helpscreen(WINDOW *w, int startline, int no_lines)
{
  int j;

  for(j=0; j<no_lines && j < height; j++) {
    show_line(w,startline+j,topic_txt[startline+j]);
  }
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: first_link_on_screen()
     PURPOSE: Find first link which fits on the screen
      SYNTAX: int first_link_on_screen(int sl, int el, int n);
              int sl: first textbuffer line in window.
              int el: last textbuffer line in window
              int ll: line of textbuffer with first link in it
              int n : number of topics
 DESCRIPTION:
     RETURNS: The index of the first link which fits on the screen,
              -1 if no link on the screen visible.
     HISTORY: 07-Oct-1992 20:42:24 V0.1
--#-func----------------------------------------------------------------------*/
int first_link_on_screen(int sl, int el, int *ll, int n)
{
  int i;

  i = 0;
  *ll = topic_link[i].line;

  while (i < n && *ll < sl) {
    *ll = topic_link[i].line;
    if (*ll >= sl && *ll <= el)
      return(i);
    if (*ll >= el)
      return(-1);
    i++;
  }
  return(i-1);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: last_link_on_screen()
     PURPOSE: Find last link which fits on the screen
      SYNTAX: int last_link_on_screen(int sl, int el, int n);
              int sl: number of first textline on the screen
              int el: number of last textline on the screen,
              int *ll: linenumber of textbuffer with link found
              int n : number of topics
 DESCRIPTION: -
     RETURNS: The index of the last link which fits on the screen,
              -1 if no link on the screen visible.
     HISTORY: 07-Oct-1992 20:42:24 V0.1
--#-func----------------------------------------------------------------------*/
int last_link_on_screen(int sl, int el, int *ll, int n)
{
  int i;

  i = n-1;
  *ll = topic_link[i].line;

  while (i >= 0 && *ll >= 0) {
    *ll = topic_link[i].line;
    if (*ll <= el && *ll >= sl)         /* Break if within screen      */
      return(i);
    if (*ll < sl)                       /* Break if past top of screen */
      return(-1);
    i--;
  }
  return(-1);

}
/*#+func-------------------------------------------------------------------
   FUNCTION: show_topic(long offset)
    PURPOSE: Show text of current topic
 DESRIPTION: The text of the topic will be read and displayed. As a start,
             only the first page will be read in and displayed, then the
             rest of the text. The links will have a special color, and th
             active link will be highlighted.
             The following keys will be handeled:
               ARROW_UP,
               ARROW_DOWN,
               PAGE_UP,
               PAGE_DOWN,
               ARRROW_LEFT,
               ARROW_RIGHT,
               ENTER,
               ESC.

             Reaction on keys:

             ARROW_RIGHT: go to next link on the page. If the link is not
               on the page, then rearrange the page, so the next link is
               on the top line of the page.

             ARROW_LEFT: go to previous link on the page. If the link is not
               on the page, then rearrange the page, so the link is on the
               startline of the page.

             PAGE_UP: Go one screen of text back. The selected link will be
               the last link on the screen.

             PAGE_DOWN: Go one screen of text ahead. The selected link will
               be the first link on the screen.

             ARROW_UP: Scroll the page one line down (back in text). If the
               selected link disappears from the screen, the new selected link
               will be the last link on the screen.

             ARROW_DOWN: Scroll the page one line up (forward in text). If the
               selected link 'disappears' from the screen, the new selected
               link will be the first link on the screen.

             ENTER: Go to selected link and display its text

             ESC: return to previous link or exit help if no previous link
               exists.

    RETURNS: Nothing
    VERSION: 901114 V0.1
--#-func-------------------------------------------------------------------*/
static void show_topic(long offset)
{
  /*------------
    Variables
  ------------*/
  int no_lines;                 /* No of lines of topic        */
  unsigned int choise;                   /* User input                  */
  int flag = 0;                 /* Flag for reading topic text */
  int old_link = 0;
  int old_linkline;
  int linkline;

  /*-------------------
    Initializations
  -------------------*/
  startline = 0;

  link_selected = 0;
  old_linkline  = startline;


  /*------------------------------------
    Get first screen of text from topic
  ------------------------------------*/
  no_lines = filbuf(offset, flag++);

  /*-------------------
    Show first screen
  --------------------*/
  wn_clear(wn);
  redraw_helpscreen(wn,startline,no_lines);

  /*----------------
    Show the text
  ----------------*/
  while(TRUE)
  {
    endline      = startline+height-1;
    linkline     = topic_link[link_selected].line;
    old_linkline = topic_link[old_link].line;

    /*----------------------------------------
      Hide old selected link and show new one
    -----------------------------------------*/
    /* link is on the same line */
    if ((link_selected != old_link) && old_linkline == linkline)
      show_line(wn,linkline,topic_txt[linkline]);
    else {
      if (old_linkline >= startline && old_linkline <= endline)
        show_line(wn,old_linkline,topic_txt[old_linkline]);
      if (linkline >= startline && linkline <= endline)
        show_line(wn,linkline,topic_txt[linkline]);
    }

    /*------------------------------------
      Read complete text for this topic
      (only once, controlled by 'flag'
    ------------------------------------*/
    no_lines = filbuf(offset, flag++);

    /*----------------------------------------
      Wait for valid keypress and react on it
    -----------------------------------------*/
    choise = waitkey();
    wn_log("startline = %d   endline = %d   linkline = %d ",startline,endline,linkline);
    wn_log("height = %d no_lines = %d ",height,no_lines);
    wn_log("key pressed: %x\n",choise);
    switch (choise)
    {
      case PAGE_UP:
        /*---------------------------------------
          Scroll the screen 1 page up
        ---------------------------------------*/
        startline -= height;
        endline   -= height;
        check_screen(&startline,&endline,no_lines);
        redraw_helpscreen(wn,startline,no_lines);
        if (linkline > endline) {
          link_selected = last_link_on_screen(startline,endline,&linkline,no_topics);
          if (linkline >= 0)
            show_line(wn,linkline,topic_txt[linkline]);
        }
        break;

      case PAGE_DOWN:
        /*---------------------------------------
          Scroll the screen 1 page down
        ---------------------------------------*/
        startline += height;
        endline   += height;
        check_screen(&startline,&endline,no_lines);
        redraw_helpscreen(wn,startline,no_lines);
        if (linkline < startline) {
          link_selected = first_link_on_screen(startline,endline,&linkline,no_topics);
          if (linkline >= 0)
            show_line(wn,linkline,topic_txt[linkline]);
        }
        break;

      case ARROW_UP:
        /*---------------------------------------
          Scroll the screen 1 line down
        ---------------------------------------*/
        if (startline > 0) {
          startline--;
          endline--;
          wn_scroll(wn,-1);
          show_line(wn,startline,topic_txt[startline]);
        }
        /*---------------------------------------
          Check if selected link is not
          outside the screen. If so, search
          for new link and show this line
        ---------------------------------------*/
        if (linkline >= endline) {
          old_link = link_selected;       /* Save old link */
          while (linkline >= endline && link_selected > 0) {
            link_selected = prev_link(link_selected,no_topics);
          }
          show_line(wn,linkline,topic_txt[linkline]);
        }
        break;

      case ARROW_DOWN:
        /*---------------------------------------
          Scroll the screen 1 line up
        ---------------------------------------*/
        if (endline < no_lines) {
          startline++;
          endline++;
          wn_scroll(wn,1);
          show_line(wn,endline,topic_txt[endline]);
        }
        if (linkline < startline) {
          link_selected = first_link_on_screen(startline,endline,&linkline,no_topics);
          if (linkline >= 0)
            show_line(wn,linkline,topic_txt[linkline]);
        }
        break;

      case ARROW_LEFT:
        /*---------------------------------------
          Goto previous link
        ---------------------------------------*/
        old_link = link_selected;
        link_selected = prev_link(link_selected,no_topics);
        linkline = topic_link[link_selected].line;
        if (linkline < startline) {
          startline = linkline;
          redraw_helpscreen(wn,startline,no_lines);
        }
        break;

      case HT:
      case ARROW_RIGHT:
        /*---------------------------------------
          Goto next link
        ---------------------------------------*/
        old_link = link_selected;
        link_selected = next_link(link_selected,no_topics);
        linkline = topic_link[link_selected].line;
        if (linkline > endline) {
          endline = linkline;
          startline = endline - height + 1;
          redraw_helpscreen(wn,startline,no_lines);
        }
        break;

      /*----------------------------------
        Get topic key if ENTER is pressed
      -----------------------------------*/
      case ENTER:
        if (no_topics > 0)
        {
          push_subject(subject);
          strcpy(current_subject,topic_link[link_selected].key);
          subject = current_subject;
        }
        return;
        break;

      /*---------------
        Quit from help
      -----------------*/
      case ESC_KEY:
        subject = pop_subject();
        if (subject)
        {
          strcpy(current_subject,subject);
          subject = current_subject;
        }
        return;
        break;
    }
  }
}


/*#+func-------------------------------------------------------------------
   FUNCTION: filbuf()
    PURPOSE: Fill text buffer with text from helpfile
 DESRIPTION: If flag = 0 then read only first screen of topic
             If flag = 1 then read complete topic
             If flag > 1 then return immediately
    RETURNS: Number of lines on the page
    HISTORY: 901114 V0.1
             920205 V0.2 - Added flag to speed up reading
             28-Sep-1992 21:09:48 V0.3 - Changed algoritme to get tokens
--#-func-------------------------------------------------------------------*/
static int filbuf(long offset, int flag)
{
  int    i,j;
  static int no_lines;
  char   line[HELP_LINE_LEN];  /* line buffer       */
  char   token[80];            /* Pointer to token  */
  int    pos,len;

  /*-------------------------------
    If flag == 1 Return immediately.
    The complete text of the topic
    has been read already
  ---------------------------------*/
  if (flag > 1)
    return(no_lines);

  /*----------------------
    Initialize variables
  -----------------------*/
  no_lines=0;
  no_topics = 0;          /* Number of links in the list */

  /*-----------------------------------
    Position file pointer on topic key
  -----------------------------------*/
  fseek(fp, offset, 0);       /* position file               */

  /*---------------------------------
    Read and process the topic text
  ----------------------------------*/
  do
  {
    fgets(line, 132, fp);     /* fetch a line              */

    /*-----------------------
      Check for end of file
    ------------------------*/
    if (!strcmp("*END*\n",line))
      break;

    /*---------------------------------
      Check for start of next topic
    -----------------------------------*/
    if (line[0] == ':')
       break;

    /*----------------------
      Check for file title
    -----------------------*/
    if (line[0] == '@')
      break;

    /*----------------------
      Strip LF from string
    -----------------------*/
    len = strlen(line);
    line[len-1] = '\0';

    strcpy(topic_txt[no_lines],line);

    /*----------------------------------------------------
      Check the line for subtopic links.
      If one is found, add this to the subtopic link list
    -----------------------------------------------------*/
    i = 0;
    while (strchr(&line[i],'~')) {
      /*---------------------------------------
        Extract token from the line
      ---------------------------------------*/
      while (line[i] && line[i] != '~') i++;     /* Find first '~' */
      pos = i;                                   /* Save position  */

      j = 0;
      i++;                                       /* Point after first '~' */
      while (line[i] && line[i] != '~') token[j++] = line[i++]; /* Extr. token */
      token[j] = '\0';                           /* End of string         */
      i++;                                       /* Point after 2nd '~'   */

      /*---------------------------------------
        Set the topic variables
      ---------------------------------------*/
      strcpy(topic_link[no_topics].key,token);
      topic_link[no_topics].line = no_lines;
      topic_link[no_topics].pos  = pos;
      len = strlen(token);
      topic_link[no_topics].len  = len;
      topic_txt[no_lines][pos]       = ' ';   /* 0x10; */       /* Mark begin of link */
      topic_txt[no_lines][pos+len+1] = ' ';   /* 0x11; */      /* Mark end of link   */
      no_topics++;
    }

    no_lines++;

    /*----------------------------------------------
      If flag is zero, then check if first screen
      is completely read.
    ------------------------------------------------*/
    if (flag == 0 && no_lines >= height)
      break;

  }
  while (TRUE);

  /*------------------------------------
    Clear remainder of topic text lines
  -------------------------------------*/
  for (j=no_lines ; j < MAX_LINES ; j++)
    topic_txt[j][0] = '\0';

  /*------------------------------------
    Clear remainder of topic link list
  -------------------------------------*/
  for (j=no_topics ; j< MAXKEY ; j++)
  {
    topic_link[j].key[0] = '\0';
    topic_link[j].line   = -1;
    topic_link[j].pos    = -1;
    topic_link[j].len    = -1;
  }

  return(no_lines);
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: show_line()
     PURPOSE: Show a line on a help page
      SYNTAX: static void show_line(WINDOW *w, int linenr, char *s);
 DESCRIPTION: WINDOW *w : pointer to opened help window
              int linenr: linenumber of textbuffer to print in window
              char *s   : text to show
      GLOBAL: int startline = first line of textbuffer in the window
     RETURNS: Nothing
     HISTORY: 14-Oct-1993 V0.2 - Now uses a window instead of direct screen...
--#-func----------------------------------------------------------------------*/
static void show_line(WINDOW *w, int linenr, char *s)
{
  int  link;
  int  i;
  int  w_line;        /* Window line number */

  /*-------------------
    Initializations
  -------------------*/
  w_line = linenr - startline;

  /*-------------------------------
    Clear the line in the window
  -------------------------------*/
  wn_fill(w,w_line,0,w->xsize-2,1,' ',_help_text);

  switch(s[0]) {
    case '^':       /* Centered, highlighted line */
      wn_locate(w,w_line,5);  /* Not centered yet */
      wn_printf(w,"%s",s+1);
      return;
      break;
    case '%':       /* Complete highlighted line */
      wn_setattrib(w,w_line,0,w->xsize-2,1,_help_bold);
      wn_locate(w,w_line,0);
      wn_printf(w,"%s",s+1);
      return;
      break;
    default:
      if (*s) {
        wn_locate(w,w_line,0);
        wn_printf(w,"%s",s+1);
      }
      break;
  }

  /*------------------------------------------
    Check if this line has a topic link in it
  --------------------------------------------*/
  i = 0;
  link = find_topiclink(linenr,i);
  while (link >= 0) {
    /*----------------------------------
      Set the attributes of a link key
    -----------------------------------*/
    if (link == link_selected)
      wn_setattrib(w,w_line,topic_link[link].pos,topic_link[link].len,1,
                _help_linkselect);
    /*------------------------------
      Highlight the selected link
    ------------------------------*/
    else
      wn_setattrib(w,w_line,topic_link[link].pos,topic_link[link].len,1,
                _help_topiclink);

    /*---------------------------------
      Find the next topic on this line
    ----------------------------------*/
    i++;
    link = find_topiclink(linenr,i);
  }
}

/*#+func-----------------------------------------------------------------------
    FUNCTION: exit_help()
     PURPOSE: exit from windowed help function
 DESCRIPTION: free allocated memory for topic key buffer
     RETURNS: nothing
     HISTORY: 920107 V0.1 - Initial version
--#-func----------------------------------------------------------------------*/
void exit_help()
{
  free(topic_key_buffer);
}
