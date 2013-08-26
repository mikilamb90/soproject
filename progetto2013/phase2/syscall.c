
#include "../include/asl.e"
#include "../include/pcb.e"

#include "../include/libumps.h"
#include "../include/types13.h"
#include "funzioni.c"


/*11 syscall da fare, 
 * 1)	Create_process -
 * 2)	Create_Brother
 * 3)	Terminate_Process -
 * 4)	Verhogen -
 * 5)	Passeren -
 * 6)	Get_CPU_Time -
 * 7)	Wait_For_Clock -
 * 8)	Wait_For_IO_Device
 * 
 * 9)	Specify_PRG_State_Vector
 * 10)	Specify_TLB_State_Vector
 * 11)	Specify_SYS_State_Vector
 * 
 * */
 
 
void Create_Process (pcb_t *attuale){
	 pcb_t *figlio = get_Pcb();
	  /*registro a1 e l'indirizzo fisico dello state_t del nuovo processo*/
	 state_t *stato_figlio = (state_t *) attuale -> p_s.reg_a1;
	
	 if ((stato_figlio) || (figlio)){
		 /*se il processo figlio viene creato corretamente, settiamo tutti i suoi campi*/
		/*non impplementiamo la priorita'*/
		/*figlio->priority=p_s.reg_a2;*/
		 insertChild(attuale, figlio);
		 attuale -> p_s.reg_v0 = 0;
		 enqueueProcess(attuale);
	 } else {
		 /*se il processo figlio non viene chiamato*/
		 attuale -> p_s.reg_v0 = -1;
		 return;
	 } 
}


void Create_Brother(pcb_t *attuale){
	pcb_t *fratello;
	state_t *stato_fratello;
	fratello = get_Pcb();
	stato_fratello = (state_t *) attuale ->p_s.reg_a1;
	if(stato_fratello || fratello){
		//fratello->priority=p_s.reg_a2;
		insertChild(attuale, fratello);
		attuale->p_s.reg_v0 =0;
		enqueueProcess(attuale);
	} else {
		attuale -> p_s.reg_v0 = -1;
		return;
	}
}

void Terminate_Process (pcb_t *attuale){
	/*outChildBloched si trova in pcb.c e termina il processo passatoli in input e tutti 
	 * i suoi figli.
	 */
	outChildBloched(attuale);
}

void Verhogen(pcb_t *attuale){
	int chiave = attuale->p_s.reg_a1;
	pcb_t *nonbloc;
	if(!(chiave < 0 || chiave >= USER_SEM) && (nonbloc = V(chiave))){
		enqueueProcess(nonbloc);
		decSoftCounter();
	}
	enqueueProcess(attuale);
}

void Passeren (pcb_t *attuale){
	/*la chiave del processo che ha chiamato una P*/
	int chiave = attuale -> p_s.reg_a1;
	
	if((chiave < 0 || chiave >= USER_SEM ) || P(chiave, attuale)){
		enqueueProcess(attuale);
	}
}

void Get_CPU_Time(pcb_t *attuale){
	attuale->p_s.reg_v0 = (attuale->cpu_time += (GET_TODLOW - (attuale->systemTOD)));
	
	enqueueProcess(attuale);
}

void Wait_For_Clock (pcb_t *attuale){
	if(P(PSEUDO_CLOCK_SEM, attuale)){
		enqueueProcess(attuale);
	}
}

/*

*/
void Wait_For_IO_Device(pcb_t* attuale){
	
	if(attuale->p_s.reg_a1 == INT_TERMINAL){
	
		int chiave;
		
		if(attuale->p_s.reg_a3){
			chiave = TERMINAL_SEM_R(attuale->p_s.reg_a2);
			
		} else {
			chiave = TERMINAL_SEM_W(attuale->p_s.reg_a2;
		}
	}
}

/*
da qui in poi non va bene un cazzo
*/
void SPSV(pcb_t *attuale){
	if(attuale -> def_areas[SPEC_PGMTRAP_NEWAREA_INDEX] == NULL){
		
		attuale ->def_areas[SPEC_PGMTRAP_NEWAREA_INDEX] = (state_t *) attuale -> p_s.reg_a2;
		
		attuale -> def_areas[SPEC_PGMTRAP_OLDAREA_INDEX] = (state_t *) attuale -> p_s.reg_a1;
		
		enqueueProcess(attuale);
	} else {
		/*termina attuale e tutti i suoi figli */
		outChildBloched(attuale);
	}
	return;
}

/*

 */
void STSV(pcb_t* attuale){

	if(attuale->def_areas[SPEC_TLB_NEWAREA_INDEX]==NULL){
		attuale->def_areas[SPEC_TLB_NEWAREA_INDEX] = (state_t*)attuale->p_s.reg_a2;
		
		attuale->def_areas[SPEC_TLB_OLDAREA_INDEX]= (state_t*) attuale->p_s.reg_a1;
		
		enqueueProcess(attuale);
		
	} else {
		/*termina attuale e tutti i suoi figli*/
		outChildBloched(attuale);
		
	}
}

/*

*/
void SSSV(pcb_t* attuale){

	if(attuale->def_areas[SPEC_SYSBK_NEWAREA_INDEX] == NULL){
	
		attuale->def_areas[SPEC_SYSBK_NEWAREA_INDEX] = (state_t*) attuale ->p_s.reg_a2;
		
		attuale->def_areas[SPEC_SYSBK_OLDAREA_INDEX]= (state_t *) attuale ->p_s.reg_a1;
		
		enqueueProcess(attuale);
		} else {
		/*outChildBloched termina attuale e tutti i suoi figli*/
			outChildBloched(attuale);
		}
}

/*

*/
int syscall(){
	int prid= getPRID(), cause = getCAUSE();
	pcb_t *attuale;
	
	if(prid == 0){
		if(((state_t*)SYSBK_OLDARE) ->status & STATUS_KUp){
			cpuStateCopy(((state_t*)SYSBK_OLDAREA), ((state_t*)PGMTRAP_OLDAREA));
			((state_t*)SYSBK_OLDARE)->cause = CAUSE_EXCCODE_SET(((state_t*)PGMTRAP_OLDAREA)->cause, EXC_RESERVEDINSTR);
		
			trap();
		}
	}
	while (attuale -> p_s.reg_a0){
		if(attuale -> p_s.reg_a0 == 1)		Create_Process(attuale);
		if(attuale -> p_s.reg_a0 == 2)		Create_Brother(attuale);
		if(attuale -> p_s.reg_a0 == 3)		Terminate_Process(attuale);
		if(attuale -> p_s.reg_a0 == 4)		Verhogen(attuale);
		if(attuale -> p_s.reg_a0 == 5)		Passeren(attuale);
		if(attuale -> p_s.reg_a0 == 6)		Get_CPU_Time(attuale);
		if(attuale -> p_s.reg_a0 == 7)		Wait_For_Clock(attuale);
		if(attuale -> p_s.reg_a0 == 8)		Wait_For_IO_Device(attuale);
		if(attuale -> p_s.reg_a0 == 9)		SPSV(attuale);
		if(attuale -> p_s.reg_a0 == 10)		STSV(attuale);
		if(attuale -> p_s.reg_a0 == 11)		SSSV(attuale);
	}
		
		
		
	
	
	return 0;
}
