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

#define FP_PI			(0x3244) // PI
#define FP_PI_OVER_2	(0x1922) // PI/2
#define FP_PI_OVER_4	(0xC91)  // PI/4
#define FP_PI_2			(0x6488) // PI * 2

#define FP_E			(0x2B7E) // e

// Macro for creating FP constants.
#define FP_4_12(x) ((fixed4_12)(((x) >= 0) ? ((x) * ONE + 0.5) : ((x) * ONE - 0.5)))

// Conversions
static inline fixed4_12 F32toFP(float v) { return (int32)(v * (float)ONE); }
static inline fixed4_12 I32toFP(int32 v) { return v * ONE; }
static inline float FPtoF32(fixed4_12 v) { return (float)v / (float)ONE; }
static inline int32 FPtoI32(fixed4_12 v) { return v / ONE; }

// Operations
static inline fixed4_12 MulFP(fixed4_12 v0, fixed4_12 v1) { return (fixed4_12)(((int64)v0 * (int64)v1) / ONE); }
static inline fixed4_12 DivFP(fixed4_12 v0, fixed4_12 v1) { return (fixed4_12)(((int64)v0 * ONE) / v1); }
static inline fixed4_12 AddFP(fixed4_12 v0, fixed4_12 v1) { return v0 + v1; }
static inline fixed4_12 SubFP(fixed4_12 v0, fixed4_12 v1) { return v0 - v1; }
static inline fixed4_12 ModFP(fixed4_12 v0, fixed4_12 v1) { return v0 % v1; }
static inline fixed4_12 LerpFP(fixed4_12 v0, fixed4_12 v1, fixed4_12 f) { return MulFP(v1, f) + MulFP(v0, ONE - f); }

#endif // ENGINE_H_INC
