#include "sort_def.h"

int main(int argc, char **argv) {
	int nproc, status;
	int tids[SLAVENUM], ok, mytid;
	nproc = pvm_spawn("sort_slave",0,PvmTaskDefault, "LINUX64",SLAVENUM, tids);
	printf("Master id %d\n", nproc );
    //app for sorting
    do{
    	status = 0;
        //if 0 - all done
        //if 1 - continue
    	for(int i = 0; i < SLAVENUM; i++) {
    		int slave_status ;
    		pvm_recv (-1 , TAG_SLAVE);
    		pvm_upkint(&slave_status, 1, 1);
    		if (slave_status == 0){
    			status = 1;
    		}
    	}
    	for (int i = 0; i < SLAVENUM; ++i) {
			pvm_initsend( PvmDataRaw );
			pvm_pkint(&status, 1, 1);
			pvm_send( tids[i], TAG_MASTER );    	
		}
	}while(status == 1);
	printf("Master end!");
	pvm_exit();
}
