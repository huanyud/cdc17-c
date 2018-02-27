# makefile for cs2 and mmr_search
# compileler definitions
# COMP_DUALS to compute prices
# PRINT_ANS to print flow (and prices if COMP_DUAL defined)
# COST_RESTART to be able to restart after a cost function change
# NO_ZERO_CYCLES finds an optimal flow with no zero-cost cycles
# CHECK_SOLUTION check feasibility/optimality. HIGH OVERHEAD!

# change these to suit your system
#CCOMP = g++
#CCOMP = gcc
#CFLAGS = -O4 -DNDEBUG
#CFLAGS = -g -DCHECK_SOLUTION -Wall
#CFLAGS = -g -Wall
CFLAGS = -O4 -DNDEBUG -DNO_ZERO_CYCLES
#BIN=cs2

all: cs2 mr_search lemon clean

cs2: CS2/cs2.c CS2/parser_cs2.c CS2/types_cs2.h CS2/timer.c CS2/wintimer.c
	gcc $(CFLAGS) -o cs2  CS2/cs2.c CS2/parser_cs2.c -lm

mr_search: MR_Search/mmr_search.c
	gcc $(CFLAGS)  -o mr_search MR_Search/mmr_search.c -lm
	
lemon: LEMON/lemon_dimacs.cpp
	g++ -I ~/lemon/include -L ~/lemon/lib -lemon -std=c++11 -o lemon LEMON/lemon_dimacs.cpp

clean:
	rm -f $(BIN) *~
