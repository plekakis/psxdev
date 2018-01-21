#ifndef ENGINE_H_INC
#define ENGINE_H_INC

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libapi.h>
#include <libpress.h>
#include <libcd.h>

#include <limits.h>

// Data types
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned long			uint32;
typedef unsigned long long		uint64;
typedef char					int8;
typedef short					int16;
typedef long					int32;
typedef long long				int64;

#ifndef NULL
#define NULL (0)
#endif // NULL

// Math macros & constants
#define PI 3.14159265359f

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif // MIN

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif // MAX

// Error code returned by most functions
#define E_OK					(0)
#define E_FAILURE				(1)
#define E_OUT_OF_MEMORY			(2)
#define E_SUBMISSION_ERROR		(3)


// Utility functions
// memory & pointers
void* AlignPtr(void* i_ptr, uint32 i_alignment);

// Misc
#define ARRAY_SIZE(x) sizeof( (x) ) / sizeof( (x)[0] )
#define PACK_RGB(r, g, b) ( ( (r) << 16) | ( (g) << 8) | (b) )
#define UNPACK_RGB(v, r, g, b) { *(r) = ( (v) >> 16) & 0xff; *(g) = ( (v) >> 8) & 0xff; *(b) = (v) & 0xff; }

#endif // ENGINE_H_INC
