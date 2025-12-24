// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : libEFlash.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef __LIBEFLASH_H__
#define __LIBEFLASH_H__

#include "type.h"

/*
 * Description:
 *     init eflash related functions and timings.
 *
 * Input:
 *     @f_eflash_khz: eflash modules clock
 *
 * Output:
 *     none
 */
extern void EFM_Init(UINT32 f_eflash_khz);

/*
 * Description:
 *     eflash program function.
 *
 * Input:
 *     @addr: eflash addr
 *     @data: programmed word
 *
 * Output:
 *     @0: EFLASH_PROG_ERASE_PASS
 *     @1: EFLASH_PROG_ERASE_FAIL
 */
extern UINT8 EFlash_Program(UINT32 addr, UINT32 data);

/*
 * Description:
 *     eflash bulk program function.
 *
 * Input:
 *     @addr: eflash addr
 *     @num_words: number of words to be programmed
 *     @data_buf: data buffer to be programmed
 *
 * Output:
 *     @0: EFLASH_PROG_ERASE_PASS
 *     @1: EFLASH_PROG_ERASE_FAIL
 */
extern UINT8 EFlash_Bulk_Program(UINT32 addr, UINT32 num_words, UINT32 *data_buf);

/*
 * Description:
 *     eflash page program function.
 *     flash page size is 512 byte.
 *
 * Input:
 *     @addr: eflash addr
 *
 * Output:
 *     @0: EFLASH_PROG_ERASE_PASS
 *     @1: EFLASH_PROG_ERASE_FAIL
 */
extern UINT8 EFlash_Page_Erase(UINT32 addr);

extern UINT8 EFlash_Disboot(void);
extern UINT8 EFlash_Recovery_to_Boot(void);
extern void EFlash_Set_Main_Permission(BOOL lp_writeable, BOOL lp_readable);

#endif // end of __EFLASH_DRV_H__
