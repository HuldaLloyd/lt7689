// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : type.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __TYPE_H__
#define __TYPE_H__

#include "LT7689_C.h"

typedef unsigned char  			BOOLEAN;
typedef unsigned char           UINT8;
typedef signed char             INT8;
typedef unsigned short int      UINT16;
typedef signed short int        INT16;
typedef unsigned int            UINT32;
typedef signed int              INT32;
typedef float          		    FP32;
typedef double         		    FP64;

typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;

typedef unsigned long           ULONG;
typedef signed long             LONG;

typedef char 			        TCHAR ;
typedef unsigned char 	        TBYTE ;
typedef unsigned int  	        TWORD ;
typedef unsigned long           TDWORD;
typedef unsigned char           BOOL;


#define   __I     volatile const       /*!< Defines 'read only' permissions                 */
#define   __O     volatile             /*!< Defines 'write only' permissions                */
#define   __IO    volatile             /*!< Defines 'read / write' permissions              */

#define BIT0 (1<<0)
#define BIT1 (1<<1)
#define BIT2 (1<<2)
#define BIT3 (1<<3)
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT7 (1<<7)
#define BIT8 (1<<8)
#define BIT9 (1<<9)
#define BIT11 (1<<11)

#ifndef U32
typedef unsigned int U32;
#endif

#ifndef U16
typedef unsigned short int U16;
#endif

#ifndef U8
typedef unsigned char U8;
#endif

#ifndef Uint32
typedef unsigned int Uint32;
#endif

#ifndef Uint16
typedef unsigned short int Uint16;
#endif

#ifndef Uint8
typedef unsigned char Uint8;
#endif

#ifndef   uchar
#define   uchar   unsigned char
#endif
#ifndef   uint
#define   uint    unsigned int
#endif
#ifndef   ushort
#define   ushort  unsigned short
#endif
#ifndef   ulong
#define   ulong   unsigned long
#endif


typedef unsigned       char  u8;
typedef signed         char  s8;
typedef unsigned short int   u16;
typedef signed   short int  s16;
typedef unsigned       int   u32;
typedef signed         int   s32;
typedef unsigned long  int   u64;
typedef signed   long  int   s64;

typedef unsigned int  const uc32;  /* Read Only */
typedef unsigned short int const uc16;  /* Read Only */
typedef unsigned char  const uc8;   /* Read Only */

typedef volatile unsigned int  vu32;
typedef volatile unsigned short int vu16;
typedef volatile unsigned char  vu8;

typedef volatile unsigned int  const vuc32;  /* Read Only */
typedef volatile unsigned short int const vuc16;  /* Read Only */
typedef volatile unsigned char  const vuc8;   /* Read Only */
typedef unsigned char bool_t;

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))


/* Bit_SET and Bit_RESET enumeration -----------------------------------------*/
typedef enum
{ Bit_RESET = 0,
  Bit_SET
}BitAction;
#define IS_GPIO_BIT_ACTION(ACTION) (((ACTION) == Bit_RESET) || ((ACTION) == Bit_SET))


#define GPIO_INPUT    0
#define GPIO_OUTPUT   1
#define IS_GPIO_DIR_BIT(BIT) (((BIT) == GPIO_INPUT) || \
                              ((BIT) == GPIO_OUTPUT))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define	_CLOCK_T_	unsigned long		/* clock() */
#define	_TIME_T_	long			/* time() */
#define _CLOCKID_T_ 	unsigned long
#define _TIMER_T_   	unsigned long

#ifndef __clock_t_defined
typedef _CLOCK_T_ clock_t;
#define __clock_t_defined
#endif

#ifndef __time_t_defined
typedef _TIME_T_ time_t;
#define __time_t_defined
#endif

typedef unsigned long useconds_t;
typedef long suseconds_t;

//#define _U	0x01	/* upper */
//#define _L	0x02	/* lower */
//#define _D	0x04	/* digit */
//#define _C	0x08	/* cntrl */
//#define _P	0x10	/* punct */
//#define _S	0x20	/* white space (space/lf/tab) */
//#define _X	0x40	/* hex digit */
//#define _SP	0x80	/* hard space (0x20) */

//extern unsigned char _ctype[];
//extern char _ctmp;

//#define isalnum(c) ((_ctype+1)[c]&(_U|_L|_D))
//#define isalpha(c) ((_ctype+1)[c]&(_U|_L))
//#define iscntrl(c) ((_ctype+1)[c]&(_C))
//#define isdigit(c) ((_ctype+1)[c]&(_D))
//#define isgraph(c) ((_ctype+1)[c]&(_P|_U|_L|_D))
//#define islower(c) ((_ctype+1)[c]&(_L))
//#define isprint(c) ((_ctype+1)[c]&(_P|_U|_L|_D|_SP))
//#define ispunct(c) ((_ctype+1)[c]&(_P))
//#define isspace(c) ((_ctype+1)[c]&(_S))
//#define isupper(c) ((_ctype+1)[c]&(_U))
//#define isxdigit(c) ((_ctype+1)[c]&(_D|_X))

//#define isascii(c) (((unsigned) c)<=0x7f)
//#define toascii(c) (((unsigned) c)&0x7f)

//#define tolower(c) (_ctmp=c,isupper(_ctmp)?_ctmp-('A'-'a'):_ctmp)
//#define toupper(c) (_ctmp=c,islower(_ctmp)?_ctmp-('a'-'A'):_ctmp)

#endif   /* __TYPE_H__ */

