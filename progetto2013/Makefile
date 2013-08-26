# Paths for the used libraries
LIBDIR = /usr/local/lib/umps2/
SHAREDIR = /usr/local/share/umps2/

# Directory containing the inclusion files
INCLUDEDIR = include/ 
# Directory containing the phase1 files
PHASE1DIR = phase1/
# Directory containing the phase2 files
PHASE2DIR = phase2/
# Directory containing the files for testing
TESTDIR = testing/

# Compiler
CC = mipsel-linux-gcc -ansi -pedantic -Wall
# Linker
LD = mipsel-linux-ld

# Compiler's parameters.
CFLAGS = -c -I ${INCLUDEDIR}

# Linker's parameter.
LFLAGS = -T ${SHAREDIR}elf32ltsmip.h.umpscore.x ${LIBDIR}crtso.o ${LIBDIR}libumps.o

all:	p2
	
p2:		clean pcb asl funzioni p2test p2testS scheduler traps syscalls interrupts boot linkp2
	umps2-elf2umps -k kernel
	
p2s:	clean pcb asl funzioni p2test p2testS scheduler traps syscalls interrupts boot linkp2s
	umps2-elf2umps -k kernel
	
# Compiling operations

pcb:
	${CC} ${CFLAGS} ${PHASE1DIR}pcb.c

asl:
	${CC} ${CFLAGS} ${PHASE1DIR}asl.c
	

	
scheduler:
	${CC} ${CFLAGS} ${PHASE2DIR}scheduler.c
	
traps:
	${CC} ${CFLAGS} ${PHASE2DIR}traps.c
	
syscalls:
	${CC} ${CFLAGS} ${PHASE2DIR}syscalls.c
	
interrupts:
	${CC} ${CFLAGS} ${PHASE2DIR}interrupts.c
	
boot:
	${CC} ${CFLAGS} ${PHASE2DIR}boot.c
	
funzioni:
	${CC} ${CFLAGS} ${PHASE2DIR}funzioni.c

p2test: 
	${CC} ${CFLAGS} ${TESTDIR}p2test.c
	
p2testS:
	${CC} ${CFLAGS} ${TESTDIR}p2test2012_scalability_v1.0.c
	
# Linking operation

linkp2:
	${LD} ${LFLAGS} pcb.o asl.o funzioni.o p2test.o scheduler.o traps.o syscalls.o interrupts.o boot.o -o kernel
linkp2s:
	${LD} ${LFLAGS} pcb.o asl.o funzioni.o p2test2012_scalability_v1.0.o scheduler.o traps.o syscalls.o interrupts.o boot.o -o kernel

# Clean up operations

clean:
	rm -f *.o kernel*
cleanall:
	make clean
	rm -f *.o
