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

#define CRASHPSX { uint8* ptr = NULL; *ptr = (*ptr / 0); }

// Data types
typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned long			uint32;
typedef char					int8;
typedef short					int16;
typedef long					int32;

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

#define ASSERT_ENABLED (CONFIG_DEBUG | CONFIG_RELEASE)

#if ASSERT_ENABLED
#define TTY_OUT(msg) printf("%s\n", msg)
#define VERIFY_ASSERT(x, ...) if (!(x)) { char t[128]; sprintf2(t, __VA_ARGS__); TTY_OUT(t); CRASHPSX; }
#define STATIC_ASSERT(x, msg) static int static_assertion_##msg[(x)?1:-1];
#else
#define TTY_OUT(msg)
#define VERIFY_ASSERT(x, msg, ...)
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

#define SUCCESS(x)	((x) == E_OK)
#define FAILURE(x)	((x) != E_OK)

#endif // ENGINE_H_INC
