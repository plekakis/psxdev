/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *  (C) Copyright 1995 Sony Computer Entertainment Inc. Tokyo, Japan.
 *                      All Rights Reserved
 *
 *	Sample Program Viewer (pcmenu, cdmenu)
 *	printerr.c:
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		Oct,20,1995	sachiko
 */

#include <sys/types.h>
#include "menu.h"
#include "errmsg.h"

void set_execno(int execno);
void set_datano(int datano);
void print_errmsg(int errno,MENU *menu,int lang);
/*
static char *make_exename(MENU *menu,char *fname);
static char *make_dname(MENU *menu,char *fname);
*/
char *make_exename(MENU *menu,char *fname);
char *make_dname(MENU *menu, char *fname, int n);

static int      execno;
static int      datano;

void set_execno(int eno)
{
        execno = eno;
}

void set_datano(int dno)
{
        datano = dno;
}

void print_errmsg(int errno,MENU *menu,int lang)
{
        char            fname[80];
        char            *msg;
        MENU            *mp;

        static char     *errmsgj[MAX_ERRNO] = {
        "実行に必要なファイルが、見つかりません.\n(%s)",
        "実行に必要なデータが、見つかりません.\n(%s)",
        "ファイルが読めません.\n(%s)",
        "ファイルが読めません.\n(%s)"
        };

        static char     *errmsge[MAX_ERRNO] = {
        "File is not found.  This can't be run.\n(%s)",
        "File is not found.  This can't be run.\n(%s)",
        "File can't be read. This can't be run.\n(%s)",
        "File can't be read. This can't be run.\n(%s)"
        };


        if ( lang==ENGLISH )
                msg = errmsge[errno-1];
        else
                msg = errmsgj[errno-1];

        mp = &menu[execno - 1];
        switch (errno) {
        case ERR_EXE_NOTFOUND   :
                KanjiFntPrint(msg,make_exename(mp,fname));
                break;
        case ERR_DATA_NOTFOUND  :
                KanjiFntPrint(msg,make_dname(mp,fname, datano));
                break;
        case ERR_EXE_NOTREAD    :
                KanjiFntPrint(msg,make_exename(mp,fname));
                break;
        case ERR_DATA_NOTREAD   :
                KanjiFntPrint(msg,make_dname(mp,fname, datano));
                break;
        }
}

#if 0
char *make_exename(MENU *menu,char *fname)
{
        int             i;
        MENU            *mp;

        i = execno;

        mp = &menu[execno-1];
        strcpy(fname,mp->root);
        for ( i=0; i<mp->depth; i++ )
                strcat(fname,mp->fname[i]);
        return(fname);
}
#endif

/* static char *make_exename(MENU *menu,char *fname) */
char *make_exename(MENU *menu,char *fname)
{
        int             i;

        strcpy(fname, menu->root);
        for ( i = 0; i < menu->depth; i++ )
                strcat(fname, menu->fname[i]);
        return(fname);
}

/* static char *make_dname(MENU *menu,char *fname) */
char *make_dname(MENU *menu, char *fname, int n)
{
        return(strcpy(fname, menu->dname[n]));
}

