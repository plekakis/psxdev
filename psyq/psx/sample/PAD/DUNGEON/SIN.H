/*
 * $PSLibId: Run-time Library Release 4.4$
 */
inline long _rsin(long a);
inline long _rcos(long a);
inline long _ratan(long a);
inline long _ratan2( int dx, int dy );

#define	rsin(a)		_rsin(a)
#define	rcos(a)		_rcos(a)
#define	ratan(a)	_ratan(a)
#define	ratan2(a)	_ratan2(a)

