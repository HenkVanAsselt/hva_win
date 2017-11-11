/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**************************************************************************
$Header: UTIL.C Thu 11-02-2000 9:25:56 pm HvA V1.00 $

Some common utilities
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
This function strips the leading and trailing spaces of a string
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *trimstr(char *s)
{
  int i = 0;
  int len;

  /*--- Strip leading blanks  */
  /*--- First find 1st non-blank  */
  while (s[i] == ' ') {
    i++;
  }
  strcpy(s,s+i);

  /*--- Strip trailing blanks  */
  len = strlen(s);
  while (isspace(s[len-1])) {
    s[len-1] = '\0';
	len--;
  }

  return(s);
}

	  
    



