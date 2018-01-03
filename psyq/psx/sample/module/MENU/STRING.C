/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *  (C) Copyright 1995 Sony Computer Entertainment Inc. Tokyo, Japan.
 *                      All Rights Reserved
 *
 *	Sample Program Viewer (pcmenu, cdmenu)
 *	string.c:
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		Oct,20,1995	hatto
 */

#include <ctype.h>
#include <strings.h>

char *strupper(char *s);
int   stricmp(char *s1, char *s2);

char *strupper(char *s)
{
    char *s0 = s;

    if(s == NULL) return NULL;

    while(*s) {
        *s = toupper(*s);
        *s++;
    }

    return s0;
}

int stricmp(char *s1, char *s2)
{
        if(s1 == NULL || s2 == NULL)  {
                if(s1 == s2) return(0);
                if(s1 == NULL) return (-1);
                return(1);
        }

        while(toupper(*s1) == toupper(*s2++))
                if(*s1++ == '\0')
                        return(0);
        return(*s1 - * --s2);
}

