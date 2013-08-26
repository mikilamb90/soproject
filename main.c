#include "types13.h"
#include "const13.h"
#include "uMPStypes.h"
#include "libumps.h"

#include "asl.e"
#include "const.h"
extern void test();
extern void scheduler();

extern void boot_int();
extern void boot_tlb();
extern void boot_prgtrap();
extern void boot_syscall();

   /*questa è magia */

   /* extern state_t *** new_old_areas;*/
extern state_t * new_old_areas[8];
   /* new_old_areas=pass;*/

int main(void){
    int i;
    /* punto 1 */

        new_old_areas[0]=(state_t*)INT_NEWAREA;           
        new_old_areas[0]->pc_epc=new_old_areas[0]->reg_t9=(memaddr)boot_int;
        new_old_areas[1]=(state_t*)INT_OLDAREA;

        new_old_areas[2]=(state_t*)TLB_NEWAREA;           
        new_old_areas[2]->pc_epc=new_old_areas[2]->reg_t9=(memaddr)boot_tlb;
        new_old_areas[3]=(state_t*)TLB_OLDAREA;

        new_old_areas[4]=(state_t*)PGMTRAP_NEWAREA;       
        new_old_areas[4]->pc_epc=new_old_areas[4]->reg_t9=(memaddr)boot_prgtrap;
        new_old_areas[5]=(state_t*)PGMTRAP_OLDAREA;

        new_old_areas[6]=(state_t*)SYSBK_NEWAREA;         
        new_old_areas[6]->pc_epc=new_old_areas[6]->reg_t9=(memaddr)boot_syscall;
        new_old_areas[7]=(state_t*)SYSBK_OLDAREA;

   for   (i=0;i<7;i=i+2){

        new_old_areas[i]->reg_sp=RAMTOP;
        new_old_areas[i]->status=STATUS_NEWAREAS;
    }

                /*si può fare!!!!*/


/*inserire il timer*/

/*punto 2*/
    initPcbs();
    initASL();
/*punto 3 */
/*    extern int * Process_Count;
    extern int * Soft_Block_Count;
    extern pcb_t ** Ready_Queue;
    extern pcb_t ** Current_Process;

    extern  semd_t * vsem;
    int i;
    for (i=0;i<GET_NCPU;i++){
            Process_Count[i]=0;
            Soft_Block_Count[i]=0;
            Ready_Queue[i]=NULL;
            Current_Process[i]=NULL;
        }*/
/*punto 4 */
 /*   for (i=0;i<MAX_DEVICES;i=i+1){
            vsem[i].s_key=NULL;
            vsem[i].s_procQ=NULL;   
            vsem[i].s_next=NULL;    
        }
*/
/*punto 5*/
    pcb_t * tmp=allocPcb();
    if (tmp==NULL) PANIC();
    tmp->p_s.pc_epc=tmp->p_s.reg_t9=(memaddr)test;
    tmp->p_s.reg_sp=RAMTOP-FRAME_SIZE;
    tmp->p_s.status=DEFAULT_STATUS; 
  /*  Current_Process[0]=tmp;*/

/*punto 6 e punto 7 multicore 
    for (i=1;i<GET_NCPU;i=i+1){
            INITCPU(i,(memaddr)sinc,new_old_areas+i*AREAS_SIZE);
            HALT();
           scheduler(); }*/

    scheduler(tmp);
return 0; 
}
