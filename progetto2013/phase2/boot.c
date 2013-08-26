/*boot.c*/

#include "../include/pcb.e"
#include "../include/asl.e"

#include "../include/libumps.h"
#include "../include/types11.h"

/*Poiche al solo il processore 0 e inizializato di default per tutti gli altri
 * abbiamo la matrice 16x8 , per i processori e i loro 8 campi*/
state_t new_old_areas[16][8];

/*test chiama test2 automaticamente*/
extern void test();

/*chiamo le funzioni esterne che mi servirano pe le new old
area dei processori*/

extern void interrupt();	/*interrupt*/
extern void trap();	/*TODO*/
extern void tlb();	/*TODO*/
extern void syscall();/* system call*/
extern void initscheduler();

/*Per garantire la mutua esclusione dei semafori, visto che i processi sono 20, i semafori devono essere
 * almeno il dopio.
 */
int semaforLock[40];

/*indirizzo plt*/
#define STATUS_TE = 0x08000000

/*numero cpu*/
#define NUM_CPU = 0x10000500

#define Ncpu *((memaddr*) NUM_CPU)



void init_area(state_t *area, memaddr handler, int offset) {
	area->status = (area->status & ~(STATUS_IEc | STATUS_KUc | STATUS_VMc)) | STATUS_TE;	
	area->reg_sp = RAMTOP - (FRAME_SIZE * offset);
	area->pc_epc = area->reg_t9 = handler;
}






void libera(void){
	/*tutte le variabili critiche dei semafori sono libere*/
	int i=0;
	while (i < 40){
		semaforLock[i] = 1;		/*con 1 indico che e libero*/
		i++;
	}
}


int main (){
	int i, j;
	 
	/*cpu 0*/
	init_area((state_t *)INT_NEWAREA, (memaddr)interrupt, 0);
	init_area((state_t *)TLB_NEWAREA, (memaddr)tlb, 0);
	init_area((state_t *)PGMTRAP_NEWAREA, (memaddr)trap, 0);
	init_area((state_t *)SYSBK_NEWAREA, (memaddr)syscall, 0);
	
	for(i = 1; i < Ncpu; i++) {
		for(j = 0; j < 4 ; j++) {
			switch(j) {
				case 0: init_area(&new_old_areas[i][1], (memaddr)interrupt, i);
				 break;
				case 1: init_area(&new_old_areas[i][3], (memaddr)tlb, i);
				 break;
				case 2: init_area(&new_old_areas[i][5], (memaddr)trap, i);
				 break;
				case 3: init_area(&new_old_areas[i][7], (memaddr)syscall, i);
				 break;
			}
		}
	}
	
	/*Inizializare le strutture dati di fase uno*/
	initPcbs();
	initASL();

	libera();

	/*chiamare lo scheduler*/
	initscheduler();
	PANIC();
	
	return 1;
}
