CC = gcc
#OPT = -fno-unroll-all-loops
#OPT =-O2
#OPT=-O2
WARN = -g -Wall
INC= 
LIB= 
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)
CFLAGS1 = $(OPT) $(WARN) $(INC) 
# List all your .c files here (source files, excluding header files)

SPEC_SRC = acapp.c write.c common.c read.c changeAssoc.c contention.c 

# List corresponding compiled object files here (.o files)

SPEC_OBJ = acapp.o createrp.o common.o changerp.o changeAssoc.o contention.o 
#################################

# default rule

all: hec_benchmark
	@echo "my work is done here..."

# rule for making cache_sim

hec_benchmark: $(SPEC_OBJ)
	$(CC) -o acapp $(CFLAGS) $(SPEC_OBJ) -lm
	@echo "-----------DONE WITH CACHE_SIM-----------"


# generic rule for converting any .cpp file to any .o file

 .c.o:
	$(CC) $(CFLAGS1) -c $*.c


# type "make clean" to remove all .o files plus the cache_sim binary
clean:
	rm -f *.o acapp
# type "make clobber" to remove all .o files (leaves cache_sim binary)
clobber:
	rm -f *.o
	
