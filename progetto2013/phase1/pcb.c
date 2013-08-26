#include <const13.h>
#include <types13.h>

/* PCB handling functions */

/* PCB free list handling functions */

/* Array statico di MAXPROC PCB */ 
HIDDEN pcb_t pcb_table[MAXPROC];

/* Testa della lista dei PCB liberi */
HIDDEN pcb_t *pcbfree_h;

/* Funzione che azzera tutti i campi del PCB usata in allocPcb; resetto prima i valori di state_t p_s.gpr[n] dal momento che non posso usare l'iterazione per azzerare l'array gpr[29] e devo utilizzare la ricorsione */
void resetpcb(pcb_t *p, int n) {			
	if (n<29) {		
		p->p_s.gpr[n] = 0;
		resetpcb(p, n+1);
	}
	else {				
		p->p_next = NULL;			
		p->p_parent = NULL;
		p->p_first_child = NULL;
		p->p_sib = NULL;
		p->p_s.entry_hi = 0;
		p->p_s.cause = 0;
		p->p_s.status = 0;
		p->p_s.pc_epc = 0;
		p->p_s.hi = 0;
		p->p_s.lo = 0;
		p->priority = 0;
		p->p_semkey = NULL;
	}
}

	 
/* Funzione che inizializza la lista dei PCB liberi */
void initPcbs(void) {
	static int n = 0;
	if (n<MAXPROC) {			
		pcb_table[n].p_next = &pcb_table[n+1];
		n++;
		initPcbs();
	}
	else {
		pcbfree_h = &pcb_table[0];
		pcb_table[MAXPROC-1].p_next = NULL;
	}
}

/* Inserisce il PCB puntato da p nella lista dei PCB liberi */
/* per semplicità lo inserisce in testa alla lista */
/* se la lista dei liberi era vuota, l'elemento che inserisco ne diventerà la testa */
void freePcb(pcb_t *p) {
	if (pcbfree_h == NULL) {			
		pcbfree_h = p;
		p->p_next = NULL;
	}
	else {
		p->p_next = pcbfree_h;
		pcbfree_h = p;	
	}
}

/* Rimuove un elemento dalla lista dei PCB liberi, se non è vuota, e lo restituisce con tutti i campi azzerati, restituisce NULL altrimenti */
/* per semplicità rimuove l'elemento in testa alla lista */
/* se la lista non è vuota controllo anche se il PCB che vado a rimuovere sia l'ultimo nella lista dei liberi, in questo caso lo rimuovo dai liberi e la testa punterà ora a NULL */
pcb_t *allocPcb(void) {
	if (pcbfree_h == NULL) {
		return (NULL);
	}
	else {
		pcb_t *b;  /*b è un puntatore a PCB per non perdere informazione*/					
		b = pcbfree_h;
		pcbfree_h = pcbfree_h->p_next;  /*NULL se il PCB rimosso era l'ultimo della lista*/ 
		resetpcb(b, 0);	 /*funzione ausiliaria che resetta tutti i campi del PCB*/					
		return(b);
		
	}			
}

/* PCB queue handling functions */

/* Inserisce l'elemento puntato da p nella coda dei PCB puntati da head, tenendo conto della priorità (head max priorità) */
/* se la lista puntata da head è vuota, head punterà al nuovo elemento */
/* se la priorità di p è maggiore di quella del PCB puntato da head, head dovrà ora puntare a p */
/* altrimenti scansiono la lista alla ricerca della giusta posizione di per p */
void insertProcQ(pcb_t **head, pcb_t *p) {
	if (((*head) != NULL) && ((*head)->priority >= p->priority)) {
		insertProcQ (&((*head)->p_next), p);
	}
	else {
		p->p_next = *head;
		*head = p;
	}
}
		

/* Restituisce l’elemento di testa della coda dei processi puntata da head, SENZA RIMUOVERLO. Ritorna NULL se la coda non ha elementi. */
pcb_t* headProcQ(pcb_t *head) {
	if (head == NULL) return(NULL);
	else return(head);
}

/* Rimuove e restituisce l’elemento di testa della coda dei processi puntata da head. Ritorna NULL se la coda non ha elementi. */
pcb_t* removeProcQ(pcb_t **head) {
	if ((*head) == NULL) return (NULL);
	else {
		pcb_t *t = *head;  /*puntatore ausiliario per non perdere info*/
		*head = (*head)->p_next;
		return (t);
	}
}		

/* Rimuove il PCB puntato da p dalla coda dei processi puntata da head. Se p non e’ presente nella coda, restituisce NULL; altrimenti restituisce p stesso */
/* se p è l'elemento in testa alla coda, il puntatore head viene aggiornato di conseguenza */
pcb_t *outProcQ(pcb_t **head, pcb_t *p) {
	if (((*head) != NULL ) && ((*head) != p)) {
		return (outProcQ(&(*head)->p_next, p));
	}
	else {
		if ((*head) == NULL) return(NULL);
		else {
			(*head) = p->p_next;
			return(p);
		}
	}
}

/* Richiama la funzione fun per ogni elemento della lista puntata da head. */ 
void forallProcQ(pcb_t *head, void *fun(pcb_t *pcb, void *), void *arg) {
	if (head != NULL) {
		fun(head, arg);
		forallProcQ(head->p_next, fun, arg);
	}
}

/* Tree view functions */

/* Funzione ausiliaria per insertChild per inserire p in coda alla lista dei figli */
/* Inserisce p in coda alla lista di fratelli puntata da h */
void insertLastSib(pcb_t **h, pcb_t *p) {
	if ((*h) != NULL) {
		insertLastSib(&(*h)->p_sib, p);
	}
	else {
		(*h) = p;
		p->p_sib = NULL;
	}
}

/* Inserisce il PCB puntato da p come figlio del PCB puntato da parent. */ 
/* Lo inserisce in coda */
void insertChild(pcb_t *parent, pcb_t *p) {
	insertLastSib(&(parent->p_first_child), p);  /*funzione ausiliaria per inserire il child*/
	p->p_parent = parent;
}

/* Rimuove e ritorna il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL. */	
pcb_t* removeChild(pcb_t *p) {
	if (p->p_first_child == NULL) return(NULL);
	else {
		pcb_t *b = p->p_first_child;  /*puntatore ausiliario per non perdere info*/
		p->p_first_child = (p->p_first_child)->p_sib;
		return(b);
	}
}

/* Funzione ausiliaria di outChild che rimuove e ritorna il PCB puntato da p dalla lista dei fratelli puntata da parent  */
/* Se non esiste restituisce NULL */
/* Simile alla outProcQ */
pcb_t *removeSib(pcb_t **fc, pcb_t *p) {
	if (((*fc) != NULL ) && ((*fc) != p)) {
		return (removeSib(&((*fc)->p_sib), p));
	}
	else {
		if ((*fc) == NULL) return(NULL);
		else {
			(*fc) = p->p_sib;
			return(p);
		}
	}
}
	
/* Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non ha un padre, restituisce NULL. Altrimenti restituisce l’elemento rimosso (cioe’ p). */
/* A differenza della removeChild, p puo’ trovarsi in una posizione arbitraria (ossia non é necessariamente il primo figlio del padre). */
/* Utilizzo una funzione ausiliaria per scorrere la lista dei fratelli del PCB puntato da p, per poterlo rimuovere, nel caso in cui esso non sia il primo della lista */
pcb_t* outChild(pcb_t *p) {
	if (p->p_parent == NULL) return(NULL);
	else return(removeSib(&((p->p_parent)->p_first_child), p));  /*funzione ausiliaria per trovare p nella lista dei fratelli. vedi sopra*/
}


		

