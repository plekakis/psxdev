/**********************************************************
	pcbload.exe by izumi
	bcc -Iinclude -Llib -v -mt -epcbload pcbload.c rs.asm

	Feb 6, 1998 - Mike Koziniak replaced call to atoh( )
	with strtoul().
***********************************************************/

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <conio.h>
#include "rs.h"

extern void out_port(int port, int data);
extern char in_port(int port);

char buf[2500];
char *bufp;

FILE	*fp;

char* stop_point;

char DAT_ID[] = {"DAT"};
char EXE_ID[] = {"EXE"};

void main(int argc, char *argv[]);
int ConectCheck(char *id);
int SendLongData(unsigned long dat);
int PacketWrite(unsigned int chlen, unsigned int sum, char *buf);
void RSinit(long bps);
char RSputch(char c);
char RSgetch(void);


void main(int argc, char *argv[])
{
	unsigned int	i, j;
	char	c;
	unsigned int chlen, sts;
	unsigned int sum = 0;
	unsigned long	ld_adrs;
	unsigned long	flen;
	unsigned int	pcktnum, remain;

	if(--argc > 0) {
		if((fp = fopen(*++argv, "rb")) == 0) {
			printf("Can not open %s\n", *argv);
			_exit(0);
		}
	}

	
        if(--argc > 0) ld_adrs = strtoul(*++argv, stop_point, BASE); /* MK 2-6-98 */
	else {
		printf("Nothing load address\n");
		_exit(0);
	}
	fseek(fp, 0L, SEEK_END);
	flen = (unsigned long)ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	pcktnum = (flen+PACKET_LEN-1)/PACKET_LEN;
	remain = (unsigned int)flen%PACKET_LEN;

	RSinit(BPS);
        printf("file size = %ld load address = 0x%lx\n", flen, ld_adrs); 

	printf("Send start ID...\n");
	if(!ConectCheck(DAT_ID)) {		/* send Start ID */
		printf("conect error!\n");
	}
	printf("Send load address...\n");
	if(!SendLongData(ld_adrs)) {		/* send Load Address */
		printf("Can not send load address!\n");
	}
	printf("Send file size...\n");
	if(!SendLongData(flen)) {		/* send File Size */
		printf("Can not send file size!\n");
	}

	printf("Send packet data...\n");
	i = 0;
	while(1) {
		if(i >= pcktnum) break;		/* finish sending packect? */
		fseek(fp, (unsigned long)(i)*PACKET_LEN, SEEK_SET);
		if((i==pcktnum-1)&&(remain)) {	/* Ending packetÅH */
			memset(buf, 0x00, PACKET_LEN);
			for(j=0, sum=0; j<remain; j++) {
				buf[j] = fgetc(fp);
				sum += buf[j];	/* value of check sum */
			}
			chlen = remain;		/* packet length */
		}
		else {
			for(j=0, sum=0; j<PACKET_LEN; j++) {
				buf[j] = fgetc(fp);
				sum += buf[j];	/* value of check sum */
			}
			chlen = (unsigned int)PACKET_LEN; /* packet length */
		}
		sum = (unsigned int)sum&0xff;
		sts = PacketWrite(chlen, sum, buf);	/* send packet */
/*		printf("#%d chlen=%d sum=%02x sts=%x\n", i, chlen, sum, sts&0xff);*/
		if(sts==PCKT_SUCCESS) i++; /* success of sending pakect? */
	}

	/* wait end code */
	printf("Data transfer finished.\n");
	sts = (unsigned int)RSgetch()&0xff;
/*	printf("end sts=%x\n",sts);*/
	if(sts==RECEIVE_END) printf("complete!\n");
	else printf("Download failed\n");
	fclose(fp);
	_exit(0);
}

int ConectCheck(char *id)
{
	int	sts;

	RSputch(id[0]);
	RSputch(id[1]);
	RSputch(id[2]);
	sts = (int)RSgetch()&0xff;
	if(sts) return(1);	/* OK */
	else return(0);		/* ERROR */
}

int SendLongData(unsigned long dat)
{
	int	sts;

	RSputch(dat>>24);
	RSputch(dat>>16);
	RSputch(dat>>8);
	RSputch(dat&0xff);
	sts = (int)RSgetch();
	if(sts) return(1);	/* OK */
	else return(0);		/* ERROR */
}

int PacketWrite(unsigned int chlen, unsigned int sum, char *buf)
{
	int	i;
	int 	sts;

	chlen &= 0xff;
	sum &= 0xff;
	RSputch((char)chlen);
	RSputch((char)sum);
/*	printf("chlen=%d sum=%x\n", chlen, sum);*/
	for(i=0; i<chlen; i++) {
		if(kbhit()) {
			printf("exit:PacketWrite()\n");
			fclose(fp);
			_exit(0);
		}
		RSputch(buf[i]);
	}
	sts = (int)RSgetch();
	return(sts);
}

void RSinit(long bps)
{
	int	r;
	int	sts;

	/* set RS232C mode */
	r = 384 / (bps / 300);		/* calculate board rate */
					/* 300Å`115200bps */

	out_port(LCR, 0x80);		/* board rate setting mode */
	out_port(DLL, r&0x00ff);	/* set lower byte */
	out_port(DLM, r>>8);		/* set higher byte */

	out_port(LCR, N81MODE);		/* N81, 1/16 mode */

/*	out_port(IER, 0x01);		/* RxRDY interrupt enable */

	out_port(MCR, 0x08);		/* OUT2:on RTS:off DTR:off */

}

char RSputch(char c)
{
	while(!((in_port(MSR) & 0x10)&&(in_port(LSR) & 0x20))) {
		/* CTS:on & Transferd Data Register is empty? */
		if(kbhit()) {
			printf("exit:RSputch()\n");
			fclose(fp);
			_exit(0);
		}
	}
	out_port(TDB, c);		/* output data */
	return c;
}

char RSgetch(void)
{
	char	c;
	int	sts;

	out_port(MCR, 0x0a);		/* OUT2:on RTS:on DTR:off */
	while (!(in_port(LSR) & 0x01)) {	/* Received Data? */
		if(kbhit()) {
			printf("exit:RSgetch()\n");
			fclose(fp);
			_exit(0);
		}
	}
	c = in_port(RDB);		/* input data */
	out_port(MCR, 0x08);		/* OUT2:on RTS:off DTR:off */
	return c;
}

