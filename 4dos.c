/**************************************************************************
$Header: 4DOS.C Fri 11-10-2000 4:12:12 pm HvA V1.00 $
***************************************************************************/

#include <bios.h>

#ifndef FALSE
#define FALSE 0
#define TRUE !FALSE
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    FUNCTION: detect_4DOS()
     PURPOSE: Detect if 4DOS is present
 DESCRIPTION: Use INT 2Fh call
     RETURNS: TRUE if 4dos detected, FALSE if not
     HISTORY: 911214 V0.1 - Initial version
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int detect_4DOS()
{
  union REGS regs;

  regs.x.ax = 0xD44D;
  regs.x.bx = 0x0000;
  int86(0x2F,&regs,&regs);

  if (regs.x.ax == 0xD44D)
  {
    /*-------------------
      4DOS not present
    --------------------*/
    return(FALSE);
  }

  if (regs.x.ax == 0x44DD)
  {
    /*-----------------
      4DOS is present
    ------------------*/
    return(TRUE);
  }
  return(FALSE);
}


