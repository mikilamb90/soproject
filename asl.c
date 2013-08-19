#include <const13.h>
#include <types13.h>
#include "pcb.e"

/* array di SEMD con dimensione massima di MAXPROC. */
HIDDEN semd_t semd_table[MAXPROC];
/* Testa della lista dei SEMD liberi o inutilizzati */
HIDDEN semd_t *semdfree_h;
/* Testa della lista dei ASL */
HIDDEN semd_t *semd_h;

/* ASL handling functions */

/* Funzione ausiliaria di getSemd, scorre la lista di ASL e restituisce il SEMD cercato se c'è, NULL altrimenti */
semd_t* getRightSemd(int *key, semd_t *h) {
	if ((h != NULL) && (h->s_key != key)) return(getRightSemd(key, h->s_next));
	else {
		if (h == NULL) return(NULL);
		else return (h);
	}
}

/* Passato un puntatore a semaforo, restituisce il SEMD corrispodente se questo si trova nella ASL. NULL altrimenti */
semd_t* getSemd(int *key) {
	return getRightSemd(key, semd_h);
}

/* Inizializza la lista dei semdFree in modo da contenere tutti gli elementi della semdTable. */
/* Questo metodo viene invocato una volta sola durante l’inizializzazione della struttura dati. */
void initASL() {
	static int n = 0;
	if (n<MAXPROC) {
		semd_table[n].s_next = &semd_table[n+1];
		semd_table[n].s_key = NULL;
		semd_table[n].s_procQ = NULL;
		n++;
		initASL();
	}
	semd_h = NULL;
	semdfree_h = &semd_table[0];
	semd_table[MAXPROC-1].s_next = NULL;
	semd_table[MAXPROC-1].s_key = NULL;
	semd_table[MAXPROC-1].s_procQ = NULL;
}

/* Viene inserito il PCB puntato da p nella coda dei processi bloccati associata al SEMD con chiave key. */
/* Se il semaforo corrispondente non e’ presente nella ASL, alloca un nuovo SEMD dalla lista di quelli liberi (semdFree) e lo inserisce nella ASL, settando i campi in maniera opportuna. */
/* Se non e’ possibile allocare un nuovo SEMD perchè la lista di quelli liberi è vuota, restituisce TRUE. In tutti gli altri casi, restituisce FALSE. */
int insertBlocked(int *key, pcb_t *p) {
	semd_t *s = getSemd(key);
	if (s != NULL) {
		p->p_semkey = key;
		insertProcQ(&(s->s_procQ), p);
		return(FALSE);
	}
	else {
		if (semdfree_h != NULL) {
			semd_t *b = semdfree_h;
			semdfree_h = semdfree_h->s_next;
			b->s_next = semd_h;
			b->s_key = key;
			p->p_semkey = key;
			insertProcQ(&(b->s_procQ), p);
			semd_h = b;
			return(FALSE);
		}
		else return(TRUE);
	}
}

/* Funzione ausiliaria che rimuove il SEMD puntato da s dalla lista puntata da h */
void removeSemd(semd_t *s, semd_t **h) {
	if (((*h) != NULL) && ((*h) != s)) {
		removeSemd(s, &(*h)->s_next);
	}
	else {
		(*h) = s->s_next;
	}
}

/* Rimuove e ritorna il primo PCB dalla coda dei processi bloccati (s_ProcQ) associata al SEMD della ASL con chiave key. */
/* Se tale descrittore non esiste nella ASL restituisce NULL. Altrimenti restituisce l’elemento rimosso. */
/* Se la coda dei processi bloccati per il semaforo diventa vuota, rimuove il descrittore corrispondente dalla ASL e lo inserisce in testa alla lista dei descrittori liberi (semdfree). */
pcb_t* removeBlocked(int *key) {
	semd_t *s = getSemd(key);
	if (s != NULL) {
		pcb_t *b = removeProcQ(&(s->s_procQ));
		if (s->s_procQ == NULL) {
			removeSemd(s, &semd_h);
			s->s_next = semdfree_h;
			semdfree_h = s;
		}
		return(b);	
	}		
	else return(NULL);
}

/* Restituisce il puntatore al pcb del primo processo bloccato sul semaforo, senza deaccordarlo. */
/* Se il semaforo non è nella ASL o se la sua coda dei processi bloccati è vuota restituisce NULL. */
pcb_t* headBlocked(int *key) {
	semd_t *s = getSemd(key);
	if (s != NULL) {
		pcb_t *b = headProcQ(s->s_procQ);
		return(b);
	}
	else return(NULL);
}

/* richiama la funzione puntata da fun per ogni elemento della lista di fratelli */
void forallSib (pcb_t *head, void *fun(pcb_t *pcb, void *), void *arg) {
	if (head != NULL) {
		fun(head, arg);
		forallSib(head->p_sib, fun, arg);
	}
}

/* Elimina il PCB puntato da p dalla coda del semaforo su cui è bloccato */
void outBlocked(pcb_t *p) {
	semd_t *s = getSemd(p->p_semkey); 
		if (s != NULL) {
			pcb_t *t = outProcQ(&(s->s_procQ), p);
		}
}

/* Rimuove il PCB puntato da p dalla coda del semaforo su cui e’ bloccato */
/* outChildBlocked deve eliminare anche tutti i processi discendenti. */
void outChildBlocked(pcb_t *p) {
	if (p->p_first_child != NULL) {
		forallSib(p->p_first_child, outChildBlocked, NULL);
	}
	outBlocked(p);
}

/* richiama la funzione fun per ogni processo bloccato sul semaforo identificato da key */
void forallBlocked(int *key, void *fun(pcb_t *pcb, void *), void *arg) {
	forallProcQ(getSemd(key)->s_procQ, fun, arg);
}

