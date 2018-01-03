#include <stdlib.h>

main()
{
	void *stock[100];
	long i;
	
	printf("InitHeap3()\n");
	InitHeap3(0x80100000,0x80000);

	printf("start of malloc3\n");
	for(i=0;i<20;i++)
		if((stock[i]=malloc3(0x8000))==NULL)
			printf("%3d:allocation error\n",i);
		else
			printf("%3d:%08x\n",i,stock[i]);
	printf("start of free\n");
	for(i=0;i<20;i++)
		if(stock[i]!=NULL)
			free3(stock[i]);
	printf("end of test\n");
}

