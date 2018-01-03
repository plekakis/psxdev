#include <libsn.h>

main()
{
	int i,j;

	for(;;)
	{
		for(i=0;i<100;i++)
		{
			printf("This is a test message ");

			for(j=10000;j>0;j--)
				;

			pollhost();
		}
	}
}

