/**************************************************************************
$Header: KEYS.C Fri 11-06-2000 9:36:10 pm HvA V1.00 $
***************************************************************************/

#include <bios.h>
#include "keys.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
key_pressed() checks the status of the keyboard by means of
bioskey(x) function. If a key was pressed it returns TRUE, else it
returns (FALSE). Note that the keypress is not dequeued for the
buffer. 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int key_pressed()
{
  if (bioskey(1)) {
    wn_log("key_pressed() TRUE\n");
    wn_log("TRUE \n");
    return(TRUE);
  }
  return(FALSE);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
shift_status returns the state of the shift state with bioskey(2)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned char shift_status()
{
  return(bioskey(2));
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
waitkey() waits for a keypress. If a key is pressed, it will return
the scancode in case of a special key, or the ASCII value (that is,
the scancode & 0x00FF)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned int waitkey()
{
  unsigned int scancode;

  wn_log("waitkey()\n");

  /*--- Wait for a keypress  */
  while (!key_pressed());
  
  scancode = bioskey(0x10);
  wn_log("scancode = %0lx\n",scancode);
  return(scancode);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
is_fkey() if the parameter is the scancode of a function key.
If so, this function returns TRUE, else it returns FALSE. 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int is_fkey(unsigned int scancode)
{
  switch (scancode) {
  	/*--- F1...F12  */
    case  	0x3B00 :
    case  	0x3C00 :
    case  	0x3D00 :
    case  	0x3E00 :
    case  	0x3F00 :
    case  	0x4000 :
    case  	0x4100 :
    case  	0x4200 :
    case  	0x4300 :
    case  	0x4400 :
    case  	0x8500 :
    case  	0x8600 :
    /*--- Shift F1..F2  */      
    case  	0x5400 :
    case  	0x5500 :
    case  	0x5600 :
    case  	0x5700 :
    case  	0x5800 :
    case  	0x5900 :
    case  	0x5A00 :
    case  	0x5B00 :
    case  	0x5C00 :
    case  	0x5D00 :
    case  	0x8700 :
    case  	0x8800 :
    /*--- CTRL F1..F12  */	      
    case  	0x5E00 :
    case  	0x5F00 :
    case  	0x6000 :
    case  	0x6100 :
    case  	0x6200 :
    case  	0x6300 :
    case  	0x6400 :
    case  	0x6500 :
    case  	0x6600 :
    case  	0x6700 :
    case  	0x8900 :
    case  	0x8A00 :
    /*---  ALT F1..F12  */	      
    case  	0x6800 :
    case  	0x6900 :
    case  	0x6A00 :
    case  	0x6B00 :
    case  	0x6C00 :
    case  	0x6D00 :
    case  	0x6E00 :
    case  	0x6F00 :
    case    0x7000 :
	case    0x7100 :
  	case  	0x8B00 :
	case  	0x8C00 : return TRUE;
	default        : return FALSE;
  }	
}
