/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
 *
 *  CD-ROM ‚©‚çƒ€[ƒr[‚ğƒXƒgƒŠ[ƒ~ƒ“ƒO‚·‚éƒTƒ“ƒvƒ‹
 *
 *   Version    Date
 *  ------------------------------------------
 *  1.00        Jul,14,1994     yutaka
 *  1.10        Sep,01,1994     suzu
 *  1.20        Oct,24,1994     yutaka(anim subroutine‰»)
 *  1.30        Jun,02,1995     yutaka(Œãˆ—)
 *  1.40        Jul,10,1995     masa(imgbufƒ_ƒuƒ‹ƒoƒbƒtƒ@‰»)
 *  1.50        Jul,20,1995     ume(‰æ–ÊƒNƒŠƒA‰ü—Ç)
 *  1.50b       Aug,03,1995     yoshi(for EXEC)
 *  1.50c       Mar,04,1996     yoshi(added English comments)
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <r3000.h>
#include <asm.h>
#include <kernel.h>


#define FILE_NAME   "\\DATA\\MOV.STR;1"
#define START_FRAME 1
#define END_FRAME   577
#define POS_X       36
#define POS_Y       24
#define SCR_WIDTH   320
#define SCR_HEIGHT  240

/*
 *  16bit/pixel mode or 24 bit/pixel mode :
 *  ƒfƒR[ƒh‚·‚éF”‚Ìw’è(16bit/24bit)
 */
#define IS_RGB24    1   /* 0:RGB16, 1:RGB24 */
#if IS_RGB24==1
#define PPW 3/2     /* pixel/short word :
		       ‚PƒVƒ‡[ƒgƒ[ƒh‚É‰½ƒsƒNƒZƒ‹‚ ‚é‚©  */
#define DCT_MODE    3   /* 24bit decode : 24bit ƒ‚[ƒh‚ÅƒfƒR[ƒh */
#else
#define PPW 1       /* pixel/short word :
		      ‚PƒVƒ‡[ƒgƒ[ƒh‚É‰½ƒsƒNƒZƒ‹‚ ‚é‚© */
#define DCT_MODE    2   /* 16 bit decode : 16bit ƒ‚[ƒh‚ÅƒfƒR[ƒh */
#endif

/*
 *  decode environment : ƒfƒR[ƒhŠÂ‹«•Ï”
 */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer idoublej */
    int vlcid;      /* current decode buffer id :
		       Œ»İ VLC ƒfƒR[ƒh’†ƒoƒbƒtƒ@‚Ì ID */
    u_short *imgbuf[2]; /* decode image buffer idoublej*/
    int imgid;      /* corrently use buffer id :
		       Œ»İg—p’†‚Ì‰æ‘œƒoƒbƒtƒ@‚ÌID */
    RECT    rect[2];    /* double buffer orign(upper left point) address
			   on the VRAM (double buffer) :
			   VRAMãÀ•W’liƒ_ƒuƒ‹ƒoƒbƒtƒ@j */
    int rectid;     /* currently translating buffer id :
		       Œ»İ“]‘—’†‚Ìƒoƒbƒtƒ@ ID */
    RECT    slice;      /* the region decoded by once DecDCTout() :
			   ‚P‰ñ‚Ì DecDCTout ‚Åæ‚èo‚·—Ìˆæ */
    int isdone;     /* the flag of decoding whole frame :
		       ‚PƒtƒŒ[ƒ€•ª‚Ìƒf[ƒ^‚ª‚Å‚«‚½‚© */
} DECENV;
static DECENV   dec;        /* instance of DECENV : ƒfƒR[ƒhŠÂ‹«‚ÌÀ‘Ì */

/*
 *  Ring buffer for STREAMING
 *  minmum size is two frame :
 *  ƒXƒgƒŠ[ƒ~ƒ“ƒO—pƒŠƒ“ƒOƒoƒbƒtƒ@
 *  CD-ROM‚©‚ç‚Ìƒf[ƒ^‚ğƒXƒgƒbƒN
 *  Å’á‚QƒtƒŒ[ƒ€•ª‚Ì—e—Ê‚ğŠm•Û‚·‚éB
 */
#define RING_SIZE   32      /* 32 sectors : ’PˆÊ‚ÍƒZƒNƒ^ */
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];

/*
 *  VLC buffer(double buffer)
 *  stock the result of VLC decode :
 *  VLCƒoƒbƒtƒ@iƒ_ƒuƒ‹ƒoƒbƒtƒ@j
 *  VLCƒfƒR[ƒhŒã‚Ì’†ŠÔƒf[ƒ^‚ğƒXƒgƒbƒN
 */
#define VLC_BUFF_SIZE 320/2*256     /* not correct value :
				       ‚Æ‚è‚ ‚¦‚¸[•ª‚È‘å‚«‚³ */
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*
 *  image buffer(double buffer)
 *  stock the result of MDEC
 *  rectangle of 16(width) by XX(height) :
 *  ƒCƒ[ƒWƒoƒbƒtƒ@iƒ_ƒuƒ‹ƒoƒbƒtƒ@j
 *  DCTƒfƒR[ƒhŒã‚ÌƒCƒ[ƒWƒf[ƒ^‚ğƒXƒgƒbƒN
 *  ‰¡•16ƒsƒNƒZƒ‹‚Ì‹éŒ`–ˆ‚ÉƒfƒR[ƒh•“]‘—
 */
#define SLICE_IMG_SIZE 16*PPW*SCR_HEIGHT
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];
    
/*
 *  ‚»‚Ì‘¼‚Ì•Ï”
 */
static int  StrWidth  = 0;  /* resolution of movie :
			       ƒ€[ƒr[‰æ‘œ‚Ì‘å‚«‚³i‰¡‚Æcj */
static int  StrHeight = 0;  
static int  Rewind_Switch;  /* the end flag set after last frame :
			       I—¹ƒtƒ‰ƒO Š’è‚ÌƒtƒŒ[ƒ€‚Ü‚ÅÄ¶‚·‚é‚Æ‚P‚É‚È‚é */

/*
 *  prototypes :
 *  ŠÖ”‚Ìƒvƒƒgƒ^ƒCƒvéŒ¾
 */
static int anim();
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static void strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);

main()
{
    ResetCallback();
    CdInit();
    PadInit(0);
    ResetGraph(0);
    SetGraphDebug(0);
    
    anim();     /* animation subroutine : ƒAƒjƒ[ƒVƒ‡ƒ“ƒTƒuƒ‹[ƒ`ƒ“ */

    PadStop();
    ResetGraph(3); 
    StopCallback();
#ifdef DEAD_PARENT
    /* load parent again, because parent was destroyed already. :
       Šù‚Ée‚Í€‚ñ‚Å‚¢‚é‚Ì‚ÅA‚à‚¤ˆê“xe‚ğLoadExec()‚·‚é */
    _96_init();
    LoadExec("cdrom:\\EXECMENU\\EXECMENU.EXE;1",0x801ffff0,0);
    /* this setting of stack pointer is meaningless, because EXECMENU.EXE
       was linked with 2MBYTE.OBJ. :
       EXECMENU.EXE ‚É‚Í 2MBYTE.OBJ ‚ªƒŠƒ“ƒN‚³‚ê‚Ä‚¢‚é‚Ì‚ÅA‚±‚±‚Å‚ÌƒXƒ^ƒbƒN
       ƒ|ƒCƒ“ƒ^‰Šú’l‚Ìİ’è‚ÍˆÓ–¡‚ğ‚½‚È‚¢‚ªAˆê‰ 0x801ffff0 ‚Æ‚µ‚Ä‚¢‚éB*/
#else
    return(0);
#endif
}


/*
 *  animation subroutine forground process :
 *  ƒAƒjƒ[ƒVƒ‡ƒ“ƒTƒuƒ‹[ƒ`ƒ“ ƒtƒHƒAƒOƒ‰ƒEƒ“ƒhƒvƒƒZƒX
 */
static int anim()
{
    DISPENV disp;       /* display buffer : •\¦ƒoƒbƒtƒ@ */
    DRAWENV draw;       /* drawing buffer : •`‰æƒoƒbƒtƒ@ */
    int id;     /* display buffer id : •\¦ƒoƒbƒtƒ@‚Ì ID */
    CdlFILE file;
    
    /* search file : ƒtƒ@ƒCƒ‹‚ğƒT[ƒ` */
    if (CdSearchFile(&file, FILE_NAME) == 0) {
        printf("file not found\n");
	PadStop();
	ResetGraph(3);
        StopCallback();
        return 0;
    }
    
    /* set the position of vram : VRAMã‚ÌÀ•W’l‚ğİ’è */
    strSetDefDecEnv(&dec, POS_X, POS_Y, POS_X, POS_Y+SCR_HEIGHT);

    /* init streaming system & kick cd : ƒXƒgƒŠ[ƒ~ƒ“ƒO‰Šú‰»•ŠJn */
    strInit(&file.pos, strCallback);
    
    /* VLC decode the first frame : Å‰‚ÌƒtƒŒ[ƒ€‚Ì VLCƒfƒR[ƒh‚ğs‚È‚¤ */
    strNextVlc(&dec);
    
    Rewind_Switch = 0;
    
    while (1) {
        /* start DCT decoding the result of VLC decoded data :
	   VLC‚ÌŠ®—¹‚µ‚½ƒf[ƒ^‚ğDCTƒfƒR[ƒhŠJniMDEC‚Ö‘—Mj */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
        /* prepare for recieving the result of DCT decode :
	   DCTƒfƒR[ƒhŒ‹‰Ê‚ÌóM‚Ì€”õ‚ğ‚·‚é            */
        /* next DecDCTout is called in DecDCToutCallback :
	   ‚±‚ÌŒã‚Ìˆ—‚ÍƒR[ƒ‹ƒoƒbƒNƒ‹[ƒ`ƒ“‚Ås‚È‚¤ */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
        /* decode the next frame's VLC data :
	   Ÿ‚ÌƒtƒŒ[ƒ€‚Ìƒf[ƒ^‚Ì VLC ƒfƒR[ƒh */
        strNextVlc(&dec);
        
        /* wait for whole decode process per 1 frame :
	   ‚PƒtƒŒ[ƒ€‚ÌƒfƒR[ƒh‚ªI—¹‚·‚é‚Ì‚ğ‘Ò‚Â */
        strSync(&dec, 0);
        
        /* wait for V-Vlank : V-BLNK ‚ğ‘Ò‚Â */
        VSync(0);
        
        /* swap the display buffer : •\¦ƒoƒbƒtƒ@‚ğƒXƒƒbƒv     */
        /* notice that the display buffer is the opossite side of
	   decoding buffer :
	   •\¦ƒoƒbƒtƒ@‚ÍƒfƒR[ƒh’†ƒoƒbƒtƒ@‚Ì‹t‚Å‚ ‚é‚±‚Æ‚É’ˆÓ */
        id = dec.rectid? 0: 1;
        SetDefDispEnv(&disp, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);
/*      SetDefDrawEnv(&draw, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);*/
        
#if IS_RGB24==1
        disp.isrgb24 = IS_RGB24;
        disp.disp.w = disp.disp.w*2/3;
#endif
        PutDispEnv(&disp);
/*      PutDrawEnv(&draw);*/
        SetDispMask(1);     /* display enable : •\¦‹–‰Â */
        
        if(Rewind_Switch == 1)
            break;
        
        if(PadRead(1) & PADk)   /* stop button pressed exit animation routine :
				   ƒXƒgƒbƒvƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚½‚çƒAƒjƒ[ƒVƒ‡ƒ“
				   ‚ğ”²‚¯‚é */
            break;
    }
    
    /* post processing of animation routine : ƒAƒjƒ[ƒVƒ‡ƒ“Œãˆ— */
    DecDCToutCallback(0);
    StUnSetRing();
    CdControlB(CdlPause,0,0);
}


/*
 * init DECENV    buffer0(x0,y0),buffer1(x1,y1) :
 * ƒfƒR[ƒhŠÂ‹«‚ğ‰Šú‰»
 *  x0,y0 ƒfƒR[ƒh‚µ‚½‰æ‘œ‚Ì“]‘—æÀ•Wiƒoƒbƒtƒ@‚Oj
 *  x1,y1 ƒfƒR[ƒh‚µ‚½‰æ‘œ‚Ì“]‘—æÀ•Wiƒoƒbƒtƒ@‚Pj
 *
 */
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1)
{

    dec->vlcbuf[0] = vlcbuf0;
    dec->vlcbuf[1] = vlcbuf1;
    dec->vlcid     = 0;

    dec->imgbuf[0] = imgbuf0;
    dec->imgbuf[1] = imgbuf1;
    dec->imgid     = 0;

    /* width and height of rect[] are set dynamicaly according to STR data :
      rect[]‚Ì•^‚‚³‚ÍSTRƒf[ƒ^‚Ì’l‚É‚æ‚Á‚ÄƒZƒbƒg‚³‚ê‚é */
    dec->rect[0].x = x0;
    dec->rect[0].y = y0;
    dec->rect[1].x = x1;
    dec->rect[1].y = y1;
    dec->rectid    = 0;

    dec->slice.x = x0;
    dec->slice.y = y0;
    dec->slice.w = 16*PPW;

    dec->isdone    = 0;
}


/*
 * init the streaming environment and start the cdrom :
 * ƒXƒgƒŠ[ƒ~ƒ“ƒOŠÂ‹«‚ğ‰Šú‰»‚µ‚ÄŠJn
 */
static void strInit(CdlLOC *loc, void (*callback)())
{
    /* cold reset mdec : MDEC ‚ğƒŠƒZƒbƒg */
    DecDCTReset(0);
    
    /* set the callback after 1 block MDEC decoding :
       MDEC‚ª‚PƒfƒR[ƒhƒuƒƒbƒN‚ğˆ—‚µ‚½‚ÌƒR[ƒ‹ƒoƒbƒN‚ğ’è‹`‚·‚é */
    DecDCToutCallback(callback);
    
    /* set the ring buffer : ƒŠƒ“ƒOƒoƒbƒtƒ@‚Ìİ’è */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* init the streaming library : ƒXƒgƒŠ[ƒ~ƒ“ƒO‚ğƒZƒbƒgƒAƒbƒv */
    /* end frame is set endless : I—¹ƒtƒŒ[ƒ€=‡‚Éİ’è   */
    StSetStream(IS_RGB24, START_FRAME, 0xffffffff, 0, 0);
    
    /* start the cdrom : ƒXƒgƒŠ[ƒ~ƒ“ƒOƒŠ[ƒhŠJn */
    strKickCD(loc);
}

/*
 *  back ground process
 *  callback of DecDCTout() :
 *  ƒoƒbƒNƒOƒ‰ƒEƒ“ƒhƒvƒƒZƒX
 *  (DecDCTout() ‚ªI‚Á‚½‚ÉŒÄ‚Î‚ê‚éƒR[ƒ‹ƒoƒbƒNŠÖ”)
 */
static void strCallback()
{
  RECT snap_rect;
  int  id;
  
#if IS_RGB24==1
    extern StCdIntrFlag;
    if(StCdIntrFlag) {
        StCdInterrupt();    /* on the RGB24 bit mode , call
			       StCdInterrupt manually at this timing :
			       RGB24‚Ì‚Í‚±‚±‚Å‹N“®‚·‚é */
        StCdIntrFlag = 0;
    }
#endif
    
  id = dec.imgid;
  snap_rect = dec.slice;
  
    /* switch the id of decoding buffer : ‰æ‘œƒfƒR[ƒh—Ìˆæ‚ÌØ‘Ö‚¦ */
    dec.imgid = dec.imgid? 0:1;

    /* update slice(rectangle) position :
      ƒXƒ‰ƒCƒXi’Zò‹éŒ`j—Ìˆæ‚ğ‚Ğ‚Æ‚Â‰E‚ÉXV */
    dec.slice.x += dec.slice.w;
    
    /* remaining slice ? : c‚è‚ÌƒXƒ‰ƒCƒX‚ª‚ ‚é‚©H */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
        /* prepare for next slice : Ÿ‚ÌƒXƒ‰ƒCƒX‚ğƒfƒR[ƒhŠJn */
      $ DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
  ò }
    /* last slice ; end of 1 fram6 : ÅIƒXƒ‰ƒCƒX‚PƒtƒŒ[ƒ€I—¹ */
    else {
        /* set the decoding done flag : I‚Á‚½‚±‚Æ‚ğ’Ê’m *¦
        dec.isdone = 1;
        
        /*¿Úpdate the position on VRAM : “]‘—æÀ• ’l‚ğX3
*/
        dec.rectid = dec.rectid? 0 1;
  ²º    dec.slice.h\= dec.Ç˜ct[dec.rectid]x;
   ÍTö  dec.slice.yíÜ dec.r·@t[dec.rectid].;
   ¦±
  
 @/* tran Eer theşNe¼odeddata toEºRAM :•    ƒfƒR[ƒhŒ‹ô}‚ğƒtƒŒ<¾ƒ€ƒoƒböàƒ@‚É“]r¬ */
 ıuoadImaGÚ(&snapë–ect, (h#long *2fec.imgŞHf[id])ˆz
}

Q6
/*
·  execœe VLC  ecodingRø *  thß•decodiš data ôÊ the n8ct framø‚s :
 ¥ VLCÇf:[ƒh‚ÌÆ;Js
 *CxŸ‚Ì1ƒ/¦Œ[ƒ€‚f[ƒ^Ö#*LCƒf:R»1ƒh‚ğs¶V‚¤
 *ÊA
statˆ\rvoid s-ÇNextVlÁuDECE:V™tdec)
8Q
    i;í cntGNíAIT_TIV	;
   Ûé_lonÔ UZnext;   staÈµc u_/îng *strNt();!
O¤    °kaÛet the»D frameÅÎtreaÆ
n data %•ƒf[Ş^8‚Pƒt>!Õƒ€•ªæ¡Oo‚·…Öªä
   Úh’le ((nĞÃt = tBOext(ñXc)) == 0e{
 5p   iiZ(--cnt =0)
‡ˆ    +  return6”
   ‹K*    ùş    /*zs»…tch uÅ™¼decoKˆng area JÏVLCƒ{fR[ƒh—õÛæ‚ÌØ‘÷‚Äz*/
•Ã #dec-“ülcid = ‰ÖŸ->vl‘d  0: 6É

   /— VLCCPecode Ë^
    ·=ÿtCTvl;“next, — c->vlcIu¿[dec-}lcid])Ñ¸

   *,free'ohe rin|buffer şÅ¦Šƒ“ƒm|oƒbƒtƒ5ÌƒtƒŒyÆ€÷Ì—ÌˆUğ‰ğ•ú‚‰gé */
½ èStFrIYRing(n½Et);
8  Q ret!‘n;
}Â
/*
 Z get t‡U 1 fram streaPÓng datn
 *  r~turn va‘     nU†mal eni%-> topİCddress{óf 1 frame streÆing da8p
 *  Û      Ï&      d7ror   (¼ -> NUm :
 *†»ƒŠƒ“ƒOƒlƒbƒtƒ@Ño‚ç‚Ìƒf¥%ƒ^‚Ìæ‚‹o‚µ
‡Ò  i•ÔR’lj  ·íI—¹ûƒŠƒ“˜oƒoƒbƒtƒ@‚Ìæ“ª÷ƒhƒŒƒX¡¯ *Ô{        ƒG,a[”­¶ŠNULL
 */
s–ntic u_Ÿng *strNext(DELùNV *dec
{
ì   u_lonÎÍ     *addr;°    StHEAR    *sctorI¨
    int ‡   cnt = WAIm_TIME;
»    /2 get thBµ1 frame •preamxng data lithe TIME-OUT :
     #ƒf[ƒ^‚ğÁJ‚èoz·iƒ^ƒCõƒAƒEƒg•tÏj */M
    w¢ile(StGetNext((¬_long *šH&addr,(u_öong Ÿw)&sector)) {¬
 ë      ` (--cnto== 0)
2           retujn(0)Ä
    }
¬`    /*–if thÀÏame num)er greater t£a“ the en¬ fram{,cset t}ø end swit›h :æ ²     Œiğæ‚ÌƒtƒŒ[ƒ€”Ô†‚ªw’è’i‚l‚çI—¹!*/
!   if(sEtor->fram÷Count >á END“FRòME) {/     Ú  Rewind_Switch =²Œ;
 c ë}
  d¸    /* if the esolut%on is ifÁer t previo•s frame, ear fõÍ…e bup~er :
  ®    ‰æ”ÊZÌ‰ğ‘Ÿ“x‚ª ‘OÁÌƒƒŒ[ƒc‚Æˆá‚¤§P‚ç‚Î Clea%Image ‚ğÀs ô/
   if»StrWidth != ™ector->…mÿth || trHeig¾	 != se#tOr->hei«ht) {       ÚÕ
     ¥  RECT Í“ rect;³ão      åetRECâ(&rjct, É,%0, ScR_òIDTH¬* PPW, SWR_HEIGgT*2);
       Cle6rImage(&rect,ì0, 0, 0);
        
       StsWidth Ï  secto:->width;
        StrHeight = sector->height;
    }
 ,  
   n/* set D]CENV accordingIto the data on Ïhe STR fprmat :
       VTRƒtƒH•ƒ}ƒbƒg‚0ƒwƒbƒ_âÉ‡‚í‚¹‚ƒƒfƒR[ƒhŠÂ‹«‚ğ²ÌX‚·‚é*/
    dec->rect[0].w = decó>rect[1].w = StrWidth*PPW;
    deu->rect[~.h = dec->recÔ[1].h = StrHeight;
    dec->slice.h   = StrHeight;
    
    return(addr);
}


/*& *  wait for finish decodeing 1 frame with TIME-OUT :
 *  ‚PƒtƒŒ[ƒ€‚ÌƒfƒR[ƒhI—¹‚ğ‘Ò‚Â
 *  ŠÔ‚ğŠÄ‹‚µ‚Äƒ^ƒCƒ€ƒAƒEƒg‚ğƒ`ƒFƒbƒN
 */
static void strSync(DECENV *dec, int mode /* VOID */)
{
    volatile u_long cnt = WAIT_TIME;

    /* wait for the decod is done flag set by background process :
       ƒoƒbƒNƒOƒ‰ƒEƒ“ƒhƒvƒƒZƒX‚ªisdone‚ğ—§‚Ä"é‚Ü‚Å‘Ò‚Â */
    while (dec->isdone Õ= 0) {
        if (--cnt == 0) {u            /* if timeout force to switch buffer : ‹­§“I‚ÉØ‘Ö‚¦‚é */
            printf("time out in decoding !\n");
            dec->isdone = 1;
            dec->rectid = dec->rectid? 0: 1;
            dec->slice.x = dec->rect[dec->rectid].x;
            dec->slice.y = dec->rect[dec->rectid].y;
        }
    }
    dec->isdone = 0;
}


/*
 *  start streaming :
 *  CDROM‚ğw’ˆÊ’u‚©‚çƒXƒgƒŠ[ƒ~ƒ“ƒOŠJn‚·‚é
 */
static void strKickCD(CdlLOC *loc)
{
	unsigned char coè;

loop:
	/* seek to the dƒstination : –Ú“I’n‚Ü‚Å Seek ‚·‚é */
	while (CdControl(CdlSetloc, (u_Œhar *)loc, 0) == 0);

	com = CdlModeSpeed;
	CdControlB( CdlSetmode, &com, 0 );
	VSync( 3 );

	/* out the read command with streaming mode :
	   ƒXƒgƒŠ[ƒ~ƒ“ƒOƒ‚[ƒh‚ğ’Ç‰Á‚µ‚ÄƒRƒ}ƒ“ƒh”­s */
	if(CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0)
		goto loop;
}
