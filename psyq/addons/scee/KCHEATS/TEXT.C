/*****************************************************************************/
/*****************************************************************************/
/** text.c ** for new memory card project thingy **   							 **/
/*****************************************************************************/
/*****************************************************************************/

/** 8.5.95 **/

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
#include <libsn.h>
#include <libsnd.h>

#include "main.h"
#include "card.h"
#include "ctrller.h"
/**-------------------------------------------------------------------------**/


extern DB	 db[2];		/* packet double buffer */
extern DB*   cdb;			/* current db */


/****************************************************************************/
/** display text string using	my own font data                             **/
/****************************************************************************/

dave_text(char* tex,int xpos,int ypos)
{
int counter=0;
int x,y;
int fntxoffset = 576;
int fntyoffset = 256+2;

RECT source_buffer;

do{
	y=fntyoffset + 14*((int )(*tex-32)/8);
	x=fntxoffset + 8*((int )(*tex-32)%8);

	setRECT(&source_buffer,x,y,8,11);

	/*if the character is to be printed on a printable area of the screen then */
	/*go ahead and print it                                                    */

	if ( (cdb->disp.disp.x+xpos+((counter)*(8))<310) && (cdb->disp.disp.y+ypos<cdb->disp.disp.y+cdb->disp.disp.h))
		{
		MoveImage(&source_buffer,cdb->disp.disp.x+xpos+((counter)*(8)),cdb->disp.disp.y+ypos);
		}
		counter++;
		tex++;
	}while(*tex!=NULL); 
}


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
dave_text2(char* tex,int xpos,int ypos)
{
char text_buffer[32];

center_string(tex ,&text_buffer[0],sizeof(text_buffer),' ');

new_text(&text_buffer[0],xpos,ypos,0);


}
/**-------------------------------------------------------------------------**/



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
dave_text3(char* tex,int xpos,int ypos)
{
char text_buffer[32];

center_string(tex ,&text_buffer[0],sizeof(text_buffer),' ');

new_text(&text_buffer[0],xpos,ypos,1);


}
/**-------------------------------------------------------------------------**/




/**-------------------------------------------------------------------------**/
/**---center_string()-------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/** this function places the input string into the output string with an    **/
/** equal number of leading and trailing pad chars                          **/
/**-------------------------------------------------------------------------**/

center_string(char input_string[] ,char output_string[],int string_length, char pad)
{
int start_offset;
int counter;
int in_counter;


/* the start offset is the length of the output string minus the length of   */
/* the output string divided by two */

start_offset = (string_length-strlen(input_string))/2;

in_counter=0;

for(counter=0; counter<string_length; counter++)
   {
   /*  printf("counter %d ",counter);   */

   if (counter<start_offset)
      {
      output_string[counter]=pad;
      }                        
   else
   if(input_string[in_counter]!=NULL)
      {
      output_string[counter]=input_string[in_counter];
      in_counter++;
      }
   else
      {
      output_string[counter]=pad;
      }
   /*printf(" out %c\n",output_string[counter]);*/
   }

output_string[string_length-1]=NULL;

/*printf("done! \n");*/
}
/**-------------------------------------------------------------------------**/





/**-------------------------------------------------------------------------**/
/**-- new_text(char* tex,int xpos,int ypos)                               --**/                               
/**-------------------------------------------------------------------------**/

new_text(char* tex,int xpos,int ypos,int type)
{
int counter=0;
int x,y;
int fntxoffset = 0; /*320; */
int fntyoffset = 2; /*256+2; */

RECT source_buffer;


do{
   cdb->text[counter].u0 = fntxoffset + 8*((int )(*tex-32)%8);     cdb->text[counter].v0 = fntyoffset + 14*((int )(*tex-32)/8);
   cdb->text[counter].u1 = fntxoffset + 8*((int )(*tex-32)%8)+8;   cdb->text[counter].v1 = fntyoffset + 14*((int )(*tex-32)/8);
   cdb->text[counter].u2 = fntxoffset + 8*((int )(*tex-32)%8);     cdb->text[counter].v2 = fntyoffset + 14*((int )(*tex-32)/8)+14;
   cdb->text[counter].u3 = fntxoffset + 8*((int )(*tex-32)%8)+8;   cdb->text[counter].v3 = fntyoffset + 14*((int )(*tex-32)/8)+14;

   cdb->text[counter].x0 = xpos+(8*counter);     cdb->text[counter].y0 = ypos;
   cdb->text[counter].x1 = xpos+(8*counter)+8;   cdb->text[counter].y1 = ypos;
   cdb->text[counter].x2 = xpos+(8*counter);     cdb->text[counter].y2 = ypos+14;
   cdb->text[counter].x3 = xpos+(8*counter)+8;   cdb->text[counter].y3 = ypos+14;

   if(type==0)
      {
      AddPrim(cdb->ot+3,&cdb->text[counter]); 
      }
   else
      {
      DrawPrim(&cdb->text[counter]); 
      }
   
	counter++;
	tex++;
	}while(counter!=32); 

}
/**-------------------------------------------------------------------------**/






/**-------------------------------------------------------------------------**/
/**-- init_text_prim(DB *db)                                              --**/
/**-------------------------------------------------------------------------**/

init_text_prim(DB *db)
{
int counter;

for(counter=0; counter<32; counter++)
	{
	SetPolyFT4(&db->text[counter]);	
	db->text[counter].r0 = 128;
	db->text[counter].g0 = 128;
	db->text[counter].b0 = 128;
	db->text[counter].tpage = GetTPage(2,1,576,256);	
	SetShadeTex(&db->text[counter],1);
	SetSemiTrans(&db->text[counter],0);
	}
}
/**-------------------------------------------------------------------------**/
