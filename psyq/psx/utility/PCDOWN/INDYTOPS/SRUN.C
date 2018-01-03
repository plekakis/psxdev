/*
 *           Download&Exec Tool for Indy -> PlayStaion
 *           srun by izumi
 *
 *           use Serial Port No.2 (DIN 8) -> /dev/ttyd2
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#define BPS9600        9600           /* 9600bps */
#define BPS19200       19200          /* 19200bps */
#define BPS38400       38400          /* 38400bps */
#define BPS            BPS38400

#define PACKET_LEN     128            /* packet length */

#define PCKT_SUCCESS   1
#define PCKT_ERROR     2

#define RECEIVE_END    0x7e           /* end code */

#define TIMEOUT_MAX    10000          /* time out */

int  fs;
struct termios t;

char buf[2500];
char *bufp;

FILE	*fp;

char DAT_ID[] = {"DAT"};
char EXE_ID[] = {"EXE"};

unsigned long GetExecAdrs(FILE *fp);
int ConectCheck(char *id);
int SendLongData(unsigned long dat);
int PacketWrite(unsigned int chlen, unsigned int sum, char *buf);
void RSinit(int bps);
void RSputch(char c);
char RSgetch(void);
unsigned long atoh(char *s);

main(int argc, char *argv[])
{
  unsigned int	i, j;
  char	c;
  unsigned int chlen, sts;
  unsigned int sum = 0;
  unsigned long	ld_adrs;
  unsigned long	flen;
  unsigned int	pcktnum, remain;

  if(argc != 2) {
    printf("Usage: srun <file>\n");
    exit(1);
  }
  
  if(--argc > 0) {
    if((fp = fopen(*++argv, "rb")) == 0) {
      printf("Can not open %s\n", *argv);
      exit(1);
    }
  }

  fseek(fp, 0L, SEEK_END);
  flen = (unsigned long)ftell(fp);
  fseek(fp, 24L, SEEK_SET);
  ld_adrs = GetExecAdrs(fp);	/* get exec address */
  fseek(fp, 0L, SEEK_SET);
  pcktnum = (flen+PACKET_LEN-1)/PACKET_LEN;
  remain = (unsigned int)flen%PACKET_LEN;

  RSinit(BPS);
  printf("file size = %ld load address = 0x%lx\n", flen, ld_adrs);

  printf("Send start ID...\n");
  if(!ConectCheck(EXE_ID)) {		/* send start ID */
    printf("conect error!\n");
  }
  printf("Send load address...\n");
  if(!SendLongData(ld_adrs)) {	/* send load address */
    printf("Can not send load address!\n");
  }
  printf("Send file size...\n");
  if(!SendLongData(flen)) {	/* send file size */
    printf("Can not send file size!\n");
  }

  printf("Send packet data...\n");
  i = 0;
  while(1) {
    if(i >= pcktnum) break;	/* finish packet sending */
    fseek(fp, (unsigned long)(i)*PACKET_LEN, SEEK_SET);
    if((i==pcktnum-1)&&(remain)) {	/* end packet ? */
      memset(buf, 0x00, PACKET_LEN);
      for(j=0, sum=0; j<remain; j++) {
	buf[j] = fgetc(fp);
	sum += buf[j];	                /* check sum */
      }
      chlen = remain;		/* packet length */
    }
    else {
      for(j=0, sum=0; j<PACKET_LEN; j++) {
	buf[j] = fgetc(fp);
	sum += buf[j];	/* check sum */
      }
      chlen = (unsigned int)PACKET_LEN; /* packet length */
    }
    sum = (unsigned int)sum&0xff;
    sts = PacketWrite(chlen, sum, buf);	/* send packet */
/*  printf("#%d chlen=%d sum=%02x sts=%x\n", i, chlen, sum, sts&0xff);*/
    if(sts==PCKT_SUCCESS) i++;	/* success packect sending ? */
  }

  /* check end of data */
  printf("Data transfer finished.\n");
  sts = (unsigned int)RSgetch()&0xff;
/*  printf("end sts=%x\n",sts);*/
  if(sts==RECEIVE_END) printf("complete!\n");
  else printf("Download failed\n");
  fclose(fp);
  close(fs);
}

unsigned long GetExecAdrs(FILE *fp)
{
  int	i;
  unsigned long adrs = 0;
	
  for(i=0; i<4; i++) {
    adrs |= ((unsigned long)fgetc(fp)&0xff)<<(i*8);
  }
  return adrs;
}


int ConectCheck(char *id)
{
  int	sts;

  RSputch(id[0]);
  RSputch(id[1]);
  RSputch(id[2]);
  sts = (int)RSgetch()&0xff;
  if(sts) return(1);	        /* OK */
  else return(0);	        /* ERROR */
}

int SendLongData(unsigned long dat)
{
  int	sts;

  RSputch(dat>>24);
  RSputch(dat>>16);
  RSputch(dat>>8);
  RSputch(dat&0xff);
  sts = (int)RSgetch();
  if(sts) return(1);	        /* OK */
  else return(0);		/* ERROR */
}

int PacketWrite(unsigned int chlen, unsigned int sum, char *buf)
{
  int	i;
  int 	sts;
  int   timeout;
  
  chlen &= 0xff;
  sum &= 0xff;
  RSputch((char)chlen);
  RSputch((char)sum);
/*  printf("chlen=%d sum=%x\n", chlen, sum);*/
  for(i=0, timeout=0; i<chlen; i++, timeout++) {
    RSputch(buf[i]);
    if(timeout==TIMEOUT_MAX) {
      printf("Cannot write data from serial port.\n");
      fclose(fp);
      close(fs);
      exit(1);
    }
  }
  sts = (int)RSgetch();
  return(sts);
}

void RSinit(int bps)
{
  int  baud;
  int  line;
  
  fs = open( "/dev/ttyd2",O_RDWR );    /* Serial Port No.2 (DIN 8)*/

  if (fs < 0 ) {
    printf( "Can't open /dev/ttyd2\n" );
    exit( 1 );
  }
    
  if ( bps == BPS9600 ) baud = B9600;
  if ( bps == BPS19200 ) baud = B19200;
  if ( bps == BPS38400 ) baud = B38400;
  
  t.c_iflag = 0;
  t.c_oflag = 0;
  t.c_cflag = baud|CS8|CREAD;
  t.c_lflag = 0;
  ioctl(fs, TCSETS, &t);

  ioctl(fs, TIOCMGET, &line);
  line &= ~(TIOCM_RTS);
  ioctl(fs, TIOCMSET, &line);           /* RTS off */
}

void RSputch(char c)
{
  int  line;
  char wbuf[256];
  int  timeout;
  
  for(timeout=0; timeout<=TIMEOUT_MAX; timeout++) {
    if(timeout==TIMEOUT_MAX) {
      printf("CTS:time out\n");
      fclose(fp);
      close(fs);
      exit(1);
    }
    ioctl(fs, TIOCMGET, &line);
    if(line&0x20) break;                 /* CTS on ? */
  }

  wbuf[0] = c;
  write(fs, wbuf, 1);

}

char RSgetch(void)
{
  char c;
  int  sts;
  int  line;
  char rbuf[256];
  int  timeout;
  
  ioctl(fs, TIOCMGET, &line);
  line |= TIOCM_RTS;
  ioctl(fs, TIOCMSET, &line);               /* RTS on */
  
  for(timeout=0; timeout<=TIMEOUT_MAX; timeout++) {
    if(timeout==TIMEOUT_MAX) {
      printf("Cannot read data from serial port.\n");
      fclose(fp);
      close(fs);
      exit(1);
    }
    if(sts = read(fs, rbuf, 1) > 0) break;
    
  }

  ioctl(fs, TIOCMGET, &line);
  line &= ~(TIOCM_RTS);
  ioctl(fs, TIOCMSET, &line);               /* RTS off */

  c = rbuf[0];
  return c;
}

unsigned long atoh(char *s)
{
  int	i;
  char	*c;
  int	clen = 0;
  unsigned long	hex = 0;

  c = s;
  while(*c++) clen++;
  if(clen>=8) clen = 8;
  for(i=0; i<clen; i++) {
    hex |= (unsigned long)(s[i]&0x0f)<<(4*(clen-1-i));
  }
  return hex;
}
