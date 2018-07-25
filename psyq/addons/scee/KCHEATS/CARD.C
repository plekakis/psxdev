/**-------------------------------------------------------------------------**/
/**-- card.c                                                              --**/
/**-------------------------------------------------------------------------**/




#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libcd.h>

#include "card.h"
#include "ctrller.h"


extern unsigned long 	ev0,ev1,ev2,ev3,ev10,ev11,ev12,ev13;
extern struct	DIRENTRY card_dir[15];	

extern int elapsed_time;




/**----------------------------------------------------------------------**/
/** init the card system                                                 **/
/**----------------------------------------------------------------------**/
init_cards()
{

EnterCriticalSection();

ev0  = OpenEvent(SwCARD,EvSpIOE,EvMdNOINTR,NULL);
ev1  = OpenEvent(SwCARD,EvSpERROR,EvMdNOINTR,NULL);
ev2  = OpenEvent(SwCARD,EvSpTIMOUT,EvMdNOINTR,NULL);
ev3  = OpenEvent(SwCARD,EvSpNEW,EvMdNOINTR,NULL);
ev10 = OpenEvent(HwCARD,EvSpIOE,EvMdNOINTR,NULL);
ev11 = OpenEvent(HwCARD,EvSpERROR,EvMdNOINTR,NULL);
ev12 = OpenEvent(HwCARD,EvSpTIMOUT,EvMdNOINTR,NULL);
ev13 = OpenEvent(HwCARD,EvSpNEW,EvMdNOINTR,NULL);  

ExitCriticalSection();

EnableEvent(ev0);
EnableEvent(ev1);
EnableEvent(ev2);
EnableEvent(ev3);
EnableEvent(ev10);
EnableEvent(ev11);
EnableEvent(ev12);
EnableEvent(ev13);	


}




/***************************************************************************/
/** test_card                                                             **/
/***************************************************************************/

/* test card.c
 * Detecting a card (blocking function)

	Function: 	long TestCard( long chan )
	Arguments:	chan	the target slot
	Return Value:	

         0:No card detected
			1:Detected a formatted card
			2:Detected a newly connected card and marked it
			3:Communication error happened
			4:Detected an unformatted card

	possible values for chan

	chan 0x00 0x01 0x02 0x03  
	chan 0x10 0x11 0x12 0x13  

*/


test_card(long chan)
{
long ret;
char buf[128];	/* internal data buffer for format test */

/* call _card_info() */
while(_card_info(chan)==0);
ret = _card_event();
/* ret contains result of _card_info() */
if(ret==0) 
	{
	#ifdef VERB
	printf("Exist\n");
	#endif
	return 1;
	}
else 
if(ret==1) 
	{
	#ifdef VERB
	printf("Bad card\n");
	#endif
	return 3;
	}

else if(ret==2) 
	{
	#ifdef VERB
	printf("No card\n");
	#endif
	return 0;
	}
else if(ret==3) 
	{
	/* detected a newly connected card */
	/* mark the card for future test by _card_info() */
	_clear_event_x();
	_card_clear(chan);
	ret = _card_event_x();
	}

/* test format */
_clear_event_x();
_card_read(chan,0,buf);
ret = _card_event_x();
if(ret==1) 
	{
	#ifdef VERB
		printf("Bad card\n");
	#endif
	return 3;
	}
else if(ret==2) 
	{
	#ifdef VERB
		printf("No card\n");
	#endif
	return 0;
	}

#ifdef VERB
	printf(newly connected card)
#endif

if(buf[0]=='M' && buf[1]=='C') 
	{
	#ifdef VERB
		printf( (formatted)\n);
	#endif
	return 2;
	}
else 
	{
	#ifdef VERB
		printf(" (unformated)\n");
	#endif
	return 4;
	}
}
/******************************************************************************/






/**----------------------------------------------------------------------**/
/**-------------- status ------------------------------------------------**/
/**------------- this is taken from cman.c  3.3 -------------------------**/
/**----------------------------------------------------------------------**/

status()
{
int ret;

_card_info(0x00);
ret = _card_event();

switch(ret) 
	{
	case IOE:
		break;
		goto done;
	case TIMEOUT:
		goto done;
		break;
	case NEWCARD:
		_clear_event_x();
		_card_clear(0x00);
   	ret = _card_event_x();
		break;
	case ERROR:
		default:
		break;
	}
_clear_event();
_card_load(0x00);	  
ret = _card_event();

done:	
return ret;
}


/**------------------------------------------------------------------------**/
/** card_present_status                                                    **/
/**------------------------------------------------------------------------**/

/** this function returns 1 if there is a card plugged in or 0 if there is **/
/** not. its handy because it is very quick                                **/

card_present_status(int card)
{
int ret;

_card_info(card);
ret = _card_event();

if (ret == 2)    /* no card */
	return 0;
else 
	return 1;
}



/**------------------------------------------------------------------------**/
/** check for software events                                              **/
/**------------------------------------------------------------------------**/
_card_event()
{
while(1) 
	{
	/* you could get stuck in this loop forever */
	if(TestEvent(ev0)==1) { /* IOE */ 		return 0; }
   if(TestEvent(ev1)==1) { /* ERROR */		return 1; }
   if(TestEvent(ev2)==1) { /* TIMEOUT */ 	return 2; }
  	if(TestEvent(ev3)==1) { /* NEW CARD */	return 3; }
   }
}

/**------------------------------------------------------------------------**/
/** clear software events                                                  **/
/**------------------------------------------------------------------------**/
_clear_event()
{
	TestEvent(ev0);
	TestEvent(ev1);
	TestEvent(ev2);
	TestEvent(ev3);
}


/**------------------------------------------------------------------------**/
/** check for hardware events                                              **/
/**------------------------------------------------------------------------**/
_card_event_x()
{
while(1) 
	{
	/* you could get stuck in this loop forever */
	if(TestEvent(ev10)==1) { /* IOE */ 			return 0; }
   if(TestEvent(ev11)==1) { /* ERROR */		return 1; }
   if(TestEvent(ev12)==1) { /* TIMEOUT */		return 2; }
   if(TestEvent(ev13)==1) { /* NEW CARD */	return 3; }
   }
}


/**------------------------------------------------------------------------**/
/** clear hardware events                                                  **/
/**------------------------------------------------------------------------**/
_clear_event_x()
{
	TestEvent(ev10);
	TestEvent(ev11);
	TestEvent(ev12);
	TestEvent(ev13);
}



/**------------------------------------------------------------------------**/
/**- get a list of all the files on a card --------------------------------**/
/**------------------------------------------------------------------------**/

dir_file(struct DIRENTRY *d)
{
long i;
char key[128];

extern struct DIRENTRY *firstfile(), *nextfile();

	strcpy(key,"bu00:");
	strcat(key,"*");

i = 0;
if(firstfile(key, d)==d) 
	{
  	do 
		{
   	i++;
		d++;
     	} 
		while(nextfile(d)==d);
   }
return i;
}





/**------------------------------------------------------------------------**/
/**-------------------- format_routine ------------------------------------**/
/**------------------------------------------------------------------------**/
format_routine()
{
/* returns a 0 on failure*/
return (format("bu00:"));
}
/**------------------------------------------------------------------------**/



/**------------------------------------------------------------------------**/
/**-------------------- delete_routine ------------------------------------**/
/**------------------------------------------------------------------------**/
delete_routine(int delete_selection)
{
char key[5+21];  /*device+filename max size*/

strcpy(key,"bu00:");
strcat(key,card_dir[delete_selection].name);
printf("delete_file : %s \n",key);

return (delete(&key));

}
/**------------------------------------------------------------------------**/


/**------------------------------------------------------------------------**/
/**---block_read ----------------------------------------------------------**/
/**------------------------------------------------------------------------**/



block_read(int blocks,char* buffer)
{

int 			fd;
char card_filename[21];
int i;

strcpy(card_filename,"bu00:");
strcat(card_filename,"testfile"); 

printf("\n");
printf("********************************************************\n");
printf("blocking read file: %s  blocks %d\n",card_filename,blocks);
printf("********************************************************\n");


printf("********>read begins!\n");
if((fd=open(card_filename,O_RDONLY))>=0)
	{
   printf("file open:: reading");
	i = read(fd,&buffer[0],8192*blocks);
	if(i!=(8192*blocks)) 
		{
		printf("Error failed whilst reading data\n"); 
		close(fd);
		return 0;
		}
 	close(fd);
	}
else
	{
	printf("error: could not open file!\n");
	return 0;
	}



}
/**------------------------------------------------------------------------**/
