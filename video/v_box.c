/**************************************************************************
$Header: v_box.c Mon 11-13-2000 2:47:38 pm HvA V1.00 $

Test file for video routines 

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "v_video.h"

/*+ Box character +*/
unsigned char ul_char; /*+ upper left corner  +*/
unsigned char le_char; /*+ left side          +*/
unsigned char ri_char; /*+ right side         +*/
unsigned char ll_char; /*+ lower left corner  +*/
unsigned char to_char; /*+ top                +*/
unsigned char bo_char; /*+ bottom             +*/
unsigned char lr_char; /*+ lower right corner +*/
unsigned char ur_char; /*+ upper right corner +*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_setboxchar - Set the box drawing characters
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_setboxchars(int ul, int to, int ur, int ri, int lr, int bo, int ll, int le)
{
  ul_char =	ul;
  le_char =	le;
  ri_char =	ri;
  ll_char =	ll;
  to_char =	to;
  bo_char =	bo;
  lr_char =	lr;
  ur_char =	ur;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_drawbox() - Draw a box on the screen with the characters already set
by v_setboxchar()
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void v_drawbox(int row, int col, int xsize, int ysize, int att)
{
  static int first_time = 1;
  int i;

  wn_log("v_drawbox() (%d,%d) w=%d h=%d\n",row,col,xsize,ysize);

  if (first_time) {
    ul_char = 218;
    le_char = 179;
    ri_char = 179;
    ll_char = 192;
    to_char = 196;
    bo_char = 196;
    lr_char = 217;
    ur_char = 191;
	first_time = 0;
  }

  /*--- Upper line  */
  for (i=col ; i<col+xsize-1 ; i++)
    v_printca(row,i,to_char,att);

  /*--- Left line  */
  for (i=row ; i<row+ysize-1 ; i++)
    v_printca(i,col,le_char,att);

  /*--- Right line  */
  for (i=row ; i<row+ysize-1 ; i++)
    v_printca(i,col+xsize-1,ri_char,att);

  /*---  Bottom line  */
  for (i=col ; i<col+xsize-1 ; i++)
    v_printca(row+ysize-1,i,to_char,att);

  /*--- Upper left corner  */
  v_printca(row,col,ul_char,att);

  /*--- Upper Right Corner  */
  v_printca(row,col+xsize-1,ur_char,att);

  /*--- lower left corner  */
  v_printca(row+ysize-1,col,ll_char,att);

  /*--- lower right corner  */
  v_printca(row+ysize-1,col+xsize-1,lr_char,att);

}


