# Makefile for arith (Comp 40 Assignment 4)
# 
# This Makefile is more verbose than necessary.  In each assignment
# we will simplify the Makefile using more powerful syntax and implicit rules.
#
# Last updated: February 16, 2016


############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path
# CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)
CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/usr/sup/cii40/lib64 -L/comp/40/lib64 

# Libraries needed for linking
# All programs cii40 (Hanson binaries) and *may* need -lm (math)
# LDLIBS = -l40locality -lnetpbm -lcii40 -lm -lrt
LDLIBS = -l40locality -larith40 -lnetpbm -lcii40 -lm

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
#
# This bugs Mark, who dislikes false dependencies, but
# he agrees with Noah that you'll probably spend hours 
# debugging if you forget to put .h files in your 
# dependency list.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: ppmdiff smalltest 40image


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

ppmdiff: ppmdiff.o meansquarediff.o a2plain.o uarray2.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

smalltest: smalltest.o a2plain.o uarray2.o bitpack.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

40image: 40image.o compress40.o a2plain.o uarray2.o floatrgb.o \
	 component.o transcomp.o codeword.o helpers.o bitpack.o \
	 codeword.o

clean:
	rm -f ppmdiff meansquarediff a2plain uarray2 smalltest \
	40image compress40 floatrgb component transcomp codeword \
	helpers bitpack codeword *.o

