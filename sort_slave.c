#include "sort_def.h"

int main()
{
	int ptid, mytid; // parent TID, mój TID
	int i; // wspólrzedne x i y wylosowanego punktu, iterator
	int * pointsTab;
	int points = 5;
	srand( pvm_mytid() ); // wlacznie generacji liczb losowych

	// 1 - tworzy tablice z wartosciami
	pointsTab = (int *)malloc(5*sizeof(int));
	for (i=0; i<5; i++) {
		pointsTab[i] = rand()%100;
		printf(" %d", pointsTab[i] );
		printf("\n");
	}

	// 2 - dolaczenie do grupy i poczekanie na koniec zadan
	int ginst;
	ginst = pvm_joingroup(GROUP_NAME);
	printf("Czekam na barierze %s\n",GROUP_NAME);
	pvm_barrier(GROUP_NAME, SLAVENUM);
	printf("Ok, zaczynam prace %d\n", ginst);

	// 3.1 - wyszukanie najmniejszego elementu 
	int min = pointsTab[0];
	int ind_min = 0;
	for (i=1; i<points; i++) {
		if ( pointsTab[i] < min ) {
			min = pointsTab[i];
			ind_min = i;
		}
	}
	printf("Slave: %d, Element minimalny:%d \n", ginst, min);
	
	// 3.2 - wyszukanie najwiekszego elementu 
	int max = pointsTab[0];
	int ind_max = 0;
	for (i=1; i<points; i++) {
		if ( pointsTab[i] > min ) {
			max = pointsTab[i];
			ind_max = i;
		}
	}
	printf("Slave: %d, Element maxymalny:%d \n", ginst, max);

	// 4.1 - przeslanie elementu min do sasiada po lewej stronie
	if (ginst != 0) {
	int tid = pvm_gettid( GROUP_NAME, ginst -1 );
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&min,1,1);
		pvm_send(tid, TAG_MIN);
	}
	
	// 4.2 - przeslanie elementu max do sasiada po prawej stronie
	if (ginst != pvm_gsize( GROUP_NAME ) -1) {
	int tid = pvm_gettid( GROUP_NAME, ginst -1 );
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&max,1,1);
		pvm_send(tid, TAG_MAX);
	}

	int max_sa;
	// 5.1 - czekanie na odpowiedz z elementem maks od sasiada
	if (ginst != (pvm_gsize( GROUP_NAME )) -1){
		pvm_recv(-1, TAG_MAX);
		pvm_upkint(&max_sa, 1, 1);
		printf("Slave: %d, odebral max: %d \n", ginst, max_sa);
	}
	int min_sa;
	// 5.2 - czekanie na odpowiedz z elementem min od sasiada
	if (ginst != 0){
		pvm_recv(-1, TAG_MIN);
		pvm_upkint(&min_sa, 1, 1);
		printf("Slave: %d, odebral min: %d \n", ginst, min_sa);
	}
	
	int ok;	
	// 6 - podmienienie elementu min z maks
	if(min < max_sa)
	{
		// podmiana 
		pointsTab[ind_min] = max_sa;
		ok = 0;
	}
	else 
		ok=1;

	if( max > min_sa ) 
	{
		// podmiana
		pointsTab[ind_max] = min_sa;
		ok = 0;
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
		printf("suc");
        pvm_exit();
		exit(0);
	}
	pvm_exit();
}
