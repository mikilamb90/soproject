#include "../include/asl.e"
#include "../include/pcb.e"

#include "../include/types13.h"
#include "../include/libumps.h"


#define FREE 0
#define BUSY 1

/*V	prende in input una chiave
 * P	prende in input una chiave e il pcb chiamante
 * vclock
 * get_pcb
 * relase_pcb
 * Term_Pro*/

int sem_lock[40];

int lock = 1;

/*cpuStateCopy - copia lo stato del processore
	@dst: puntatore al destinatario
	@src: puntatore al sorgente

*/

void cpuStateCopy (state_t *dst, state_t *src){
	int count = 0;
	
	dst->status = src->status;
	dst->cause = src->cause;
	dst->entry_hi = src->entry_hi;
	dst->pc_epc = src->pc_epc;
	dst->hi = src->hi;
	dst->lo = src->lo;
	for (count ; count < 29; count ++){
		dst->gpr[count] = src->gpr[count];
	}
}

pcb_t *getPcb(){
	/*cicla finche non e libero*/
	while(! (CAS ( &lock, FREE, BUSY)));
	/*prendo un pcb libero*/
	pcb_t *nuovo=allocPcb();
	/*rilascia*/
	CAS(&lock, FREE, BUSY);
	return nuovo;
}

void rilascia(pcb_t *pcb){
	/*ciclo finche non entro nella sezione critica*/
	while(!(CAS(&lock, FREE, BUSY)));
	/*distrugo tutti i link*/
	pcb->p_parent = NULL;
	
	listdel(&pcb->p_sib);
	/*inserisco il pcb nella lista dei pcb liberi*/
	freePcb(pcb);
	/*rilascio la sezione critica*/
	CAS(&lock, BUSY, FREE);
}

pcb_t V(int chiave){
	
}

int P(int chiave, pcb_t * chiamante){
	
	
	
}





void Term_Pro(pcb_t *termpro){
	/*EVITABILE
	 * Termina termpro e tutti i suoi figli, implementato in asl.c
	 */
	outChildBloched(termpro);
}
