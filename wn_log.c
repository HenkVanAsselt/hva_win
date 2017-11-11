/**************************************************************************
$Header: wn_log.c Sun 11-19-2000 10:48:25 pm HvA V1.00 $
Disk logging routines
HISTORY: 920919 V1.0    
***************************************************************************/

/*------------------------------------------------------------------------------
                        HEADER FILES
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "window.h"

static int indent_len = 0;
static char indent_str[80];


/*#+func-----------------------------------------------------------------------
    FUNCTION: wn_log_indent()
     PURPOSE: Set indent in window stream loggin
      SYNTAX: int wn_log_indent(int i);
 DESCRIPTION: int i: Number of spaces to add to indentation
     RETURNS: 0 if all OK, -1 if error occured.
     HISTORY: 920919 V0.1
--#-func----------------------------------------------------------------------*/
int wn_log_indent(int adjust_len)
{
 int i;

 /*----------------------------------
   Adjust number of spaces to indent
 -----------------------------------*/
 indent_len += adjust_len;

 /*-----------------------------------------------------
   Check on valid indent lenght: 0 <= indent_len <= 40
   return 0 if OK, -1 if not
 -----------------------------------------------------*/
 if (indent_len < 0)
   indent_len = 0;
 if (indent_len > 40)
   indent_len = 40;

 /*-----------------
   Set the string
 -----------------*/
 i = 0;
 while (i < indent_len)
   indent_str[i++] = 0x20;
 indent_str[i] = '\0';

 return(0);

}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_log()
    PURPOSE: Log window actions to a log file
     SYNTAX: void wn_log(char *format, ...);
DESCRIPTION: If 'WN_LOG' is defined, a file called 'window.log' will be
             opened. The arguments given will be printed in the logfile.
             The format of the arguments is the same as for 'printf()'.
    RETURNS: nothing
    HISTORY: 920203 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
void wn_log(char *format, ...)
{
  va_list arguments;
  static char *ep = NULL;     /* Points to environment var that names file */
  FILE *fp = NULL;

  if (ep == NULL                /* First time through, just create blank file */
  && (ep = getenv("WN_LOG"))
  && (fp = fopen(ep,"w")) ) {
    fclose(fp);
    fp = NULL;
    indent_str[0] = '\0';
  }

  if (ep)  {                     /* If we have a filename, proceed */
    fp = fopen(ep,"r+");
    if (!fp) {
      fprintf(stderr,"\nCannot open %s\n\a",ep);
      return;
    }
    else {
      va_start(arguments,format);       /* va_... housekeeping             */
      fseek(fp,0L,SEEK_END);            /* position pointer on end of file */
      fprintf(fp,"%s",indent_str);      /* place indentation               */
      vfprintf(fp,format,arguments);    /* print the log message           */
      fclose(fp);                       /* close the file                  */
    } va_end(arguments);                /* va_... housekeeping             */

  }
}
