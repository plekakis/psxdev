/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *  (C) Copyright 1995 Sony Computer Entertainment Inc. Tokyo, Japan.
 *                      All Rights Reserved
 *
 *	Sample Program Viewer (pcmenu, cdmenu)
 *	loadmenu.c:
 *
 *	 Version	Date		Design		memo
 *	------------------------------------------------------------
 *	1.00		Oct,20,1995	hatto
 *	1.01		Sep,12,1997	sachiko		add fdflg
 */

#include <sys/types.h>
#include <libetc.h>
#include <libcd.h>
#include <libapi.h>
#include <libgte.h>
#include <libgpu.h>
#include <malloc.h>
#include <string.h>
#include "menu.h"
#include "loadmenu.h"
#include "string.h"

int load_menu(MENU **menu, int *menu_max);
/* this function's directories max is 200 */
extern CdlFILE *CdSearchFile2(CdlFILE *fp, char *name);

#ifndef CDEXEC
static char *pc_read_menu(char *fname);
#else
static char *cd_read_menu(char *fname);
#endif

static int    alloc_menu(MENU **menu, int *menu_max);
static int    get_menu_max(char *buf);
static int    get_menu(char *buf, MENU **menu, char *fixroot);
static char  *bin_gets(char *s, int n, char *buf);
static int    get_2terms(char *s1, char *s2, char *buf);
static u_long tohex(char *str);
static void   reset_root(MENU *menu,int *menu_max,char *fixroot);
static int    chgctrl(char *s);
static char  *reform_path(char *s, char *root);

/* load menu file */
int load_menu(MENU **menu, int *menu_max)
{
    static char fname[] = MENUFILE;
    char *buf;
    char fixroot[81];

    *menu_max = 0;

    /* menu item number */
    *menu_max = 200;

    /* allocate of menu informations */
    if(alloc_menu(menu, menu_max) == -1) {
        *menu_max = 0;
        return -1;
    }

    /* read menu */
#ifndef CDEXEC
    if( (buf = pc_read_menu(fname)) == (char *)NULL) {
#else
    if( (buf = cd_read_menu(fname)) == (char *)NULL) {
#endif
        printf("LOAD_MENU: %s can't be opened. \n", fname); /* ERROR */

        return -1;
    }

    /* get item information */
    *menu_max = get_menu(buf, menu, fixroot);

    /* reset file name*/
    reset_root(*menu, menu_max, fixroot);

    free(buf);
    return 0;
}

static int get_menu_max(char *buf)
{
    char s[512], a1[512], a2[512];
    int i = 0, j, dno = 0;

    while((buf = bin_gets(s, 512, buf)) != (char *)NULL) {
        if(get_2terms(a1, a2, s) == 2) {
            if(dno) {                                 /* data */
                j++;
                if(dno == j) dno = 0;
            } else if(stricmp(a1, "-ITEM") == 0) {    /* item number */
            } else if(stricmp(a1, "-ROOT") == 0) {    /* root */
            } else if(stricmp(a1, "-FIXROOT") == 0) { /* fix root */
            } else if(stricmp(a1, "-SENTJ") == 0) {   /* Japanese */
            } else if(stricmp(a1, "-SENTE") == 0) {   /* English */
            } else if(stricmp(a1, "-DATA") == 0) {    /* data */
                if(i > 0) {
                    dno = atoi(a2);
                    j = 0;
                }
            } else {                                  /* exe file */
                i++;
            }
        }
    }

    return i;
}

static int get_menu(char *buf, MENU **menu, char *fixroot)
{
    char s[512], a1[512], a2[512];
    int i = 0, j, dno = 0;
    char root[81];

    strcpy(root,    "\\"); /* default ROOT    = "\" */
    strcpy(fixroot, "\\"); /* default FIXROOT = "\" */
    i = dno = 0;
    while((buf = bin_gets(s, 512, buf)) != (char *)NULL) {
        if(get_2terms(a1, a2, s) == 2) {
            if(dno) {                                 /* data */
                strcpy((*menu)[i - 1].dname[j],
                       reform_path(strupper(a1), root));
                    /* strupper(a1)); */
                (*menu)[i - 1].daddr[j] = tohex(a2);
                j++;
                if(dno == j) dno = 0;
            } else if(stricmp(a1, "-ITEM") == 0) {    /* item number(ignore) */
            } else if(stricmp(a1, "-ROOT") == 0) {    /* root */
                strcpy(root, strupper(a2));
            } else if(stricmp(a1, "-FIXROOT") == 0) { /* fix root */
                strcpy(fixroot, strupper(a2));
            } else if(stricmp(a1, "-DATA") == 0) {    /* data number */
                if(i > 0) {
                    (*menu)[i - 1].ndata = dno = atoi(a2);
                    j = 0;
                }
            } else if(stricmp(a1, "-SENTJ") == 0) {   /* Japanese */
                if(i > 0) {
                    strcpy((*menu)[i - 1].sentj, a2);
                    (*menu)[i - 1].sentj[160] = NULL;
                    chgctrl((*menu)[i-1].sentj);
                }
            } else if(stricmp(a1, "-SENTE") == 0) {   /* English */
                if(i > 0) {
                    strcpy((*menu)[i - 1].sente, a2);
                    (*menu)[i - 1].sente[160] = NULL;
                    chgctrl((*menu)[i-1].sente);
                }
            } else {                                  /* exe file */
                strcpy((*menu)[i].str, a1);
                chgctrl((*menu)[i].str);
                strcpy((*menu)[i].fname[0], strupper(a2));
                strcpy((*menu)[i].root,  root);
                strcpy((*menu)[i].sente, "");
                strcpy((*menu)[i].sentj, "");
                (*menu)[i].ndata = 0;
                i++;
            }
        }
    }

    return i;
}

static int alloc_menu(MENU **menu, int *menu_max)
{
    /* printf("MALLOC %d\n", (*menu_max + 1) * sizeof(MENU)); /* DEBUG */

    *menu = (MENU *)malloc((*menu_max + 1) * sizeof(MENU));
    if(*menu == NULL) {
        printf("Not Enough Memory.\n");
        *menu_max = 0;
        return -1;
    }

    return 0;
}

#ifndef CDEXEC
static char *pc_read_menu(char *fname)
{
    u_long fsize;
    int fd;
    char *buf, *buf2;

    /* open menu file */
    fd = PCopen(fname, 0, 0);
    if(fd == -1) {
        printf("PC_READ_MENU: Menu file (%s) not Found.\n", fname);
        return (char *)NULL;
    }
    fsize = PClseek(fd, 0L, 2);
    PClseek(fd, 0L, 0);

    /* allocate buffer and read menu file */
    buf = (char *)malloc((fsize + 1) * sizeof(char));
    if(buf == (char *)NULL) {
        printf("PC_READ_MENU: Not Enough Memory.\n");
        return (char *)NULL;
    }

    PCread(fd, buf, fsize);
    buf2 = buf + fsize;

    /* '\0' is end of file. */
    *buf2 = '\0';

    return buf;
}

#else

static char *cd_read_menu(char *fname)
{
    CdlFILE fp;
    CdlLOC  pos;
    char *buf, *buf2;

    CdInit();
    CdSetDebug(0);

    /* open menu file */
    if (CdSearchFile2(&fp, fname) == 0) {
        printf("CD_READ_MENU: Menu file (%s) not Found.\n", fname);
        return (char *)NULL;
    }

    pos = fp.pos;

    /* allocate buffer and read menu file */
    buf = (char *)malloc((fp.size + 1) * sizeof(char));
    if(buf == (char *)NULL) {
        printf("CD_READ_MENU: Not Enough Memory.\n");
        return (char *)NULL;
    }

    CdControl(CdlSetloc, (u_char *)&pos, 0);
    if(CdRead((fp.size + 2047)/2048, (u_long *)buf, CdlModeSpeed) == 0) {
        printf("CD_READ_MENU: Error READ %d.\n", (fp.size + 2047)/2048);
        /* DEBUG */
    }
    buf2 = buf + fp.size;
    while(CdReadSync(1, (unsigned char *)NULL) != 0); /* DEBUG */
    CdReadSync(0, 0);

    /* '\0' is end of file. */
    *buf2 = '\0';

    return buf;
}
#endif

/* get string from buffer */
static char *bin_gets(char *s, int n, char *buf)
{
    int i = 0;

    while(1) {
        if(*buf == '\0') {
            buf = (char *)NULL;
            break;
        }
        if(*buf != 0x0A && *buf != 0x0D) {
            if(i < n) {
                *s++ = *buf;
                i++;
            }
            buf++;
        } else {
            buf++;
            if(*buf == 0x0A || *buf == 0x0D) buf++;
            break;
        }
    }
    *s = '\0';

    return buf;
}


/* get two terms from buffer */
static int get_2terms(char *s1, char *s2, char *buf)
{
    int flag_dbquot;
    int n = 0;

    flag_dbquot = 0;
    while(*buf != '\0' && *buf == ' ') buf++;
    if(*buf != ';')
    while(*buf != '\0' && (*buf != ' ' || flag_dbquot) ) {
        if(*buf == '"') {
            if(!flag_dbquot) {
                flag_dbquot = 1;
                buf++;
            } else {
                buf++;
                break;
            }
        } else {
            *s1++ = *buf++;
            n = 1;
        }
    }
    *s1 = '\0';

    flag_dbquot = 0;
    while(*buf != '\0' && *buf == ' ') buf++;
    if(*buf != ';')
    while(*buf != '\0' && (*buf != ' ' || flag_dbquot) ) {
        if(*buf == '"') {
            if(!flag_dbquot) {
                flag_dbquot = 1;
                buf++;
            } else {
                buf++;
                break;
            }
        } else {
            *s2++ = *buf++;
            n = 2;
        }
    }
    *s2 = '\0';

    return n;
}

static u_long tohex(char *str)
{
    u_long l = 0, n;
    int len, i;
    char s;

    len = strlen(str);

    for(i = 0; i < len || i < 8 ;i++) {
        s = str[len - i - 1];

             if('0' <= s && s <= '9') n = s - '0';
        else if('A' <= s && s <= 'F') n = 10 + s - 'A';
        else if('a' <= s && s <= 'f') n = 10 + s - 'a';
        else n = 0;
        l += n << (i * 4);
    }

    return l;
}

static void reset_root(MENU *menu,int *menu_max,char *fixroot)
{
    MENU            *mp;		/* MENU pointer */
    int             mc;			/* MENU counter */
    int             i,j;		/* counter */
    int             depth;		/* the depth of tree */
    int             kind;
    char            buf[256],*str;	/* work */
    int             fdflg;		/* file=1 : dir=0 */
    
    for ( mp = menu, mc = 0; mc < *menu_max; ) {

        /* for dname*/
        for ( i = 0; i < mp->ndata; i++ ) {
            if(mp->dname[i][0] != '\\' && mp->dname[i][1] != ':') {
                strcpy(buf, mp->root);
                strcat(buf, mp->dname[i]);
                strcpy(mp->dname[i], buf);
            }
        }

        /* get ROOT*/
        strcpy(buf, mp->root);
        strcat(buf, mp->fname[0]);
        if ( !(str = strstr(buf, fixroot)) ) {
            for ( i = mc; i < *menu_max; i++ )
                memcpy(&(menu[i]), &(menu[i+1]), sizeof(MENU));
            --(*menu_max);
            continue;
        }
        strcpy(buf, str + strlen(fixroot));
        mp->root[str + strlen(fixroot) - buf] = NULL;

        /* make path for fname*/
        for ( depth = 0; depth < MAX_DIRDEPTH && str; depth++ ) {

            if ( (str = strchr(buf,'\\')) ) {
                /* set dir name*/
                strncpy(mp->fname[depth],buf,str-buf+1);
                mp->fname[depth][str-buf+1] = NULL;
                strcpy(buf,str+1);
	        fdflg = 0;
            } else {
		/* set file name*/
                strcpy(mp->fname[depth],buf);
                mp->fname[depth+1][0]=0;
                fdflg = 1;
	    }

            /* set kind*/
            for ( i = kind = 0; i < mc; i++ ) {
                for ( j = 0; j <= depth; j++ )
                    if ( strcmp(menu[i].fname[j], mp->fname[j]) ) break;

                if ( (j <= depth || menu[i].depth < depth) || fdflg ) {
                    if ( (menu[i].kind[depth] & 0x7fff) >= kind ) ++kind;
                } else {
                    menu[i].kind[depth] |= 0x8000;
                    mp->kind[depth] = menu[i].kind[depth];
                    break;
                }
/*
                if ( j <= depth || menu[i].depth < depth ) {
                    if ( (menu[i].kind[depth] & 0x7fff) >= kind ) ++kind;
                } else {
                    menu[i].kind[depth] |= 0x8000;
                    mp->kind[depth] = menu[i].kind[depth];
                    break;
                }
*/
            }
            if ( i >= mc )
                menu[i].kind[depth] = kind;
        }
        mp->depth = depth;
        ++mp;
        ++mc;
    }
}

/* replace escaped control code defined after '\' */
static int chgctrl(char *s)
{
	char *s0 = s;

        while (*s) {
                if (s[0] == '\\')
                if ((s0 == s) || ((s0 != s) && (s[-1] < 0x80))) {
                        s[0] = ' ';
                        switch (s[1]) {
                                case 'N': s[1] = '\n'; break;
                                case 'n': s[1] = '\n'; break;
                                case 'T': s[1] = '\t'; break;
                                case 't': s[1] = '\t'; break;
                        }
                }
                s++;
        }
}

/* change relative path to absolute */
static char *reform_path(char *s, char *root)
{
    char path[81], *p;
    char *s0 = s;
    int i, n = 0;

    while(strncmp(s, "..\\", 3) == 0) {
        s += 3;
        n++;
    }

    if(n) {

        strcpy(path, root);
        path[strlen(path) - 1] = '\0';
        for(i = 0; i < n; i++) {
            if((p = strrchr(path, '\\')) != (char *)NULL) {
                *p = '\0';
            } else {
                break;
            }
        }
        strcat(path, "\\");
        strcat(path, s);
        strcpy(s0, path);

    }

    return s0;
}

