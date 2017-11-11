/**************************************************************************
$Header: GENINDEX.C Fri 10-27-2000 9:37:26 pm HvA V2.01 $

HIST: 20001027 V2.01 - Adapted for DJGPP
***************************************************************************/

/************************************************************************
*                                                                       *
*  GENINDEX - Create index from text file.                              *
*                                                                       *
*  Support Program for The Window BOSS                                  *
*                                                                       *
*  Copyright (c) 1984,1985,1986 - Philip A. Mongelluzzo                 *
*  All rights reserved.                                                 *
*                                                                       *
*  Note: The GENINDEX.EXE file was created using the CI86 compiler.     *
*        If you attempt to recompile this program with either Lattice   *
*        or MSC you may have to adjust the logic to account for the way *
*        the various compilers treat <CR><LF> sequences.  This usually  *
*        amounts to nothing more than chaning the "rb" to "r" in the    *
*        fopen statement.                                               *
*                                                                       *
*  History:                                                             *
*                                                                       *
*  911126 V0.2 - Generized subject heading key to the                   *
*                sequence "#%" so this can be anywhere on               *
*                a line                                                 *
*  911127 V0.3 - Adjusted special characters to make a text-            *
*                file which also can be used by 'HELPPC' (V2.10)        *
*                                                                       *
************************************************************************/

/*------------------------------------------------------------------------
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
   ':'  in column 1 indicates a subtopic key.  Multiple keys separated
        by colons ':' can be entered on the same line.  Single spaces
        are allowed in a key, multiple spaces are compressed to single.
   '%'  in column 1 indicates a highlighted title line.
   '^'  in column 1 indicates a centered highlighted title.
   ' '  (space) in column 1 indicates normal text.
   '~'  Tilde is used to mark text as a subtopic link.  Use two
        tilde characters to represent an actual tilde in the data.
        A word or phrase enclosed between tilde's will become a
        subtopic link for the current topic.
   TAB  in column 1 starts text in column 9
   any other character in column 1 is truncated

   Limits of the genindex() and help()

   Max items in main topic menu:          16
   Max items in subtopic menu:           512
   Max topics in index:                 1800
   Max size of topic text:             16384 bytes
   Max lines of text per topic:          512
   Max topic key length:                  20
   Max file title length:                 40
   Max subtopic links:                   120
   No limit on text file size

-----------------------------------------------------------------------*/

#include <stdio.h>                      /* Standard stuff */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef FALSE
#define FALSE 0
#define TRUE !FALSE
#endif

/*#+func-------------------------------------------------------------------
   FUNCTION: trimstr(char *s)
    PURPOSE: Trim leading and trailing blanks of a string
 DESRIPTION: -
    RETURNS: TRUE if all OK, FALSE if it 's' is a NULL pointer
    VERSION: 901101 V0.1
--#-func-------------------------------------------------------------------*/
int trimstr(char *s)
{
  int i;

  if (s == NULL)
    return(FALSE);

  i = strlen(s);
  if (i==0)
    return(TRUE);

  while (isspace(s[--i]))       /* Delete trailing blanks */
    s[i] = '\0';

  i = 0;                      /* Delete leading blanks */
  while (isspace(s[i]))
    i++;
  strcpy(s,s+i);

  return(TRUE);
}

/*#+func-------------------------------------------------------------------
   FUNCTION: main()
    PURPOSE: main function of 'GENINDEX.C'
 DESRIPTION: -
    RETURNS: Alway's returns 0
    VERSION: 901101 V0.1
             920205 V0.1 - Added trimstr() to trim topic key
--#-func-------------------------------------------------------------------*/
int main(int argc,char *argv[])
{
  FILE *fp;
  FILE *fo;
  char buf[132];
  static char fname[132];
  unsigned i;
  long pos;
  char *p;
  char *token;

  if(argc < 2)       /* check command line */
  {
    printf("Usage: genindex <filename.ext>");
    exit(1);
  }

  p = argv[1];      /* parse input filename */
  i = 0;
  while(*p) {
    fname[i++] = *p++;
    if(*p == ' ' || *p == '.') break;
  }
  strcat(fname, ".ndx");                /* create output filename */

  fp = fopen(argv[1], "r");             /* open input */
  if(!fp) {
    printf("Open failed: %s", argv[1]);
    exit(1);
  }
  printf("Creating: %s\n", fname);        /* say whats going on */
  fo = fopen(fname, "w");                 /* open output */

  while(fgets(&buf[0],132,fp))            /* read lines till eof */
  {
    /*-------------------------------------------
      Check if this line starts with a topic key
    ---------------------------------------------*/
    if(buf[0] == ':')                     /* look for topic key  */
    {
      token = strtok(buf,":\n\r");
      trimstr(token);
      while (token)
      {
        pos = ftell(fp);                        /* get file position */
        fprintf(fo,"%s\n%ld\n",token,pos);      /* write to index file */
        fprintf(stdout,"%s\n%ld\n",token,pos);  /* display on console */
        token = strtok(NULL,":\n\r");           /* Look for next subtopic key */
      }
    }

  }
  fclose(fp);                             /* close files */
  fclose(fo);
  return(0);
}

