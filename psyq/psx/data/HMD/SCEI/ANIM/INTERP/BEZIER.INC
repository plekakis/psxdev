BezierPolyPrimSet_0000:	/* 0x00000088 */
	TERMINATE;	/* next prim */
	BezierPolyPrimHdr_0000 / 4;
	M(2);	/* num of types */

BezierPolyPrim_0000:	/* 0x00000094 */
#ifdef DO_INI
	DEV_ID(SCE)|CTG(CTG_POLY)|DRV(INI|STP)
		|PRIM_TYPE(QUAD|IIP);	/* type[0] */
#else
	DEV_ID(SCE)|CTG(CTG_POLY)|DRV(STP)
		|PRIM_TYPE(QUAD|IIP);	/* type[0] */
#endif
	H(2); M(H(200));	/* size, data */
	(BezierPoly_0000 - BezierPoly_0000) / 4;

BezierPolyPrim_0001:	/* 0x000000a0 */
	DEV_ID(SCE)|CTG(CTG_POLY)|DRV(STP)
		|PRIM_TYPE(TRI|IIP);	/* type[1] */
	H(2); M(H(28));	/* size, data */
	(BezierPoly_0001 - BezierPoly_0000) / 4;

BezierPoly_0000:	/* 0x000000ac */
	0x3a00ffff; 0x006d006d; 0x006e006e; 0x00a500a5;
	0x00a400a4; 0x3a00ffff; 0x006b006b; 0x00000000;
	0x006c006c; 0x006d006d; 0x3a00ffff; 0x00000000;
	0x00010001; 0x006d006d; 0x006e006e; 0x3a00ffff;
	0x00010001; 0x00020002; 0x006e006e; 0x006f006f;
	0x3a00ffff; 0x00020002; 0x00030003; 0x006f006f;
	0x00700070; 0x3a00ffff; 0x00030003; 0x00040004;
	0x00700070; 0x00710071; 0x3a00ffff; 0x00040004;
	0x00050005; 0x00710071; 0x00720072; 0x3a00ffff;
	0x00050005; 0x00060006; 0x00720072; 0x00730073;
	0x3a00ffff; 0x00060006; 0x00070007; 0x00730073;
	0x00740074; 0x3a00ffff; 0x00070007; 0x00080008;
	0x00740074; 0x00750075; 0x3a00ffff; 0x00080008;
	0x00090009; 0x00750075; 0x00760076; 0x3a00ffff;
	0x00090009; 0x000a000a; 0x00760076; 0x00770077;
	0x3a00ffff; 0x000a000a; 0x000b000b; 0x00770077;
	0x00780078; 0x3a00ffff; 0x000b000b; 0x000c000c;
	0x00780078; 0x00790079; 0x3a00ffff; 0x000c000c;
	0x000d000d; 0x00790079; 0x007a007a; 0x3a00ffff;
	0x000d000d; 0x000e000e; 0x007a007a; 0x007b007b;
	0x3a00ffff; 0x000e000e; 0x000f000f; 0x007b007b;
	0x007c007c; 0x3a00ffff; 0x000f000f; 0x00100010;
	0x007c007c; 0x007d007d; 0x3a00ffff; 0x00100010;
	0x00110011; 0x007d007d; 0x007e007e; 0x3a00ffff;
	0x00110011; 0x00120012; 0x007e007e; 0x007f007f;
	0x3a00ffff; 0x00120012; 0x00130013; 0x007f007f;
	0x00800080; 0x3a00ffff; 0x00130013; 0x00140014;
	0x00800080; 0x00810081; 0x3a00ffff; 0x00140014;
	0x00150015; 0x00810081; 0x00820082; 0x3a00ffff;
	0x00150015; 0x00160016; 0x00820082; 0x00830083;
	0x3a00ffff; 0x00160016; 0x00170017; 0x00830083;
	0x00840084; 0x3a00ffff; 0x00170017; 0x00180018;
	0x00840084; 0x00850085; 0x3a00ffff; 0x00180018;
	0x00190019; 0x00850085; 0x00860086; 0x3a00ffff;
	0x00190019; 0x001a001a; 0x00860086; 0x00870087;
	0x3a00ffff; 0x001a001a; 0x001b001b; 0x00870087;
	0x00880088; 0x3a00ffff; 0x001b001b; 0x001c001c;
	0x00880088; 0x00890089; 0x3a00ffff; 0x001c001c;
	0x001d001d; 0x00890089; 0x008a008a; 0x3a00ffff;
	0x001d001d; 0x001e001e; 0x008a008a; 0x008b008b;
	0x3a00ffff; 0x001e001e; 0x001f001f; 0x008b008b;
	0x008c008c; 0x3a00ffff; 0x001f001f; 0x00200020;
	0x008c008c; 0x008d008d; 0x3a00ffff; 0x00200020;
	0x00210021; 0x008d008d; 0x008e008e; 0x3a00ffff;
	0x00210021; 0x00220022; 0x008e008e; 0x008f008f;
	0x3a00ffff; 0x00220022; 0x00230023; 0x008f008f;
	0x00900090; 0x3a00ffff; 0x00230023; 0x00240024;
	0x00900090; 0x00910091; 0x3a00ffff; 0x00240024;
	0x00250025; 0x00910091; 0x00920092; 0x3a00ffff;
	0x00250025; 0x00260026; 0x00920092; 0x00930093;
	0x3a00ffff; 0x00260026; 0x00270027; 0x00930093;
	0x00940094; 0x3a00ffff; 0x00270027; 0x00280028;
	0x00940094; 0x00950095; 0x3a00ffff; 0x00280028;
	0x00290029; 0x00950095; 0x00960096; 0x3a00ffff;
	0x00290029; 0x002a002a; 0x00960096; 0x00970097;
	0x3a00ffff; 0x002a002a; 0x002b002b; 0x00970097;
	0x00980098; 0x3a00ffff; 0x002b002b; 0x002c002c;
	0x00980098; 0x00990099; 0x3a00ffff; 0x002c002c;
	0x002d002d; 0x00990099; 0x009a009a; 0x3a00ffff;
	0x002d002d; 0x002e002e; 0x009a009a; 0x009b009b;
	0x3a00ffff; 0x002e002e; 0x002f002f; 0x009b009b;
	0x009c009c; 0x3a00ffff; 0x002f002f; 0x00300030;
	0x009c009c; 0x009d009d; 0x3a00ffff; 0x00300030;
	0x00310031; 0x009d009d; 0x009e009e; 0x3a00ffff;
	0x00310031; 0x00320032; 0x009e009e; 0x009f009f;
	0x3a00ffff; 0x00320032; 0x00330033; 0x009f009f;
	0x00a000a0; 0x3a00ffff; 0x00330033; 0x00340034;
	0x00a000a0; 0x00a100a1; 0x3a00ffff; 0x00340034;
	0x00350035; 0x00a100a1; 0x00a200a2; 0x3a00ffff;
	0x00350035; 0x00360036; 0x00a200a2; 0x00a300a3;
	0x3a00ffff; 0x00360036; 0x00370037; 0x00a300a3;
	0x00a400a4; 0x3a00ffff; 0x00370037; 0x00380038;
	0x00a400a4; 0x00a500a5; 0x3a00ffff; 0x00380038;
	0x00390039; 0x00a500a5; 0x00a600a6; 0x3a00ffff;
	0x00390039; 0x003a003a; 0x00a600a6; 0x00a700a7;
	0x3a00ffff; 0x003a003a; 0x003b003b; 0x00a700a7;
	0x00a800a8; 0x3a00ffff; 0x003b003b; 0x003c003c;
	0x00a800a8; 0x00a900a9; 0x3a00ffff; 0x003c003c;
	0x003d003d; 0x00a900a9; 0x00aa00aa; 0x3a00ffff;
	0x003d003d; 0x003e003e; 0x00aa00aa; 0x00ab00ab;
	0x3a00ffff; 0x003e003e; 0x003f003f; 0x00ab00ab;
	0x00ac00ac; 0x3a00ffff; 0x003f003f; 0x00400040;
	0x00ac00ac; 0x00ad00ad; 0x3a00ffff; 0x00400040;
	0x00410041; 0x00ad00ad; 0x00ae00ae; 0x3a00ffff;
	0x00410041; 0x00420042; 0x00ae00ae; 0x00af00af;
	0x3a00ffff; 0x00420042; 0x00430043; 0x00af00af;
	0x00b000b0; 0x3a00ffff; 0x00430043; 0x00440044;
	0x00b000b0; 0x00b100b1; 0x3a00ffff; 0x00440044;
	0x00450045; 0x00b100b1; 0x00b200b2; 0x3a00ffff;
	0x00450045; 0x00460046; 0x00b200b2; 0x00b300b3;
	0x3a00ffff; 0x00460046; 0x00470047; 0x00b300b3;
	0x00b400b4; 0x3a00ffff; 0x00470047; 0x00480048;
	0x00b400b4; 0x00b500b5; 0x3a00ffff; 0x00480048;
	0x00490049; 0x00b500b5; 0x00b600b6; 0x3a00ffff;
	0x00490049; 0x004a004a; 0x00b600b6; 0x00b700b7;
	0x3a00ffff; 0x004a004a; 0x004b004b; 0x00b700b7;
	0x00b800b8; 0x3a00ffff; 0x004b004b; 0x004c004c;
	0x00b800b8; 0x00b900b9; 0x3a00ffff; 0x004c004c;
	0x004d004d; 0x00b900b9; 0x00ba00ba; 0x3a00ffff;
	0x004d004d; 0x004e004e; 0x00ba00ba; 0x00bb00bb;
	0x3a00ffff; 0x004e004e; 0x004f004f; 0x00bb00bb;
	0x00bc00bc; 0x3a00ffff; 0x004f004f; 0x00500050;
	0x00bc00bc; 0x00bd00bd; 0x3a00ffff; 0x00500050;
	0x00510051; 0x00bd00bd; 0x00be00be; 0x3a00ffff;
	0x00510051; 0x00520052; 0x00be00be; 0x00bf00bf;
	0x3a00ffff; 0x00520052; 0x00530053; 0x00bf00bf;
	0x00c000c0; 0x3a00ffff; 0x00530053; 0x00540054;
	0x00c000c0; 0x00c100c1; 0x3a00ffff; 0x00540054;
	0x00550055; 0x00c100c1; 0x00c200c2; 0x3a00ffff;
	0x00550055; 0x00560056; 0x00c200c2; 0x00c300c3;
	0x3a00ffff; 0x00560056; 0x00570057; 0x00c300c3;
	0x00c400c4; 0x3a00ffff; 0x00570057; 0x00580058;
	0x00c400c4; 0x00c500c5; 0x3a00ffff; 0x00580058;
	0x00590059; 0x00c500c5; 0x00c600c6; 0x3a00ffff;
	0x00590059; 0x005a005a; 0x00c600c6; 0x00c700c7;
	0x3a00ffff; 0x005a005a; 0x005b005b; 0x00c700c7;
	0x00c800c8; 0x3a00ffff; 0x005b005b; 0x005c005c;
	0x00c800c8; 0x00c900c9; 0x3a00ffff; 0x005c005c;
	0x005d005d; 0x00c900c9; 0x00ca00ca; 0x3a00ffff;
	0x005d005d; 0x005e005e; 0x00ca00ca; 0x00cb00cb;
	0x3a00ffff; 0x005e005e; 0x005f005f; 0x00cb00cb;
	0x00cc00cc; 0x3a00ffff; 0x005f005f; 0x00600060;
	0x00cc00cc; 0x00cd00cd; 0x3a00ffff; 0x00600060;
	0x00610061; 0x00cd00cd; 0x00ce00ce; 0x3a00ffff;
	0x00610061; 0x00620062; 0x00ce00ce; 0x00cf00cf;
	0x3a00ffff; 0x00620062; 0x00630063; 0x00cf00cf;
	0x00d000d0; 0x3a00ffff; 0x00630063; 0x00640064;
	0x00d000d0; 0x00d100d1; 0x3a00ffff; 0x00640064;
	0x00650065; 0x00d100d1; 0x00d200d2; 0x3a00ffff;
	0x00650065; 0x00660066; 0x00d200d2; 0x00d300d3;
	0x3a00ffff; 0x00660066; 0x00670067; 0x00d300d3;
	0x00d400d4; 0x3a00ffff; 0x00670067; 0x00680068;
	0x00d400d4; 0x00d500d5; 0x3a00ffff; 0x00680068;
	0x00690069; 0x00d500d5; 0x00d600d6; 0x3a00ffff;
	0x00690069; 0x006a006a; 0x00d600d6; 0x00d700d7;
	0x3a00ffff; 0x006a006a; 0x006b006b; 0x00d700d7;
	0x006c006c; 0x3a00ffff; 0x004b004b; 0x00500050;
	0x004c004c; 0x004d004d; 0x3a00ffff; 0x003a003a;
	0x006a006a; 0x003b003b; 0x00690069; 0x3a00ffff;
	0x006a006a; 0x003a003a; 0x006b006b; 0x00390039;
	0x3a00ffff; 0x00020002; 0x00360036; 0x00030003;
	0x00350035; 0x3a00ffff; 0x00380038; 0x00000000;
	0x00390039; 0x006b006b; 0x3a00ffff; 0x00010001;
	0x00370037; 0x00020002; 0x00360036; 0x3a00ffff;
	0x00370037; 0x00010001; 0x00380038; 0x00000000;
	0x3a00ffff; 0x005c005c; 0x003e003e; 0x005d005d;
	0x003d003d; 0x3a00ffff; 0x00530053; 0x00440044;
	0x00540054; 0x00550055; 0x3a00ffff; 0x00590059;
	0x00440044; 0x005a005a; 0x00430043; 0x3a00ffff;
	0x005b005b; 0x003f003f; 0x005c005c; 0x003e003e;
	0x3a00ffff; 0x005a005a; 0x00410041; 0x005b005b;
	0x00400040; 0x3a00ffff; 0x00510051; 0x00460046;
	0x00520052; 0x00450045; 0x3a00ffff; 0x00500050;
	0x00470047; 0x00510051; 0x00460046; 0x3a00ffff;
	0x00290029; 0x00060006; 0x002a002a; 0x00050005;
	0x3a00ffff; 0x002b002b; 0x00320032; 0x002c002c;
	0x00310031; 0x3a00ffff; 0x00280028; 0x00070007;
	0x00290029; 0x00060006; 0x3a00ffff; 0x00240024;
	0x000f000f; 0x00250025; 0x000e000e; 0x3a00ffff;
	0x00250025; 0x000e000e; 0x00260026; 0x000d000d;
	0x3a00ffff; 0x00260026; 0x000d000d; 0x00270027;
	0x000c000c; 0x3a00ffff; 0x00230023; 0x00100010;
	0x00240024; 0x000f000f; 0x3a00ffff; 0x001d001d;
	0x00110011; 0x001e001e; 0x001f001f; 0x3a00ffff;
	0x00110011; 0x001d001d; 0x00120012; 0x001c001c;
	0x3a00ffff; 0x00210021; 0x00110011; 0x00220022;
	0x00230023; 0x3a00ffff; 0x001f001f; 0x00110011;
	0x00200020; 0x00210021; 0x3a00ffff; 0x00270027;
	0x00080008; 0x00280028; 0x00070007; 0x3a00ffff;
	0x00350035; 0x002a002a; 0x00040004; 0x00050005;
	0x3a00ffff; 0x002e002e; 0x00310031; 0x002f002f;
	0x00300030; 0x3a00ffff; 0x00340034; 0x002b002b;
	0x00350035; 0x002a002a; 0x3a00ffff; 0x00320032;
	0x002b002b; 0x00330033; 0x00340034; 0x3a00ffff;
	0x002c002c; 0x00310031; 0x002d002d; 0x002e002e;
	0x3a00ffff; 0x00570057; 0x00440044; 0x00580058;
	0x00590059; 0x3a00ffff; 0x00440044; 0x00530053;
	0x00450045; 0x00520052; 0x3a00ffff; 0x00550055;
	0x00440044; 0x00560056; 0x00570057; 0x3a00ffff;
	0x00680068; 0x005d005d; 0x003b003b; 0x003c003c;
	0x3a00ffff; 0x00660066; 0x005e005e; 0x00670067;
	0x00680068; 0x3a00ffff; 0x00610061; 0x00640064;
	0x00620062; 0x00630063; 0x3a00ffff; 0x00640064;
	0x005e005e; 0x00650065; 0x00660066; 0x3a00ffff;
	0x005f005f; 0x00640064; 0x00600060; 0x00610061;
	0x3a00ffff; 0x00500050; 0x00490049; 0x00470047;
	0x00480048; 0x3a00ffff; 0x00500050; 0x004b004b;
	0x00490049; 0x004a004a; 0x3a00ffff; 0x004d004d;
	0x00500050; 0x004e004e; 0x004f004f; 0x3a00ffff;
	0x001c001c; 0x00140014; 0x00120012; 0x00130013;
	0x3a00ffff; 0x00170017; 0x001a001a; 0x00180018;
	0x00190019; 0x3a00ffff; 0x001c001c; 0x00160016;
	0x00140014; 0x00150015; 0x3a00ffff; 0x00170017;
	0x001c001c; 0x001a001a; 0x001b001b; 0x3a00ffff;
	0x009f009f; 0x00980098; 0x009e009e; 0x00990099;
	0x3a00ffff; 0x00960096; 0x00730073; 0x00950095;
	0x00740074; 0x3a00ffff; 0x008e008e; 0x007e007e;
	0x008d008d; 0x008c008c; 0x3a00ffff; 0x00910091;
	0x007c007c; 0x00900090; 0x007d007d; 0x3a00ffff;
	0x00920092; 0x007b007b; 0x00910091; 0x007c007c;
	0x3a00ffff; 0x00930093; 0x007a007a; 0x00920092;
	0x007b007b; 0x3a00ffff; 0x00940094; 0x00790079;
	0x00930093; 0x007a007a; 0x3a00ffff; 0x00970097;
	0x00720072; 0x00960096; 0x00730073; 0x3a00ffff;
	0x00ca00ca; 0x00aa00aa; 0x00c900c9; 0x00ab00ab;
	0x3a00ffff; 0x00c900c9; 0x00ab00ab; 0x00c800c8;
	0x00ac00ac; 0x3a00ffff; 0x00bf00bf; 0x00b200b2;
	0x00be00be; 0x00b300b3; 0x3a00ffff; 0x00be00be;
	0x00b300b3; 0x00bd00bd; 0x00b400b4; 0x3a00ffff;
	0x00bd00bd; 0x00b800b8; 0x00ba00ba; 0x00b900b9;
	0x3a00ffff; 0x00c400c4; 0x00b100b1; 0x00c300c3;
	0x00c200c2; 0x3a00ffff; 0x00c700c7; 0x00b000b0;
	0x00c600c6; 0x00b100b1; 0x3a00ffff; 0x00c800c8;
	0x00ad00ad; 0x00c700c7; 0x00ae00ae; 0x3a00ffff;
	0x00a800a8; 0x00d600d6; 0x00a700a7; 0x00d700d7;
	0x3a00ffff; 0x006c006c; 0x00a600a6; 0x00d700d7;
	0x00a700a7; 0x3a00ffff; 0x00a300a3; 0x006f006f;
	0x00a200a2; 0x00700070; 0x3a00ffff; 0x006d006d;
	0x00a500a5; 0x006c006c; 0x00a600a6; 0x3a00ffff;
	0x00a400a4; 0x006e006e; 0x00a300a3; 0x006f006f;
	0x3a00ffff; 0x00cb00cb; 0x00d300d3; 0x00d500d5;
	0x00d400d4; 0x3a00ffff; 0x00d100d1; 0x00ce00ce;
	0x00d000d0; 0x00cf00cf; 0x3a00ffff; 0x00cb00cb;
	0x00d100d1; 0x00d300d3; 0x00d200d2; 0x3a00ffff;
	0x00d100d1; 0x00cc00cc; 0x00ce00ce; 0x00cd00cd;
	0x3a00ffff; 0x00ca00ca; 0x00d500d5; 0x00a900a9;
	0x00a800a8; 0x3a00ffff; 0x00b100b1; 0x00c400c4;
	0x00c600c6; 0x00c500c5; 0x3a00ffff; 0x00c000c0;
	0x00b100b1; 0x00bf00bf; 0x00b200b2; 0x3a00ffff;
	0x00c200c2; 0x00b100b1; 0x00c100c1; 0x00c000c0;
	0x3a00ffff; 0x00b600b6; 0x00bd00bd; 0x00b500b5;
	0x00b400b4; 0x3a00ffff; 0x00b800b8; 0x00bd00bd;
	0x00b700b7; 0x00b600b6; 0x3a00ffff; 0x00bd00bd;
	0x00ba00ba; 0x00bc00bc; 0x00bb00bb; 0x3a00ffff;
	0x00810081; 0x00890089; 0x00800080; 0x007f007f;
	0x3a00ffff; 0x00870087; 0x00840084; 0x00860086;
	0x00850085; 0x3a00ffff; 0x00830083; 0x00890089;
	0x00820082; 0x00810081; 0x3a00ffff; 0x00890089;
	0x00840084; 0x00880088; 0x00870087; 0x3a00ffff;
	0x008a008a; 0x007e007e; 0x00890089; 0x007f007f;
	0x3a00ffff; 0x007e007e; 0x008e008e; 0x00900090;
	0x008f008f; 0x3a00ffff; 0x008c008c; 0x007e007e;
	0x008b008b; 0x008a008a; 0x3a00ffff; 0x00750075;
	0x00940094; 0x00740074; 0x00950095; 0x3a00ffff;
	0x00970097; 0x00a200a2; 0x00720072; 0x00710071;
	0x3a00ffff; 0x009e009e; 0x009b009b; 0x009d009d;
	0x009c009c; 0x3a00ffff; 0x00980098; 0x00a100a1;
	0x00970097; 0x00a200a2; 0x3a00ffff; 0x00980098;
	0x009f009f; 0x00a100a1; 0x00a000a0; 0x3a00ffff;
	0x009e009e; 0x00990099; 0x009b009b; 0x009a009a;

BezierPoly_0001:	/* 0x0000104c */
	0x3200ffff; 0x00160016; 0x001c001c; 0x00170017;
	0x3200ffff; 0x005e005e; 0x00640064; 0x005f005f;
	0x3200ffff; 0x00030003; 0x00350035; 0x00040004;
	0x3200ffff; 0x003b003b; 0x00690069; 0x00680068;
	0x3200ffff; 0x00680068; 0x005e005e; 0x005d005d;
	0x3200ffff; 0x005d005d; 0x003d003d; 0x003c003c;
	0x3200ffff; 0x005a005a; 0x00430043; 0x00420042;
	0x3200ffff; 0x005a005a; 0x00420042; 0x00410041;
	0x3200ffff; 0x005b005b; 0x00400040; 0x003f003f;
	0x3200ffff; 0x00230023; 0x00110011; 0x00100010;
	0x3200ffff; 0x00270027; 0x000c000c; 0x000b000b;
	0x3200ffff; 0x00080008; 0x000a000a; 0x00090009;
	0x3200ffff; 0x000a000a; 0x00270027; 0x000b000b;
	0x3200ffff; 0x00270027; 0x000a000a; 0x00080008;
	0x3200ffff; 0x00940094; 0x00750075; 0x00770077;
	0x3200ffff; 0x00890089; 0x00830083; 0x00840084;
	0x3200ffff; 0x00900090; 0x007d007d; 0x007e007e;
	0x3200ffff; 0x00940094; 0x00780078; 0x00790079;
	0x3200ffff; 0x00770077; 0x00750075; 0x00760076;
	0x3200ffff; 0x00940094; 0x00770077; 0x00780078;
	0x3200ffff; 0x00ca00ca; 0x00a900a9; 0x00aa00aa;
	0x3200ffff; 0x00c800c8; 0x00ac00ac; 0x00ad00ad;
	0x3200ffff; 0x00c700c7; 0x00af00af; 0x00b000b0;
	0x3200ffff; 0x00c700c7; 0x00ae00ae; 0x00af00af;
	0x3200ffff; 0x00d500d5; 0x00ca00ca; 0x00cb00cb;
	0x3200ffff; 0x00d100d1; 0x00cb00cb; 0x00cc00cc;
	0x3200ffff; 0x00a200a2; 0x00700070; 0x00710071;
	0x3200ffff; 0x00d600d6; 0x00a800a8; 0x00d500d5;

BezierVert_0000:	/* 0x0000120c */
	SVECTOR(  441,  -581,     0);	/* vert[0] */
	SVECTOR(  448,  -590,     0);	/* vert[1] */
	SVECTOR(  455,  -600,     0);	/* vert[2] */
	SVECTOR(  463,  -612,     0);	/* vert[3] */
	SVECTOR(  464,  -618,     0);	/* vert[4] */
	SVECTOR(  462,  -625,     0);	/* vert[5] */
	SVECTOR(  457,  -630,     0);	/* vert[6] */
	SVECTOR(  451,  -632,     0);	/* vert[7] */
	SVECTOR(  436,  -633,     0);	/* vert[8] */
	SVECTOR(  425,  -632,     0);	/* vert[9] */
	SVECTOR(  366,  -635,     0);	/* vert[10] */
	SVECTOR(  330,  -636,     0);	/* vert[11] */
	SVECTOR(  280,  -634,     0);	/* vert[12] */
	SVECTOR(  238,  -629,     0);	/* vert[13] */
	SVECTOR(  215,  -622,     0);	/* vert[14] */
	SVECTOR(  191,  -610,     0);	/* vert[15] */
	SVECTOR(  170,  -594,     0);	/* vert[16] */
	SVECTOR(  153,  -574,     0);	/* vert[17] */
	SVECTOR(  143,  -552,     0);	/* vert[18] */
	SVECTOR(  136,  -538,     0);	/* vert[19] */
	SVECTOR(  133,  -534,     0);	/* vert[20] */
	SVECTOR(  129,  -531,     0);	/* vert[21] */
	SVECTOR(  125,  -529,     0);	/* vert[22] */
	SVECTOR(  119,  -529,     0);	/* vert[23] */
	SVECTOR(  114,  -531,     0);	/* vert[24] */
	SVECTOR(  107,  -536,     0);	/* vert[25] */
	SVECTOR(  104,  -544,     0);	/* vert[26] */
	SVECTOR(  104,  -548,     0);	/* vert[27] */
	SVECTOR(  107,  -562,     0);	/* vert[28] */
	SVECTOR(  130,  -661,     0);	/* vert[29] */
	SVECTOR(  132,  -665,     0);	/* vert[30] */
	SVECTOR(  138,  -671,     0);	/* vert[31] */
	SVECTOR(  145,  -674,     0);	/* vert[32] */
	SVECTOR(  157,  -676,     0);	/* vert[33] */
	SVECTOR(  164,  -676,     0);	/* vert[34] */
	SVECTOR(  187,  -674,     0);	/* vert[35] */
	SVECTOR(  201,  -673,     0);	/* vert[36] */
	SVECTOR(  228,  -672,     0);	/* vert[37] */
	SVECTOR(  307,  -670,     0);	/* vert[38] */
	SVECTOR(  355,  -669,     0);	/* vert[39] */
	SVECTOR(  395,  -669,     0);	/* vert[40] */
	SVECTOR(  507,  -670,     0);	/* vert[41] */
	SVECTOR(  575,  -671,     0);	/* vert[42] */
	SVECTOR(  604,  -672,     0);	/* vert[43] */
	SVECTOR(  635,  -672,     0);	/* vert[44] */
	SVECTOR(  650,  -670,     0);	/* vert[45] */
	SVECTOR(  657,  -668,     0);	/* vert[46] */
	SVECTOR(  659,  -666,     0);	/* vert[47] */
	SVECTOR(  660,  -663,     0);	/* vert[48] */
	SVECTOR(  659,  -657,     0);	/* vert[49] */
	SVECTOR(  658,  -651,     0);	/* vert[50] */
	SVECTOR(  657,  -648,     0);	/* vert[51] */
	SVECTOR(  650,  -638,     0);	/* vert[52] */
	SVECTOR(  645,  -634,     0);	/* vert[53] */
	SVECTOR(  216,   -94,     0);	/* vert[54] */
	SVECTOR(  208,   -84,     0);	/* vert[55] */
	SVECTOR(  202,   -75,     0);	/* vert[56] */
	SVECTOR(  195,   -65,     0);	/* vert[57] */
	SVECTOR(  193,   -60,     0);	/* vert[58] */
	SVECTOR(  192,   -52,     0);	/* vert[59] */
	SVECTOR(  194,   -47,     0);	/* vert[60] */
	SVECTOR(  198,   -43,     0);	/* vert[61] */
	SVECTOR(  209,   -39,     0);	/* vert[62] */
	SVECTOR(  236,   -35,     0);	/* vert[63] */
	SVECTOR(  295,   -36,     0);	/* vert[64] */
	SVECTOR(  338,   -38,     0);	/* vert[65] */
	SVECTOR(  410,   -42,     0);	/* vert[66] */
	SVECTOR(  459,   -50,     0);	/* vert[67] */
	SVECTOR(  488,   -61,     0);	/* vert[68] */
	SVECTOR(  509,   -75,     0);	/* vert[69] */
	SVECTOR(  525,   -93,     0);	/* vert[70] */
	SVECTOR(  538,  -117,     0);	/* vert[71] */
	SVECTOR(  551,  -139,     0);	/* vert[72] */
	SVECTOR(  557,  -146,     0);	/* vert[73] */
	SVECTOR(  562,  -150,     0);	/* vert[74] */
	SVECTOR(  570,  -153,     0);	/* vert[75] */
	SVECTOR(  574,  -152,     0);	/* vert[76] */
	SVECTOR(  578,  -149,     0);	/* vert[77] */
	SVECTOR(  582,  -141,     0);	/* vert[78] */
	SVECTOR(  582,  -135,     0);	/* vert[79] */
	SVECTOR(  581,  -122,     0);	/* vert[80] */
	SVECTOR(  563,   -25,     0);	/* vert[81] */
	SVECTOR(  560,   -18,     0);	/* vert[82] */
	SVECTOR(  555,    -8,     0);	/* vert[83] */
	SVECTOR(  551,    -3,     0);	/* vert[84] */
	SVECTOR(  547,     1,     0);	/* vert[85] */
	SVECTOR(  539,     3,     0);	/* vert[86] */
	SVECTOR(  521,     4,     0);	/* vert[87] */
	SVECTOR(  482,     3,     0);	/* vert[88] */
	SVECTOR(  416,     0,     0);	/* vert[89] */
	SVECTOR(  350,    -1,     0);	/* vert[90] */
	SVECTOR(  282,    -2,     0);	/* vert[91] */
	SVECTOR(  259,    -1,     0);	/* vert[92] */
	SVECTOR(  152,     1,     0);	/* vert[93] */
	SVECTOR(   68,     3,     0);	/* vert[94] */
	SVECTOR(   34,     4,     0);	/* vert[95] */
	SVECTOR(   16,     4,     0);	/* vert[96] */
	SVECTOR(    8,     2,     0);	/* vert[97] */
	SVECTOR(    2,     0,     0);	/* vert[98] */
	SVECTOR(   -1,    -3,     0);	/* vert[99] */
	SVECTOR(   -5,    -9,     0);	/* vert[100] */
	SVECTOR(   -6,   -16,     0);	/* vert[101] */
	SVECTOR(   -4,   -23,     0);	/* vert[102] */
	SVECTOR(    6,   -37,     0);	/* vert[103] */
	SVECTOR(   22,   -55,     0);	/* vert[104] */
	SVECTOR(   50,   -87,     0);	/* vert[105] */
	SVECTOR(   64,  -103,     0);	/* vert[106] */
	SVECTOR(   74,  -116,     0);	/* vert[107] */
	SVECTOR(   74,  -116,  -918);	/* vert[108] */
	SVECTOR(  441,  -581,  -918);	/* vert[109] */
	SVECTOR(  448,  -590,  -918);	/* vert[110] */
	SVECTOR(  455,  -600,  -918);	/* vert[111] */
	SVECTOR(  463,  -612,  -918);	/* vert[112] */
	SVECTOR(  464,  -618,  -918);	/* vert[113] */
	SVECTOR(  462,  -625,  -918);	/* vert[114] */
	SVECTOR(  457,  -630,  -918);	/* vert[115] */
	SVECTOR(  451,  -632,  -918);	/* vert[116] */
	SVECTOR(  436,  -633,  -918);	/* vert[117] */
	SVECTOR(  425,  -632,  -918);	/* vert[118] */
	SVECTOR(  366,  -635,  -918);	/* vert[119] */
	SVECTOR(  330,  -636,  -918);	/* vert[120] */
	SVECTOR(  280,  -634,  -918);	/* vert[121] */
	SVECTOR(  238,  -629,  -918);	/* vert[122] */
	SVECTOR(  215,  -622,  -918);	/* vert[123] */
	SVECTOR(  191,  -610,  -918);	/* vert[124] */
	SVECTOR(  170,  -594,  -918);	/* vert[125] */
	SVECTOR(  153,  -574,  -918);	/* vert[126] */
	SVECTOR(  143,  -552,  -918);	/* vert[127] */
	SVECTOR(  136,  -538,  -918);	/* vert[128] */
	SVECTOR(  133,  -534,  -918);	/* vert[129] */
	SVECTOR(  129,  -531,  -918);	/* vert[130] */
	SVECTOR(  125,  -529,  -918);	/* vert[131] */
	SVECTOR(  119,  -529,  -918);	/* vert[132] */
	SVECTOR(  114,  -531,  -918);	/* vert[133] */
	SVECTOR(  107,  -536,  -918);	/* vert[134] */
	SVECTOR(  104,  -544,  -918);	/* vert[135] */
	SVECTOR(  104,  -548,  -918);	/* vert[136] */
	SVECTOR(  107,  -562,  -918);	/* vert[137] */
	SVECTOR(  130,  -661,  -918);	/* vert[138] */
	SVECTOR(  132,  -665,  -918);	/* vert[139] */
	SVECTOR(  138,  -671,  -918);	/* vert[140] */
	SVECTOR(  145,  -674,  -918);	/* vert[141] */
	SVECTOR(  157,  -676,  -918);	/* vert[142] */
	SVECTOR(  164,  -676,  -918);	/* vert[143] */
	SVECTOR(  187,  -674,  -918);	/* vert[144] */
	SVECTOR(  201,  -673,  -918);	/* vert[145] */
	SVECTOR(  228,  -672,  -918);	/* vert[146] */
	SVECTOR(  307,  -670,  -918);	/* vert[147] */
	SVECTOR(  355,  -669,  -918);	/* vert[148] */
	SVECTOR(  395,  -669,  -918);	/* vert[149] */
	SVECTOR(  507,  -670,  -918);	/* vert[150] */
	SVECTOR(  575,  -671,  -918);	/* vert[151] */
	SVECTOR(  604,  -672,  -918);	/* vert[152] */
	SVECTOR(  635,  -672,  -918);	/* vert[153] */
	SVECTOR(  650,  -670,  -918);	/* vert[154] */
	SVECTOR(  657,  -668,  -918);	/* vert[155] */
	SVECTOR(  659,  -666,  -918);	/* vert[156] */
	SVECTOR(  660,  -663,  -918);	/* vert[157] */
	SVECTOR(  659,  -657,  -918);	/* vert[158] */
	SVECTOR(  658,  -651,  -918);	/* vert[159] */
	SVECTOR(  657,  -648,  -918);	/* vert[160] */
	SVECTOR(  650,  -638,  -918);	/* vert[161] */
	SVECTOR(  645,  -634,  -918);	/* vert[162] */
	SVECTOR(  216,   -94,  -918);	/* vert[163] */
	SVECTOR(  208,   -84,  -918);	/* vert[164] */
	SVECTOR(  202,   -75,  -918);	/* vert[165] */
	SVECTOR(  195,   -65,  -918);	/* vert[166] */
	SVECTOR(  193,   -60,  -918);	/* vert[167] */
	SVECTOR(  192,   -52,  -918);	/* vert[168] */
	SVECTOR(  194,   -47,  -918);	/* vert[169] */
	SVECTOR(  198,   -43,  -918);	/* vert[170] */
	SVECTOR(  209,   -39,  -918);	/* vert[171] */
	SVECTOR(  236,   -35,  -918);	/* vert[172] */
	SVECTOR(  295,   -36,  -918);	/* vert[173] */
	SVECTOR(  338,   -38,  -918);	/* vert[174] */
	SVECTOR(  410,   -42,  -918);	/* vert[175] */
	SVECTOR(  459,   -50,  -918);	/* vert[176] */
	SVECTOR(  488,   -61,  -918);	/* vert[177] */
	SVECTOR(  509,   -75,  -918);	/* vert[178] */
	SVECTOR(  525,   -93,  -918);	/* vert[179] */
	SVECTOR(  538,  -117,  -918);	/* vert[180] */
	SVECTOR(  551,  -139,  -918);	/* vert[181] */
	SVECTOR(  557,  -146,  -918);	/* vert[182] */
	SVECTOR(  562,  -150,  -918);	/* vert[183] */
	SVECTOR(  570,  -153,  -918);	/* vert[184] */
	SVECTOR(  574,  -152,  -918);	/* vert[185] */
	SVECTOR(  578,  -149,  -918);	/* vert[186] */
	SVECTOR(  582,  -141,  -918);	/* vert[187] */
	SVECTOR(  582,  -135,  -918);	/* vert[188] */
	SVECTOR(  581,  -122,  -918);	/* vert[189] */
	SVECTOR(  563,   -25,  -918);	/* vert[190] */
	SVECTOR(  560,   -18,  -918);	/* vert[191] */
	SVECTOR(  555,    -8,  -918);	/* vert[192] */
	SVECTOR(  551,    -3,  -918);	/* vert[193] */
	SVECTOR(  547,     1,  -918);	/* vert[194] */
	SVECTOR(  539,     3,  -918);	/* vert[195] */
	SVECTOR(  521,     4,  -918);	/* vert[196] */
	SVECTOR(  482,     3,  -918);	/* vert[197] */
	SVECTOR(  416,     0,  -918);	/* vert[198] */
	SVECTOR(  350,    -1,  -918);	/* vert[199] */
	SVECTOR(  282,    -2,  -918);	/* vert[200] */
	SVECTOR(  259,    -1,  -918);	/* vert[201] */
	SVECTOR(  152,     1,  -918);	/* vert[202] */
	SVECTOR(   68,     3,  -918);	/* vert[203] */
	SVECTOR(   34,     4,  -918);	/* vert[204] */
	SVECTOR(   16,     4,  -918);	/* vert[205] */
	SVECTOR(    8,     2,  -918);	/* vert[206] */
	SVECTOR(    2,     0,  -918);	/* vert[207] */
	SVECTOR(   -1,    -3,  -918);	/* vert[208] */
	SVECTOR(   -5,    -9,  -918);	/* vert[209] */
	SVECTOR(   -6,   -16,  -918);	/* vert[210] */
	SVECTOR(   -4,   -23,  -918);	/* vert[211] */
	SVECTOR(    6,   -37,  -918);	/* vert[212] */
	SVECTOR(   22,   -55,  -918);	/* vert[213] */
	SVECTOR(   50,   -87,  -918);	/* vert[214] */
	SVECTOR(   64,  -103,  -918);	/* vert[215] */

BezierNorm_0000:	/* 0x000018cc */
	SVECTOR(-2279, -1778,  2901);	/* norm[0] */
	SVECTOR(-2300, -1749,  2902);	/* norm[1] */
	SVECTOR(-2355, -1615,  2935);	/* norm[2] */
	SVECTOR(-2519, -1038,  3058);	/* norm[3] */
	SVECTOR(-2602,   113,  3160);	/* norm[4] */
	SVECTOR(-2283,  1358,  3117);	/* norm[5] */
	SVECTOR(-1450,  2219,  3121);	/* norm[6] */
	SVECTOR( -496,  2681,  3056);	/* norm[7] */
	SVECTOR(    8,  2877,  2915);	/* norm[8] */
	SVECTOR(  -35,  2927,  2864);	/* norm[9] */
	SVECTOR(  -98,  2882,  2908);	/* norm[10] */
	SVECTOR(   34,  2864,  2927);	/* norm[11] */
	SVECTOR(  239,  2850,  2932);	/* norm[12] */
	SVECTOR(  538,  2777,  2961);	/* norm[13] */
	SVECTOR(  976,  2627,  2986);	/* norm[14] */
	SVECTOR( 1484,  2359,  3001);	/* norm[15] */
	SVECTOR( 1959,  1989,  2997);	/* norm[16] */
	SVECTOR( 2338,  1509,  3005);	/* norm[17] */
	SVECTOR( 2608,  1192,  2924);	/* norm[18] */
	SVECTOR( 2602,  1493,  2787);	/* norm[19] */
	SVECTOR( 2099,  2187,  2754);	/* norm[20] */
	SVECTOR( 1364,  2680,  2780);	/* norm[21] */
	SVECTOR(  574,  2965,  2766);	/* norm[22] */
	SVECTOR( -464,  3020,  2726);	/* norm[23] */
	SVECTOR(-1550,  2624,  2735);	/* norm[24] */
	SVECTOR(-2497,  1799,  2702);	/* norm[25] */
	SVECTOR(-2997,   603,  2725);	/* norm[26] */
	SVECTOR(-4078,  -374,     0);	/* norm[27] */
	SVECTOR(-2855,  -599,  2874);	/* norm[28] */
	SVECTOR(-2812, -1065,  2779);	/* norm[29] */
	SVECTOR(-2383, -1889,  2743);	/* norm[30] */
	SVECTOR(-1667, -2509,  2774);	/* norm[31] */
	SVECTOR( -888, -2889,  2764);	/* norm[32] */
	SVECTOR( -218, -2953,  2829);	/* norm[33] */
	SVECTOR(  177, -4092,     0);	/* norm[34] */
	SVECTOR(  231, -2881,  2902);	/* norm[35] */
	SVECTOR(  159, -2874,  2913);	/* norm[36] */
	SVECTOR(   86, -2888,  2902);	/* norm[37] */
	SVECTOR(   48, -2890,  2901);	/* norm[38] */
	SVECTOR(   15, -2891,  2901);	/* norm[39] */
	SVECTOR(   -6, -2894,  2898);	/* norm[40] */
	SVECTOR(  -28, -2891,  2901);	/* norm[41] */
	SVECTOR(  -71, -2886,  2905);	/* norm[42] */
	SVECTOR(  -62, -2907,  2884);	/* norm[43] */
	SVECTOR(  177, -2951,  2834);	/* norm[44] */
	SVECTOR(  913, -3992,     0);	/* norm[45] */
	SVECTOR( 1619, -2621,  2698);	/* norm[46] */
	SVECTOR( 2654, -1585,  2686);	/* norm[47] */
	SVECTOR( 3035,  -311,  2732);	/* norm[48] */
	SVECTOR( 2919,   442,  2838);	/* norm[49] */
	SVECTOR( 2839,   849,  2827);	/* norm[50] */
	SVECTOR( 2638,  1452,  2776);	/* norm[51] */
	SVECTOR( 2240,  1935,  2830);	/* norm[52] */
	SVECTOR( 2120,  1892,  2949);	/* norm[53] */
	SVECTOR( 2284,  1756,  2911);	/* norm[54] */
	SVECTOR( 2329,  1713,  2901);	/* norm[55] */
	SVECTOR( 2358,  1652,  2912);	/* norm[56] */
	SVECTOR( 2461,  1342,  2985);	/* norm[57] */
	SVECTOR( 2654,   684,  3043);	/* norm[58] */
	SVECTOR( 2658,  -296,  3102);	/* norm[59] */
	SVECTOR( 2211, -1419,  3141);	/* norm[60] */
	SVECTOR( 1455, -2282,  3074);	/* norm[61] */
	SVECTOR(  720, -2679,  3012);	/* norm[62] */
	SVECTOR(  180, -2816,  2968);	/* norm[63] */
	SVECTOR(  -86, -2878,  2912);	/* norm[64] */
	SVECTOR( -150, -2888,  2900);	/* norm[65] */
	SVECTOR( -312, -2828,  2946);	/* norm[66] */
	SVECTOR( -736, -2696,  2994);	/* norm[67] */
	SVECTOR(-1292, -2465,  3004);	/* norm[68] */
	SVECTOR(-1824, -2091,  3012);	/* norm[69] */
	SVECTOR(-2261, -1637,  2996);	/* norm[70] */
	SVECTOR(-2516, -1417,  2904);	/* norm[71] */
	SVECTOR(-3385, -2305,     0);	/* norm[72] */
	SVECTOR(-2068, -2169,  2791);	/* norm[73] */
	SVECTOR(-1406, -2684,  2755);	/* norm[74] */
	SVECTOR( -332, -3050,  2713);	/* norm[75] */
	SVECTOR( 1245, -2869,  2643);	/* norm[76] */
	SVECTOR( 2480, -1819,  2704);	/* norm[77] */
	SVECTOR( 2945,  -729,  2751);	/* norm[78] */
	SVECTOR( 4095,    51,     0);	/* norm[79] */
	SVECTOR( 2903,   417,  2858);	/* norm[80] */
	SVECTOR( 2862,   707,  2843);	/* norm[81] */
	SVECTOR( 2747,  1155,  2809);	/* norm[82] */
	SVECTOR( 2476,  1651,  2813);	/* norm[83] */
	SVECTOR( 2113,  2109,  2803);	/* norm[84] */
	SVECTOR( 1499,  2637,  2751);	/* norm[85] */
	SVECTOR(  612,  2957,  2767);	/* norm[86] */
	SVECTOR(   27,  2939,  2852);	/* norm[87] */
	SVECTOR( -158,  4092,     0);	/* norm[88] */
	SVECTOR(  -89,  2887,  2903);	/* norm[89] */
	SVECTOR(  -38,  2887,  2905);	/* norm[90] */
	SVECTOR(   10,  2889,  2903);	/* norm[91] */
	SVECTOR(   40,  2893,  2898);	/* norm[92] */
	SVECTOR(   59,  2892,  2899);	/* norm[93] */
	SVECTOR(   98,  2885,  2905);	/* norm[94] */
	SVECTOR(   63,  2915,  2876);	/* norm[95] */
	SVECTOR( -321,  2971,  2800);	/* norm[96] */
	SVECTOR( -970,  2833,  2794);	/* norm[97] */
	SVECTOR(-1639,  2522,  2779);	/* norm[98] */
	SVECTOR(-2323,  1949,  2752);	/* norm[99] */
	SVECTOR(-2879,  1019,  2729);	/* norm[100] */
	SVECTOR(-3059,  -186,  2717);	/* norm[101] */
	SVECTOR(-2747, -1297,  2746);	/* norm[102] */
	SVECTOR(-3202, -2553,     0);	/* norm[103] */
	SVECTOR(-2156, -1928,  2899);	/* norm[104] */
	SVECTOR(-2181, -1885,  2908);	/* norm[105] */
	SVECTOR(-2228, -1833,  2906);	/* norm[106] */
	SVECTOR(-2262, -1801,  2900);	/* norm[107] */
	SVECTOR(-2262, -1801, -2900);	/* norm[108] */
	SVECTOR(-2279, -1778, -2901);	/* norm[109] */
	SVECTOR(-2300, -1749, -2902);	/* norm[110] */
	SVECTOR(-2355, -1615, -2935);	/* norm[111] */
	SVECTOR(-2519, -1038, -3058);	/* norm[112] */
	SVECTOR(-2602,   113, -3160);	/* norm[113] */
	SVECTOR(-2283,  1358, -3117);	/* norm[114] */
	SVECTOR(-1450,  2219, -3121);	/* norm[115] */
	SVECTOR( -496,  2681, -3056);	/* norm[116] */
	SVECTOR(    8,  2877, -2915);	/* norm[117] */
	SVECTOR(  -35,  2927, -2864);	/* norm[118] */
	SVECTOR(  -98,  2882, -2908);	/* norm[119] */
	SVECTOR(   34,  2864, -2927);	/* norm[120] */
	SVECTOR(  239,  2850, -2932);	/* norm[121] */
	SVECTOR(  538,  2777, -2961);	/* norm[122] */
	SVECTOR(  976,  2627, -2986);	/* norm[123] */
	SVECTOR( 1484,  2359, -3001);	/* norm[124] */
	SVECTOR( 1959,  1989, -2997);	/* norm[125] */
	SVECTOR( 2338,  1509, -3005);	/* norm[126] */
	SVECTOR( 2608,  1192, -2924);	/* norm[127] */
	SVECTOR( 2602,  1493, -2787);	/* norm[128] */
	SVECTOR( 2099,  2187, -2754);	/* norm[129] */
	SVECTOR( 1364,  2680, -2780);	/* norm[130] */
	SVECTOR(  574,  2965, -2766);	/* norm[131] */
	SVECTOR( -464,  3020, -2726);	/* norm[132] */
	SVECTOR(-1550,  2624, -2735);	/* norm[133] */
	SVECTOR(-2497,  1799, -2702);	/* norm[134] */
	SVECTOR(-2997,   603, -2725);	/* norm[135] */
	SVECTOR(-4078,  -374,     0);	/* norm[136] */
	SVECTOR(-2855,  -599, -2874);	/* norm[137] */
	SVECTOR(-2812, -1065, -2779);	/* norm[138] */
	SVECTOR(-2383, -1889, -2743);	/* norm[139] */
	SVECTOR(-1667, -2509, -2774);	/* norm[140] */
	SVECTOR( -888, -2889, -2764);	/* norm[141] */
	SVECTOR( -218, -2953, -2829);	/* norm[142] */
	SVECTOR(  177, -4092,     0);	/* norm[143] */
	SVECTOR(  231, -2881, -2902);	/* norm[144] */
	SVECTOR(  159, -2874, -2913);	/* norm[145] */
	SVECTOR(   86, -2888, -2902);	/* norm[146] */
	SVECTOR(   48, -2890, -2901);	/* norm[147] */
	SVECTOR(   15, -2891, -2901);	/* norm[148] */
	SVECTOR(   -6, -2894, -2898);	/* norm[149] */
	SVECTOR(  -28, -2891, -2901);	/* norm[150] */
	SVECTOR(  -71, -2886, -2905);	/* norm[151] */
	SVECTOR(  -62, -2907, -2884);	/* norm[152] */
	SVECTOR(  177, -2951, -2834);	/* norm[153] */
	SVECTOR(  913, -3992,     0);	/* norm[154] */
	SVECTOR( 1619, -2621, -2698);	/* norm[155] */
	SVECTOR( 2654, -1585, -2686);	/* norm[156] */
	SVECTOR( 3035,  -311, -2732);	/* norm[157] */
	SVECTOR( 2919,   442, -2838);	/* norm[158] */
	SVECTOR( 2839,   849, -2827);	/* norm[159] */
	SVECTOR( 2638,  1452, -2776);	/* norm[160] */
	SVECTOR( 2240,  1935, -2830);	/* norm[161] */
	SVECTOR( 2120,  1892, -2949);	/* norm[162] */
	SVECTOR( 2284,  1756, -2911);	/* norm[163] */
	SVECTOR( 2329,  1712, -2901);	/* norm[164] */
	SVECTOR( 2358,  1652, -2912);	/* norm[165] */
	SVECTOR( 2461,  1342, -2985);	/* norm[166] */
	SVECTOR( 2654,   684, -3043);	/* norm[167] */
	SVECTOR( 2658,  -296, -3102);	/* norm[168] */
	SVECTOR( 2211, -1419, -3141);	/* norm[169] */
	SVECTOR( 1455, -2282, -3074);	/* norm[170] */
	SVECTOR(  720, -2679, -3012);	/* norm[171] */
	SVECTOR(  180, -2816, -2968);	/* norm[172] */
	SVECTOR(  -86, -2878, -2912);	/* norm[173] */
	SVECTOR( -150, -2888, -2900);	/* norm[174] */
	SVECTOR( -312, -2828, -2946);	/* norm[175] */
	SVECTOR( -736, -2696, -2994);	/* norm[176] */
	SVECTOR(-1292, -2465, -3004);	/* norm[177] */
	SVECTOR(-1824, -2091, -3012);	/* norm[178] */
	SVECTOR(-2261, -1637, -2996);	/* norm[179] */
	SVECTOR(-2516, -1417, -2904);	/* norm[180] */
	SVECTOR(-3385, -2305,     0);	/* norm[181] */
	SVECTOR(-2068, -2169, -2791);	/* norm[182] */
	SVECTOR(-1406, -2684, -2755);	/* norm[183] */
	SVECTOR( -332, -3050, -2713);	/* norm[184] */
	SVECTOR( 1245, -2869, -2643);	/* norm[185] */
	SVECTOR( 2480, -1819, -2704);	/* norm[186] */
	SVECTOR( 2945,  -729, -2751);	/* norm[187] */
	SVECTOR( 4095,    51,     0);	/* norm[188] */
	SVECTOR( 2903,   417, -2858);	/* norm[189] */
	SVECTOR( 2862,   707, -2843);	/* norm[190] */
	SVECTOR( 2747,  1155, -2809);	/* norm[191] */
	SVECTOR( 2476,  1651, -2813);	/* norm[192] */
	SVECTOR( 2113,  2109, -2803);	/* norm[193] */
	SVECTOR( 1499,  2637, -2751);	/* norm[194] */
	SVECTOR(  612,  2957, -2767);	/* norm[195] */
	SVECTOR(   27,  2939, -2852);	/* norm[196] */
	SVECTOR( -158,  4092,     0);	/* norm[197] */
	SVECTOR(  -89,  2887, -2903);	/* norm[198] */
	SVECTOR(  -38,  2887, -2905);	/* norm[199] */
	SVECTOR(   10,  2889, -2903);	/* norm[200] */
	SVECTOR(   40,  2893, -2898);	/* norm[201] */
	SVECTOR(   59,  2892, -2899);	/* norm[202] */
	SVECTOR(   98,  2885, -2905);	/* norm[203] */
	SVECTOR(   63,  2915, -2876);	/* norm[204] */
	SVECTOR( -321,  2971, -2800);	/* norm[205] */
	SVECTOR( -970,  2833, -2794);	/* norm[206] */
	SVECTOR(-1639,  2522, -2779);	/* norm[207] */
	SVECTOR(-2323,  1949, -2752);	/* norm[208] */
	SVECTOR(-2879,  1019, -2729);	/* norm[209] */
	SVECTOR(-3059,  -186, -2717);	/* norm[210] */
	SVECTOR(-2747, -1297, -2746);	/* norm[211] */
	SVECTOR(-3202, -2553,     0);	/* norm[212] */
	SVECTOR(-2156, -1928, -2899);	/* norm[213] */
	SVECTOR(-2181, -1885, -2908);	/* norm[214] */
	SVECTOR(-2228, -1833, -2906);	/* norm[215] */
	SVECTOR(    0,     0, -4096);	/* norm[216] */
	SVECTOR(    0,     0,  4096);	/* norm[217] */
	SVECTOR(-3216, -2536,     0);	/* norm[218] */
	SVECTOR(-3243, -2501,     0);	/* norm[219] */
	SVECTOR(-3276, -2457,     0);	/* norm[220] */
	SVECTOR(-3473, -2170,     0);	/* norm[221] */
	SVECTOR(-3994,  -907,     0);	/* norm[222] */
	SVECTOR(-3899,  1253,     0);	/* norm[223] */
	SVECTOR(-2962,  2828,     0);	/* norm[224] */
	SVECTOR(-1399,  3849,     0);	/* norm[225] */
	SVECTOR(  -70,  4095,     0);	/* norm[226] */
	SVECTOR(   95,  4094,     0);	/* norm[227] */
	SVECTOR( -193,  4091,     0);	/* norm[228] */
	SVECTOR(  -87,  4095,     0);	/* norm[229] */
	SVECTOR(  185,  4091,     0);	/* norm[230] */
	SVECTOR(  498,  4065,     0);	/* norm[231] */
	SVECTOR( 1055,  3957,     0);	/* norm[232] */
	SVECTOR( 1786,  3686,     0);	/* norm[233] */
	SVECTOR( 2549,  3205,     0);	/* norm[234] */
	SVECTOR( 3166,  2598,     0);	/* norm[235] */
	SVECTOR( 3672,  1814,     0);	/* norm[236] */
	SVECTOR( 3775,  1589,     0);	/* norm[237] */
	SVECTOR( 3276,  2457,     0);	/* norm[238] */
	SVECTOR( 2320,  3375,     0);	/* norm[239] */
	SVECTOR( 1363,  3862,     0);	/* norm[240] */
	SVECTOR(  177,  4092,     0);	/* norm[241] */
	SVECTOR(-1399,  3849,     0);	/* norm[242] */
	SVECTOR(-2697,  3082,     0);	/* norm[243] */
	SVECTOR(-3777,  1584,     0);	/* norm[244] */
	SVECTOR(-4096,     0,     0);	/* norm[245] */
	SVECTOR(-4027,  -745,     0);	/* norm[246] */
	SVECTOR(-3987,  -935,     0);	/* norm[247] */
	SVECTOR(-3606, -1941,     0);	/* norm[248] */
	SVECTOR(-2713, -3067,     0);	/* norm[249] */
	SVECTOR(-1776, -3690,     0);	/* norm[250] */
	SVECTOR( -603, -4051,     0);	/* norm[251] */
	SVECTOR(    0, -4096,     0);	/* norm[252] */
	SVECTOR(  354, -4080,     0);	/* norm[253] */
	SVECTOR(  302, -4084,     0);	/* norm[254] */
	SVECTOR(  151, -4093,     0);	/* norm[255] */
	SVECTOR(   92, -4094,     0);	/* norm[256] */
	SVECTOR(   44, -4095,     0);	/* norm[257] */
	SVECTOR(    0, -4096,     0);	/* norm[258] */
	SVECTOR(  -18, -4095,     0);	/* norm[259] */
	SVECTOR(  -62, -4095,     0);	/* norm[260] */
	SVECTOR( -141, -4093,     0);	/* norm[261] */
	SVECTOR(  -34, -4095,     0);	/* norm[262] */
	SVECTOR(  524, -4062,     0);	/* norm[263] */
	SVECTOR( 1295, -3885,     0);	/* norm[264] */
	SVECTOR( 2896, -2896,     0);	/* norm[265] */
	SVECTOR( 3923, -1177,     0);	/* norm[266] */
	SVECTOR( 4080,   354,     0);	/* norm[267] */
	SVECTOR( 4002,   870,     0);	/* norm[268] */
	SVECTOR( 3822,  1470,     0);	/* norm[269] */
	SVECTOR( 3286,  2444,     0);	/* norm[270] */
	SVECTOR( 2896,  2896,     0);	/* norm[271] */
	SVECTOR( 3206,  2548,     0);	/* norm[272] */
	SVECTOR( 3286,  2444,     0);	/* norm[273] */
	SVECTOR( 3312,  2409,     0);	/* norm[274] */
	SVECTOR( 3394,  2291,     0);	/* norm[275] */
	SVECTOR( 3764,  1613,     0);	/* norm[276] */
	SVECTOR( 4075,   407,     0);	/* norm[277] */
	SVECTOR( 3885, -1295,     0);	/* norm[278] */
	SVECTOR( 2794, -2994,     0);	/* norm[279] */
	SVECTOR( 1536, -3796,     0);	/* norm[280] */
	SVECTOR(  573, -4055,     0);	/* norm[281] */
	SVECTOR(  -52, -4095,     0);	/* norm[282] */
	SVECTOR( -194, -4091,     0);	/* norm[283] */
	SVECTOR( -232, -4089,     0);	/* norm[284] */
	SVECTOR( -666, -4041,     0);	/* norm[285] */
	SVECTOR(-1479, -3819,     0);	/* norm[286] */
	SVECTOR(-2299, -3389,     0);	/* norm[287] */
	SVECTOR(-3046, -2737,     0);	/* norm[288] */
	SVECTOR(-3551, -2040,     0);	/* norm[289] */
	SVECTOR(-3586, -1978,     0);	/* norm[290] */
	SVECTOR(-3155, -2611,     0);	/* norm[291] */
	SVECTOR(-2457, -3276,     0);	/* norm[292] */
	SVECTOR(-1295, -3885,     0);	/* norm[293] */
	SVECTOR(  428, -4073,     0);	/* norm[294] */
	SVECTOR( 2682, -3095,     0);	/* norm[295] */
	SVECTOR( 3758, -1628,     0);	/* norm[296] */
	SVECTOR( 4083,  -314,     0);	/* norm[297] */
	SVECTOR( 4074,   415,     0);	/* norm[298] */
	SVECTOR( 4027,   748,     0);	/* norm[299] */
	SVECTOR( 3911,  1214,     0);	/* norm[300] */
	SVECTOR( 3603,  1947,     0);	/* norm[301] */
	SVECTOR( 3183,  2577,     0);	/* norm[302] */
	SVECTOR( 2582,  3178,     0);	/* norm[303] */
	SVECTOR( 1410,  3845,     0);	/* norm[304] */
	SVECTOR(  233,  4089,     0);	/* norm[305] */
	SVECTOR( -158,  4092,     0);	/* norm[306] */
	SVECTOR( -159,  4092,     0);	/* norm[307] */
	SVECTOR(  -93,  4094,     0);	/* norm[308] */
	SVECTOR(  -15,  4095,     0);	/* norm[309] */
	SVECTOR(   46,  4095,     0);	/* norm[310] */
	SVECTOR(   68,  4095,     0);	/* norm[311] */
	SVECTOR(   99,  4094,     0);	/* norm[312] */
	SVECTOR(  179,  4092,     0);	/* norm[313] */
	SVECTOR(    0,  4096,     0);	/* norm[314] */
	SVECTOR( -875,  4001,     0);	/* norm[315] */
	SVECTOR(-1761,  3698,     0);	/* norm[316] */
	SVECTOR(-2665,  3109,     0);	/* norm[317] */
	SVECTOR(-3526,  2083,     0);	/* norm[318] */
	SVECTOR(-4051,   600,     0);	/* norm[319] */
	SVECTOR(-3948, -1089,     0);	/* norm[320] */
	SVECTOR(-3349, -2357,     0);	/* norm[321] */
	SVECTOR(-3044, -2740,     0);	/* norm[322] */
	SVECTOR(-3062, -2719,     0);	/* norm[323] */
	SVECTOR(-3134, -2636,     0);	/* norm[324] */
	SVECTOR(-3192, -2566,     0);	/* norm[325] */
	SVECTOR(    0,     0,  4096);	/* norm[326] */
	SVECTOR(    0,     0,  4096);	/* norm[327] */
	SVECTOR(    0,     0,  4096);	/* norm[328] */
	SVECTOR(    0,     0,  4096);	/* norm[329] */
	SVECTOR(    0,     0,  4096);	/* norm[330] */
	SVECTOR(    0,     0,  4096);	/* norm[331] */
	SVECTOR(    0,     0,  4096);	/* norm[332] */
	SVECTOR(    0,     0,  4096);	/* norm[333] */
	SVECTOR(    0,     0,  4096);	/* norm[334] */
	SVECTOR(    0,     0,  4096);	/* norm[335] */
	SVECTOR(    0,     0,  4096);	/* norm[336] */
	SVECTOR(    0,     0,  4096);	/* norm[337] */
	SVECTOR(    0,     0,  4096);	/* norm[338] */
	SVECTOR(    0,     0,  4096);	/* norm[339] */
	SVECTOR(    0,     0,  4096);	/* norm[340] */
	SVECTOR(    0,     0,  4096);	/* norm[341] */
	SVECTOR(    0,     0,  4096);	/* norm[342] */
	SVECTOR(    0,     0,  4096);	/* norm[343] */
	SVECTOR(    0,     0,  4096);	/* norm[344] */
	SVECTOR(    0,     0,  4096);	/* norm[345] */
	SVECTOR(    0,     0,  4096);	/* norm[346] */
	SVECTOR(    0,     0,  4096);	/* norm[347] */
	SVECTOR(    0,     0,  4096);	/* norm[348] */
	SVECTOR(    0,     0,  4096);	/* norm[349] */
	SVECTOR(    0,     0,  4096);	/* norm[350] */
	SVECTOR(    0,     0,  4096);	/* norm[351] */
	SVECTOR(    0,     0,  4096);	/* norm[352] */
	SVECTOR(    0,     0,  4096);	/* norm[353] */
	SVECTOR(    0,     0,  4096);	/* norm[354] */
	SVECTOR(    0,     0,  4096);	/* norm[355] */
	SVECTOR(    0,     0,  4096);	/* norm[356] */
	SVECTOR(    0,     0,  4096);	/* norm[357] */
	SVECTOR(    0,     0,  4096);	/* norm[358] */
	SVECTOR(    0,     0,  4096);	/* norm[359] */
	SVECTOR(    0,     0,  4096);	/* norm[360] */
	SVECTOR(    0,     0,     0);	/* norm[361] */
	SVECTOR(    0,     0,     0);	/* norm[362] */
	SVECTOR(    0,     0,     0);	/* norm[363] */
	SVECTOR(    0,     0,     0);	/* norm[364] */
	SVECTOR(    0,     0,     0);	/* norm[365] */
	SVECTOR(    0,     0,     0);	/* norm[366] */
	SVECTOR(    0,     0,     0);	/* norm[367] */
	SVECTOR(    0,     0,     0);	/* norm[368] */
	SVECTOR(    0,     0,     0);	/* norm[369] */
	SVECTOR(    0,     0,     0);	/* norm[370] */
	SVECTOR(    0,     0,     0);	/* norm[371] */
	SVECTOR(    0,     0,     0);	/* norm[372] */
	SVECTOR(    0,     0,     0);	/* norm[373] */
	SVECTOR(    0,     0,     0);	/* norm[374] */
	SVECTOR(    0,     0,     0);	/* norm[375] */
	SVECTOR(    0,     0,     0);	/* norm[376] */
	SVECTOR(    0,     0,     0);	/* norm[377] */
	SVECTOR(    0,     0,     0);	/* norm[378] */
	SVECTOR(    0,     0,     0);	/* norm[379] */
	SVECTOR(    0,     0,     0);	/* norm[380] */
	SVECTOR(    0,     0,     0);	/* norm[381] */
	SVECTOR(    0,     0,     0);	/* norm[382] */
	SVECTOR(    0,     0,     0);	/* norm[383] */
	SVECTOR(    0,     0,     0);	/* norm[384] */
	SVECTOR(    0,     0, -4096);	/* norm[385] */
	SVECTOR(    0,     0, -4096);	/* norm[386] */
	SVECTOR(    0,     0, -4096);	/* norm[387] */
	SVECTOR(    0,     0, -4096);	/* norm[388] */
	SVECTOR(    0,     0, -4096);	/* norm[389] */
	SVECTOR(    0,     0, -4096);	/* norm[390] */
	SVECTOR(    0,     0, -4096);	/* norm[391] */
	SVECTOR(    0,     0, -4096);	/* norm[392] */
	SVECTOR(    0,     0, -4096);	/* norm[393] */
	SVECTOR(    0,     0, -4096);	/* norm[394] */
	SVECTOR(    0,     0, -4096);	/* norm[395] */
	SVECTOR(    0,     0, -4096);	/* norm[396] */
	SVECTOR(    0,     0, -4096);	/* norm[397] */
	SVECTOR(    0,     0, -4096);	/* norm[398] */
	SVECTOR(    0,     0, -4096);	/* norm[399] */
	SVECTOR(    0,     0, -4096);	/* norm[400] */
	SVECTOR(    0,     0, -4096);	/* norm[401] */
	SVECTOR(    0,     0, -4096);	/* norm[402] */
	SVECTOR(    0,     0, -4096);	/* norm[403] */
	SVECTOR(    0,     0, -4096);	/* norm[404] */
	SVECTOR(    0,     0, -4096);	/* norm[405] */
	SVECTOR(    0,     0, -4096);	/* norm[406] */
	SVECTOR(    0,     0, -4096);	/* norm[407] */
	SVECTOR(    0,     0, -4096);	/* norm[408] */
	SVECTOR(    0,     0, -4096);	/* norm[409] */
	SVECTOR(    0,     0, -4096);	/* norm[410] */
	SVECTOR(    0,     0, -4096);	/* norm[411] */
	SVECTOR(    0,     0, -4096);	/* norm[412] */
	SVECTOR(    0,     0, -4096);	/* norm[413] */
	SVECTOR(    0,     0, -4096);	/* norm[414] */
	SVECTOR(    0,     0, -4096);	/* norm[415] */
	SVECTOR(    0,     0, -4096);	/* norm[416] */
	SVECTOR(    0,     0, -4096);	/* norm[417] */
	SVECTOR(    0,     0, -4096);	/* norm[418] */
	SVECTOR(    0,     0, -4096);	/* norm[419] */
	SVECTOR(    0,     0,     0);	/* norm[420] */
	SVECTOR(    0,     0,     0);	/* norm[421] */
	SVECTOR(    0,     0,     0);	/* norm[422] */
	SVECTOR(    0,     0,     0);	/* norm[423] */
	SVECTOR(    0,     0,     0);	/* norm[424] */
	SVECTOR(    0,     0,     0);	/* norm[425] */
	SVECTOR(    0,     0,     0);	/* norm[426] */
	SVECTOR(    0,     0,     0);	/* norm[427] */
	SVECTOR(    0,     0,     0);	/* norm[428] */
	SVECTOR(    0,     0,     0);	/* norm[429] */
	SVECTOR(    0,     0,     0);	/* norm[430] */
	SVECTOR(    0,     0,     0);	/* norm[431] */
	SVECTOR(    0,     0,     0);	/* norm[432] */
	SVECTOR(    0,     0,     0);	/* norm[433] */
	SVECTOR(    0,     0,     0);	/* norm[434] */
	SVECTOR(    0,     0,     0);	/* norm[435] */
	SVECTOR(    0,     0,     0);	/* norm[436] */
	SVECTOR(    0,     0,     0);	/* norm[437] */
	SVECTOR(    0,     0,     0);	/* norm[438] */
	SVECTOR(    0,     0,     0);	/* norm[439] */
	SVECTOR(    0,     0,     0);	/* norm[440] */
	SVECTOR(    0,     0,     0);	/* norm[441] */
	SVECTOR(    0,     0,     0);	/* norm[442] */
	SVECTOR(    0,     0,     0);	/* norm[443] */
