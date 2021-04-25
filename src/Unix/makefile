#!smake

# /usr/include/make/commondefs and /usr/include/make/commonrules
# define some useful Makefile rules.  For example, they
# defines a 'clean' target, so you can type 'make clean'
# to remove all .o files (or other files generated during
# compilation). See the file /usr/include/make/commonrules for
# documentation on what targets are supported.

include /usr/include/make/commondefs

LLDLIBS =-lInventorXt -lInventor -lm

# Use compiler flag IV_STRICT to catch things in the code that are not
# recommended for use with Inventor 2.1
LC++DEFS = -DIV_STRICT

# Makepoints setzen
default: bart main control ability objects

bart: main.o control.o ability.o objects.o
	$(C++F) main.o control.o ability.o objects.o -o bart $(LDFLAGS)-OPT:Olimit=0

main: main.c++
	$(C++F) -c  main.c++

control: control.c++
	$(C++F) -c  control.c++

ability: ability.c++
	$(C++F) -c  ability.c++

objects: objects.c++
	$(C++F) -c  objects.c++

