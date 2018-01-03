/*
	rs.h
*/


#define	BUF_MAX		0x2000		/* size of received buffer */
#define	FLOW_ON		0x1800		/* flow control:on */
#define	FLOW_OFF	0x0800		/* flow control:off */

#define	BPS9600		9600		/* 9600bps */
#define	BPS19200	19200		/* 19200bps */
#define	BPS38400	38400		/* 38400bps */
#define	BPS57600	57600		/* 57600bps */
#define	BPS115200	115200		/* 115200bps */
#define	BPS		BPS57600

#define	N81MODE		0x03		/* Parity None, 8bit, Stop bit:1,
					   1/16 Mode Data */
#define	ERRS_ON		0x0B		/* ERROR, RTS ON */
#define	ERRS_OFF	0x08		/* ERROR, RTS OFF */

#define	MHI		0x0021	/* control 8259 master hardware interrupt */

#define	COM1_PORT	0x03f8		/* RS-232C Port0:I/O address */
#define	COM2_PORT	0x02f8		/* RS-232C Port1:I/O address */
#define	COM3_PORT	0x3220		/* RS-232C Port2:I/O address */
#define	COM4_PORT	0x3228		/* RS-232C Port3:I/O address */

#define	RSport		COM1_PORT	/* use COM1 Port */

#define	RDB		RSport+0	/* Read Register */
#define	TDB		RSport+0	/* Write Register */
#define	IER		RSport+1	/* Interrupt Enable Register */
#define	LCR		RSport+3	/* Line Control Register */
#define	MCR		RSport+4	/* Modem Control Register */
#define	LSR		RSport+5	/* Line Status Register */
#define	MSR		RSport+6	/* Modem Status Register */
#define	DLL		RSport+0	/* */
#define	DLM		RSport+1	/* */

#define	PACKET_LEN	128		/* data length of packet */

#define	PCKT_SUCCESS	1
#define	PCKT_ERROR	2

#define	RECEIVE_END	0x7e		/* end code */

