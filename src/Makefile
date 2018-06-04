# this is the Makefile for the sudoku solver

COMPILER	 	= gcc
WARNINGS	 	= -Wchar-subscripts -Wimplicit -Wparentheses -Wreturn-type -Wmissing-braces -Wundef
COMPILER_FLAGS 	= -c -g -O0 -Werror -Wfatal-errors $(WARNINGS)
LINKER 			= gcc
LINKER_OPT 		= 

OBJ_BF = bruteforcesolver.o
OBJ_DS = dynamicsolver.o

all : BruteForceSolver DynamicSolver

BruteForceSolver : $(OBJ_BF)
	$(LINKER) $(LINKER_OPT) $(OBJ_BF) -o BruteForceSolver

bruteforcesolver.o : bruteforcesolver.c bruteforcesolver.h
	$(COMPILER) $(COMPILER_FLAGS) bruteforcesolver.c

DynamicSolver : $(OBJ_DS)
	$(LINKER) $(LINKER_OPT) $(OBJ_DS) -o DynamicSolver

dynamicsolver.o : dynamicsolver.c dynamicsolver.h
	$(COMPILER) $(COMPILER_FLAGS) dynamicsolver.c

clean:
	-rm -f *.o *~ BruteForceSolver DynamicSolver
	-rm -r BruteForceSolver.dSYM DynamicSolver.dSYM
