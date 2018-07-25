BezierPolyPrimSet_0000:	/* 0x00000088 */
	TERMINATE;	/* next prim */
	BezierPolyPrimHdr_0000 / 4;
	M(2);	/* num of types */

BezierPolyPrim_0000:	/* 0x00000094 */
#if DO_INI
	DEV_ID(SCE)|CTG(CTG_POLY)|DRV(INI|STP)
		|PRIM_TYPE(TRI|IIP);	/* type[0] */
#else
	DEV_ID(SCE)|CTG(CTG_POLY)|DRV(STP)
		|PRIM_TYPE(TRI|IIP);	/* type[0] */
#endif
	H(2); M(H(208));	/* size, data */
	(BezierPoly_0000 - BezierPoly_0000) / 4;

BezierPolyPrim_0001:	/* 0x000000a0 */
	DEV_ID(SCE)|CTG(CTG_POLY)|DRV(STP)
		|PRIM_TYPE(TRI);	/* type[1] */
	H(2); M(H(212));	/* size, data */
	(BezierPoly_0001 - BezierPoly_0000) / 4;

BezierPoly_0000:	/* 0x000000ac */
	0x3200ffff; 0x00000021; 0x004f006f; 0x0001006f;
	0x3200ffff; 0x00000021; 0x004e00ab; 0x004f006f;
	0x3200ffff; 0x000200a6; 0x00510046; 0x000300a7;
	0x3200ffff; 0x000200a6; 0x005000a6; 0x00510046;
	0x3200ffff; 0x000300a7; 0x005200d1; 0x0004004d;
	0x3200ffff; 0x000300a7; 0x00510046; 0x005200d1;
	0x3200ffff; 0x0004004d; 0x00530062; 0x00050022;
	0x3200ffff; 0x0004004d; 0x005200d1; 0x00530062;
	0x3200ffff; 0x00050022; 0x00540011; 0x00060023;
	0x3200ffff; 0x00050022; 0x00530062; 0x00540011;
	0x3200ffff; 0x00060023; 0x0055008c; 0x00070035;
	0x3200ffff; 0x00060023; 0x00540011; 0x0055008c;
	0x3200ffff; 0x00070035; 0x005600c3; 0x000800bf;
	0x3200ffff; 0x00070035; 0x0055008c; 0x005600c3;
	0x3200ffff; 0x000800bf; 0x005700d2; 0x0009004e;
	0x3200ffff; 0x000800bf; 0x005600c3; 0x005700d2;
	0x3200ffff; 0x0009004e; 0x005800b8; 0x000a0040;
	0x3200ffff; 0x0009004e; 0x005700d2; 0x005800b8;
	0x3200ffff; 0x000a005a; 0x0059008d; 0x000b005b;
	0x3200ffff; 0x000a005a; 0x0058005a; 0x0059008d;
	0x3200ffff; 0x000b005b; 0x005a0074; 0x000c005c;
	0x3200ffff; 0x000b005b; 0x0059008d; 0x005a0074;
	0x3200ffff; 0x000c005c; 0x005b0012; 0x000d00b2;
	0x3200ffff; 0x000c005c; 0x005a0074; 0x005b0012;
	0x3200ffff; 0x000d00b2; 0x005c008e; 0x000e0024;
	0x3200ffff; 0x000d00b2; 0x005b0012; 0x005c008e;
	0x3200ffff; 0x000e0024; 0x005d0075; 0x000f0025;
	0x3200ffff; 0x000e0024; 0x005c008e; 0x005d0075;
	0x3200ffff; 0x000f0025; 0x005e0076; 0x00100089;
	0x3200ffff; 0x000f0025; 0x005d0075; 0x005e0076;
	0x3200ffff; 0x00100089; 0x005f008f; 0x00110001;
	0x3200ffff; 0x00100089; 0x005e0076; 0x005f008f;
	0x3200ffff; 0x00110001; 0x006000c4; 0x00120026;
	0x3200ffff; 0x00110001; 0x005f008f; 0x006000c4;
	0x3200ffff; 0x00120026; 0x00610013; 0x001300b3;
	0x3200ffff; 0x00120026; 0x006000c4; 0x00610013;
	0x3200ffff; 0x001300b3; 0x00620090; 0x0014000b;
	0x3200ffff; 0x001300b3; 0x00610013; 0x00620090;
	0x3200ffff; 0x0014000b; 0x0063003e; 0x0015005d;
	0x3200ffff; 0x0014000b; 0x00620090; 0x0063003e;
	0x3200ffff; 0x0015005d; 0x00640081; 0x00160041;
	0x3200ffff; 0x0015005d; 0x0063003e; 0x00640081;
	0x3200ffff; 0x00160041; 0x0065002e; 0x001700cd;
	0x3200ffff; 0x00160041; 0x00640081; 0x0065002e;
	0x3200ffff; 0x001700cd; 0x00660014; 0x00180027;
	0x3200ffff; 0x001700cd; 0x0065002e; 0x00660014;
	0x3200ffff; 0x00180027; 0x00670004; 0x0019000c;
	0x3200ffff; 0x00180027; 0x00660014; 0x00670004;
	0x3200ffff; 0x0019000c; 0x00680051; 0x001a00c0;
	0x3200ffff; 0x0019000c; 0x00670004; 0x00680051;
	0x3200ffff; 0x001a00c0; 0x00690047; 0x001b00c1;
	0x3200ffff; 0x001a00c0; 0x00680051; 0x00690047;
	0x3200ffff; 0x001b00c1; 0x006a0077; 0x001c0028;
	0x3200ffff; 0x001b00c1; 0x00690047; 0x006a0077;
	0x3200ffff; 0x001c0028; 0x006b002f; 0x001d005e;
	0x3200ffff; 0x001c0028; 0x006a0077; 0x006b002f;
	0x3200ffff; 0x001d005e; 0x006c0052; 0x001e0036;
	0x3200ffff; 0x001d005e; 0x006b002f; 0x006c0052;
	0x3200ffff; 0x001e0036; 0x006d0048; 0x001f0095;
	0x3200ffff; 0x001e0036; 0x006c0052; 0x006d0048;
	0x3200ffff; 0x001f0095; 0x006e0049; 0x0020005f;
	0x3200ffff; 0x001f0095; 0x006d0048; 0x006e0049;
	0x3200ffff; 0x0020005f; 0x006f009b; 0x00210060;
	0x3200ffff; 0x0020005f; 0x006e0049; 0x006f009b;
	0x3200ffff; 0x00210060; 0x00700063; 0x0022008a;
	0x3200ffff; 0x00210060; 0x006f009b; 0x00700063;
	0x3200ffff; 0x0022008a; 0x00710078; 0x00230029;
	0x3200ffff; 0x0022008a; 0x00700063; 0x00710078;
	0x3200ffff; 0x00230029; 0x007200ac; 0x002400a8;
	0x3200ffff; 0x00230029; 0x00710078; 0x007200ac;
	0x3200ffff; 0x002400a8; 0x00730082; 0x002500ce;
	0x3200ffff; 0x002400a8; 0x007200ac; 0x00730082;
	0x3200ffff; 0x002500ce; 0x00740070; 0x00260070;
	0x3200ffff; 0x002500ce; 0x00730082; 0x00740070;
	0x3200ffff; 0x00260096; 0x00750064; 0x00270071;
	0x3200ffff; 0x00260096; 0x00740096; 0x00750064;
	0x3200ffff; 0x00270071; 0x00760091; 0x00280037;
	0x3200ffff; 0x00270071; 0x00750064; 0x00760091;
	0x3200ffff; 0x00280037; 0x007700ad; 0x002900b4;
	0x3200ffff; 0x00280037; 0x00760091; 0x007700ad;
	0x3200ffff; 0x002900b4; 0x00780083; 0x002a000d;
	0x3200ffff; 0x002900b4; 0x007700ad; 0x00780083;
	0x3200ffff; 0x002a000d; 0x00790065; 0x002b0002;
	0x3200ffff; 0x002a000d; 0x00780083; 0x00790065;
	0x3200ffff; 0x002b0002; 0x007a0066; 0x002c0042;
	0x3200ffff; 0x002b0002; 0x00790065; 0x007a0066;
	0x3200ffff; 0x002c0042; 0x007b0005; 0x002d00cf;
	0x3200ffff; 0x002c0042; 0x007a0066; 0x007b0005;
	0x3200ffff; 0x002d00cf; 0x007c000e; 0x002e000e;
	0x3200ffff; 0x002d00cf; 0x007b0005; 0x007c000e;
	0x3200ffff; 0x002f0038; 0x007e009d; 0x00300072;
	0x3200ffff; 0x002f0038; 0x007d0038; 0x007e009d;
	0x3200ffff; 0x00300072; 0x007f0043; 0x00310043;
	0x3200ffff; 0x00300072; 0x007e009d; 0x007f0043;
	0x3200ffff; 0x00310080; 0x00800006; 0x00320044;
	0x3200ffff; 0x00310080; 0x007f0080; 0x00800006;
	0x3200ffff; 0x00320044; 0x008100d3; 0x00330045;
	0x3200ffff; 0x00320044; 0x00800006; 0x008100d3;
	0x3200ffff; 0x00330045; 0x00820053; 0x003400b5;
	0x3200ffff; 0x00330045; 0x008100d3; 0x00820053;
	0x3200ffff; 0x003400b5; 0x00830015; 0x00350061;
	0x3200ffff; 0x003400b5; 0x00820053; 0x00830015;
	0x3200ffff; 0x00350061; 0x008400c5; 0x00360039;
	0x3200ffff; 0x00350061; 0x00830015; 0x008400c5;
	0x3200ffff; 0x00360039; 0x008500c6; 0x0037004f;
	0x3200ffff; 0x00360039; 0x008400c5; 0x008500c6;
	0x3200ffff; 0x0037004f; 0x008600b9; 0x003800a9;
	0x3200ffff; 0x0037004f; 0x008500c6; 0x008600b9;
	0x3200ffff; 0x003800a9; 0x008700c7; 0x0039002a;
	0x3200ffff; 0x003800a9; 0x008600b9; 0x008700c7;
	0x3200ffff; 0x0039002a; 0x00880079; 0x003a002b;
	0x3200ffff; 0x0039002a; 0x008700c7; 0x00880079;
	0x3200ffff; 0x003a002b; 0x008900c8; 0x003b002c;
	0x3200ffff; 0x003a002b; 0x00880079; 0x008900c8;
	0x3200ffff; 0x003b002c; 0x008a009e; 0x003c0097;
	0x3200ffff; 0x003b002c; 0x008900c8; 0x008a009e;
	0x3200ffff; 0x003c0097; 0x008b0016; 0x003d0050;
	0x3200ffff; 0x003c0097; 0x008a009e; 0x008b0016;
	0x3200ffff; 0x003d0050; 0x008c0067; 0x003e003a;
	0x3200ffff; 0x003d0050; 0x008b0016; 0x008c0067;
	0x3200ffff; 0x003e003a; 0x008d0017; 0x003f0098;
	0x3200ffff; 0x003e003a; 0x008c0067; 0x008d0017;
	0x3200ffff; 0x003f0098; 0x008e004a; 0x00400099;
	0x3200ffff; 0x003f0098; 0x008d0017; 0x008e004a;
	0x3200ffff; 0x00400099; 0x008f004b; 0x0041003b;
	0x3200ffff; 0x00400099; 0x008e004a; 0x008f004b;
	0x3200ffff; 0x0041003b; 0x00900084; 0x004200b6;
	0x3200ffff; 0x0041003b; 0x008f004b; 0x00900084;
	0x3200ffff; 0x004200b6; 0x00910030; 0x004300c2;
	0x3200ffff; 0x004200b6; 0x00900084; 0x00910030;
	0x3200ffff; 0x004300c2; 0x009200ae; 0x004400aa;
	0x3200ffff; 0x004300c2; 0x00910030; 0x009200ae;
	0x3200ffff; 0x004400aa; 0x0093009f; 0x0045002d;
	0x3200ffff; 0x004400aa; 0x009200ae; 0x0093009f;
	0x3200ffff; 0x0045002d; 0x00940068; 0x00460003;
	0x3200ffff; 0x0045002d; 0x0093009f; 0x00940068;
	0x3200ffff; 0x00460003; 0x009500ba; 0x0047008b;
	0x3200ffff; 0x00460003; 0x00940068; 0x009500ba;
	0x3200ffff; 0x0047008b; 0x009600a0; 0x0048009a;
	0x3200ffff; 0x0047008b; 0x009500ba; 0x009600a0;
	0x3200ffff; 0x0048009a; 0x00970018; 0x004900d0;
	0x3200ffff; 0x0048009a; 0x009600a0; 0x00970018;
	0x3200ffff; 0x004900d0; 0x009800a1; 0x004a003c;
	0x3200ffff; 0x004900d0; 0x00970018; 0x009800a1;
	0x3200ffff; 0x004a003c; 0x00990007; 0x004b000f;
	0x3200ffff; 0x004a003c; 0x009800a1; 0x00990007;
	0x3200ffff; 0x004b000f; 0x009a0008; 0x004c00b7;
	0x3200ffff; 0x004b000f; 0x00990007; 0x009a0008;
	0x3200ffff; 0x004c00b7; 0x009b00d4; 0x004d0010;
	0x3200ffff; 0x004c00b7; 0x009a0008; 0x009b00d4;
	0x3200ffff; 0x004d0073; 0x004e00ab; 0x00000021;
	0x3200ffff; 0x004d0073; 0x009b0092; 0x004e00ab;
	0x3200ffff; 0x009c0019; 0x00a9007b; 0x009d001a;
	0x3200ffff; 0x009c0019; 0x00a800af; 0x00a9007b;
	0x3200ffff; 0x009e0054; 0x00ab000a; 0x009f001b;
	0x3200ffff; 0x009e0054; 0x00aa0054; 0x00ab000a;
	0x3200ffff; 0x009f001b; 0x00ac00d6; 0x00a00069;
	0x3200ffff; 0x009f001b; 0x00ab000a; 0x00ac00d6;
	0x3200ffff; 0x00a00069; 0x00ad0055; 0x00a1007a;
	0x3200ffff; 0x00a00069; 0x00ac00d6; 0x00ad0055;
	0x3200ffff; 0x00a1007a; 0x00ae0056; 0x00a20085;
	0x3200ffff; 0x00a1007a; 0x00ad0055; 0x00ae0056;
	0x3200ffff; 0x00a20085; 0x00af00a2; 0x00a300bb;
	0x3200ffff; 0x00a20085; 0x00ae0056; 0x00af00a2;
	0x3200ffff; 0x00a300bb; 0x00b0001c; 0x00a400d5;
	0x3200ffff; 0x00a300bb; 0x00af00a2; 0x00b0001c;
	0x3200ffff; 0x00a400d5; 0x00b10086; 0x00a5006a;
	0x3200ffff; 0x00a400d5; 0x00b0001c; 0x00b10086;
	0x3200ffff; 0x00a5006a; 0x00b20032; 0x00a6004c;
	0x3200ffff; 0x00a5006a; 0x00b10086; 0x00b20032;
	0x3200ffff; 0x00a70031; 0x00a800af; 0x009c0019;
	0x3200ffff; 0x00a70031; 0x00b30031; 0x00a800af;
	0x3200ffff; 0x00b400a3; 0x00c7006d; 0x00b500d7;
	0x3200ffff; 0x00b400a3; 0x00c600a3; 0x00c7006d;
	0x3200ffff; 0x00b500d7; 0x00c800a5; 0x00b60087;
	0x3200ffff; 0x00b500d7; 0x00c7006d; 0x00c800a5;
	0x3200ffff; 0x00b60087; 0x00c900d8; 0x00b7007c;
	0x3200ffff; 0x00b60087; 0x00c800a5; 0x00c900d8;
	0x3200ffff; 0x00b7007c; 0x00ca001f; 0x00b8001d;
	0x3200ffff; 0x00b7007c; 0x00c900d8; 0x00ca001f;
	0x3200ffff; 0x00b8001d; 0x00cb007d; 0x00b9001e;
	0x3200ffff; 0x00b8001d; 0x00ca001f; 0x00cb007d;
	0x3200ffff; 0x00b9001e; 0x00cc003f; 0x00ba0088;
	0x3200ffff; 0x00b9001e; 0x00cb007d; 0x00cc003f;
	0x3200ffff; 0x00ba0088; 0x00cd007e; 0x00bb00bc;
	0x3200ffff; 0x00ba0088; 0x00cc003f; 0x00cd007e;
	0x3200ffff; 0x00bb00bc; 0x00ce006b; 0x00bc006b;
	0x3200ffff; 0x00bb00bc; 0x00cd007e; 0x00ce006b;
	0x3200ffff; 0x00bd0057; 0x00d000d9; 0x00be0058;
	0x3200ffff; 0x00bd0057; 0x00cf0057; 0x00d000d9;
	0x3200ffff; 0x00be0058; 0x00d100cc; 0x00bf006c;
	0x3200ffff; 0x00be0058; 0x00d000d9; 0x00d100cc;
	0x3200ffff; 0x00bf006c; 0x00d20020; 0x00c000bd;
	0x3200ffff; 0x00bf006c; 0x00d100cc; 0x00d20020;
	0x3200ffff; 0x00c000bd; 0x00d30059; 0x00c10093;
	0x3200ffff; 0x00c000bd; 0x00d20020; 0x00d30059;
	0x3200ffff; 0x00c10093; 0x00d40033; 0x00c200ca;
	0x3200ffff; 0x00c10093; 0x00d30059; 0x00d40033;
	0x3200ffff; 0x00c200ca; 0x00d500b0; 0x00c300be;
	0x3200ffff; 0x00c200ca; 0x00d40033; 0x00d500b0;
	0x3200ffff; 0x00c300be; 0x00d600b1; 0x00c400a4;
	0x3200ffff; 0x00c300be; 0x00d500b0; 0x00d600b1;
	0x3200ffff; 0x00c400a4; 0x00d7006e; 0x00c500cb;
	0x3200ffff; 0x00c400a4; 0x00d600b1; 0x00d7006e;
	0x3200ffff; 0x00010094; 0x00020034; 0x00030094;
	0x3200ffff; 0x007b003d; 0x007d003d; 0x007c009c;
	0x3200ffff; 0x00a9003d; 0x00ab003d; 0x00aa009c;
	0x3200ffff; 0x00d7003d; 0x00c7003d; 0x00c6009c;

BezierPoly_0001:	/* 0x00000dac */
	0x2200ffff; 0x00010000; 0x00020050; 0x2200ffff;
	0x00010000; 0x0050004f; 0x2200ffff; 0x002e007f;
	0x002f007d; 0x2200ffff; 0x002e007f; 0x007d007c;
	0x2200ffff; 0x009d0009; 0x009e00aa; 0x2200ffff;
	0x009d0009; 0x00aa00a9; 0x2200ffff; 0x00a60000;
	0x00a700b3; 0x2200ffff; 0x00a60000; 0x00b300b2;
	0x2200ffff; 0x00bc00c9; 0x00bd00cf; 0x2200ffff;
	0x00bc00c9; 0x00cf00ce; 0x2200ffff; 0x00c50009;
	0x00b400c6; 0x2200ffff; 0x00c50009; 0x00c600d7;
	0x2200ffff; 0x002d0094; 0x002f002e; 0x2200ffff;
	0x00010094; 0x00040003; 0x2200ffff; 0x00000094;
	0x00040001; 0x2200ffff; 0x00000094; 0x00050004;
	0x2200ffff; 0x00000094; 0x00060005; 0x2200ffff;
	0x00000094; 0x00070006; 0x2200ffff; 0x00000094;
	0x00080007; 0x2200ffff; 0x00000094; 0x00090008;
	0x2200ffff; 0x00000094; 0x000a0009; 0x2200ffff;
	0x004d0094; 0x000a0000; 0x2200ffff; 0x004c0094;
	0x000a004d; 0x2200ffff; 0x004b0094; 0x000a004c;
	0x2200ffff; 0x002c0094; 0x002f002d; 0x2200ffff;
	0x002c0094; 0x0030002f; 0x2200ffff; 0x002b0094;
	0x0030002c; 0x2200ffff; 0x002a0094; 0x0030002b;
	0x2200ffff; 0x00290094; 0x0030002a; 0x2200ffff;
	0x00280094; 0x00300029; 0x2200ffff; 0x00270094;
	0x00300028; 0x2200ffff; 0x00270094; 0x00310030;
	0x2200ffff; 0x00260094; 0x00310027; 0x2200ffff;
	0x00260094; 0x00320031; 0x2200ffff; 0x00260094;
	0x00330032; 0x2200ffff; 0x00260094; 0x00340033;
	0x2200ffff; 0x004a0094; 0x000a004b; 0x2200ffff;
	0x00490094; 0x000a004a; 0x2200ffff; 0x00260094;
	0x00350034; 0x2200ffff; 0x00250094; 0x00350026;
	0x2200ffff; 0x00250094; 0x00360035; 0x2200ffff;
	0x00240094; 0x00360025; 0x2200ffff; 0x00240094;
	0x00370036; 0x2200ffff; 0x00230094; 0x00370024;
	0x2200ffff; 0x00230094; 0x00380037; 0x2200ffff;
	0x00480094; 0x000a0049; 0x2200ffff; 0x00480094;
	0x000b000a; 0x2200ffff; 0x00480094; 0x000c000b;
	0x2200ffff; 0x00470094; 0x000c0048; 0x2200ffff;
	0x00470094; 0x000d000c; 0x2200ffff; 0x00460094;
	0x000d0047; 0x2200ffff; 0x00460094; 0x000e000d;
	0x2200ffff; 0x00450094; 0x000e0046; 0x2200ffff;
	0x00220094; 0x00380023; 0x2200ffff; 0x00450094;
	0x000f000e; 0x2200ffff; 0x00440094; 0x000f0045;
	0x2200ffff; 0x00440094; 0x0010000f; 0x2200ffff;
	0x00430094; 0x00100044; 0x2200ffff; 0x00430094;
	0x00110010; 0x2200ffff; 0x00420094; 0x00110043;
	0x2200ffff; 0x00420094; 0x00120011; 0x2200ffff;
	0x00420094; 0x00130012; 0x2200ffff; 0x00410094;
	0x00130042; 0x2200ffff; 0x00410094; 0x00140013;
	0x2200ffff; 0x00400094; 0x00140041; 0x2200ffff;
	0x00400094; 0x00150014; 0x2200ffff; 0x00400094;
	0x00160015; 0x2200ffff; 0x003f0094; 0x00160040;
	0x2200ffff; 0x003f0094; 0x00170016; 0x2200ffff;
	0x003e0094; 0x0017003f; 0x2200ffff; 0x003e0094;
	0x00180017; 0x2200ffff; 0x003e0094; 0x00190018;
	0x2200ffff; 0x003e0094; 0x001a0019; 0x2200ffff;
	0x003e0094; 0x001b001a; 0x2200ffff; 0x003d0094;
	0x001b003e; 0x2200ffff; 0x003d0094; 0x001c001b;
	0x2200ffff; 0x003c0094; 0x001c003d; 0x2200ffff;
	0x003c0094; 0x001d001c; 0x2200ffff; 0x003b0094;
	0x001d003c; 0x2200ffff; 0x003b0094; 0x001e001d;
	0x2200ffff; 0x003b0094; 0x001f001e; 0x2200ffff;
	0x003b0094; 0x0020001f; 0x2200ffff; 0x003a0094;
	0x0020003b; 0x2200ffff; 0x003a0094; 0x00210020;
	0x2200ffff; 0x00390094; 0x0021003a; 0x2200ffff;
	0x00390094; 0x00220021; 0x2200ffff; 0x00390094;
	0x00380022; 0x2200ffff; 0x009d0094; 0x009f009e;
	0x2200ffff; 0x00a60094; 0x009c00a7; 0x2200ffff;
	0x00a50094; 0x009c00a6; 0x2200ffff; 0x00a50094;
	0x009d009c; 0x2200ffff; 0x00a40094; 0x009d00a5;
	0x2200ffff; 0x00a30094; 0x009d00a4; 0x2200ffff;
	0x00a20094; 0x009d00a3; 0x2200ffff; 0x00a10094;
	0x009d00a2; 0x2200ffff; 0x00a00094; 0x009d00a1;
	0x2200ffff; 0x00a00094; 0x009f009d; 0x2200ffff;
	0x00c50094; 0x00b500b4; 0x2200ffff; 0x00c50094;
	0x00b600b5; 0x2200ffff; 0x00c40094; 0x00b600c5;
	0x2200ffff; 0x00c40094; 0x00b700b6; 0x2200ffff;
	0x00c40094; 0x00b800b7; 0x2200ffff; 0x00c40094;
	0x00b900b8; 0x2200ffff; 0x00c40094; 0x00ba00b9;
	0x2200ffff; 0x00c40094; 0x00bb00ba; 0x2200ffff;
	0x00c40094; 0x00bc00bb; 0x2200ffff; 0x00c30094;
	0x00bc00c4; 0x2200ffff; 0x00c20094; 0x00bc00c3;
	0x2200ffff; 0x00c10094; 0x00bc00c2; 0x2200ffff;
	0x00c10094; 0x00bd00bc; 0x2200ffff; 0x00c00094;
	0x00bd00c1; 0x2200ffff; 0x00bf0094; 0x00bd00c0;
	0x2200ffff; 0x00be0094; 0x00bd00bf; 0x2200ffff;
	0x004f003d; 0x00500051; 0x2200ffff; 0x004f003d;
	0x00510052; 0x2200ffff; 0x004e003d; 0x004f0052;
	0x2200ffff; 0x004e003d; 0x00520053; 0x2200ffff;
	0x004e003d; 0x00530054; 0x2200ffff; 0x004e003d;
	0x00540055; 0x2200ffff; 0x004e003d; 0x00550056;
	0x2200ffff; 0x004e003d; 0x00560057; 0x2200ffff;
	0x004e003d; 0x00570058; 0x2200ffff; 0x009b003d;
	0x004e0058; 0x2200ffff; 0x009a003d; 0x009b0058;
	0x2200ffff; 0x0099003d; 0x009a0058; 0x2200ffff;
	0x007a003d; 0x007b007d; 0x2200ffff; 0x007a003d;
	0x007d007e; 0x2200ffff; 0x0079003d; 0x007a007e;
	0x2200ffff; 0x0078003d; 0x0079007e; 0x2200ffff;
	0x0077003d; 0x0078007e; 0x2200ffff; 0x0076003d;
	0x0077007e; 0x2200ffff; 0x0075003d; 0x0076007e;
	0x2200ffff; 0x0075003d; 0x007e007f; 0x2200ffff;
	0x0074003d; 0x0075007f; 0x2200ffff; 0x0074003d;
	0x007f0080; 0x2200ffff; 0x0074003d; 0x00800081;
	0x2200ffff; 0x0074003d; 0x00810082; 0x2200ffff;
	0x0098003d; 0x00990058; 0x2200ffff; 0x0097003d;
	0x00980058; 0x2200ffff; 0x0074003d; 0x00820083;
	0x2200ffff; 0x0073003d; 0x00740083; 0x2200ffff;
	0x0073003d; 0x00830084; 0x2200ffff; 0x0072003d;
	0x00730084; 0x2200ffff; 0x0072003d; 0x00840085;
	0x2200ffff; 0x0071003d; 0x00720085; 0x2200ffff;
	0x0071003d; 0x00850086; 0x2200ffff; 0x0096003d;
	0x00970058; 0x2200ffff; 0x0096003d; 0x00580059;
	0x2200ffff; 0x0096003d; 0x0059005a; 0x2200ffff;
	0x0095003d; 0x0096005a; 0x2200ffff; 0x0095003d;
	0x005a005b; 0x2200ffff; 0x0094003d; 0x0095005b;
	0x2200ffff; 0x0094003d; 0x005b005c; 0x2200ffff;
	0x0093003d; 0x0094005c; 0x2200ffff; 0x0070003d;
	0x00710086; 0x2200ffff; 0x0093003d; 0x005c005d;
	0x2200ffff; 0x0092003d; 0x0093005d; 0x2200ffff;
	0x0092003d; 0x005d005e; 0x2200ffff; 0x0091003d;
	0x0092005e; 0x2200ffff; 0x0091003d; 0x005e005f;
	0x2200ffff; 0x0090003d; 0x0091005f; 0x2200ffff;
	0x0090003d; 0x005f0060; 0x2200ffff; 0x0090003d;
	0x00600061; 0x2200ffff; 0x008f003d; 0x00900061;
	0x2200ffff; 0x008f003d; 0x00610062; 0x2200ffff;
	0x008e003d; 0x008f0062; 0x2200ffff; 0x008e003d;
	0x00620063; 0x2200ffff; 0x008e003d; 0x00630064;
	0x2200ffff; 0x008d003d; 0x008e0064; 0x2200ffff;
	0x008d003d; 0x00640065; 0x2200ffff; 0x008c003d;
	0x008d0065; 0x2200ffff; 0x008c003d; 0x00650066;
	0x2200ffff; 0x008c003d; 0x00660067; 0x2200ffff;
	0x008c003d; 0x00670068; 0x2200ffff; 0x008c003d;
	0x00680069; 0x2200ffff; 0x008b003d; 0x008c0069;
	0x2200ffff; 0x008b003d; 0x0069006a; 0x2200ffff;
	0x008a003d; 0x008b006a; 0x2200ffff; 0x008a003d;
	0x006a006b; 0x2200ffff; 0x0089003d; 0x008a006b;
	0x2200ffff; 0x0089003d; 0x006b006c; 0x2200ffff;
	0x0089003d; 0x006c006d; 0x2200ffff; 0x0089003d;
	0x006d006e; 0x2200ffff; 0x0088003d; 0x0089006e;
	0x2200ffff; 0x0088003d; 0x006e006f; 0x2200ffff;
	0x0087003d; 0x0088006f; 0x2200ffff; 0x0087003d;
	0x006f0070; 0x2200ffff; 0x0087003d; 0x00700086;
	0x2200ffff; 0x00b2003d; 0x00b300a8; 0x2200ffff;
	0x00b1003d; 0x00b200a8; 0x2200ffff; 0x00b1003d;
	0x00a800a9; 0x2200ffff; 0x00b0003d; 0x00b100a9;
	0x2200ffff; 0x00af003d; 0x00b000a9; 0x2200ffff;
	0x00ae003d; 0x00af00a9; 0x2200ffff; 0x00ad003d;
	0x00ae00a9; 0x2200ffff; 0x00ac003d; 0x00ad00a9;
	0x2200ffff; 0x00ac003d; 0x00a900ab; 0x2200ffff;
	0x00d7003d; 0x00c700c8; 0x2200ffff; 0x00d6003d;
	0x00d700c8; 0x2200ffff; 0x00d6003d; 0x00c800c9;
	0x2200ffff; 0x00d6003d; 0x00c900ca; 0x2200ffff;
	0x00d6003d; 0x00ca00cb; 0x2200ffff; 0x00d6003d;
	0x00cb00cc; 0x2200ffff; 0x00d6003d; 0x00cc00cd;
	0x2200ffff; 0x00d6003d; 0x00cd00ce; 0x2200ffff;
	0x00d5003d; 0x00d600ce; 0x2200ffff; 0x00d4003d;
	0x00d500ce; 0x2200ffff; 0x00d3003d; 0x00d400ce;
	0x2200ffff; 0x00d3003d; 0x00ce00cf; 0x2200ffff;
	0x00d2003d; 0x00d300cf; 0x2200ffff; 0x00d1003d;
	0x00d200cf; 0x2200ffff; 0x00d0003d; 0x00d100cf;

BezierVert_0000:	/* 0x0000179c */
	SVECTOR(  154,   167,     0);	/* vert[0] */
	SVECTOR(  363,   327,     0);	/* vert[1] */
	SVECTOR(  453,   237,     0);	/* vert[2] */
	SVECTOR(  433,   227,     0);	/* vert[3] */
	SVECTOR(  408,   212,     0);	/* vert[4] */
	SVECTOR(  379,   195,     0);	/* vert[5] */
	SVECTOR(  348,   176,     0);	/* vert[6] */
	SVECTOR(  317,   156,     0);	/* vert[7] */
	SVECTOR(  288,   136,     0);	/* vert[8] */
	SVECTOR(  261,   118,     0);	/* vert[9] */
	SVECTOR(    0,   -86,     0);	/* vert[10] */
	SVECTOR(   -4,   -90,     0);	/* vert[11] */
	SVECTOR(  -10,   -94,     0);	/* vert[12] */
	SVECTOR(  -17,   -98,     0);	/* vert[13] */
	SVECTOR(  -24,  -102,     0);	/* vert[14] */
	SVECTOR(  -30,  -106,     0);	/* vert[15] */
	SVECTOR(  -37,  -109,     0);	/* vert[16] */
	SVECTOR(  -42,  -111,     0);	/* vert[17] */
	SVECTOR(  -48,  -113,     0);	/* vert[18] */
	SVECTOR(  -55,  -115,     0);	/* vert[19] */
	SVECTOR(  -63,  -117,     0);	/* vert[20] */
	SVECTOR(  -71,  -119,     0);	/* vert[21] */
	SVECTOR(  -79,  -120,     0);	/* vert[22] */
	SVECTOR(  -87,  -121,     0);	/* vert[23] */
	SVECTOR(  -93,  -121,     0);	/* vert[24] */
	SVECTOR(  -98,  -121,     0);	/* vert[25] */
	SVECTOR( -103,  -120,     0);	/* vert[26] */
	SVECTOR( -110,  -119,     0);	/* vert[27] */
	SVECTOR( -116,  -117,     0);	/* vert[28] */
	SVECTOR( -122,  -115,     0);	/* vert[29] */
	SVECTOR( -127,  -113,     0);	/* vert[30] */
	SVECTOR( -132,  -111,     0);	/* vert[31] */
	SVECTOR( -137,  -109,     0);	/* vert[32] */
	SVECTOR( -143,  -106,     0);	/* vert[33] */
	SVECTOR( -149,  -102,     0);	/* vert[34] */
	SVECTOR( -155,   -98,     0);	/* vert[35] */
	SVECTOR( -161,   -94,     0);	/* vert[36] */
	SVECTOR( -167,   -90,     0);	/* vert[37] */
	SVECTOR( -171,   -86,     0);	/* vert[38] */
	SVECTOR( -326,    50,     0);	/* vert[39] */
	SVECTOR( -342,    62,     0);	/* vert[40] */
	SVECTOR( -360,    76,     0);	/* vert[41] */
	SVECTOR( -380,    90,     0);	/* vert[42] */
	SVECTOR( -399,   104,     0);	/* vert[43] */
	SVECTOR( -418,   117,     0);	/* vert[44] */
	SVECTOR( -435,   127,     0);	/* vert[45] */
	SVECTOR( -449,   136,     0);	/* vert[46] */
	SVECTOR( -370,   214,     0);	/* vert[47] */
	SVECTOR( -275,   134,     0);	/* vert[48] */
	SVECTOR( -124,     0,     0);	/* vert[49] */
	SVECTOR( -122,    -2,     0);	/* vert[50] */
	SVECTOR( -120,    -4,     0);	/* vert[51] */
	SVECTOR( -117,    -6,     0);	/* vert[52] */
	SVECTOR( -115,    -8,     0);	/* vert[53] */
	SVECTOR( -112,    -9,     0);	/* vert[54] */
	SVECTOR( -109,   -11,     0);	/* vert[55] */
	SVECTOR( -107,   -12,     0);	/* vert[56] */
	SVECTOR( -104,   -13,     0);	/* vert[57] */
	SVECTOR( -101,   -13,     0);	/* vert[58] */
	SVECTOR(  -98,   -14,     0);	/* vert[59] */
	SVECTOR(  -95,   -15,     0);	/* vert[60] */
	SVECTOR(  -91,   -15,     0);	/* vert[61] */
	SVECTOR(  -88,   -16,     0);	/* vert[62] */
	SVECTOR(  -85,   -16,     0);	/* vert[63] */
	SVECTOR(  -82,   -15,     0);	/* vert[64] */
	SVECTOR(  -79,   -15,     0);	/* vert[65] */
	SVECTOR(  -75,   -14,     0);	/* vert[66] */
	SVECTOR(  -72,   -13,     0);	/* vert[67] */
	SVECTOR(  -68,   -12,     0);	/* vert[68] */
	SVECTOR(  -65,   -11,     0);	/* vert[69] */
	SVECTOR(  -62,   -10,     0);	/* vert[70] */
	SVECTOR(  -59,    -8,     0);	/* vert[71] */
	SVECTOR(  -56,    -6,     0);	/* vert[72] */
	SVECTOR(  -52,    -4,     0);	/* vert[73] */
	SVECTOR(  -48,    -2,     0);	/* vert[74] */
	SVECTOR(  -45,     0,     0);	/* vert[75] */
	SVECTOR(  -41,     1,     0);	/* vert[76] */
	SVECTOR(  -38,     3,     0);	/* vert[77] */
	SVECTOR(  154,   167,  -300);	/* vert[78] */
	SVECTOR(  363,   327,  -300);	/* vert[79] */
	SVECTOR(  453,   237,  -300);	/* vert[80] */
	SVECTOR(  433,   227,  -300);	/* vert[81] */
	SVECTOR(  408,   212,  -300);	/* vert[82] */
	SVECTOR(  379,   195,  -300);	/* vert[83] */
	SVECTOR(  348,   176,  -300);	/* vert[84] */
	SVECTOR(  317,   156,  -300);	/* vert[85] */
	SVECTOR(  288,   136,  -300);	/* vert[86] */
	SVECTOR(  261,   118,  -300);	/* vert[87] */
	SVECTOR(    0,   -86,  -300);	/* vert[88] */
	SVECTOR(   -4,   -90,  -300);	/* vert[89] */
	SVECTOR(  -10,   -94,  -300);	/* vert[90] */
	SVECTOR(  -17,   -98,  -300);	/* vert[91] */
	SVECTOR(  -24,  -102,  -300);	/* vert[92] */
	SVECTOR(  -30,  -106,  -300);	/* vert[93] */
	SVECTOR(  -37,  -109,  -300);	/* vert[94] */
	SVECTOR(  -42,  -111,  -300);	/* vert[95] */
	SVECTOR(  -48,  -113,  -300);	/* vert[96] */
	SVECTOR(  -55,  -115,  -300);	/* vert[97] */
	SVECTOR(  -63,  -117,  -300);	/* vert[98] */
	SVECTOR(  -71,  -119,  -300);	/* vert[99] */
	SVECTOR(  -79,  -120,  -300);	/* vert[100] */
	SVECTOR(  -87,  -121,  -300);	/* vert[101] */
	SVECTOR(  -93,  -121,  -300);	/* vert[102] */
	SVECTOR(  -98,  -121,  -300);	/* vert[103] */
	SVECTOR( -103,  -120,  -300);	/* vert[104] */
	SVECTOR( -110,  -119,  -300);	/* vert[105] */
	SVECTOR( -116,  -117,  -300);	/* vert[106] */
	SVECTOR( -122,  -115,  -300);	/* vert[107] */
	SVECTOR( -127,  -113,  -300);	/* vert[108] */
	SVECTOR( -132,  -111,  -300);	/* vert[109] */
	SVECTOR( -137,  -109,  -300);	/* vert[110] */
	SVECTOR( -143,  -106,  -300);	/* vert[111] */
	SVECTOR( -149,  -102,  -300);	/* vert[112] */
	SVECTOR( -155,   -98,  -300);	/* vert[113] */
	SVECTOR( -161,   -94,  -300);	/* vert[114] */
	SVECTOR( -167,   -90,  -300);	/* vert[115] */
	SVECTOR( -171,   -86,  -300);	/* vert[116] */
	SVECTOR( -326,    50,  -300);	/* vert[117] */
	SVECTOR( -342,    62,  -300);	/* vert[118] */
	SVECTOR( -360,    76,  -300);	/* vert[119] */
	SVECTOR( -380,    90,  -300);	/* vert[120] */
	SVECTOR( -399,   104,  -300);	/* vert[121] */
	SVECTOR( -418,   117,  -300);	/* vert[122] */
	SVECTOR( -435,   127,  -300);	/* vert[123] */
	SVECTOR( -449,   136,  -300);	/* vert[124] */
	SVECTOR( -370,   214,  -300);	/* vert[125] */
	SVECTOR( -275,   134,  -300);	/* vert[126] */
	SVECTOR( -124,     0,  -300);	/* vert[127] */
	SVECTOR( -122,    -2,  -300);	/* vert[128] */
	SVECTOR( -120,    -4,  -300);	/* vert[129] */
	SVECTOR( -117,    -6,  -300);	/* vert[130] */
	SVECTOR( -115,    -8,  -300);	/* vert[131] */
	SVECTOR( -112,    -9,  -300);	/* vert[132] */
	SVECTOR( -109,   -11,  -300);	/* vert[133] */
	SVECTOR( -107,   -12,  -300);	/* vert[134] */
	SVECTOR( -104,   -13,  -300);	/* vert[135] */
	SVECTOR( -101,   -13,  -300);	/* vert[136] */
	SVECTOR(  -98,   -14,  -300);	/* vert[137] */
	SVECTOR(  -95,   -15,  -300);	/* vert[138] */
	SVECTOR(  -91,   -15,  -300);	/* vert[139] */
	SVECTOR(  -88,   -16,  -300);	/* vert[140] */
	SVECTOR(  -85,   -16,  -300);	/* vert[141] */
	SVECTOR(  -82,   -15,  -300);	/* vert[142] */
	SVECTOR(  -79,   -15,  -300);	/* vert[143] */
	SVECTOR(  -75,   -14,  -300);	/* vert[144] */
	SVECTOR(  -72,   -13,  -300);	/* vert[145] */
	SVECTOR(  -68,   -12,  -300);	/* vert[146] */
	SVECTOR(  -65,   -11,  -300);	/* vert[147] */
	SVECTOR(  -62,   -10,  -300);	/* vert[148] */
	SVECTOR(  -59,    -8,  -300);	/* vert[149] */
	SVECTOR(  -56,    -6,  -300);	/* vert[150] */
	SVECTOR(  -52,    -4,  -300);	/* vert[151] */
	SVECTOR(  -48,    -2,  -300);	/* vert[152] */
	SVECTOR(  -45,     0,  -300);	/* vert[153] */
	SVECTOR(  -41,     1,  -300);	/* vert[154] */
	SVECTOR(  -38,     3,  -300);	/* vert[155] */
	SVECTOR(  209,  -189,     0);	/* vert[156] */
	SVECTOR(  132,  -234,     0);	/* vert[157] */
	SVECTOR(   86,  -188,     0);	/* vert[158] */
	SVECTOR(   95,  -183,     0);	/* vert[159] */
	SVECTOR(  106,  -176,     0);	/* vert[160] */
	SVECTOR(  118,  -169,     0);	/* vert[161] */
	SVECTOR(  131,  -162,     0);	/* vert[162] */
	SVECTOR(  143,  -154,     0);	/* vert[163] */
	SVECTOR(  154,  -147,     0);	/* vert[164] */
	SVECTOR(  164,  -141,     0);	/* vert[165] */
	SVECTOR(  246,   -82,     0);	/* vert[166] */
	SVECTOR(  296,  -133,     0);	/* vert[167] */
	SVECTOR(  209,  -189,  -300);	/* vert[168] */
	SVECTOR(  132,  -234,  -300);	/* vert[169] */
	SVECTOR(   86,  -188,  -300);	/* vert[170] */
	SVECTOR(   95,  -183,  -300);	/* vert[171] */
	SVECTOR(  106,  -176,  -300);	/* vert[172] */
	SVECTOR(  118,  -169,  -300);	/* vert[173] */
	SVECTOR(  131,  -162,  -300);	/* vert[174] */
	SVECTOR(  143,  -154,  -300);	/* vert[175] */
	SVECTOR(  154,  -147,  -300);	/* vert[176] */
	SVECTOR(  164,  -141,  -300);	/* vert[177] */
	SVECTOR(  246,   -82,  -300);	/* vert[178] */
	SVECTOR(  296,  -133,  -300);	/* vert[179] */
	SVECTOR(  168,  -281,     0);	/* vert[180] */
	SVECTOR(  176,  -276,     0);	/* vert[181] */
	SVECTOR(  187,  -270,     0);	/* vert[182] */
	SVECTOR(  198,  -263,     0);	/* vert[183] */
	SVECTOR(  210,  -256,     0);	/* vert[184] */
	SVECTOR(  222,  -248,     0);	/* vert[185] */
	SVECTOR(  232,  -241,     0);	/* vert[186] */
	SVECTOR(  242,  -234,     0);	/* vert[187] */
	SVECTOR(  324,  -172,     0);	/* vert[188] */
	SVECTOR(  371,  -223,     0);	/* vert[189] */
	SVECTOR(  360,  -231,     0);	/* vert[190] */
	SVECTOR(  348,  -240,     0);	/* vert[191] */
	SVECTOR(  336,  -250,     0);	/* vert[192] */
	SVECTOR(  322,  -260,     0);	/* vert[193] */
	SVECTOR(  310,  -269,     0);	/* vert[194] */
	SVECTOR(  298,  -277,     0);	/* vert[195] */
	SVECTOR(  289,  -283,     0);	/* vert[196] */
	SVECTOR(  214,  -328,     0);	/* vert[197] */
	SVECTOR(  168,  -281,  -300);	/* vert[198] */
	SVECTOR(  176,  -276,  -300);	/* vert[199] */
	SVECTOR(  187,  -270,  -300);	/* vert[200] */
	SVECTOR(  198,  -263,  -300);	/* vert[201] */
	SVECTOR(  210,  -256,  -300);	/* vert[202] */
	SVECTOR(  222,  -248,  -300);	/* vert[203] */
	SVECTOR(  232,  -241,  -300);	/* vert[204] */
	SVECTOR(  242,  -234,  -300);	/* vert[205] */
	SVECTOR(  324,  -172,  -300);	/* vert[206] */
	SVECTOR(  371,  -223,  -300);	/* vert[207] */
	SVECTOR(  360,  -231,  -300);	/* vert[208] */
	SVECTOR(  348,  -240,  -300);	/* vert[209] */
	SVECTOR(  336,  -250,  -300);	/* vert[210] */
	SVECTOR(  322,  -260,  -300);	/* vert[211] */
	SVECTOR(  310,  -269,  -300);	/* vert[212] */
	SVECTOR(  298,  -277,  -300);	/* vert[213] */
	SVECTOR(  289,  -283,  -300);	/* vert[214] */
	SVECTOR(  214,  -328,  -300);	/* vert[215] */

BezierNorm_0000:	/* 0x00001e5c */
	SVECTOR( 2896,  2896,     0);	/* norm[0] */
	SVECTOR( 1490, -3815,     0);	/* norm[1] */
	SVECTOR(-2310, -3381,     0);	/* norm[2] */
	SVECTOR(-1810,  3674,     0);	/* norm[3] */
	SVECTOR( -357, -4080,     0);	/* norm[4] */
	SVECTOR(-2191, -3460,     0);	/* norm[5] */
	SVECTOR( 2694,  3084,     0);	/* norm[6] */
	SVECTOR(-2123,  3502,     0);	/* norm[7] */
	SVECTOR(-2178,  3468,     0);	/* norm[8] */
	SVECTOR(-2896, -2896,     0);	/* norm[9] */
	SVECTOR(-1932,  3611,     0);	/* norm[10] */
	SVECTOR(  826, -4011,     0);	/* norm[11] */
	SVECTOR( -468, -4069,     0);	/* norm[12] */
	SVECTOR(-2361, -3346,     0);	/* norm[13] */
	SVECTOR(-2119, -3504,     0);	/* norm[14] */
	SVECTOR(-2139,  3492,     0);	/* norm[15] */
	SVECTOR(-2224,  3439,     0);	/* norm[16] */
	SVECTOR( 2180, -3467,     0);	/* norm[17] */
	SVECTOR( 2148, -3487,     0);	/* norm[18] */
	SVECTOR( 1057, -3957,     0);	/* norm[19] */
	SVECTOR(   37, -4095,     0);	/* norm[20] */
	SVECTOR( 2225,  3438,     0);	/* norm[21] */
	SVECTOR(  524,  4062,     0);	/* norm[22] */
	SVECTOR(   29,  4095,     0);	/* norm[23] */
	SVECTOR(-2034,  3554,     0);	/* norm[24] */
	SVECTOR( 2127, -3500,     0);	/* norm[25] */
	SVECTOR( 2080, -3528,     0);	/* norm[26] */
	SVECTOR(-1962,  3594,     0);	/* norm[27] */
	SVECTOR(-2241,  3428,     0);	/* norm[28] */
	SVECTOR(-2210,  3448,     0);	/* norm[29] */
	SVECTOR(-2247,  3424,     0);	/* norm[30] */
	SVECTOR(-2198,  3455,     0);	/* norm[31] */
	SVECTOR( 2455, -3278,     0);	/* norm[32] */
	SVECTOR(-2545,  3209,     0);	/* norm[33] */
	SVECTOR( 2147, -3488,     0);	/* norm[34] */
	SVECTOR( 2195, -3457,     0);	/* norm[35] */
	SVECTOR( 2018, -3564,     0);	/* norm[36] */
	SVECTOR( 1907, -3624,     0);	/* norm[37] */
	SVECTOR( 1208, -3913,     0);	/* norm[38] */
	SVECTOR( -104, -4094,     0);	/* norm[39] */
	SVECTOR(-1113, -3941,     0);	/* norm[40] */
	SVECTOR(-2278, -3403,     0);	/* norm[41] */
	SVECTOR( 1156,  3929,     0);	/* norm[42] */
	SVECTOR(  946,  3985,     0);	/* norm[43] */
	SVECTOR(  783,  4020,     0);	/* norm[44] */
	SVECTOR(-1559,  3787,     0);	/* norm[45] */
	SVECTOR(  348, -4081,     0);	/* norm[46] */
	SVECTOR(-1241, -3903,     0);	/* norm[47] */
	SVECTOR(-1053,  3958,     0);	/* norm[48] */
	SVECTOR( 2218, -3442,     0);	/* norm[49] */
	SVECTOR(-2380,  3333,     0);	/* norm[50] */
	SVECTOR( 2381, -3332,     0);	/* norm[51] */
	SVECTOR(    0,     0,  4096);	/* norm[52] */
	SVECTOR( 2244, -3426,     0);	/* norm[53] */
	SVECTOR(-1559, -3787,     0);	/* norm[54] */
	SVECTOR(-2468, -3268,     0);	/* norm[55] */
	SVECTOR( 2628,  3141,     0);	/* norm[56] */
	SVECTOR( 2018,  3563,     0);	/* norm[57] */
	SVECTOR(  247,  4088,     0);	/* norm[58] */
	SVECTOR( -731,  4030,     0);	/* norm[59] */
	SVECTOR(-2092,  3521,     0);	/* norm[60] */
	SVECTOR(    0,     0, -4096);	/* norm[61] */
	SVECTOR(  721, -4031,     0);	/* norm[62] */
	SVECTOR(-2277,  3404,     0);	/* norm[63] */
	SVECTOR( 2526, -3224,     0);	/* norm[64] */
	SVECTOR(  494, -4066,     0);	/* norm[65] */
	SVECTOR(-2248, -3423,     0);	/* norm[66] */
	SVECTOR( 2733,  3050,     0);	/* norm[67] */
	SVECTOR( 2627,  3142,     0);	/* norm[68] */
	SVECTOR( 2457,  3276,     0);	/* norm[69] */
	SVECTOR( 1968, -3591,     0);	/* norm[70] */
	SVECTOR( -869, -4002,     0);	/* norm[71] */
	SVECTOR(-1745, -3705,     0);	/* norm[72] */
	SVECTOR(-1980, -3585,     0);	/* norm[73] */
	SVECTOR( -357,  4080,     0);	/* norm[74] */
	SVECTOR( -655,  4043,     0);	/* norm[75] */
	SVECTOR(-2380,  3333,     0);	/* norm[76] */
	SVECTOR( 2090, -3522,     0);	/* norm[77] */
	SVECTOR( 2459, -3275,     0);	/* norm[78] */
	SVECTOR( 1794,  3682,     0);	/* norm[79] */
	SVECTOR(  465,  4069,     0);	/* norm[80] */
	SVECTOR( -655, -4043,     0);	/* norm[81] */
	SVECTOR(-1468, -3823,     0);	/* norm[82] */
	SVECTOR( 2362,  3346,     0);	/* norm[83] */
	SVECTOR(-1901,  3627,     0);	/* norm[84] */
	SVECTOR(-2073,  3532,     0);	/* norm[85] */
	SVECTOR(-2124,  3502,     0);	/* norm[86] */
	SVECTOR( 2557, -3199,     0);	/* norm[87] */
	SVECTOR( 2523, -3226,     0);	/* norm[88] */
	SVECTOR( 2420, -3304,     0);	/* norm[89] */
	SVECTOR( 2448, -3283,     0);	/* norm[90] */
	SVECTOR( 2345, -3357,     0);	/* norm[91] */
	SVECTOR( 2218, -3443,     0);	/* norm[92] */
	SVECTOR(  669, -4040,     0);	/* norm[93] */
	SVECTOR(-1310, -3880,     0);	/* norm[94] */
	SVECTOR(-2025, -3560,     0);	/* norm[95] */
	SVECTOR(-2130, -3498,     0);	/* norm[96] */
	SVECTOR( 2179,  3468,     0);	/* norm[97] */
	SVECTOR( 2130, -3498,     0);	/* norm[98] */
	SVECTOR(-2184, -3465,     0);	/* norm[99] */
	SVECTOR(-2650, -3123,     0);	/* norm[100] */
	SVECTOR(-2328, -3370,     0);	/* norm[101] */
	SVECTOR(-2271, -3408,     0);	/* norm[102] */
	SVECTOR(  327,  4082,     0);	/* norm[103] */
	SVECTOR(-1730,  3712,     0);	/* norm[104] */
	SVECTOR(-2035,  3554,     0);	/* norm[105] */
	SVECTOR(-2351,  3353,     0);	/* norm[106] */
	SVECTOR(-2482,  3258,     0);	/* norm[107] */
	SVECTOR( 2479, -3260,     0);	/* norm[108] */
	SVECTOR(-2063,  3538,     0);	/* norm[109] */
	SVECTOR( 2120, -3504,     0);	/* norm[110] */
	SVECTOR(-2491,  3251,     0);	/* norm[111] */
	SVECTOR(-2486, -3255,     0);	/* norm[112] */
	SVECTOR(-2582, -3179,     0);	/* norm[113] */
	SVECTOR( 2698,  3081,     0);	/* norm[114] */
	SVECTOR(-2650,  3123,     0);	/* norm[115] */
	SVECTOR( 2255, -3418,     0);	/* norm[116] */
	SVECTOR( 1946, -3603,     0);	/* norm[117] */
	SVECTOR( 1812, -3673,     0);	/* norm[118] */
	SVECTOR(-1053, -3958,     0);	/* norm[119] */
	SVECTOR(-2255, -3418,     0);	/* norm[120] */
	SVECTOR( 1007,  3970,     0);	/* norm[121] */
	SVECTOR(-2090,  3522,     0);	/* norm[122] */
	SVECTOR( 2080, -3528,     0);	/* norm[123] */
	SVECTOR(-2175,  3470,     0);	/* norm[124] */
	SVECTOR(-2234,  3432,     0);	/* norm[125] */
	SVECTOR(-2370,  3340,     0);	/* norm[126] */
	SVECTOR(-2896,  2896,     0);	/* norm[127] */
	SVECTOR( 2760,  3025,     0);	/* norm[128] */
	SVECTOR(  555, -4058,     0);	/* norm[129] */
	SVECTOR(-2416, -3307,     0);	/* norm[130] */
	SVECTOR(-2377, -3335,     0);	/* norm[131] */
	SVECTOR( -869,  4002,     0);	/* norm[132] */
	SVECTOR(-2140,  3492,     0);	/* norm[133] */
	SVECTOR(-2322,  3373,     0);	/* norm[134] */
	SVECTOR(-2136,  3494,     0);	/* norm[135] */
	SVECTOR(-2295,  3392,     0);	/* norm[136] */
	SVECTOR( 1755, -3700,     0);	/* norm[137] */
	SVECTOR(-2208, -3449,     0);	/* norm[138] */
	SVECTOR(-1941,  3606,     0);	/* norm[139] */
	SVECTOR( 2228, -3436,     0);	/* norm[140] */
	SVECTOR( 2397, -3320,     0);	/* norm[141] */
	SVECTOR( 2051, -3545,     0);	/* norm[142] */
	SVECTOR( 1595, -3772,     0);	/* norm[143] */
	SVECTOR(  879, -4000,     0);	/* norm[144] */
	SVECTOR(-2491, -3251,     0);	/* norm[145] */
	SVECTOR(-2650,  3123,     0);	/* norm[146] */
	SVECTOR( 2408, -3313,     0);	/* norm[147] */
	SVECTOR(    0,     0,  4096);	/* norm[148] */
	SVECTOR(-1841, -3658,     0);	/* norm[149] */
	SVECTOR(-2716, -3065,     0);	/* norm[150] */
	SVECTOR(  632,  4046,     0);	/* norm[151] */
	SVECTOR( -108,  4094,     0);	/* norm[152] */
	SVECTOR( -468,  4069,     0);	/* norm[153] */
	SVECTOR(-2002,  3573,     0);	/* norm[154] */
	SVECTOR(-2100, -3516,     0);	/* norm[155] */
	SVECTOR(    0,     0, -4096);	/* norm[156] */
	SVECTOR( 2663,  3111,     0);	/* norm[157] */
	SVECTOR(  682,  4038,     0);	/* norm[158] */
	SVECTOR(-1468,  3823,     0);	/* norm[159] */
	SVECTOR(-1985,  3582,     0);	/* norm[160] */
	SVECTOR(-2078,  3529,     0);	/* norm[161] */
	SVECTOR(-2176,  3469,     0);	/* norm[162] */
	SVECTOR(-2040,  3551,     0);	/* norm[163] */
	SVECTOR( 2176, -3470,     0);	/* norm[164] */
	SVECTOR(-2122,  3503,     0);	/* norm[165] */
	SVECTOR( 1930, -3612,     0);	/* norm[166] */
	SVECTOR( 2006, -3570,     0);	/* norm[167] */
	SVECTOR(-2354, -3351,     0);	/* norm[168] */
	SVECTOR( 1458,  3827,     0);	/* norm[169] */
	SVECTOR(-1310,  3880,     0);	/* norm[170] */
	SVECTOR(-2597,  3166,     0);	/* norm[171] */
	SVECTOR(-2328, -3369,     0);	/* norm[172] */
	SVECTOR(-2428, -3298,     0);	/* norm[173] */
	SVECTOR(-1242,  3903,     0);	/* norm[174] */
	SVECTOR( 2173, -3471,     0);	/* norm[175] */
	SVECTOR( 2329, -3368,     0);	/* norm[176] */
	SVECTOR( 2230, -3435,     0);	/* norm[177] */
	SVECTOR( 2116, -3506,     0);	/* norm[178] */
	SVECTOR(  994, -3973,     0);	/* norm[179] */
	SVECTOR(-2411, -3311,     0);	/* norm[180] */
	SVECTOR( 2317,  3377,     0);	/* norm[181] */
	SVECTOR( -931,  3988,     0);	/* norm[182] */
	SVECTOR(-2201,  3454,     0);	/* norm[183] */
	SVECTOR( 2526, -3224,     0);	/* norm[184] */
	SVECTOR( 1584,  3776,     0);	/* norm[185] */
	SVECTOR(-1915,  3620,     0);	/* norm[186] */
	SVECTOR(-2195,  3457,     0);	/* norm[187] */
	SVECTOR(-2426,  3299,     0);	/* norm[188] */
	SVECTOR( 2443, -3287,     0);	/* norm[189] */
	SVECTOR( 2307, -3384,     0);	/* norm[190] */
	SVECTOR( 2303, -3387,     0);	/* norm[191] */
	SVECTOR( -731, -4030,     0);	/* norm[192] */
	SVECTOR( -931, -3988,     0);	/* norm[193] */
	SVECTOR(-1113,  3941,     0);	/* norm[194] */
	SVECTOR( 2282, -3401,     0);	/* norm[195] */
	SVECTOR( 1297, -3885,     0);	/* norm[196] */
	SVECTOR( 2075,  3530,     0);	/* norm[197] */
	SVECTOR( 1878,  3639,     0);	/* norm[198] */
	SVECTOR( 1244,  3902,     0);	/* norm[199] */
	SVECTOR(  834,  4010,     0);	/* norm[200] */
	SVECTOR( 3009,  2778,     0);	/* norm[201] */
	SVECTOR( 2366, -3342,     0);	/* norm[202] */
	SVECTOR( 2120, -3504,     0);	/* norm[203] */
	SVECTOR( 2492, -3250,     0);	/* norm[204] */
	SVECTOR(  263, -4087,     0);	/* norm[205] */
	SVECTOR(-2451, -3281,     0);	/* norm[206] */
	SVECTOR(-2155, -3482,     0);	/* norm[207] */
	SVECTOR(-2049,  3546,     0);	/* norm[208] */
	SVECTOR( 2067, -3536,     0);	/* norm[209] */
	SVECTOR( 2392, -3324,     0);	/* norm[210] */
	SVECTOR( 2508,  3237,     0);	/* norm[211] */
	SVECTOR(-2224,  3439,     0);	/* norm[212] */
	SVECTOR(-2267,  3411,     0);	/* norm[213] */
	SVECTOR(-2014,  3566,     0);	/* norm[214] */
	SVECTOR(-2085,  3525,     0);	/* norm[215] */
	SVECTOR(-2163,  3478,     0);	/* norm[216] */
	SVECTOR( 2540, -3212,     0);	/* norm[217] */
