#
# Makefile by Samantha Foley
# 
# Just a very simple Makefile for compiling the OSM post-processing program.
#

#
# choose a compiler
#
#CC = cc
CC = gcc

CFLAGS=

#CLIBS = -L./ -L./GD/ -lgd -lm -lc -lmpi
CLIBS =

TARGET = readdata3

readdata3:
	$(CC) readdata3.c -o readdata3 $(CLIBS)

clean :
	rm readdata3

