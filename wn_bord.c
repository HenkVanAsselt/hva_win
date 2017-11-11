/**************************************************************************
$Header: WN_BORD.C Fri 10-27-2000 9:34:42 pm HvA V2.01 $

DESC: Border routines for HVA_WIN
HIST: 20001027 V2.01 - Adapted for DJGPP
***************************************************************************/

#include "video/v_video.h"
#include "window.h"

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_border()
    PURPOSE: Draw a border around a window
     SYNTAX: int wn_border(WINDOW *w, int style);
DESCRIPTION: WINDOW *w = pointer to a window control block
             int style = style of border to draw
             The style does not change the attribute of the border.
    RETURNS: NO_WINDOW: window not opened
             TRUE     : all ok
    HISTORY: 910709 V0.1
--#-func-------------------------------------------------------------------*/
int wn_border(WINDOWPTR w, int style)
{

  wn_log("wn_border(%p,%s, style = %2d)\n",w,w->title,style);

  /*----------------------------------
    Check if window opened and topped
  -----------------------------------*/
  if (!w)
    return(NO_WINDOW);
  if (w != top_window)
    wn_top(w);

  if (w->btype != _NO_BORDER) {
    wn_setbox(style);
    v_drawbox(w->wy1,w->wx1,w->xsize,w->ysize,w->bcolor);
    return(TRUE);
  }
  else
    return(FALSE);
}

/*#+func-------------------------------------------------------------------
   FUNCTION: wn_dborder()
    PURPOSE: Draw a border around a window
     SYNTAX: int wn_dborder(WINDOW *w,
                            int ul, int ur, int tb, int sd, int ll, int lr);
DESCRIPTION: Sets box characters and calls wn_border
             ul = upper left
             ur = upper right
             tb = top/bottom
             sd = sides
             ll = lower left
             lr = lower right
    RETURNS: Always TRUE
    HISTORY: 910709 V0.1 - Initial version
--#-func-------------------------------------------------------------------*/
#ifdef DONT_SKIP_THIS

int wn_dborder(WINDOW *w, int ul, int ur, int tb, int sd, int ll, int lr)
{
  wn_log("wn_dborder(%p,%s)\n",w,w->title);

  /*------------------------
    Check if window opened
  --------------------------*/
  if (!w)
    return(NO_WINDOW);
  if (w != top_window)
    wn_top(w);

  v_setboxchars(ul,ur,tb,sd,ll,lr);
  wn_border(w,w->btype);
  return(TRUE);
}

#endif

/*#+func-------------------------------------------------------------------
    FUNCTION: wn_setbox()
     PURPOSE: Set box outline-art to double or single lines
      SYNTAX: void wn_setbox(int style);
 DESCRIPTION: The style can be _SINGLE_LINE or _DOUBLE_LINE
              ÉÍÍÍÍÍÍÍÍÍ» ÚÄÄÄÄÄÄÄÄÄ¿ ÖÄÄÄÄÄÄÄÄÄ·  ÕÍÍÍÍÍÍÍÍÍ¸  ÕÍÍÍÍÍÍÍÍ¸
              º FRAME 0 º ³ FRAME 1 ³ º FRAME 2 º  ³ FRAME 3 ³  ³ FRAME 4³
              ÈÍÍÍÍÍÍÍÍÍ¼ ÀÄÄÄÄÄÄÄÄÄÙ ÓÄÄÄÄÄÄÄÄÄ½  ÔÍÍÍÍÍÍÍÍÍ¾  ÀÄÄÄÄÄÄÄÄÙ

                          Ü Ü ÜÜÜÜÜÜÜ Ü  °°°°°°°°°°°  ±±±±±±±±±±±  ²²²²²²²²²²

                FRAME 5   İ FRAME 6Ş  ° FRAME 7 °  ±FRAME 8  ±  ²FRAME 9 ²

                          ß ß ßßßßßßß ß  °°°°°°°°°°°  ±±±±±±±±±±±  ²²²²²²²²²²

              ÛÛÛÛÛÛÛÛÛÛ  ********** Ü
              ÛFRAME 10Û  *FRAME 11*
              ÛÛÛÛÛÛÛÛÛÛ  **********
     RETURNS: nothing
     HISTORY: 910717 V0.1 - Initial version
              920920 V0.2 - Added additional frame types
--#-func-------------------------------------------------------------------*/
void wn_setbox(int style)
{
  switch (style) {
                    /* setboxchars( ul, to, ur, ri, lr, bo, ll, le); */
    case _SINGLE_LINE: v_setboxchars(218,196,191,179,217,196,192,179); break;
    case _DOUBLE_LINE: v_setboxchars(201,205,187,186,188,205,200,186); break;
    case FRAME_0     : v_setboxchars(218,196,191,179,217,196,192,179); break;
    case FRAME_1     : v_setboxchars(201,205,187,186,188,205,200,186); break;
    case FRAME_2     : v_setboxchars(214,196,183,186,189,196,211,186); break;
    case FRAME_3     : v_setboxchars(213,205,184,179,190,205,212,179); break;
    case FRAME_4     : v_setboxchars(213,205,184,179,217,196,192,179); break;
    case FRAME_5     : v_setboxchars( 32, 32, 32, 32, 32, 32, 32, 32); break;
    case FRAME_6     : v_setboxchars(220,220,220,222,223,223,223,221); break;
    case FRAME_7     : v_setboxchars(176,176,176,176,176,176,176,176); break;
    case FRAME_8     : v_setboxchars(177,177,177,177,177,177,177,177); break;
    case FRAME_9     : v_setboxchars(178,178,178,178,178,178,178,178); break;
    case FRAME_10    : v_setboxchars(219,219,219,219,219,219,219,219); break;
    case FRAME_11    : v_setboxchars('*','*','*','*','*','*','*','*'); break;
    default          : v_setboxchars(218,191,191,179,217,196,192,179); break;
  }
}
