#include "sort_def.h"

int main(int argc, char **argv) {
    int nproc, i;
    int tids[SLAVENUM], ok, mytid;

    nproc = pvm_spawn("sort_slave", 0, 0, "LINUX64", SLAVENUM, tids);
    mytid = pvm_mytid();
	
    // 2. zbieranie info od slavow
    int licznik = 0;
    for (i = 0; i < nproc; i++) { // odbieranie wiadomości
   
        pvm_recv(-1, TAG_SLAVE);
        pvm_upkint(&ok, 1, 1);
        printf("Master: ok %d \n", ok);
        if (ok == 1)
            licznik++;
    }
    if (licznik == SLAVENUM) {  // wyslij wiadomosc do slaveow, ze jest koniec sortowania

        for (i = 0; i < SLAVENUM; i++) {// wysłanie wiadomości
        
            pvm_initsend(PvmDataRaw);
            pvm_send(tids[i], TAG_MASTER);
        }
    }
	pvm_exit();
	return 0;
}
