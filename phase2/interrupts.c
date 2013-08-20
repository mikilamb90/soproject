#include "asl.e"
#include "pcb.e"
#include "libumps.h"
#include "types13.h"
#include "const13.h"
#include "scheduler.h"
#include "utils.h"

HIDDEN void intervalIntHandler(state_t *int_oldarea)
{
	unsigned int cpuID = getPRID();
	
	// Sblocca tutti i processi in coda sul semaforo dello pseudoClock
	flushPseudoClock();

	// Risetta il timer di pseudoClock
	setPseudoClockTimer();

	if(getCurrentProcess(cpuID) != NULL)
		// RILANCIO IL PROCESSO CHE ESEGUIVA SULLA CPU
		currentProcessRestart(cpuID, FALSE);
	else
	{
		processInterleave(NULL);
		currentProcessRestart(cpuID, TRUE);
	}
}


HIDDEN void IOIntHandler(unsigned int devAddr, unsigned int devCommand)
{
	unsigned int cpuID = getPRID();
	memaddr *devCommandReg;
	
	// Esegue una V sul semaforo del device, risvegliando il primo processo bloccato
	unsigned char unlockedProcess = devSemV(devAddr);
	
	if(unlockedProcess == FALSE)
		saveRetDevStatus(devAddr);
	
	mutexLock(getDevDbMutex());

	/* Mando l'ack dell'interrupt */
	devCommandReg = (memaddr *) devCommand;
	*devCommandReg = DEV_C_ACK;

	mutexUnlock(getDevDbMutex());

	if(getCurrentProcess(cpuID) != NULL)
		// RILANCIO IL PROCESSO CHE ESEGUIVA SULLA CPU
		currentProcessRestart(cpuID, FALSE);
	else
	{
		processInterleave(NULL);
		currentProcessRestart(cpuID, TRUE);
	}
}

HIDDEN void pltIntHandler(pcb_t* current)
{
	// esegue l'interleaving tra processi, scarica il processo in current nella coda ready più corta e ne prende uno dalla coda ready più lunga mettendolo in current sulla cpu
	processInterleave(current);
	
	// rifa partire il processo current settando i timer e salvando il tempo di partenza per l'aggiornamento dei tempi di esecuzione
	currentProcessRestart(getPRID(), TRUE);
}
void intHandler(void) {

	unsigned int timeStamp = GET_TODLOW;
	unsigned int prid = getPRID();
	
	state_t *ints_oldarea;
	pcb_t* current = getCurrentProcess(prid);

	if(prid == 0)
		ints_oldarea = (state_t *) INT_OLDAREA;
	else
		ints_oldarea = getNewOldAreaPtr(prid, INT_OLDAREA_INDEX);

	// Aggiorno i tempi di esecuzione del processo
	updateProcessExecTime(timeStamp, current);

	// Aggiorno lo stato del processore nel pcb
	updatePcbCPUState(ints_oldarea, current);

	/* selezione del gestore per gli interrupt giusto */
	if(CAUSE_IP_GET(ints_oldarea->cause, 1)) /* interrupt processor local timer */
	{
		pltIntHandler(current);
	}	
	else if (CAUSE_IP_GET(ints_oldarea->cause, 2))
	{
		intervalIntHandler(ints_oldarea); /* interval timer interrupt */
	}
	else if (CAUSE_IP_GET(ints_oldarea->cause, 7)) /* int terminali */
 	{
		unsigned int dBA = dev_search(devCalc(4));
		
		/* calcolo se l'int è del sub-device trasmettitore, ricevitore o entrambi */
		if (((*(memaddr *) dBA + 0x8) >= ILLEGAL_OPCODE) || ((*(memaddr *) dBA + 0x8) <= CHAR_TRASM))
			IOIntHandler(dBA + 0x8, dBA + 0xc);
		
		if (((*(memaddr *) dBA) >= ILLEGAL_OPCODE) || ((*(memaddr *) dBA) <= CHAR_RECV))
			IOIntHandler(dBA, dBA + 0x4);
	}
}