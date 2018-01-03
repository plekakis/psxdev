/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *			All rights Reserved
*/

/* 
 * sjis code to access number transfer table (non-Kanji)
 */
static unsigned short kanji_0_table[19][2] = {
	{0x8140, 0}, 
	{0x8180, 63}, 
	{0x81b8, 108},
	{0x81c8, 116},
	{0x81da, 123},
	{0x81f0, 138},
	{0x81fc, 146},

	{0x824f, 147},
	{0x8260, 157},
	{0x8281, 183},
	{0x829f, 209},
	{0x8340, 292},
	{0x8380, 355},

	{0x839f, 378},
	{0x83bf, 402},
	{0x8440, 426},
	{0x8470, 459},
	{0x8480, 474},
	{0x849f, 492}
};

/* 
 * sjis code to access number transfer table (user defined characters)
 */
static unsigned short kanji_g_table[19][2] = {
	{0x8540, 0},
	{0x8546, 5},
	{0x8548, 6},
	{0x854b, 8},
	{0x8550, 9},

	{0x85a0, 21},
	{0x85b0, 34},

	{0x8640, 47},
	{0x8643, 49},
	{0x8645, 50},
	{0x8647, 51},
	{0x864a, 53},
	{0x864e, 56},
	{0x8660, 58},

	{0x869f, 74},
	{0x86b8, 98},
	{0x8740, 160},
	{0x875c, 187},
	{0x8764, 194},
};

/* 
 * sjis code to access number transfer table (First level Kanji)
 */
static unsigned short kanji_1_table[32][2] = {
	{0x889f, 0},
	{0x8940, 94},
	{0x899f, 188},
	{0x8a40, 282},
	{0x8a9f, 376},
	{0x8b40, 470},
	{0x8b9f, 564},
	{0x8c40, 658},
	{0x8c9f, 752},
	{0x8d40, 846},
	{0x8d9f, 940},
	{0x8e40, 1034},
	{0x8e9f, 1128},
	{0x8f40, 1222},
	{0x8f9f, 1316},
	{0x9040, 1410},
	{0x909f, 1504},
	{0x9140, 1598},
	{0x919f, 1692},
	{0x9240, 1786},
	{0x929f, 1880},
	{0x9340, 1974},
	{0x939f, 2068},
	{0x9440, 2162},
	{0x949f, 2256},
	{0x9540, 2350},
	{0x959f, 2444},
	{0x9640, 2538},
	{0x969f, 2632},
	{0x9740, 2726},
	{0x979f, 2820},
	{0x9840, 2914}
};

/* 
 * sjis code to access number transfer table (Second level Kanji)
 */
static unsigned short kanji_2_table[37][2] = {
	{0x989f, 0},
	{0x9940, 94},
	{0x999f, 188},
	{0x9a40, 282},
	{0x9a9f, 376},
	{0x9b40, 470},
	{0x9b9f, 564},
	{0x9c40, 658},
	{0x9c9f, 752},
	{0x9d40, 846},
	{0x9d9f, 940},
	{0x9e40, 1034},
	{0x9e9f, 1128},
	{0x9f40, 1222},
	{0x9f9f, 1316},
	{0xe040, 1410},
	{0xe09f, 1504},
	{0xe140, 1598},
	{0xe19f, 1692},
	{0xe240, 1786},
	{0xe29f, 1880},
	{0xe340, 1974},
	{0xe39f, 2068},
	{0xe440, 2162},
	{0xe49f, 2256},
	{0xe540, 2350},
	{0xe59f, 2444},
	{0xe640, 2538},
	{0xe69f, 2632},
	{0xe740, 2726},
	{0xe79f, 2820},
	{0xe840, 2914},
	{0xe89f, 3008},
	{0xe940, 3102},
	{0xe99f, 3196},
	{0xea40, 3290},
	{0xea9f, 3384}
};

/*
 * sjis code to access number transfer table (Vertical writing)
 */
static unsigned short kanji_t_table[2][2] = {
	{0xeb40, 0},
	{0xeb42, 4}
};
