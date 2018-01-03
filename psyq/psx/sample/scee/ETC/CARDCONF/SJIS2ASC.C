
/****************************************************************************/
/** Shift-JIS to ASCII code look up table for speicial characters          **/ 
/** DC 15/1/95                                                             **/
/****************************************************************************/
/**************************************************************************** 
History:
		9/2/98 - added type cast@ line 113 to prevent compile error - B.D.(SCEA)

*****************************************************************************/
								
unsigned short sjischar[]=
{
0x8140,		/*   */
0x8149,		/* ! */
0x8168,		/* " */
0x8194,		/* # */
0x8190,		/* $ */
0x8193,		/* % */
0x8195,		/* & */
0x8166,		/* ' */
0x8169,		/* ( */
0x816a,		/* ) */
0x8196,		/* * */
0x817b,		/* + */
0x8143,		/* , */
0x817c,		/* - */
0x8144,		/* . */
0x815e,		/* / */
0x8146,		/* : */
0x8147,		/* ; */
0x8171,		/* < */
0x8181,		/* = */
0x8172,		/* > */
0x8148,		/* ? */
0x8197,		/* @ */
0x816d,		/* [ */
0x818f,		/* \ */
0x816e,		/* ] */
0x814f,		/* ^ */
0x8151,		/* _ */
0x8165,		/* ` */
0x816f,		/* { */
0x8162,		/* | */
0x8170,		/* } */
0x8150		/* ~ */
};

char ascchar[]=
{
' ' ,
'!' ,
'"' ,
'#' ,
'$' ,
'%' ,
'&' ,
'"' ,
'(' ,
')' ,
'*' ,
'+' ,
',' ,
'-' ,
'.' ,
'/' ,
':' ,
';' ,
'<' ,
'=' ,
'>' ,
'?' ,
'@' ,
'[' ,
'\\' ,
']' ,
'^' ,
'_' ,
'`' ,
'{' ,
'|' ,
'}' ,
'~'
}; 
  



typedef struct
{
char byte1;
char byte2;
}DCHAR;

typedef union
{
unsigned short sjis_char;
DCHAR double_char;
}SJIS_CHAR_BUFFER;



/** DAVES HORRID DIRTY LITTLE CONVERTOR **/

sjis2ascii(unsigned short sjis_code)
{
unsigned char ascii_code=-1; /*FF*/
int i;

char temp_byte;
SJIS_CHAR_BUFFER temp;



/*printf("incoming code = %x ",sjis_code);/**/

temp=(SJIS_CHAR_BUFFER)sjis_code;

temp_byte = temp.double_char.byte2;
temp.double_char.byte2 = temp.double_char.byte1;
temp.double_char.byte1 = temp_byte;

/* if there is a string terminator quit*/
if (temp.double_char.byte2 == 0 || temp.double_char.byte1 ==0)
	{	
	return 0;
	}


sjis_code = temp.sjis_char;

/* printf(" modified = %x ",sjis_code);/**/

	if ((sjis_code >= 0x8260) && (sjis_code <= 0x8279))
		{
		/* A..Z */
		/* A =65 Z =90 */

		ascii_code = 65 + (sjis_code - 0x8260);
		}

	else
	if ((sjis_code >= 0x8281) && (sjis_code <= 0x829A))
		{
		/* a..z */
		/* a =97 z =122*/
		ascii_code = 97 + (sjis_code - 0x8281);
		}

	else
	if ((sjis_code >= 0x824F) && (sjis_code <= 0x8258))
		{
		/* 0..9 */
		/* 0=48 9=57*/
		ascii_code = 48 + (sjis_code - 0x824F);
		}
	else

	for(i=0; i<33; i++)				  /*special chars use look up table */
		{
		if (sjischar[i] ==  sjis_code)
			ascii_code = ascchar[i];	
		}

/*	printf("returned code = %X %c\n",ascii_code,ascii_code);/**/

	return(ascii_code);
}


















































