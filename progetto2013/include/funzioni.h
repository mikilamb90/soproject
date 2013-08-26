#ifndef __FUNZIONI_H_
#define __FUNZIONI_H_


#define MAX_CPU 16
#define FREE 1
#define BUSY 0



void cpuStateCopy(state_t *dst, state_t *src);

pcb_t *getPcb();

void rilascia(pcb_t *pcb);

pcb_t V(int chiave);

int P(int chiave, pcb_t *chiamante);

void Term_Pro(pcb_t *termpro);




U32 TEST;


#endif
