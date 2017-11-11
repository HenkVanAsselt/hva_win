/**************************************************************************
$Header: v_attrib.c Tue 11-14-2000 2:39:19 pm HvA V1.00 $

lowlevel video attribute routines
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include "v_video.h"

extern SCREEN _cursvar;			/*+ Curses internal variables +*/
extern int _default_lines;   	/*+ default terminal height   +*/
extern int LINES;            	/*+ current terminal height   +*/
extern int COLS;         	 	/*+ current terminal width    +*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_setattrib_area()
Set attribute for an complete area
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_setattrib_area(int row, int col, int width, int height, int att)
{
  int w,h;

  wn_log("v_settattrib_area() (%d,%d) w=%d h=%d\n",row,col,width,height);

  for (w=0 ; w<width; w++) {
    for (h=0 ; h<height; h++) {
	  v_setattrib(row+h,col+w,att);
	}
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_setattrib() - Set attribute 'att' on given row and column by
reading the character at that position, and writing the character
back with the given attribute
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_setattrib(int row, int col, int att)
{
  union REGS regs;

/*
  wn_log("v_settattrib() (%d,%d)\n",row,col);
*/
  /* v_setcurpos(row,col); */ 
  regs.h.ah = 2;
  regs.h.bh = _cursvar.video_page;
  regs.h.dh = (BYTE) row;
  regs.h.dl = (BYTE) col;
  int86(0x10,&regs,&regs);


  /*--- Read character (and attribute)  */
  regs.h.ah = 8;
  regs.h.bh = 0;
  int86(0x10,&regs,&regs);

  /*--- Rewrite character with given attribute  */
  regs.h.ah = 9;
  regs.h.bl = (BYTE) att;
  regs.x.cx = 1;
  int86(0x10,&regs,&regs);
}



