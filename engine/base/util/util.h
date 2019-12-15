#ifndef UTIL_H_DEF
#define UTIL_H_DEF

#include <engine.h>

//-----------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(a, b, v) (MAX((a), MIN((v), (b))) )

//-----------------------------------------------------------------------
#define PACK_RGB(r, g, b) ( ( (r) << 16) | ( (g) << 8) | (b) )
#define UNPACK_RGB(v, r, g, b) { *(r) = ( (v) >> 16) & 0xff; *(g) = ( (v) >> 8) & 0xff; *(b) = (v) & 0xff; }

//-----------------------------------------------------------------------
#define ARRAY_SIZE(x) sizeof( (x) ) / sizeof( (x)[0] )
#define IS_POW2(x) ( (((x) & ~((x)-1))==(x))? (x) : 0)

//-----------------------------------------------------------------------
#define sgn(x) ((x) > 0) ? 1 : ( ((x) < 0) ? -1 : 0)

//-----------------------------------------------------------------------
#define RAND_MAX (0x7fff)
#define RAND_RANGE_TYPE(min, max, type) (min) + (##type)(rand()) /((##type)(RAND_MAX/((max)-(min) + 1)))
#define RAND_RANGE_I32(min, max) (RAND_RANGE_TYPE((min), (max), int32))
#define RAND_RANGE_F32(min, max) (RAND_RANGE_TYPE((min), (max), float))
#define RAND_RANGE_FP(min, max) (RAND_RANGE_I32((min), (max)) * ONE)

//-----------------------------------------------------------------------
#define setColor(c, _r, _g, _b) \
	(c)->r = (_r), (c)->g = (_g), (c)->b = (_b)

#define copyColor(c0, c1) \
	setColor((c0), (c1)->r, (c1)->g, (c1)->b)

#define mulColor(c, _r, _g, _b) \
	(c)->r = ( ((c)->r * _r + 0xFF) >> 8); \
	(c)->g = ( ((c)->g * _g + 0xFF) >> 8); \
	(c)->b = ( ((c)->b * _b + 0xFF) >> 8);

#define mulVector(v0, v1) \
	(v0)->vx = MulFP((v0)->vx, (v1)->vx);	\
	(v0)->vy = MulFP((v0)->vy, (v1)->vy);	\
	(v0)->vz = MulFP((v0)->vz, (v1)->vz);	

#define mulVectorF(v0, x) \
	(v0)->vx = MulFP((v0)->vx, (x)); \
	(v0)->vy = MulFP((v0)->vy, (x)); \
	(v0)->vz = MulFP((v0)->vz, (x));

#define vectorDot(v0, v1) ( MulFP((v0)->vx, (v1)->vx) + MulFP((v0)->vy, (v1)->vy) + MulFP((v0)->vz, (v1)->vz))
#define vectorLengthSq(v0) vectorDot((v0), (v0))
#define vectorLength(v0) SquareRoot0(vectorLengthSq((v0)))

#define rotateVectorXY(p, r, pivotx, pivoty) \
	{ \
		int16 oldx = (p)->vx - (pivotx); \
		int16 oldy = (p)->vy - (pivoty); \
		(p)->vx = (pivotx) + MulFP(oldx, rcos((r))) - MulFP(oldy, rsin((r))); \
		(p)->vy = (pivoty) + MulFP(oldx, rsin((r))) + MulFP(oldy, rcos((r))); \
	}

//-----------------------------------------------------------------------
#define OFFSET_OF(s,m) (uint32)&(((s *)0)->m)

//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
uint8* Util_AlignPtrUp(uint8* i_ptr, uint32 i_alignment);
uint8* Util_AlignPtrDown(uint8* i_ptr, uint32 i_alignment);
uint32 Util_AlignUp(uint32 i_value, uint32 i_alignment);
uint32 Util_AlignDown(uint32 i_value, uint32 i_alignment);
uint32 Util_AlignPtrAdjustment(uint8* i_ptr, uint32 i_alignment);
uint32 Util_AlignUpAdjustment(uint32 i_value, uint32 i_alignment);
uint32 Util_AlignPtrAdjustmentHeader(uint8* i_ptr, uint32 i_alignment, uint32 i_headerSize);
uint32 Util_AlignUpAdjustmentHeader(uint32 i_value, uint32 i_alignment, uint32 i_headerSize);
void Util_MemZero(void* i_ptr, uint32 i_size);

//-----------------------------------------------------------------------
uint8 Util_CountBits8(uint8 i_bitmask);
uint8 Util_CountBits16(uint16 i_bitmask);
uint8 Util_CountBits32(uint32 i_bitmask);

//-----------------------------------------------------------------------
StringId Util_HashString(const char* i_string);
#define ID(x) Util_HashString((x))

//-----------------------------------------------------------------------
int32 Util_GetTriangleWinding(SVECTOR* i_v0, SVECTOR* i_v1, SVECTOR* i_v2, SVECTOR* i_n);
void Util_FaceNormal(SVECTOR* i_n0, SVECTOR* i_n1, SVECTOR* i_n2, SVECTOR* o_faceNormal);

#endif // UTIL_H_DEF
