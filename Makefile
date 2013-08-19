# makefile per la phase 1
#Iommi, Monti

#commands for compiling (CMP) and linking (LNK)
CMP=mipsel-linux-gcc -ansi -pedantic -Wall
LNK=mipsel-linux-ld -T

#paths
path=/usr/local
share=/share/umps2
lib=/lib/umps2

#Objects to be linked
#umpscore.x
umpscore=$(path)$(share)/elf32ltsmip.h.umpscore.x
#crtso.o
crtso=$(path)$(lib)/crtso.o
#libumps.o
libumps=$(path)$(lib)/libumps.o

#Conversion of ELF object command
elf2umps=umps2-elf2umps

#Compilazione Phase1

CP1 : p1test.o asl.o pcb.o 
	$(LNK) $(umpscore) $(crtso) p1test.o asl.o pcb.o $(libumps) -o kernel
	$(elf2umps) -k kernel

p1test.o : include/*.h include/*.e
	$(CMP) -I include/ -c p1test.c

asl.o : include/const13.h include/types13.h
	$(CMP) -I include/ -c asl.c

pcb.o : include/const13.h include/types13.h
	$(CMP) -I include/ -c pcb.c

clean : 
	rm -f *.o kernel*
	rm -f *.umps
cleanall:
	make clean
	rm -f *.o kernel*
	rm -f *.umps
