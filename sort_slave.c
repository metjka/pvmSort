#include "sort_def.h"

int main()
{
	int ptid, mytid; // parent TID, mój TID
	int  i;	// współrzędne x i y wylosowanego punktu, iterator
	int * pointsTab;

	srand( pvm_mytid() );	// włącznie generacji liczb losowych

	// 1 - tworzy tablicę z wartościami
	pointsTab = (int *)malloc(5*sizeof(int));
	for (i=0; i<5; i++) {
		pointsTab[i] = rand()%100;
		printf(" %d", pointsTab[i] );
	}

	// 2 - dołączenie do grupy i poczekanie na koniec zadań
	int ginst;
	ginst = pvm_joingroup(GROUP_NAME);
	printf("Czekam na barierze %s\n",GROUP_NAME);
	pvm_barrier(GROUP_NAME, SLAVENUM);
	printf("Ok, zaczynam prace %d\n", ginst);

	// 3 - wyszukanie najmniejszego elementu 
	int min = pointsTab[0];
	for (i=1; i<points; i++) {
		if ( pointsTab[i] < min )
			min = pointsTab[i];
	}
	printf("Slave: %d, Element minimalny:%d \n", ginst, min);

	// 4 - przesłanie elementu min do sąsiada po lewej stronie
	if (ginst != 0) {
		int tid = pvm_gettid( GROUP_NAME, ginst -1 );
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&min,1,1);
		pvm_send(tid, TAG_MIN);
		
	}
	
	int max_sa;
	// 5 - czekanie na odpowiedz z elementem maks od sąsiada
	if (ginst != (pvm_gsize( GROUP_NAME )) -1){
		pvm_recv(-1, TAG_MAX);
		pvm_upkint(&max_sa, 1, 1);
		printf("Slave: %d, odebrał max: %d \n",ginst,max);
	}
	int ok=0;
	// 6 - podmienienie elementu min z maks
	if(min < max_sa)
	{
           // podmiana          
    }
    else 
         ok=1;
    
    if( max > min_sa ) 
    {
     // podmiana
    }
    else
        ok=1;
           
    ptid= pvm_parent();
	
	pvm_initsend( PvmDataDefault);
	pvm_pkint( &ok, 1, 1);
	pvm_send(ptid, TAG_SLAVE);

    // 8 - odebranie od mastera infa czy koniec sortowania
    int status = pvm_nrecv(-1, TAG_MASTER);
    if(status >0){
            

	}
	pvm_exit();
}
