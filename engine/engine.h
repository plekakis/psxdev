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

#ifndef CONFIG_DEBUG
#define CONFIG_DEBUG (0)
#endif // CONFIG_DEBUG

#ifndef CONFIG_RELEASE
#define CONFIG_RELEASE (0)
#endif // CONFIG_RELEASE

#ifndef CONFIG_PROFILE
#define CONFIG_PROFILE (0)
#endif // CONFIG_PROFILE

#ifndef TARGET_PSX
#define TARGET_PSX (0)
#endif // TARGET_PSX

#ifndef TARGET_EMU
#define TARGET_EMU (0)
#endif // TARGET_EMU

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

#define SUCCESS(x)	((x) == E_OK)
#define FAILURE(x)	((x) != E_OK)

#endif // ENGINE_H_INC
