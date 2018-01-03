/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>

typedef union {
	POLY_F3		f3;
	POLY_FT3	ft3;
	POLY_G3		g3;
	POLY_GT3	gt3;
	POLY_F4		f4;
	POLY_FT4	ft4;
	POLY_G4		g4;
	POLY_GT4	gt4;
} POLY_ALL;

#if 0
/*#define MAXLINE	4096/**/
#define MAXLINE	2048/**/
LINE_F3	line[2][MAXLINE];

static void init_line(PACKET *out_packet, int max_packet)
{
	LINE_F3		*line = (LINE_F3 *)out_packet;
	int i;

	for (i = 0; i < max_packet; i++) {
		setLineF3(&line[i]);
		setRGB0(&line[i], 0, 255, 0);
	}	
}
#endif

void print_line(GsOT *Wot, PACKET *out_packet, int max_packet)
{
	POLY_ALL	*p = (POLY_ALL *)Wot->tag;
	int	max_line = max_packet / sizeof(LINE_F3);
	LINE_F3		*line = (LINE_F3 *)out_packet;
	int		num = 0;
	int		flag;

	for (; !isendprim(p); p = nextPrim(p)) {
		flag = 1;
		if (getlen(p) == 0) {
			continue;
		}
		if ((num += 2) > max_line) {
			break;
		}
		switch (getcode(p) & 0xfd) {
		case 0x20:	/* F3  */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->f3.x2, p->f3.y2, 
				p->f3.x0, p->f3.y0, p->f3.x1, p->f3.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->f3.x2, p->f3.y2, 
				p->f3.x2, p->f3.y2, p->f3.x1, p->f3.y1);
			break;
		case 0x24:	/* FT3 */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->ft3.x2, p->ft3.y2, 
				p->ft3.x0, p->ft3.y0, p->ft3.x1, p->ft3.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->ft3.x2, p->ft3.y2, 
				p->ft3.x2, p->ft3.y2, p->ft3.x1, p->ft3.y1);
			break;
		case 0x30:	/* G3  */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->g3.x2, p->g3.y2, 
				p->g3.x0, p->g3.y0, p->g3.x1, p->g3.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->g3.x2, p->g3.y2, 
				p->g3.x2, p->g3.y2, p->g3.x1, p->g3.y1);
			break;
		case 0x34:	/* GT3 */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->gt3.x2, p->gt3.y2, 
				p->gt3.x0, p->gt3.y0, p->gt3.x1, p->gt3.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->gt3.x2, p->gt3.y2, 
				p->gt3.x2, p->gt3.y2, p->gt3.x1, p->gt3.y1);
			break;
		case 0x28:	/* F4  */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->f4.x2, p->f4.y2, 
				p->f4.x0, p->f4.y0, p->f4.x1, p->f4.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->f4.x2, p->f4.y2, 
				p->f4.x3, p->f4.y3, p->f4.x1, p->f4.y1);
			break;
		case 0x2c:	/* FT4 */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->ft4.x2, p->ft4.y2, 
				p->ft4.x0, p->ft4.y0, p->ft4.x1, p->ft4.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->ft4.x2, p->ft4.y2, 
				p->ft4.x3, p->ft4.y3, p->ft4.x1, p->ft4.y1);
			break;
		case 0x38:	/* G4  */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->g4.x2, p->g4.y2, 
				p->g4.x0, p->g4.y0, p->g4.x1, p->g4.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->g4.x2, p->g4.y2, 
				p->g4.x3, p->g4.y3, p->g4.x1, p->g4.y1);
			break;
		case 0x3c:	/* GT4 */
			setLineF3(&line[num-2]);
			setRGB0(&line[num-2], 0, 255, 0);
			setXY3(&line[num-2], p->gt4.x2, p->gt4.y2, 
				p->gt4.x0, p->gt4.y0, p->gt4.x1, p->gt4.y1);
			setLineF3(&line[num-1]);
			setRGB0(&line[num-1], 0, 255, 0);
			setXY3(&line[num-1], p->gt4.x2, p->gt4.y2, 
				p->gt4.x3, p->gt4.y3, p->gt4.x1, p->gt4.y1);
			break;
		default:
			flag = 0;
			break;
		}
		if (flag) {
			AddPrim(Wot->org, &line[num-2]);
			AddPrim(Wot->org, &line[num-1]);
		}
	}
}
