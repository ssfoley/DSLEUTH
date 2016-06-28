# Makefile for the Whirlgif utilities.
# Comments for the Amiga is kindly supplied by
#             Ron Jensen <rjensen@konnections.com>
# If you use Visual C++, all '.o' must be changed to '.obj'
###########################################################
# Define target names
#
# Win32:
#TARG_WH = whirlgif.exe
#
# Unix, Amiga:
TARG_WH = whirlgif

###########################################################
# Define install dirs
#
# Win32: perhaps install by hand.
#INSTDIRS = C:
#BINDIR = DOS
#
# Amiga: Install by hand
# Unix:  perhaps change according to your flavor.
INSTDIRS = /usr/local
BINDIR = bin
MANDIR = man/cat1

###########################################################
# Choose the compiler.
#
# GNU C
CC = gcc
# Amiga: SASC
# CC = sc
# standard
#CC = cc

###########################################################
# Optimization flags
#
#  DJGPP:
#OPTIMIZE= -fcombine-regs -O
# for Amiga's SASC compiler remove Optimization flags.
# standard:
OPTIMIZE = -O

###########################################################
# Debug flags
#
DEBUG = -g
#DEBUG = -ggdb

###########################################################
# CFLAGS
#
#       If you need <strings.h>, uncomment next line
#STRS_H	=  -D_USE_STRINGS_H
#       If you use DJGPP or Visual C++ uncomment next line
#VIS_GNU_W32	=  -D_FOPEN_TXT_OR_BIN
#
#       If you use GNUcc and like warnings uncomment next line
# WARNINGS = -Wall
# for the Amiga:
# CFLAGS    = DATA=FAR MATH=IEEE CPU=68020 PARAMETERS=BOTH ANSI STRICT NOICONS IGNORE=51
# standard:
CFLAGS	=  $(DEBUG) $(OPTIMIZE) $(VIS_GNU_W32) $(STRS_H) $(WARNINGS)

all: $(TARG_WH)

$(TARG_WH): whirlgif.o gifencod.o gifdecod.o
	$(CC) $(CFLAGS) -o $(TARG_WH) whirlgif.o gifencod.o gifdecod.o
#for the Amiga: remove above line and use below:
#	slink NOICONS from lib:c.o+whirlgif.o+gifencod.o+gifdecod.o to WhirlGif3 LIB LIB:scmieee.lib LIB:sc.lib LIB:amiga.lib

clean:
	rm -f $(TARG_WH) *.o

# under DOS/W32 as well as on the Amiga it is probably best to install by hand
# under Unix perhaps change below commands for your flavor and local
#    traditions.  The install(1) used here is sometimes located in /usr/ucb

install: all
	nroff -man whirlgif.1 > $(INSTDIRS)/$(MANDIR)/whirlgif.0 ;\
	install -cs -g bin -m 555 -o bin $(TARG_WH) $(INSTDIRS)/$(BINDIR)

# DEPENDECIES FOLLOW:
gifdecod.o:	 whirlgif.h	gifdecod.c
	$(CC) $(CFLAGS) -c gifdecod.c

gifencod.o:	 whirlgif.h	gifencod.c
	$(CC) $(CFLAGS) -c gifencod.c 

whirlgif.o:	 whirlgif.h	whirlgif.c
	$(CC) $(CFLAGS) -c whirlgif.c
