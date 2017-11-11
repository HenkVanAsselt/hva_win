#**************************************************************************
# $Header: Fri 10-27-2000 8:14:52 pm HvA V1.00 $
#***************************************************************************/

include Makefile.cfg

CFLAGS  += -I../include -Wall -DUSE_FORTIFY
LDFLAGS += -L../../lib
LIBS     = 

OBJS =  window.o windio.o fortify.o wn_desk.o dialog.o dir.o editstr.o\
		wn_bord.o keys.o popup.o wn_clear.o wn_error.o \
		wn_loc.o wn_log.o wn_os.o wn_print.o wn_scrol.o wn_shell.o \
		wn_shad.o wn_title.o wn_yesno.o help.o funkey.o \
		video/video.a util.o pulldown.o 4dos.o 

.PHONY:	all dep clean distclean dist depend.dep cxref lib

appl.exe: appl.c $(OBJS)
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS) $(OBJS) $(LIBS) 
#	strip $@

genindex.exe: genindex.o
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

# Make library (including v_video functions)
lib: $(OBJS)
	strip *.o
	ar rs hva_win.a $(OBJS)

window.o: window.c window.h
windio.o: windio.c
fortify.o: fortify.c
appl.o: appl.c
wn_desk.o: wn_desk.c
dialog.o: dialog.c
dir.o: dir.c
editstr.o: editstr.c
funkey.o: funkey.c
help.o: help.c
wn_bord.o: wn_bord.c
keys.o: keys.c
popup.o: popup.c
pulldown.o: pulldown.c
wn_clear.o: wn_clear.c
wn_error.o: wn_error.c
wn_loc.o: wn_loc.c
wn_log.o: wn_log.c
wn_os.o: wn_os.c
wn_print.o: wn_print.c
wn_scrol.o: wn_scrol.c
wn_shell.o: wn_shell.c
wn_shad.o: wn_shad.c
wn_title.o: wn_title.c
wn_yesno.o: wn_yesno.c
util.o: util.c
4dos.o: 4dos.c

clean:
	del *.o
	del video\*.o

dist:
	- strip *.o
	- strip *.exe
	pkzip -rp ..\dist\hva_win.zip *.*

cxref:
	rm -f doc/cxref/*.*
	cxref *.c *.h -DHVA_WIN -Id:/djgpp/include -Odoc/cxref -xref-all -Nhva_win -index-all
	cxref *.c *.h -DHVA_WIN -Id:/djgpp/include -Odoc/cxref -xref-all -Nhva_win -index-all -html32 -html32-src

