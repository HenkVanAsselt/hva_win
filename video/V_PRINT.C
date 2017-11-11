/**************************************************************************
$Header: v_print.c Tue 11-14-2000 2:39:19 pm HvA V1.00 $

lowlevel video print routines
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bios.h>
#include <conio.h>				/*--- for puttext() and gettext()  */
#include "v_video.h"

extern SCREEN _cursvar;			/*+ Curses internal variables +*/
extern int _default_lines;   	/*+ default terminal height   +*/
extern int LINES;            	/*+ current terminal height   +*/
extern int COLS;         	 	/*+ current terminal width    +*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_print()
Print character 'c' on the screen on position (row,col) with current
attribute.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_printc(int row, int col, int c)
{
  union REGS regs;

  /* v_setcurpos(row,col); */ 
  regs.h.ah = 2;
  regs.h.bh = _cursvar.video_page;
  regs.h.dh = (BYTE) row;
  regs.h.dl = (BYTE) col;
  int86(0x10,&regs,&regs);

  regs.h.ah = 10;
  regs.h.al = (BYTE) c;			/*--- character  */
  regs.h.bh = 0;
  regs.x.cx = 1;
  int86(0x10,&regs,&regs);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_printca() 
Put character 'c' on screen on position (row,col) with attribute 'att'.
The origin of the screen is (0,0).
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_printca(int row, int col, int c, int att)
{
  union REGS regs;

/*
  wn_log("v_printca() - (%d,%d) c=<%c>\n",row,col,c);
*/

  /* v_setcurpos(row,col); */ 
  regs.h.ah = 2;
  regs.h.bh = _cursvar.video_page;
  regs.h.dh = (BYTE) row;
  regs.h.dl = (BYTE) col;
  int86(0x10,&regs,&regs);

  regs.h.ah = 9;
  regs.h.al = (BYTE) c;			/*--- character  	*/
  regs.h.bh = _cursvar.video_page;
  regs.h.bl = (BYTE) att;		/*--- attribute		*/
  regs.x.cx = 1;
  int86(0x10,&regs,&regs);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_printmca() 
Put multiple times the character 'c' on screen on position (row,col) 
with attribute 'att'. Note: The origin of the screen is (0,0).
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_printmca(int row, int col, int c, int att, int n)
{
  union REGS regs;

/*
  wn_log("v_printmca() - (%d,%d) c=<%c>\n",row,col,c);
*/

  /* v_setcurpos(row,col); */ 
  regs.h.ah = 2;
  regs.h.bh = _cursvar.video_page;
  regs.h.dh = (BYTE) row;
  regs.h.dl = (BYTE) col;
  int86(0x10,&regs,&regs);

  regs.h.ah = 9;
  regs.h.al = (BYTE) c;			/*--- character  	*/
  regs.h.bh = _cursvar.video_page;
  regs.h.bl = (BYTE) att;		/*--- attribute		*/
  regs.x.cx = n; 				/*--- n times  */
  int86(0x10,&regs,&regs);
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_gettext() - calls gettext (origin(1,1)) with parameters 
adjusted for origin (0,0)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_gettext(int row1, int col1, int row2, int col2, void *array)
{
  wn_log("v_gettext(), (%d,%d)-(%d,%d) \n",row1,col1,row2,col2);
  gettext(row1+1,col1+1,row2+1,col2+1,array);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_puttext() - calls puttext (origin(1,1)) with parameters 
adjusted for origin (0,0)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_puttext(int row1, int col1, int row2, int col2, void *array)
{
  wn_log("v_puttext(), (%d,%d)-(%d,%d) \n",row1,col1,row2,col2);
  puttext(row1+1,col1+1,row2+1,col2+1,array);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_prints() - Print a string on the screen. The string has to be
NUL terminated.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_prints(int row, int col, char *s)
{
  int i,len;

  wn_log("v_prints() (%d,%d) <%s>\n",row,col,s);

  /*--- Determine string length */
  len = strlen(s);

  /*--- Print the string  */
  for (i=0 ; i<len ; i++) {
    v_printc(row,col+i,s[i]);
  }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_fillarea() - Fills a area with a given character and attribute
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_fillarea(int row, int col, int width, int height, int c, int attrib)
{
  int h;

  wn_log("v_fillarea() (%d,%d) w=%d h=%d\n",row,col,width,height);

  for (h=0 ; h<height ; h++) {
    v_printmca(row+h,col,c,attrib,width);
  }
}


