/**************************************************************************
$Header: v_adapter.c Fri 11-10-2000 9:52:36 pm HvA V1.00 $

Low level video adapter routines.
***************************************************************************/

#include <dos.h>
#include <bios.h>
#include <sys\movedata.h>
#include "v_video.h"

extern SCREEN _cursvar;      	/*+ curses variables     +*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned char getdosmembyte (int offset)
{
 unsigned char b=0;

  dosmemget (offset, sizeof(unsigned char), &b);
  return b;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned short getdosmemword (int offset)
{
  unsigned short w=0;

  dosmemget (offset, sizeof(unsigned short), &w);
  return w;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void setdosmembyte (int offset, unsigned char b)
{
  dosmemput (&b, sizeof(unsigned char), offset);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void setdosmemword (int offset, unsigned short w)
{
  dosmemput (&w, sizeof(unsigned short), offset);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_query_adapter_type()  - Determine PC video adapter type
This function returns a macro identifier indicating the adapter
type.  See the list of adapter types in CURSPRIV.H.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_query_adapter_type()
{
    int retval = _NONE;
    union REGS regs;

        /* thanks to paganini@ax.apc.org for the GO32 fix */
#  if defined(GO32) && defined(NOW_WORKS)
#    include <dpmi.h>
    _go32_dpmi_registers dpmi_regs;
#  endif
    int equip;
    short video_base = getdosmemword (0x463);

    /*
     * Attempt to call VGA Identify Adapter Function...
     */
    regs.h.ah = 0x1a;
    regs.h.al = 0;
    int86(0x10, &regs, &regs);
    if ((regs.h.al == 0x1a) && (retval == _NONE))
    {
      /*
       * We know that the PS/2 video BIOS is alive and well.
       */
      switch (regs.h.al)
      {
        case 0:
            retval = _NONE;
            break;
        case 1:
            retval = _MDA;
            break;
        case 2:
            retval = _CGA;
            break;
        case 4:
            retval = _EGACOLOR;
            _cursvar.sizeable = TRUE;
            break;
        case 5:
            retval = _EGAMONO;
            break;
        case 26:
            retval = _VGACOLOR; /* ...alt. VGA BIOS... */
        case 7:
            retval = _VGACOLOR;
            _cursvar.sizeable = TRUE;
            break;
        case 8:
            retval = _VGAMONO;
            break;
        case 10:
        case 13:
            retval = _MCGACOLOR;
            break;
        case 12:
            retval = _MCGAMONO;
            break;
        default:
            retval = _CGA;
            break;
      }

    }
    else
    {
      /*
       * No VGA BIOS, check for an EGA BIOS by selecting an
       * Alternate Function Service...
       *
       * bx == 0x0010  -->  return EGA information
       */
      regs.h.ah = 0x12;
      regs.x.bx = 0x10;
      int86(0x10, &regs, &regs);
      if ((regs.h.bl != 0x10) && (retval == _NONE))
      {
          /*
           * An EGA BIOS exists...
           */
          regs.h.ah = 0x12;
          regs.h.bl = 0x10;
          int86(0x10, &regs, &regs);
          if (regs.h.bh == 0)
              retval = _EGACOLOR;
          else
              retval = _EGAMONO;
      }
      else if (retval == _NONE)
      {
        /*
         * Now we know we only have CGA or MDA...
         */
        int86(0x11, &regs, &regs);
        equip = (regs.h.al & 0x30) >> 4;
        switch (equip)
        {
        case 1:
        case 2:
            retval = _CGA;
            break;
        case 3:
            retval = _MDA;
            break;
        default:
            retval = _NONE;
            break;
        }
      }
    }
    if (video_base == 0x3d4)
    {
      _cursvar.video_seg = 0xb800;
      switch (retval)
      {
      case _EGAMONO:
          retval = _EGACOLOR;
          break;
      case _VGAMONO:
          retval = _VGACOLOR;
          break;
      default:
          break;
      }
    }
    if (video_base == 0x3b4)
    {
      _cursvar.video_seg = 0xb000;
      switch (retval)
      {
      case _EGACOLOR:
          retval = _EGAMONO;
          break;
      case _VGACOLOR:
          retval = _VGAMONO;
          break;
      default:
          break;
      }
    }
    if ((retval == _NONE)
#ifndef CGA_DIRECT
    ||  (retval == _CGA)
#endif
    )
    {
        _cursvar.direct_video = FALSE;
    }
    if ((unsigned int) _cursvar.video_seg == 0xb000)
        _cursvar.mono = TRUE;
    else
        _cursvar.mono = FALSE;

        /* Check for DESQview shadow buffer */
        /* thanks to paganini@ax.apc.org for the GO32 fix */
#if defined(GO32) && defined(NOW_WORKS)
    dpmi_regs.h.ah = 0xfe;
    dpmi_regs.h.al = 0;
    dpmi_regs.x.di = _cursvar.video_ofs;
    dpmi_regs.x.es = _cursvar.video_seg;
    _go32_dpmi_simulate_int(0x10, &dpmi_regs);
    _cursvar.video_ofs = dpmi_regs.x.di;
    _cursvar.video_seg = dpmi_regs.x.es;
#endif

#if !defined(GO32) && !defined(WATCOMC)
    regs.h.ah = 0xfe;
    regs.h.al = 0;
    regs.x.di = _cursvar.video_ofs;
    segs.es   = _cursvar.video_seg;
    int86x(0x10, &regs, &regs, &segs);
    _cursvar.video_ofs = regs.x.di;
    _cursvar.video_seg = segs.es;
#endif

    if  (!_cursvar.adapter)
        _cursvar.adapter = retval;
    return (v_sanity_check(retval));
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
v_sanity_check() - A video adapter identification sanity check.
This routine will force sane values for various control flags.
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int v_sanity_check(int adapter)
{
    int fontsize = v_get_font();
    int rows     = v_get_rows();

    switch (adapter)
    {
    case _EGACOLOR:
    case _EGAMONO:
        switch (rows)
        {
        case 25:    break;
        case 43:    break;
        default:
            _cursvar.bogus_adapter = TRUE;
            break;
        }

        switch (fontsize)
        {
        case _FONT8:    break;
        case _FONT14:   break;
        default:
            _cursvar.bogus_adapter = TRUE;
            break;
        }
        break;

    case _VGACOLOR:
    case _VGAMONO:

/*                                                                  */
/* lets be reasonably flexible with VGAs - they could be Super VGAs */
/* capable of displaying any number of lines. MH 10-Jun-92          */
/* This also applies to font size.            MH 16-Jun-92          */
/*
        switch (rows)
        {
        case 25:    break;
        case 43:    break;
        case 50:    break;
        default:
            _cursvar.bogus_adapter = TRUE;
            break;
        }

        switch (fontsize)
        {
        case _FONT8:    break;
        case _FONT14:   break;
        case _FONT16:   break;
        default:
            _cursvar.bogus_adapter = TRUE;
            break;
        }
*/
        break;

    case _CGA:
    case _MDA:
    case _MCGACOLOR:
    case _MCGAMONO:
        switch (rows)
        {
        case 25:    break;
        default:
            _cursvar.bogus_adapter = TRUE;
            break;
        }
        break;

    default:
        _cursvar.bogus_adapter = TRUE;
        break;
    }
    if (_cursvar.bogus_adapter)
    {
        _cursvar.sizeable   = FALSE;
        _cursvar.direct_video   = FALSE;
    }
    return (adapter);
}

