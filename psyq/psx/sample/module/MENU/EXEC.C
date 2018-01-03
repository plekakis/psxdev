/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *  (C) Copyright 1995 Sony Computer Entertainment Inc. Tokyo, Japan.
 *                      All Rights Reserved
 *
 *	Sample Program Viewer (pcmenu, cdmenu)
 *	exec.c:
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		Mar,09,1995	yoshi
 *	2.00		Oct,20,1995	hatto
 *	2.01		Apr,19,1996	hatto
 */

#include <sys/types.h>
#include <libetc.h>
#include <libcd.h>
#include <libapi.h>
#include <libgte.h>
#include <libgpu.h>
#include "menu.h"
#include "errmsg.h"

int exec(MENU *menu);
/* this function's directories max is 200 */
extern CdlFILE *CdSearchFile2(CdlFILE *fp, char *name);

#ifndef CDEXEC
static int pc_load_data(char *fname, char *addr);
#else
static int cd_load_data(char *fname, char *addr);
static int cd_read(CdlLOC *pos, u_long *buf, int nbyte);
static char *cd_fname(char *fname);
#endif

extern void set_datano(int datano); /* from "printerr.c" */
extern char *make_exename(MENU *menu,char *fname);
extern char *make_dname(MENU *menu,char *fname, int n);

#ifndef CDEXEC
/* load exe file and data files from IBM-PC and execute */
int exec(MENU *menu)
{
    static u_long headbuf[2048 / 4];
    static struct XF_HDR *head = (struct XF_HDR *)headbuf;
    RECT          rec = {640, 0, 1023, 511};
    int           fd;
    char          fname[81];
    u_long        size;
    u_long        fsize;
    int           i, j;

    sndEnd();                /* sound end */

    /* load data */
    for(i = 0; i < menu->ndata; i++) {
        make_dname(menu, fname, i);

        /* printf("DATA NAME:%s\n", fname);*/
                                     /* DEBUG */

        if ( (j = pc_load_data(fname, (char *)menu->daddr[i])) != NO_ERR) {
            set_datano(i);
            return j;
        }
    }

    PCinit();

    make_exename(menu, fname);

    /* frame buffer clear */
    if(stricmp(menu->str,"SCREEN")!=0) {
        ClearImage(&rec, 0, 0, 0);
        while(DrawSync(1) != 0);
    }

    if ((fd = PCopen(fname, 0, 0)) == -1) {
        printf("PCEXEC: %s can't be opened.\n", fname);

        return ERR_EXE_NOTFOUND;
    }

    fsize = PClseek(fd, 0, 2);
    PClseek(fd, 0, 0);

    if (PCread(fd, (char *)head, 2048) != 2048) {
        printf("PCEXEC: %s can't be read.\n", fname);

        PCclose(fd);
        return ERR_EXE_NOTREAD;
    }

    if ( (size = PCread(fd, (char *)head->exec.t_addr, fsize - 2048))
            != fsize - 2048 ) {

        PCclose(fd);
        return ERR_EXE_NOTREAD;
    }

    printf("PCEXEC: %s (%d,%d/%d) read\n",
            fname, fsize - 2048, head->exec.t_size, size); /* DEBUG */

    PCclose(fd);

    head->exec.s_addr = 0;
    head->exec.s_size = 0;

    printf("exec by (pc,t_addr,t_size)= %08x,%08x,%08x\n",
        fname, head->exec.pc0, head->exec.t_addr,
        head->exec.t_size);

    printf("executing...\n\n", fname);

    SetDispMask(0);          /* display off */
    PadStop();               /* stop PAD */
    StopCallback();          /* stop callback */

    Exec(&head->exec, 1, 0); /* kick child */

    RestartCallback();       /* recover callback */
    VSync(0);
    PadInit(0);              /* recover PAD */
    sndInit();               /* sound start */
    SetDispMask(1);          /* display on */

    printf("..done\n", fname);

    return NO_ERR;
}

static int pc_load_data(char *fname, char *addr)
{
    int fd;
    u_long size2;
    u_long fsize;

    PCinit();
    if ((fd = PCopen(fname, 0, 0)) == -1) {
        printf("PC_LOAD_DATA: %s can't be opened.\n",fname);

        return ERR_DATA_NOTFOUND;
    }

    fsize = PClseek(fd, 0, 2);
    PClseek(fd, 0, 0);

    if( (size2 = PCread(fd, addr, fsize)) == fsize) {
        printf("PC_LOAD_DATA: %d Bytes Data Read.\n",fsize);
    } else {
        PCclose(fd);
        printf("PC_LOAD_DATA: Fail. %d(%d).\n",size2,fsize);

        return ERR_DATA_NOTREAD;
    }

    PCclose(fd);

    return NO_ERR;
}

#else

int exec(MENU *menu)
{
    static u_long headbuf[2048/4];
    static struct XF_HDR *head = (struct XF_HDR *)headbuf;
    RECT          rec = {640, 0, 1023, 511};
    int           fd;
    char          fname[81];
    u_long        size;
    u_long        fsize;
    int           i, j;

    CdlFILE       fp;
    CdlLOC        p0, p1;

    CdInit();
    CdSetDebug(0);

    sndEnd();                /* sound end */

    for(i = 0; i < menu->ndata; i++) {
        make_dname(menu, fname, i);
        if ( (j = cd_load_data(fname, (char *)menu->daddr[i])) != NO_ERR) {
            set_datano(i);
            return j;
        }
    }

    make_exename(menu, fname);

    /* clear frame buffer except "screen" */
    if(stricmp(menu->str, "SCREEN")!=0) {
        ClearImage(&rec, 0, 0, 0);
        while(DrawSync(1) != 0);
    }

    /* cd_exec(fname, (CdlLOC *)NULL); */
    if (CdSearchFile2(&fp, cd_fname(fname)) == 0) {
        printf("CD_EXEC %s: can't be opened.\n", cd_fname(fname));

        return ERR_EXE_NOTFOUND;
    }
    p0 = fp.pos;

    printf("exec at (%02x:%02x:%02x)\n", p0.minute, p0.second, p0.sector); /**/
    
    if(cd_read(&p0, (u_long *)head, 2048) == 0) {
        return ERR_EXE_NOTFOUND;
    }
    CdReadSync(0, 0);
        
    head->exec.s_addr = 0;
    head->exec.s_size = 0;
    
    printf("exec by (pc,t_addr,t_size)= %08x,%08x,%08x\n",
           fname, head->exec.pc0, head->exec.t_addr, head->exec.t_size);
    
    CdIntToPos(CdPosToInt(&p0)+1, &p1);
    if(cd_read(&p1, (u_long *)head->exec.t_addr, head->exec.t_size) == 0) {
        return ERR_EXE_NOTREAD;
    }
    CdReadSync(0, 0);
    
    printf("executing...\n\n", fname); /**/

    SetDispMask(0);        /* display off */
    PadStop();             /* stop PAD */
    StopCallback();        /* stop callback */

    Exec(&head->exec, 1, 0);

    RestartCallback();     /* recover callback */

    PadInit(0);            /* recover PAD */
    SetDispMask(1);        /* display on */
    sndInit();             /* souund start */

    printf("..done\n", fname); /**/
    return NO_ERR;
}

static int cd_load_data(char *fname, char *addr)
{
    CdlFILE fp;
    CdlLOC  p0;

    if (CdSearchFile2(&fp, cd_fname(fname)) == 0) {
        printf("CD_LOAD_DATA: %s can't be opened.\n", cd_fname(fname));
        return ERR_DATA_NOTFOUND;
    }
    p0 = fp.pos;

    if(cd_read(&p0, (u_long *)addr, fp.size) == 0) {
        printf("CD_LOAD_DATA: %s can't be read.\n", cd_fname(fname));
        return ERR_DATA_NOTREAD;
    }
    CdReadSync(0, 0);

    return NO_ERR;
}

static int cd_read(CdlLOC *pos, u_long *buf, int nbyte)
{
    printf("cd_read(%02x:%02x:%02x, %08x, %d)\n",
           pos->minute, pos->second, pos->sector, buf, nbyte);
    
    CdControl(CdlSetloc, (u_char *)pos, 0);
    return CdRead((nbyte+2047)/2048, buf, CdlModeSpeed);
}

static char *cd_fname(char *fname)
{
    char buf[81]; /**/

    if(fname[1] == ':') { /* cut drive name */
        strcpy(buf, fname + 2);
        strcpy(fname, buf);
    }
    strcat(fname, ";1");

#ifdef DEBUG
    printf("DEBUG: \"%s\"\n", fname);
#endif

    return fname;
}

#endif

