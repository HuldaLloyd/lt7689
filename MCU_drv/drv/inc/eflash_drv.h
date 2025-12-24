// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : eflash_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef __EFLASH_DRV_H__
#define __EFLASH_DRV_H__

#include "type.h"
#include "memmap.h"

#define EFCR	EFM_BASE_ADDR

#define EFLASH_BASE				          (0x8000000)
#define EFLASH_PAGE_SIZE_BYTE       (0x200)		//512
#define EFLASH_PAGE_SIZE_HFWORD     (0x100)		//256
#define EFLASH_PAGE_SIZE_WORD   	  (0x80)		//128
#define EFLASH_BYTE_SIZE	           (0x80000)	//512kbytes, 128k words
#define EFLASH_ALL_PAGE_LEN         (1024)      //1024page
#define PAGE_MASK                   (EFLASH_PAGE_SIZE_BYTE-1)

#define EFLASH_WRITEABLE	  TRUE
#define EFLASH_READABLE	    TRUE
#define EFLASH_DISWRITEABLE	FALSE
#define EFLASH_DISREADABLE	FALSE

#define EFM0_MAIN_BASEADDR	(0x08000000)
#define EFM0_MAIN_LEN				(0x00080000)

#define EFLASH_PAGE_SIZE_BYTE			(0x200)
#define EFLASH_PAGE_SIZE_HFWORD   (0x100)		//256
#define EFLASH_PAGE_SIZE_WORD   	(0x80)		//128

#define EFLASH_OP_PASS		0
#define EFLASH_OP_FAIL		1
#define EFLASH_PARA_ERR		2

/*******************************************************************************
* Function Name  : EFlash_Disboot
* Description    : disboot
* Input          : - None
*
* Output         : None
* Return         : TRUE or FALSE
******************************************************************************/
extern UINT8 EFlash_Disboot(void);

/*******************************************************************************
* Function Name  : EFlash_Recovery_to_Boot
* Description    : 回到ROM BOOT状态
* Input          : - None
*
* Output         : None
* Return         : TRUE or FALSE
******************************************************************************/
extern UINT8 EFlash_Recovery_to_Boot(void);

/*******************************************************************************
* Function Name  : EFLASH_Set_RWSC
* Description    : 设置EFLASH RWSC值
* Input          : -rwsc: rwsc值，默认为0x07
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Set_RWSC(UINT8 rwsc);

/*******************************************************************************
* Function Name  : EFLASH_Init
* Description    : EFLASH初始化
* Input          : - f_sys_khz: 系统时钟（单位khz）
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Init(UINT32 sys_clk_khz);

/*******************************************************************************
* Function Name  : EFLASH_SetWritePermission
* Description    : 开启EFLASH首块操作
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_SetWritePermission(void);

/*******************************************************************************
* Function Name  : EFLASH_ClrWritePermission
* Description    : 关闭EFLASH首块操作
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_ClrWritePermission(void);

/*******************************************************************************
* Function Name  : EFLASH_ByteRead
* Description    : 从EFLASH目标地址读取一个byte数据
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : 读取的byte数据
******************************************************************************/
extern UINT8   EFLASH_ByteRead(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_HalfWordRead
* Description    : 从EFLASH目标地址读取一个half word数据
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : 读取的half word数据
******************************************************************************/
extern UINT16  EFLASH_HalfWordRead(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_WordRead
* Description    : 从EFLASH目标地址读取一个word数据
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : 读取的word数据
******************************************************************************/
extern UINT32  EFLASH_WordRead(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_WordsRead
* Description    :  从EFLASH目标地址读取长度为len的word数据
* Input          : - addr: EFLASH destination address
*                  - len:  需要读取word数据的长度
*
* Output         : - des:  读取的word数据缓冲
* Return         : None
******************************************************************************/
extern void    EFLASH_WordsRead(UINT32 addr, UINT32 *des, UINT32 len);
extern void	   EFLASH_WordsRead_8(UINT32 addr, UINT8 *des, UINT32 len);
/*******************************************************************************
* Function Name  : EFLASH_PageErase
* Description    : EFLASH页擦除
* Input          : - addr: 要擦除的EFLASH页地址
*
* Output         : None
* Return         :  0：擦除成功   1：擦除失败
******************************************************************************/
extern UINT8   EFLASH_PageErase(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_WordProg
* Description    : EFLASH的单word编程
* Input          : - addr: 要编程的EFLASH地址
*                  - dat:  编程的word数据
*
* Output         : None
* Return         :  0：编程成功   1：编程失败
******************************************************************************/
extern UINT8 EFLASH_WordProg(UINT32 addr, UINT32 dat);

/*******************************************************************************
* Function Name  : EFLASH_WordsProg
* Description    :  EFLASH的多word编程
* Input          : - addr: 要编程的EFLASH地址
*                  - des:  编程的word数据指针
*                  - len:  要编程的word数据长度
*
* Output         : None
* Return         :  0：编程成功   1：编程失败
******************************************************************************/
extern UINT8   EFLASH_WordsProg(UINT32 addr, UINT32 *des, UINT32 len);

/*******************************************************************************
* Function Name  : EFLASH_Write
* Description    :  EFLASH任意地址写数据
* Input          : - flash_addr: 目的地址
*                  - ram_addr:  源地址指针
*                  - len:  数据长度
*
* Output         : None
* Return         :  TRUE：编程成功   FALSE：编程失败
******************************************************************************/
UINT8 EFLASH_Write(UINT32 flash_addr, const UINT8* ram_addr, UINT32 len);

#endif // end of __EFLASH_DRV_H__
