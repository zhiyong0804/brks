
#ifndef _GLO_DEF_H
#define _GLO_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Global defition */
#define TRUE    1
#define FALSE   0

#define INVALID_U32  0xFFFF

/* Global type defintions */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;     /* int == long */
typedef signed char    i8;
typedef signed short   i16;
typedef signed int     i32;     /* int == long */
typedef float          r32;
typedef double         r64;
typedef long double    r128;

typedef unsigned char  BOOL;

typedef u32            TBoolean;
typedef i32            TDevid;

#ifdef __DCC__ /* For diab compiler environment */

typedef unsigned long long u64;
typedef signed long long i64;

#endif

#ifdef __GNUC__ /* For GNU C compiler environment */
#if (0 < __GNUC__)

typedef unsigned long long u64;
typedef signed long long i64;

#endif
#endif



#ifdef __MSC__ /* For microsoft compiler environment */

typedef unsigned __int64 u64;
typedef __int64 i64;

#endif



#ifdef __TCC__ /* For texas compiler environment */

typedef unsigned long long u64;
typedef signed long long i64;

#endif



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GLO_DEF_H */




