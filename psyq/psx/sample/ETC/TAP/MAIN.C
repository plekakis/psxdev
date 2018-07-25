/* $PSLibId: Run-time Library Release 4.4$ */
/*			MultiTap: sample program
 *
 *		Copyright (C) 1997 by Sony Corporation
 *			All rights Reserved
 */
#include <r3000.h>
#include <asm.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libtap.h>

#define BUF_MAX	34
#define PADMAX        2
#define PADBUFFLEN    34
#define PADID_GUN     0x31
#define PADID_CON     0x41
#define PADID_MOUSE   0x12
#define PADID_NEGI    0x23
#define PADID_ANAJ    0x53
#define PADID_GCON    0x63
#define PADID_ANAC    0x73
#define PADID_MULTI   0x80

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
} DB;

typedef struct  {
        char            buff[PADMAX][PADBUFFLEN];
        u_short         data[PADMAX];
} PAD;

static	PAD   p;
static err_cnt[2]={0,0};
static u_long _PadRead(PAD *);

main()
{
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current double buffer */
	int	i, cnt;	/* work */
	
	ResetCallback();
	InitTAP(&p.buff[0][0], PADBUFFLEN, &p.buff[1][0], PADBUFFLEN);
	StartTAP();
	ChangeClearPAD(0);
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);
	
	/* init font environment */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 740));

	db[0].draw.isbg = db[1].draw.isbg = 1;
	
	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	while(1){
		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */
		FntPrint("padd = %08x\n",_PadRead(&p));
		DrawSync(0);		/* wait for end of drawing */
		cnt = VSync(-1);		/* check for count */
		VSync(0);		/* wait for V-BLNK */
		PutDispEnv(&cdb->disp); /* update display environment */
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		FntPrint("total time = %d\n", cnt);
		FntFlush(-1);
	}
	StopTAP();
}


/*
 * Read controll-pad
 */
static
u_long _PadRead(PAD *p)
{
        u_short         data;
        u_char          id;
        short           i,j,k;

        for(i=0; i<PADMAX; i++)
        {
                if(p->buff[i][0] != 0)
                {
                        id = 0;
			err_cnt[i]++;
                }else
                {
                        id = p->buff[i][1];
                }
		FntPrint("ID=%02x\n",id);

                switch(id)
                {
                        case PADID_MULTI :
                                data = 0;
        			for(j=0; j<4; j++)
				{
					{
        					for(k=0; k<8; k++)
						{
							FntPrint("%02x ",(u_char)p->buff[i][j*8+k+2]);
						}
						FntPrint("\n");
					}
				}
                                break;
                        case PADID_CON :
                        case PADID_GUN :
                                data = (u_short)((((p->buff[i][2]&0xff)<<8))|(p->buff[i][3])&0xff);
				data = ~data&0xffff;
                                break;
                        case PADID_MOUSE :
                                data = (u_short)((((p->buff[i][2]&0xff)<<8))|(p->buff[i][3])&0xff);
				data = ~data&0xffff;
                                if(!(p->buff[i][3]&0x04))       data |= 0x40;
                                if(!(p->buff[i][3]&0x08))       data |= 0x20;
				FntPrint("%02x ",(u_char)(p->buff[i][4])&0xff);
				FntPrint("%02x ",(u_char)(p->buff[i][5])&0xff);
				FntPrint("\n");
				break;
                        case PADID_ANAJ :
                        case PADID_ANAC :
			case PADID_NEGI :
			case PADID_GCON :
                                data = (u_short)((((p->buff[i][2]&0xff)<<8))|(p->buff[i][3])&0xff);
				data = ~data&0xffff;
				FntPrint("%02x ",(u_char)(p->buff[i][4])&0xff);
				FntPrint("%02x ",(u_char)(p->buff[i][5])&0xff);
				FntPrint("%02x ",(u_char)(p->buff[i][6])&0xff);
				FntPrint("%02x ",(u_char)(p->buff[i][7])&0xff);
				FntPrint("\n");
                                break;
                        default :
                                data    = 0;
                }

                p->data[i] = data;
		FntPrint("err_cnt(%d) :%d\n", i, err_cnt[i]);
        }
	return (p->data[0]&0xffff)|(((p->data[1])<<16)&0xffff0000);
}
