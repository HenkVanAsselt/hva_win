/**************************************************************************
$Header: KEYS.H Sun 11-05-2000 10:53:22 am HvA V1.00 $

Header file for the keys.c
***************************************************************************/

#ifndef FALSE
#define FALSE 0
#define TRUE  !FALSE
#endif

/*--- Prototypes  */
int key_pressed(void);
unsigned int waitkey(void);
unsigned char shift_status(void);
int is_fkey(unsigned int scancode);

/*---------------------
  Character constants
----------------------*/
#define  BELL  '\a'
#define  CR    13
#define  LF    10
#define  ESC   27
#define  STX    2
#define  ETX    3
#define  SPACE ' '
#define  COMMA ','
#define  HT     5       /* Horizontal TAB */

/*---------------
  KEY CONSTANTS
----------------*/
#define ESC_KEY		  0x011B
#define ARROW_UP      0x48E0  /* 0x4800 */
#define ARROW_DOWN    0x50E0  /* 0x5000 */
#define ARROW_LEFT    0x4BE0  /* 0x4B00 */
#define ARROW_RIGHT   0x4DE0  /* 0x4D00 */
#define PAGE_UP       0x49e0  /* 0x4900 */
#define PAGE_DOWN     0x51e0  /* 0x5100 */
#define DELETE        0x53e0  /* 0x5300 */
#define BACKSPACE     0x0E08
#define HOME          0x47e0  /* 0x4700 */
#define END_LINE      0x4fe0  /* 0x4F00 */
#define ENTER		  0x1C0D
#define RETURN		  0x1C0D

/*--- Shift status, as returned by bioskey(2)  */
#define RIGHT_SHIFT   0x01
#define LEFT_SHIFT    0x02
#define CONTROL       0x04
#define ALTERNATE     0x08
#define SCROLL_LOCK   0x10
#define NUM_LOCK      0x20
#define CAPS_LOCK     0x40
#define INSERT_ACTIVE 0x80

/*--- Function key scancodes, as returned by bioskey(1)  */

#define F1          0x3B00    
#define F2          0x3C00    
#define F3          0x3D00    
#define F4          0x3E00    
#define F5          0x3F00    
#define F6          0x4000    
#define F7          0x4100    
#define F8          0x4200    
#define F9          0x4300    
#define F10         0x4400    
#define F11         0x8500    
#define F12         0x8600    
					      
#define SHIFT_F1    0x5400
#define SHIFT_F2    0x5500
#define SHIFT_F3    0x5600
#define SHIFT_F4    0x5700
#define SHIFT_F5    0x5800
#define SHIFT_F6    0x5900
#define SHIFT_F7    0x5A00
#define SHIFT_F8    0x5B00
#define SHIFT_F9    0x5C00
#define SHIFT_F10   0x5D00
#define SHIFT_F11   0x8700
#define SHIFT_F12   0x8800
					      
#define CTRL_F1     0x5E00
#define CTRL_F2     0x5F00
#define CTRL_F3     0x6000
#define CTRL_F4     0x6100
#define CTRL_F5     0x6200
#define CTRL_F6     0x6300
#define CTRL_F7     0x6400
#define CTRL_F8     0x6500
#define CTRL_F9     0x6600
#define CTRL_F10    0x6700
#define CTRL_F11    0x8900
#define CTRL_F12    0x8A00
					      
#define ALT_F1      0x6800
#define ALT_F2      0x6900
#define ALT_F3      0x6A00
#define ALT_F4      0x6B00
#define ALT_F5      0x6C00
#define ALT_F6      0x6D00
#define ALT_F7      0x6E00
#define ALT_F8      0x6F00
#define ALT_F9      0x7000
#define ALT_F10     0x7100
#define ALT_F11     0x8B00
#define ALT_F12     0x8C00



