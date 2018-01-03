/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	Scanning routines for all of SCEI defined primitive types.
	(Envmap primitive type is beta version for now).

	Copyright (C) 1997,1998  Sony Computer Entertainment Inc.
	All rights Reserved.

	Ver 1.00	Dec 09, 1997	By N. Yoshioka
	Ver 1.10	Feb 25, 1998	By N. Yoshioka
		- "scan_equip" is added.
*/

typedef u_long	*(*GsUNIT_Funcp)(GsARGUNIT *);
typedef u_long	*(*GsUNIT_AnimFuncp)(GsARGUNIT_ANIM *);
typedef int	(*GsUNIT_InterpFuncp)(GsARGUNIT_ANIM *);

extern u_long	scan_poly(u_long type);
extern u_long	scan_shared(u_long type);
extern u_long	scan_image(u_long type);
extern u_long	scan_anim(u_long type);
extern void	scan_interp(void);
extern u_long	scan_mime(u_long type);
extern u_long	scan_gnd(u_long type);
extern u_long	scan_envmap(u_long type);
extern u_long	scan_equip(u_long type);

extern u_long	*GsU_f0000008(GsARGUNIT *);
extern u_long	*GsU_f0000009(GsARGUNIT *);
extern u_long	*GsU_f000000c(GsARGUNIT *);
extern u_long	*GsU_f000000d(GsARGUNIT *);
extern u_long	*GsU_f0000010(GsARGUNIT *);
extern u_long	*GsU_f0000011(GsARGUNIT *);
extern u_long	*GsU_f0000014(GsARGUNIT *);
extern u_long	*GsU_f0000015(GsARGUNIT *);

extern void	common_PadInit(void);
extern void	common_PadStop(void);
extern u_long	common_PadRead(void);

#define NORM_SHFT	7	/* shift value for normal vector length */
