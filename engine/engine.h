#ifndef ENGINE_H_INC
#define ENGINE_H_INC

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpad.h>
#include <libgs.h>
#include <libapi.h>
#include <libpress.h>
#include <libcd.h>
#include <GTEMAC.H>
#include <INLINE_C.H>
#include <limits.h>

#define ALIGN(x) __attribute__ ((aligned ((x))))

#define CRASHPSX { uint8* ptr = NULL; *ptr = (*ptr / 0); }

// Data types
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned long			uint32;
typedef unsigned long long		uint64;
typedef char					int8;
typedef short					int16;
typedef long					int32;
typedef long long				int64;

typedef uint16					StringId;

#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG (0)
#endif // CONFIG_DEBUG

#ifndef CONFIG_RELEASE
#define CONFIG_RELEASE (0)
#endif // CONFIG_RELEASE

#ifndef CONFIG_PROFILE
#define CONFIG_PROFILE (0)
#endif // CONFIG_PROFILE

#ifndef CONFIG_FINAL
#define CONFIG_FINAL (0)
#endif // CONFIG_FINAL

#ifndef TARGET_PSX
#define TARGET_PSX (0)
#endif // TARGET_PSX

#ifndef TARGET_EMU
#define TARGET_EMU (0)
#endif // TARGET_EMU

#define INT8_MAX    SCHAR_MAX
#define INT8_MIN    SCHAR_MIN
#define UINT8_MAX   UCHAR_MAX
#define INT16_MAX	SHRT_MAX
#define INT16_MIN   SHRT_MIN
#define UINT16_MAX  USHRT_MAX
#define INT32_MAX   INT_MAX
#define INT32_MIN   INT_MIN
#define UINT32_MAX  UINT_MAX

#ifndef NULL
#define NULL (0)
#endif // NULL

#ifndef TRUE
#define TRUE (1)
#endif // TRUE

#ifndef FALSE
#define FALSE(0)
#endif // FALSE

#ifndef bool
#define bool uint8
#endif // bool

#define PTR_SIZE (sizeof(void*))

#define ASSERT_ENABLED (CONFIG_DEBUG | CONFIG_RELEASE)

#if ASSERT_ENABLED
#define TTY_OUT(msg) printf("%s\n", msg)
#define VERIFY_ASSERT(x, ...) if (!(x)) { char t[128]; sprintf2(t, __VA_ARGS__); TTY_OUT(t); CRASHPSX; }
#define REPORT(...) { char t[128]; sprintf2(t, __VA_ARGS__); TTY_OUT(t); }
#define WARN(...) { REPORT("WARN: %s", __VA_ARGS__); }
#define STATIC_ASSERT(x, msg) static int static_assertion_##msg[(x)?1:-1];
#else
#define TTY_OUT(msg)
#define VERIFY_ASSERT(x, msg, ...)
#define REPORT(...)
#define WARN(...)
#define STATIC_ASSERT(x, msg)
#endif // ASSERT_ENABLED

// Math macros & constants
#define PI 3.14159265359f

// Error code returned by most functions
#define E_OK					(0)
#define E_FAILURE				(1)
#define E_OUT_OF_MEMORY			(2)
#define E_SUBMISSION_ERROR		(3)
#define E_INVALIDARGS			(5)
#define E_FILE_IO				(6)

#define SUCCESS(x)	((x) == E_OK)
#define FAILURE(x)	((x) != E_OK)

// Fixed point
typedef int32 fixed4_12;
typedef int32 fixed16_16;
typedef int32 fixed8_24;

#define Q4_12_SHIFT (12u)
#define ONE_Q4_12  (1u << 12u)   // equivelant to GTE ONE
#define Q4_12_MASK ((1u << Q4_12_SHIFT) - 1u)

#define Q16_16_SHIFT (16u)
#define ONE_Q16_16 (1u << Q16_16_SHIFT)
#define Q16_16_MASK ((1u << Q16_16_SHIFT) - 1u)

#define Q8_24_SHIFT (24u)
#define ONE_Q8_24 (1u << Q8_24_SHIFT)
#define Q8_24_MASK ((1u << Q8_24_SHIFT) - 1u)

#define FP_PI			(0x3244) // PI
#define FP_PI_OVER_2	(0x1922) // PI/2
#define FP_PI_OVER_4	(0xC91)  // PI/4
#define FP_PI_2			(0x6488) // PI * 2

#define FP_E			(0x2B7E) // e

// Macro for creating FP constants.
#define FP_Q4_12(x) ((fixed4_12)(((x) >= 0) ? ((x) * ONE_Q4_12 + 0.5) : ((x) * ONE_Q4_12 - 0.5)))
#define FP_Q16_16(x) ((fixed16_16)(((x) >= 0) ? ((x) * ONE_Q16_16 + 0.5) : ((x) * ONE_Q16_16 - 0.5)))
#define FP_Q8_24(x) ((fixed8_24)(((x) >= 0) ? ((x) * ONE_Q8_24 + 0.5) : ((x) * ONE_Q8_24 - 0.5)))

// Decimal points macros
#define FP_FRAC_1PT (10)
#define FP_FRAC_2PT (100)
#define FP_FRAC_3PT (1000)
#define FP_FRAC_4PT (10000)
#define FP_FRAC_5PT (100000)
#define FP_FRAC_6PT (1000000)

// Conversions
// Common
#define Convert_F32toFP(x, s) (int32)((x) * (float)(s))
#define Convert_I32toFP(x, s) (x) * (s)
#define Convert_FPtoF32(x, s) (float)(x) / (float)(s)
#define Convert_FPtoI32(x, s) (x) / (s)
#define Convert_FPtoI32Frac(x, d, m, s) (uint32)((uint64)((x) & (m)) * (d) / (s))
// Q4.12
static inline fixed4_12 F32toFP4_12(float v) { return Convert_F32toFP(v, ONE_Q4_12); }
static inline fixed4_12 I32toFP4_12(int32 v) { return Convert_I32toFP(v, ONE_Q4_12); }
static inline float FP4_12toF32(fixed4_12 v) { return Convert_FPtoF32(v, ONE_Q4_12); }
static inline int32 FP4_12toI32(fixed4_12 v) { return Convert_FPtoI32(v, ONE_Q4_12); }
static inline int32 FP4_12toI32Frac(fixed4_12 v, uint64 d) { return Convert_FPtoI32Frac(v, d, Q4_12_MASK, ONE_Q4_12);}
// Q16.16
static inline fixed16_16 F32toFP16_16(float v) { return Convert_F32toFP(v, ONE_Q16_16); }
static inline fixed16_16 I32toFP16_16(int32 v) { return Convert_I32toFP(v, ONE_Q16_16); }
static inline float FP16_16toF32(fixed16_16 v) { return Convert_FPtoF32(v, ONE_Q16_16); }
static inline int32 FP16_16toI32(fixed16_16 v) { return Convert_FPtoI32(v, ONE_Q16_16); }
static inline int32 FP16_16toI32Frac(fixed16_16 v, uint64 d) { return Convert_FPtoI32Frac(v, d, Q16_16_MASK, ONE_Q16_16);}
// Q8.24
static inline fixed8_24 F32toFP8_24(float v) { return Convert_F32toFP(v, ONE_Q8_24); }
static inline fixed8_24 I32toFP8_24(int32 v) { return Convert_I32toFP(v, ONE_Q8_24); }
static inline float FP8_24toF32(fixed8_24 v) { return Convert_FPtoF32(v, ONE_Q8_24); }
static inline int32 FP8_24toI32(fixed8_24 v) { return Convert_FPtoI32(v, ONE_Q8_24); }
static inline int32 FP8_24toI32Frac(fixed8_24 v, uint64 d) { return Convert_FPtoI32Frac(v, d, Q8_24_MASK, ONE_Q8_24);}

// Operations
// Common
#define Op_MulFP(x, y, s) (((int64)(x) * (int64)(y)) / (s))
#define Op_DivFP(x, y, s) (((int64)(x) * (s)) / (y))
#define Op_LerpFP(x, y, f, s) Op_MulFP((y), (f), (s)) + Op_MulFP((x), (s) - (f), (s))
// Q4.12
static inline fixed4_12 MulFP4_12(fixed4_12 v0, fixed4_12 v1) { return (fixed4_12)Op_MulFP(v0, v1, ONE_Q4_12); }
static inline fixed4_12 DivFP4_12(fixed4_12 v0, fixed4_12 v1) { return (fixed4_12)Op_DivFP(v0, v1, ONE_Q4_12); }
static inline fixed4_12 AddFP4_12(fixed4_12 v0, fixed4_12 v1) { return v0 + v1; }
static inline fixed4_12 SubFP4_12(fixed4_12 v0, fixed4_12 v1) { return v0 - v1; }
static inline fixed4_12 ModFP4_12(fixed4_12 v0, fixed4_12 v1) { return v0 % v1; }
static inline fixed4_12 LerpFP4_12(fixed4_12 v0, fixed4_12 v1, fixed4_12 f) { return Op_LerpFP(v0, v1, f, ONE_Q4_12); }
// Q16.16
static inline fixed16_16 MulFP16_16(fixed16_16 v0, fixed16_16 v1) { return (fixed16_16)Op_MulFP(v0, v1, ONE_Q16_16); }
static inline fixed16_16 DivFP16_16(fixed16_16 v0, fixed16_16 v1) { return (fixed16_16)Op_DivFP(v0, v1, ONE_Q16_16); }
static inline fixed16_16 AddFP16_16(fixed16_16 v0, fixed16_16 v1) { return v0 + v1; }
static inline fixed16_16 SubFP16_16(fixed16_16 v0, fixed16_16 v1) { return v0 - v1; }
static inline fixed16_16 ModFP16_16(fixed16_16 v0, fixed16_16 v1) { return v0 % v1; }
static inline fixed16_16 LerpFP16_16(fixed16_16 v0, fixed16_16 v1, fixed16_16 f) { return Op_LerpFP(v0, v1, f, ONE_Q16_16); }
// Q8.24
static inline fixed8_24 MulFP8_24(fixed8_24 v0, fixed8_24 v1) { return (fixed8_24)Op_MulFP(v0, v1, ONE_Q8_24); }
static inline fixed8_24 DivFP8_24(fixed8_24 v0, fixed8_24 v1) { return (fixed8_24)Op_DivFP(v0, v1, ONE_Q8_24); }
static inline fixed8_24 AddFP8_24(fixed8_24 v0, fixed8_24 v1) { return v0 + v1; }
static inline fixed8_24 SubFP8_24(fixed8_24 v0, fixed8_24 v1) { return v0 - v1; }
static inline fixed8_24 ModFP8_24(fixed8_24 v0, fixed8_24 v1) { return v0 % v1; }
static inline fixed8_24 LerpFP8_24(fixed8_24 v0, fixed8_24 v1, fixed8_24 f) { return Op_LerpFP(v0, v1, f, ONE_Q8_24); }

#endif // ENGINE_H_INC
