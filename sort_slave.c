#include "sort_def.h"
int sortStatus();
int main(){
	int ptid, mytid; // parent TID, mój TID
	int i; // wspólrzedne x i y wylosowanego punktu, iterator
	int * pointsTab;
	int points = 5;
	
	
	srand( pvm_mytid() ); // wlacznie generacji liczb losowych
	printf("I am slave and my id %d \n", pvm_mytid());
	//2
	int gid = pvm_joingroup(GROUP_NAME);
	printf("Waiting on barrier \'%s\'\n",GROUP_NAME);
	pvm_barrier(GROUP_NAME, SLAVENUM);
	printf("Ok, let`s do this.\nMy groun id: %d\n", gid);
	
	// 1 - creating og list
	pointsTab = (int *)malloc(5*sizeof(int));
	for (i=0; i<5; i++) {
		pointsTab[i] = rand()%100;
		printf(" %d", pointsTab[i] );
	}
	printf("\n");
    do{
    	int status = 1;
    	//1 nothing changed
    	//0 sth changed
    	printf("\n");
    	for (i=0; i<5; i++) {
			printf(" %d", pointsTab[i] );
		}
		printf("\n");
		//3.1 min value 
		int min = pointsTab[0];
		int min_index = 0;
		for (i=0; i<points; i++) {
			if ( pointsTab[i] < min ){
				min = pointsTab[i];
				min_index = i;
			}
		}
		
		//3.2 max value
		int max = pointsTab[0];
		int max_index = 0;
		for (i = 0; i < points; i++){
			if(pointsTab[i] > max){
				max = pointsTab[i];
				max_index = i;
			}
		}
		printf("Slave: %d, Element min:%d \n", gid, min);
		printf("Slave: %d, Element max:%d \n", gid, max);

		// 4.1 sending min value <<<---
		if (gid != 0) {
			printf("Slave: %d, sending MINVALUE (%d) to Slave: %d\n", gid, min, gid -1 );
			int tid = pvm_gettid( GROUP_NAME, (gid -1) );
			pvm_initsend(PvmDataDefault);
			pvm_pkint(&min,1,1);
			pvm_send(tid, TAG_MIN);
		}
		// 4.2 sending max value --->>>
		if (gid != pvm_gsize( GROUP_NAME) - 1){
			printf("Slave: %d, sending MAXVALUE (%d) to Slave: %d\n",gid, max,  gid + 1 );

			int tid = pvm_gettid( GROUP_NAME, (gid +1) );
			pvm_initsend(PvmDataDefault);
			pvm_pkint(&max,1,1);
			pvm_send(tid, TAG_MAX);
		}
		//5.1 getting max from left
		int max_sa;
		if (gid > 0){
			int tid = pvm_gettid( GROUP_NAME, gid - 1 );
			pvm_recv(tid, TAG_MAX);
			pvm_upkint(&max_sa, 1, 1);
			printf("Slave: %d, got from Slave: %d MAX value: %d \n",gid, (gid -1), max_sa);
			
			//6.1 swapping
			if (min < max_sa){
				pointsTab[min_index] = max_sa;
				status = 0;
			}
		}
		//5.2 getting min from right
		int min_sa;
		if (gid < pvm_gsize( GROUP_NAME ) -1){
			int tid = pvm_gettid( GROUP_NAME, gid + 1 );
			pvm_recv(tid, TAG_MIN);
			pvm_upkint(&min_sa, 1, 1);
			printf("Slave: %d, got from Slave: %d MIN value: %d \n", gid, (gid +1), min_sa);
			
			//6.2 swapping
			if (max > min_sa){
				pointsTab[max_index] = min_sa;
				status = 0;
			}
		}	
		//7 sending message to MASTER 
		pvm_initsend( PvmDataDefault);
		pvm_pkint( &status, 1, 1);
		pvm_send(pvm_parent(), TAG_SLAVE);

		printf("--------------------\n");
    } while(sortStatus()==1); //8 geting message from MASTER

	
	printf("Slave: %d, ended his work and his numbers are:\n",gid);

	for (i=0; i<5; i++) {
			printf(" %d", pointsTab[i] );
	}
	//end 
	printf("\n");
	printf("End of sorting!\n\n");
    pvm_exit();    
}


int sortStatus(){
	int status = 0;

	if(pvm_recv(-1, TAG_MASTER)){
		pvm_upkint(&status,1,1);
	}
	return status;
}