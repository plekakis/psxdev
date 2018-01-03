/*$PSLibId: Run-time Library Release 4.4$*/
/****************************************************************************
 *
 *     This Sample demonstrates the 3d sound key-on series of functions
 *     included in libsnd of Run-time Library 4.1.
 *  
 *  Copyright (C) 1997 Sony Computer Entertainment Inc. All rights reserved.
 *
 *  This software is furnished to you under a license by Sony Computer
 *  Entertainment Inc and must be used only in accordance with the terms and
 *  conditions of that license.
 *
 *  This software is provided as an example that demonstrates a particular
 *  feature/function of the licensed product. The software is not intended to
 *  provide a complete solution to any application problem but rather is
 *  provided as a teaching mechanism.
 *
 *  Developers may incorporate any part of this sample into their own source
 *  after making appropriate changes to make it suitable for the intended
 *  application.
 ****************************************************************************/

 
/*
 * Includes, Defines, Global Variables and Function prototypes.
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h>
#include <libspu.h>
#include <libsn.h> 

/* defines */
#define VAB_HADDR1   0x80020000L          /* sound effect vab header address */
#define VAB_BADDR1   0x80025000L            /* sound effect vab data address */

#define MVOL         127                                      /* main volume */
#define START_X      10                                  /* ball start point */
#define START_Y      50                                  /* ball start point */
#define XWID         30                                /* between ball width */

#define AMBIENT 0
#define SPATIAL 1                 
#define SPU_SOUND 2   
            		  /* SPATIAL POSITIONING GRID FOLLOWS 0=CENTER */  
#define cntr 0                                            
#define frnt 1        /*  	2 | 1 | 8 			*/  
#define lft_frnt 2    /*   	---------			*/
#define lft 3         /*   	3 | 0 | 7 			*/
#define lft_rear 4    /*   	---------    		        */
#define rear 5        /*   	4 | 5 | 6 			*/
#define rt_rear 6                                             
#define rt 7
#define rt_frnt 8

/* For Primitive Buffer */
#define OTSIZE       16                            /* size of ordering table */
#define FTSIZE       100                              /* size of font sprite */

/* Limitations */
#define    FRAME_X        320                        /* frame size (320x240) */
#define    FRAME_Y        240


/* 
 * GLOBAL VARIABLES 
 */

/* For Primitive Buffer */
typedef struct {
      DRAWENV        draw;                            /* drawing environment */
      DISPENV        disp;                            /* display environment */
      u_long         ot[OTSIZE];                           /* ordering table */
      int id;
} DB;

/* Menu info */
char *title = "SOUND MENU    ";
static char *menu[] = {   "Key On Amb", "Key On Spatial", "Move Spat Loc",
"Key On Spu", "Dec Amb Vol ", "Inc Amb Vol ", "Toggle Spu Rev", "Toggle Amb Rev",
 "All Key Off",0};

short vab ;                                     /* sound effect vab data ids */

/* ambient sound effect data - bits 0-7 bits = fine tuning of key on note, 
 *8-15 bits = key on note, 16-23 = program number containing s.e. 
 */ 
long ambient_data = (0x3B)<<8;        

/* spatial sound effect data - bits 0-7 bits= fine tuning of key on note, 
 * 8-15 bits= key on note, 16-23 = program number containing s.e. 
 */
long spatial_data = (1<<16) | (0x3C<<8); 
short  amb_vol_left, amb_vol_right;
long ambient_voices=0L;   /* bitfield containing voice data for ambient tones*/
long spatial_voices=0L;   /* bitfield containing voice data for spatial tones*/
char spu_reverb =0;             /* is reverb being applied to spu voice 12?? */
char ambient_reverb =1;       /* is reverb being applied to ambient effects? */
short spatial_location = 0;              /* set spatial s.e. to center value */
short spatial_volume_1_left[9]= {0x1fff, 0x1fff, 0x1fff, 0x1fff, 0x222,
0x222,   1,      1,   0x222};
short spatial_volume_1_rt[9]=   {0x1fff, 0x1fff, 0x222,  1,      0x111,
0x222,   0x222,  0x1fff, 0x1fff};
short spatial_volume_2_left[9]= {0x1fff, 0x222, -0x222,
0x1fff,-0x1fff,0x1fff  ,1,     -1,   1};
short spatial_volume_2_rt[9] =  {0x1fff,-0x222,  0x111, -1,
0x222,-0x1fff, -0x1fff, 0x1fff,-0x111};


/* 
 * FUNCTIONS 
 */
void  init_prim(DB *);
int   pad_read();
long SeriesKeyOn(short);
void SeriesKeyOff(long , short);
short ChangeReverb(long, short , char );
short ChangeVolume(long, short, SndVolume2);
void SpuVoiceStart(void);

main()
{
      static    RECT bg = {0, 0, 640, 480}; 
      DB   db[2];                                           /* double buffer */
      DB   *cdb;                                    /* current double buffer */
      u_long    *ot;                                           /* current OT */
      int  i;                                                        /* work */
      short i2;                                                     /* work  */
      int padd;                                                 /* pad input */
      SndVolume2  snd_vol;
      short left_amb_vc;
      short right_amb_vc;
 
      ResetCallback();
      ResetGraph(0);              /* reset graphic subsystem (0:cold,1:warm) */
      SsInit();                         /* initialize libsnd - also contains */
                                   /* ..call to initialize libspu internally */
      SsUtSetReverbType(SS_REV_TYPE_SPACE);
      SsUtReverbOn();
      FntLoad(960, 256);                                        /* load font */
 
      menuInit(0, 20, 20, 256);                 /* initialize menu in menu.c */
      db[0].id = FntOpen(32, 80, 320, 200, 0, 512);
      db[1].id = FntOpen(32, 80, 320, 200, 0, 512);
 
        PadInit(0);             /* reset PAD */
      SetGraphDebug(0);         /* set debug mode (0:off, 1:monitor, 2:dump) */
 
      SsSetTickMode(SS_TICK120);  /* set sound callback to trigger at 1/120s */
                                             /* ..- will be based on RCntCNT2*/
      SsSetReservedVoice(12);        /* reserve voices 0-11 for libsnd voice */
                                       /* ..allocation system; voices 12- 23 */
                                             /* ..for libspu or SsUtKeyOnV() */
      /*
       * Transfer sound data from main RAM to SPU RAM
       */
      /* open sound effect vab header - header must reside in main RAM */
      if ((vab = SsVabOpenHead ((u_char *)VAB_HADDR1, -1))<0)
      {
        printf("SsVabOpenHead : failed!\n");
        return;
      }
 
      /* transfer sound effect vab body or waveform data - can be removed
       * from main RAM 
       */
      if (SsVabTransBody ((u_char*)VAB_BADDR1, vab) != vab)
      {
        printf ("SsVabTransBody : failed!\n");
        return;
      }
 
      /* wait for transfer to complete 
       */
      SsVabTransCompleted (SS_WAIT_COMPLETED);

      /*
       * initialize environment for double buffer
       */
      SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
      SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
      SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
      SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);
      init_prim(&db[0]);                  /* initialize primitive buffers #0 */
      init_prim(&db[1]);                  /* initialize primitive buffers #1 */
 
      /* display 
       */
      ClearImage(&bg, 0, 0, 0);
      SetDispMask(1);             /* enable to display (0:inhibit, 1:enable) */
 
      /* set main volume 
       */
      SsSetMVol(MVOL, MVOL);
 
      /* start sound playback by allowing sound callback 
       */
      SsStart();
 
      SsUtSetReverbDepth(127,127);       /* wait to set depth to avoid noise */

        while (((padd = PadRead(1))&PADselect) == 0) {
           cdb  = (cdb==db)? db+1: db;              /* swap double buffer ID */
           ClearOTag(cdb->ot, OTSIZE);               /* clear ordering table */
           
           /* set up amb voices for volume and reverb change 
            */
           if (ambient_voices !=0)
           {
                left_amb_vc = -1;
                for (i2=0; i2<12; i2++)
                {
                 if (1<<i2 & ambient_voices)
                  if (left_amb_vc == -1)
                  {
                     SsUtGetDetVVol(i2, &amb_vol_left, &amb_vol_right); 
                     /* right not used yet*/
                     left_amb_vc = i2;
                  }
                  else
                  {
                     right_amb_vc = i2;
                     SsUtGetDetVVol(i2, &amb_vol_right, &amb_vol_right);
                     /* left not used, fill right twice - its ok! bit of a hack */
                  }
                }
           }
           switch (menuUpdate(title, menu, padd))
           {
               case 0:    /* amb key on */
                     if (ambient_voices>0) SeriesKeyOff(ambient_voices, AMBIENT);
                     ambient_voices = SeriesKeyOn(0);
                     break;
               case 1:                                       /* spat key on  */
                     if (spatial_voices>0) SeriesKeyOff(spatial_voices, SPATIAL);
                     spatial_voices = SeriesKeyOn(1);
                     break;
                case 2:     /* move spat loc */
                     if (spatial_location<8) spatial_location++;
                     else spatial_location = 0;
                     break;
               case 3:     /* spu key on */
                     SpuVoiceStart();
                     break;
               case 4:       /* dec amb vol */
                     if ((ambient_voices>0) && (amb_vol_left>127))
                     {
                         SsUtSetDetVVol(left_amb_vc, amb_vol_left-100,0);
                         SsUtSetDetVVol(right_amb_vc, 0, amb_vol_right-100);
                     }
                     break;
               case 5:   /* inc amb vol */
                     if ((ambient_voices>0) && (amb_vol_left<0x2fff))
                     {
                               snd_vol.left = amb_vol_left+100;
                               snd_vol.right = 0;
                               ChangeVolume(left_amb_vc, AMBIENT, snd_vol);
                               snd_vol.right = amb_vol_right+100;
                               snd_vol.left = 0;
                               ChangeVolume(right_amb_vc, AMBIENT,snd_vol);
                     }
                     break;
               case 6:            /* toggle spu rev */
                     if (spu_reverb == 0) spu_reverb = 1;
                          else spu_reverb = 0;
                          ChangeReverb(1<<12, SPU_SOUND, spu_reverb);
                          break;
               case 7:        /* toggle amb rev  */
                     if (ambient_voices !=0)
                     {
   		                  if (ambient_reverb == 1) ambient_reverb = 0;
                          else ambient_reverb = 1;
                          ChangeReverb(ambient_voices, AMBIENT, ambient_reverb);
                     }
                     break;
               case 8:   /* all key off */
                     if (ambient_voices>0) SeriesKeyOff(ambient_voices, AMBIENT);
                     if (spatial_voices>0) SeriesKeyOff(spatial_voices, SPATIAL);
                     SpuSetKey(SPU_OFF, SPU_12CH);
                     break;
           }
           FntPrint(cdb->id,"\n\n\n\n\n\n");
           FntPrint(cdb->id,"Spu Reverb = %s\n", (spu_reverb==0)?"Off":"On");
           FntPrint(cdb->id,"Amb Reverb = %s\n", (ambient_reverb==0)?"Off":"On");
           FntPrint(cdb->id,"Cur Spat Loc = ");
           switch (spatial_location)
           {
                case cntr: FntPrint(cdb->id,"cntr"); break;
                case frnt: FntPrint(cdb->id,"frnt"); break;
                case lft_frnt:  FntPrint(cdb->id,"lft frnt"); break;
                case lft: FntPrint(cdb->id,"lft"); break;
                case lft_rear: FntPrint(cdb->id,"lft rear"); break;
                case rear: FntPrint(cdb->id,"rear"); break;
                case rt_rear: FntPrint(cdb->id,"rt rear"); break;
                case rt: FntPrint(cdb->id,"rt"); break;
                case rt_frnt: FntPrint(cdb->id,"rt frnt"); break;
           }
           FntPrint(cdb->id,"\n\n");
           FntPrint(cdb->id,"Cur Amb LVol = %d\n",amb_vol_left);
           FntPrint(cdb->id,"Cur Amb RVol = %d\n",amb_vol_right);
           FntFlush(cdb->id);
           DrawSync(0);                           /* wait for end of drawing */
           VSync(0);
           PutDispEnv(&cdb->disp);             /* update display environment */
           PutDrawEnv(&cdb->draw);             /* update drawing environment */
           DrawOTag(cdb->ot);
      }
      SsVabClose(vab);                                     /* close vab data */
      SsEnd();                                           /* sound system end */
      SsQuit();
 
      PadStop();              
      ResetGraph(3);              /* reset graphic subsystem (0:cold,1:warm) */
      StopCallback();
      return 0;
}

void init_prim(db)
DB    *db;
{
      db->draw.isbg = 1;
      setRGB0(&db->draw, 60, 120, 120);
}
 
/* ------------------------------------------------------------------------
 *  FUNCTION :  SpuVoiceStart (void);
 *  DESCRIPTION :Sets up voice attributes for and keys on SPU voice 12
 *  ARGS: None
 *  RET: None
 * ----------------------------------------------------------------------- */

void SpuVoiceStart(void)
{
SpuVoiceAttr s_attr;
VagAtr vag_atr;
      s_attr.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_PITCH |
                    SPU_VOICE_WDSA | SPU_VOICE_ADSR_ADSR2 | SPU_VOICE_ADSR_ADSR1;
      s_attr.volume.left = 
	s_attr.volume.right = 0x1fff;/* set left and right*/
                                              /* ..vol to median sound value */
      s_attr.pitch = 0xE00;               /* set pitch to play back slightly */
                                             /* ..slower than 44100 sample   */
      s_attr.addr = SsUtGetVagAddrFromTone(vab,2,0);/* get waveform start adr*/
      SsUtGetVagAtr(vab,2,0, &vag_atr);                     /* get tone info */
      s_attr.adsr1 = vag_atr.adsr1;            /* use tone info to set adsr1 */
                                                        /* ..(envelope info) */
      s_attr.adsr2 = vag_atr.adsr2;           /* use tone info to set adsr2  */
                                                        /* ..(envelope info) */
      SpuNSetVoiceAttr(12,&s_attr);           /* set attributes for voice in */
                                                       /* ..libspu internals */
      SpuSetKey(SPU_ON, SPU_12CH);                           /* key on voice */
	  return;
}

/* ------------------------------------------------------------------------
 *  FUNCTION :  SeriesKeyOff (voices, type);  
 *  DESCRIPTION : Keys off voices which were originally keyed on by libsnd 
 *		alternate key on series functions. Confirms voices contain 
 *              correct tone data
 *		by calling SsVoiceCheck	().
 *  ARGS :long  voices - bitfield for voices to be keyed off
 *        short type   - are the voices ambient or spatial? allows for proper
 *        tone setup
 *  RET: None
 *------------------------------------------------------------------------ */
void SeriesKeyOff(long voices, short type)
{
short i;
long vabid;
short note;
      if (type == AMBIENT)
      {
       vabid = (ambient_data >>16) & 0xFF;
       note = (ambient_data>>8) & 0xFF;
       ambient_voices = 0;                            /* clear ambient voies */
      }
      else if (type == SPATIAL)
      {
       vabid = (spatial_data >>16) & 0xFF;
       note = (spatial_data>>8) & 0xFF;
       spatial_voices = 0;                           /* clear spatial voices */
      }
      for (i=0; i<24; i++)
      {
           if ((1<<i & voices) &&   (SsVoiceCheck(i, vabid, note) == 0))            
           /* confirm voice contains proper tone data and has not been
            * reallocated by libsnd voice allocation system  
            */
            SsUtKeyOffV(i);                                 /* key off voice */
 
  
    }
      return;
}

/*------------------------------------------------------------------------
 *  FUNCTION :  SeriesKeyOn (sound_effect_num);
 *  DESCRIPTION : Keys on selected tones via libsnd alternate key on series.
 *  ARGS : short sound_effect_num - desired sound effect number
 *  RET: long  - bitfield of voices set in the key on queue by the libsnd 
 *               voice allocation system 
 *		   - (-1) failure; less than two tones found to key in and 
 *                        sound effects are in pairs	
 *		   - (-2) failure; voice allocation already in process, 
 *                        voices cannot currently be allocated
 *		   - (-3) failure; not enough voices at specified priority 
 *                        available 	
 *----------------------------------------------------------------------- */
long SeriesKeyOn(short sound_effect_num)
{
VagAtr          vagatr_2, vagatr_1;
SndVoiceStats   SVS_tone_one, SVS_tone_two;
SndRegisterAttr SRA_tone_one, SRA_tone_two;
unsigned char i;                                                    /* work  */
short prog;                   /* program containing tone to key on - derived */
                                      /* ..from spatial_data or ambient_data */
short actual_prog;        /* to fill the prog_actual member of SndVoiceStats */
                           /* ..structure - used as an offset to locate tone */
                          /* ..data - differs from "real" program # - unused */
                              /* ..progs do not count toward "actual" prog # */
                                 /* ex: if progs 0-21 and 120-127 have data, */
                               /* ..the rest do not. real prog # of 127==127 */
                                               /* ..actual prog # of 127==30 */
short note;                        /* note at which to key on - derived from */
                                           /* ..spatial_data or ambient_data */
short fine;                /* semitone adjustment of note at which to key on */
                            /* ..- derived from spatial_data or ambient_data */
#define INIT_VAL_1 99
unsigned char tone_num_1 = INIT_VAL_1; /* tone # 0-15 of first tone in pair  */
unsigned char tone_num_2 = INIT_VAL_1; /* tone # 0-15 of second tone in pair */
unsigned long reverb_voices = 0L;            /* bitfield for which voices to */
                                          /* ..apply or remove reverb effect */
unsigned long voice_num_1 = INIT_VAL_1;        /* voice allocated for tone 1 */
unsigned long voice_num_2;                     /* voice allocated for tone 2 */
long voices_allocated;                              /* bitfield return value */
  
/* parse out prog, note, and fine data */

      if (sound_effect_num == 0 )  /* ambient */
      {
           prog = ambient_data >> 16;
           note = (ambient_data>> 8)  & 0xff;
           fine = ambient_data & 0xff;
      }
      else /* spatial */
      {
           prog = spatial_data >> 16;
           note = (spatial_data>> 8)  & 0xff;
           fine = spatial_data & 0xff;
      }  

      /* derive actual program # */
      actual_prog = SsGetActualProgFromProg(vab,prog); 
 
      /* Get first tone of appropriate program  */
      SsUtGetVagAtr(vab, prog, 0, &vagatr_2 );
 
      /* check tones min and max versus note - should tone key on? */
      for (i = 0; i < 16; i++)
      {
           if ((vagatr_2.min<=note) && (vagatr_2.max>=note))
           {
                if (tone_num_1 == INIT_VAL_1)
                {
                     vagatr_1 = vagatr_2;
                     tone_num_1 = i;
                }
                else
                {
                     tone_num_2 = i;
                     break;
                }
           }   
           /* get next tone if two tones not found yet */
           if (i<15) SsUtGetVagAtr(vab, prog, i+1, &vagatr_2 ); 
      }              
      /* failure - less than two tones in note range */
      if (tone_num_2 == INIT_VAL_1) return(-1); 
 
/* 
 * Set the sndregisterattrs members 
 */ 
      /* set the register masks  */
           SRA_tone_two.mask =
		 SRA_tone_one.mask = (SND_VOLL | SND_VOLR | SND_ADSR1 |
                                         SND_ADSR2 | SND_ADDR | SND_PITCH); 
   	   /* 
   	    * first tone 
   	    */
      /* set waveform data start addr - should be derived from this
        ...function or stored in array after vab load */
      SRA_tone_one.addr = SsUtGetVagAddrFromTone(vab,prog,tone_num_1)>>3;    
      
      /* set adsr1*/
      SRA_tone_one.adsr1 = vagatr_1.adsr1;     
      
      /* set adsr2 */
      SRA_tone_one.adsr2 = vagatr_1.adsr2;   
      
      /* calculate pitch for first tone - could substitute use user-defined
       * pitch look-up table */
      SRA_tone_one.pitch = SsPitchFromNote(note, fine, vagatr_1.center,vagatr_1.shift);
 
      /* set volumes - look up tables used here since objects not moved - 
        ...could emulate libsnd vol calc, use slightly faster version of same, 
        ...or create own spatial oriented vol calc function */
      if (sound_effect_num == 0)         /* ambient */
      {
           SRA_tone_one.volume.left = 0xfff;
           SRA_tone_one.volume.right = 0;
      }
      else   /* spatial */
      {
           SRA_tone_one.volume.left = spatial_volume_1_left[spatial_location];
           SRA_tone_one.volume.right = spatial_volume_1_rt[spatial_location];
      }
 
      /*  second tone */
      /* set waveform data start addr - should be derived from this
        ...function or stored in array after vab load */
      SRA_tone_two.addr = SsUtGetVagAddrFromTone(vab,prog,tone_num_2)>>3;  
      
      /* set adsr1 */
      SRA_tone_two.adsr1 = vagatr_2.adsr1;       
      
      /* set adsr2 */
      SRA_tone_two.adsr2 = vagatr_2.adsr2;     
      
      /* calculate pitch for right tone - could substitute use user-defined
       * pitch look-up table  */
      SRA_tone_two.pitch = SsPitchFromNote(note, fine, vagatr_2.center,vagatr_2.shift);
 
      /* set volumes - look up tables used here since objects not moved - 
        ...once again, could emulate libsnd vol calc, use slightly faster version of same, 
        ...or create own spatial oriented vol calc function */
      if (sound_effect_num ==0)            /* ambient */
      {
           SRA_tone_two.volume.left = 0;
           SRA_tone_two.volume.right = 0xfff;
      }
      else            /* spatial  */
      {
           SRA_tone_two.volume.left =spatial_volume_2_left[spatial_location];
           SRA_tone_two.volume.right =spatial_volume_2_rt[spatial_location];
      }

/*
 *	fill sndvoicestats structure for 1st tone - this allows volumes and 
 *  ...pans to be changed later in libsnd  
 */
 
      SVS_tone_one.vagId = vagatr_1.vag;
      SVS_tone_one.vabId = vab;
      SVS_tone_one.pitch = SRA_tone_one.pitch;
      SVS_tone_one.note = note;
	  /* volume set to greater of two volumes for tone */
      SVS_tone_one.vol = (SRA_tone_one.volume.left>SRA_tone_one.volume.right)?
         SRA_tone_one.volume.left:SRA_tone_one.volume.right;
	  
	  SVS_tone_one.pan = (SRA_tone_one.volume.left>SRA_tone_one.volume.right)?
		 SRA_tone_one.volume.right*64/SRA_tone_one.volume.left
		  :(0x3fff-SRA_tone_one.volume.left)*64/SRA_tone_one.volume.right;
		  /* 0x3fff value taken from maximum allowable volume in this sample. 
		     if using libsnd emulation, 127 would be max. */
		  /* 64 scales the pan to center if the volumes are equal */     

      SVS_tone_one.tone = tone_num_1;
      SVS_tone_one.prog_num = prog;
      SVS_tone_one.prog_actual = actual_prog;
 
/*	
 *	fill sndvoicestats structure for 1st tone - this allows volumes and 
 * 	...pans to be changed later in libsnd  
 */
 
      SVS_tone_two.vagId = vagatr_2.vag;
      SVS_tone_two.vabId = vab;
      SVS_tone_two.pitch = SRA_tone_two.pitch;
      SVS_tone_two.note = note;
	  
	  /* volume set to greater of two volumes for tone */
      SVS_tone_two.vol = (SRA_tone_two.volume.left>SRA_tone_two.volume.right)?
         SRA_tone_two.volume.left:SRA_tone_two.volume.right;

 	  SVS_tone_two.pan = (SRA_tone_two.volume.left>SRA_tone_two.volume.right)?
		 SRA_tone_two.volume.right*64/SRA_tone_two.volume.left
		  :(0x3fff-SRA_tone_two.volume.left)*64/SRA_tone_two.volume.right;
		  /* 0x3fff value taken from maximum allowable volume in this sample. 
		     if using libsnd emulation, 127 would be max. */
		  /* 64 scales the pan to center if the volumes are equal */     

      SVS_tone_two.tone = tone_num_2;
      SVS_tone_two.prog_num = prog;
      SVS_tone_two.prog_actual = actual_prog;
 
      /* block other voice allocations */
      if ((voices_allocated = (long)SsBlockVoiceAllocation()) == -1)
           return(-2) ;            /* failure - voices already in process of */
                                   /* ..allocating from another call to this */
                           /* ..function, SsUtKeyOn() or MIDI key on command */

      else    /* attempt to allocate 2 voices */
      {
           if ((voices_allocated = SsAllocateVoices(2, vagatr_1.prior)) ==-1)
           {
                SsUnBlockVoiceAllocation();
                return(-3);          /* failure - 2 free voices at specified */
                                                     /* ..priority not found */
           }
      }
 
      /* parse out voice numbers */
      for (i = 0; i<24; i++)
      {
           if ((1<<i) & voices_allocated)
           {
                if (voice_num_1 == 99) voice_num_1 = i;
                else
                {
                     voice_num_2 = i;
                     break;
                }
           }
      }
 
      /* Set internal libsnd variables for both voices */
      SsSetVoiceSettings(voice_num_1, &SVS_tone_one);
      SsSetVoiceSettings(voice_num_2, &SVS_tone_two);
 
       /* Queue the register settings for both voices */
      SsQueueRegisters(voice_num_1, &SRA_tone_one);
      SsQueueRegisters(voice_num_2, &SRA_tone_two);
 
      /* queue voices to key on */
      SsQueueKeyOn(voices_allocated);
 
      /* add reverb for both voices (if applicable) */
      if (vagatr_2.mode & 4) reverb_voices |= (1<<voice_num_2);
      if (vagatr_1.mode & 4) reverb_voices |= 1<<(voice_num_1);
 
      /* queue voices which need reverb changed */
      SsQueueReverb(voices_allocated, reverb_voices);
 
      /* free voice allocation */
      SsUnBlockVoiceAllocation();
 
      /* return voices */
      return (voices_allocated);
}

/*---------------------------------------------------------------------------
 *  FUNCTION :  ChangeReverb (voices, type, reverb);
 *  DESCRIPTION : Changes reverb of specified voices by calling 
 *                SsQueueReverb(). Confirms voice data identity via 
 *                SsVoiceCheck
 *  ARG : long  voices - bitfield of voices to be affected by reverb
 *        short type   - ambient or spatial? aids proper setup of voice check
 *        char  reverb - turn reverb on or off?
 *  RET : short  - (0) success
 *		 - (-1) some voices had been reallocated
 *		 - (-2)	failure; all voices had been reallocated	
 *-------------------------------------------------------------------------- */

short ChangeReverb(long voices, short type, char reverb)
{
short i;	/* work */
long vabid;	/* upper 8 bits of lower 2 bytes contain vab id number; 
				...lower 8 bits of lower 2 bytes contain prog # */
short note;	/* note at which tone was originally keyed on */
short confirmed_voices = 0L; /* voices with confirmed data */
short confirmed_reverb = 0L; /* reverb bitfield for confirmed voices */
 
      if (type == AMBIENT)
      {
       vabid = (ambient_data>>16) & 0xFF;
       note = (ambient_data>>8) & 0xFF;
      }
      else if (type == SPATIAL)
      {
       vabid = (spatial_data >>16) & 0xFF;
       note = (spatial_data>>8) & 0xFF;
      }
      else  /* spu */
      {
           SsQueueReverb(voices, reverb<<12);   /* change reverb accordingly */
           return(0);
      }
      for (i=   0; i<24; i++)
      {
           if ((1<<i) & voices)
           {
                /* confirm voice contains proper tone data and has not been
                 * reallocated by libsnd voice allocation system */
                if (SsVoiceCheck(i, vabid, note) == 0)
                {
                     confirmed_voices |= 1<<i;
                     confirmed_reverb |= reverb<<i;
                }
           }
      }
      if (confirmed_voices !=0)
      {
           /* change reverb accordingly */
           SsQueueReverb(confirmed_voices, confirmed_reverb); 
           if (confirmed_voices != voices) return(-1);  /* some voices had been reallocated */
           else return(0);                 /* no voices had been reallocated */
      }
      else return(-2);       /* no success - all voices had been reallocated */
}

/*---------------------------------------------------------------------------
 *  FUNCTION :  ChangeVolume (voice, type, snd_vol);
 *  DESCRIPTION : Changes volume of specified voice via SsQueueRegisters(). 
 *		  Performs identity check of voice with SsVoiceCheck()  
 *  ARGS :long  voice   - voice to change volume of (0-23)
 *        short type    - ambient or spatial? allows proper set up of voice
 *                         check
 *        SndVolume2 snd_vol - volume data left and right
 *  RET : short  - (0) success
 *				 - (-1) failure; voice had been reallocated
 *------------------------------------------------------------------------- */

short ChangeVolume(long voice, short type, SndVolume2 snd_vol)
{
SndRegisterAttr SRA_attr;	/* voice register attributes */
short i;					/* work */
long vabid;					/* upper 8 bits of lower 2 bytes contain vab id number; 
				...lower 8 bits of lower 2 bytes contain prog # */
short note;					/* note voice originally keyed on */
      if (type == AMBIENT)
      {
       vabid = (ambient_data >>16) & 0xFF;
       note = (ambient_data>>8) & 0xFF;
      }
      else
      {
       vabid = (spatial_data >>16) & 0xFF;
       note = (spatial_data>>8) & 0xFF;
      }
      /* confirm voice contains proper tone data and has not been
       * ...reallocated by libsnd voice allocation system */
      if (SsVoiceCheck(voice, vabid, note) == 0)
      {
           SRA_attr.mask = (SND_VOLL | SND_VOLR);
           SRA_attr.volume.left = snd_vol.left;
           SRA_attr.volume.right = snd_vol.right;
           SsQueueRegisters(voice, &SRA_attr);
           return(0);               /* success - voice still playing ambient */
      }
      else return(-1);                        /* failure - voice reallocated */
}
 
 
