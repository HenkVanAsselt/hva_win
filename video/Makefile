#**************************************************************************
# $Header: Fri 10-27-2000 8:14:52 pm HvA V1.00 $
#***************************************************************************/

include Makefile.cfg

CFLAGS  += -I../include -Wall -DUSE_FORTIFY
LDFLAGS += -L../../lib
LIBS     = 

.PHONY:	all dep clean distclean dist depend.dep lib cxref

OBJS =  v_adapt.o v_font.o v_curs.o v_init.o v_scroll.o v_print.o v_attrib.o \
		v_box.o


all: *.c $(OBJS)

v_curs.o: v_curs.c
v_adapt.o: v_adapt.c
v_font.o: v_font.c
v_init.o: v_init.c
v_scroll.o: v_scroll.c
v_print.o: v_print.c
v_attrib.o: v_attrib.c
v_box.o: v_box.c

lib: $(OBJS)
	ar rs video.a $(OBJS)

cxref:
	rm -f doc/cxref/*.*
	cxref *.c *.h -DVIDEO -Id:/djgpp/include -Odoc/cxref -xref-all -Nvideo -index-all
	cxref *.c *.h -DVIDEO -Id:/djgpp/include -Odoc/cxref -xref-all -Nvideo -index-all -html32 -html32-src

