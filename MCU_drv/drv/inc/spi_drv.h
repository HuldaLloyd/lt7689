// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : spi_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef SPI_DRV_H_
#define SPI_DRV_H_

#include "spi_reg.h"
#include "memmap.h"

/* SPI Init structure definition */
typedef struct
{

  UINT8 SPI_Direction;
  UINT8 SPI_Mode;
  UINT8 SPI_DataSize;
  UINT8 SPI_CPOL;//SPI空闲时，时钟信号SCLK的电平（1:空闲时高电平; 0:空闲时低电平）
  UINT8 SPI_CPHA;//SPI在SCLK第几个边沿开始采样（0:第一个边沿开始; 1:第二个边沿开始）
  UINT8 SPI_BaudRatePrescaler;//SPI Baud Rate
  UINT8 SPI_FirstBit;//LSB-First Enable Bit
  UINT8 SPI_CRCPolynomial;

}SPI_InitTypeDef;


typedef enum
{
    SPI_MISO = 0,
    SPI_MOSI,
    SPI_SCK,
    SPI_SS
}SPI_PINx;

#define IS_SPI_PINx(pin) (((pin) == SPI_MISO) || \
                          ((pin) == SPI_MOSI) || \
                          ((pin) == SPI_SCK) || \
                          ((pin) == SPI_SS))


#define SPI1		((SPI_TypeDef *)(SPI1_BASE_ADDR))
#define SPI2		((SPI_TypeDef *)(SPI2_BASE_ADDR))
#define SPI3		((SPI_TypeDef *)(SPI3_BASE_ADDR))



#define IS_SPI_ALL_PERIPH(PERIPH) (((PERIPH) == SPI1) || \
		                           ((PERIPH) == SPI2) || \
                                   ((PERIPH) == SPI3))

/* SPI master/slave mode */
#define SPI_Mode_Master                 ((UINT8)0x10)
#define SPI_Mode_Slave                  ((UINT8)0x00)

#define IS_SPI_MODE(MODE) (((MODE) == SPI_Mode_Master) || \
                           ((MODE) == SPI_Mode_Slave))


/* SPI data direction mode */
#define SPI_Direction_2Lines_FullDuplex ((UINT8)0x00)
#define SPI_Direction_1Line_RxOrTx      ((UINT8)0x01)


#define IS_SPI_DIRECTION_MODE(MODE) (((MODE) == SPI_Direction_2Lines_FullDuplex) || \
                                     ((MODE) == SPI_Direction_1Line_RxOrTx))

/* SPI data size */
#define SPI_DataSize_16b             (0x0f)
#define SPI_DataSize_8b              (0x07)


#define IS_SPI_DATASIZE(DATASIZE) (((DATASIZE) == SPI_DataSize_16b) || \
                                   ((DATASIZE) == SPI_DataSize_8b))


/* SPI Clock Polarity */
#define SPI_CPOL_Low                    ((UINT8)0x00)
#define SPI_CPOL_High                   ((UINT8)0x08)

#define IS_SPI_CPOL(CPOL) (((CPOL) == SPI_CPOL_Low) || \
                           ((CPOL) == SPI_CPOL_High))

/* SPI Clock Phase */
#define SPI_CPHA_1Edge                  ((UINT16)0x00)
#define SPI_CPHA_2Edge                  ((UINT16)0x04)

#define IS_SPI_CPHA(CPHA) (((CPHA) == SPI_CPHA_1Edge) || \
                           ((CPHA) == SPI_CPHA_2Edge))

/* SPI BaudRate Prescaler  */
#define SPI_BaudRatePrescaler_2         (0x00)
#define SPI_BaudRatePrescaler_4         (0x01)
#define SPI_BaudRatePrescaler_6         (0x20)
#define SPI_BaudRatePrescaler_8         (0x02)
#define SPI_BaudRatePrescaler_10        (0x40)
#define SPI_BaudRatePrescaler_20        (0x41)
#define SPI_BaudRatePrescaler_40        (0x42)
#define SPI_BaudRatePrescaler_64        (0x05)
#define SPI_BaudRatePrescaler_256       (0x07)


#define IS_SPI_BAUDRATE_PRESCALER(PRESCALER) (((PRESCALER) == SPI_BaudRatePrescaler_2) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_4) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_6) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_8) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_10) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_20) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_40) || \
                                              ((PRESCALER) == SPI_BaudRatePrescaler_64))

/* SPI MSB/LSB transmission */
#define SPI_FirstBit_MSB                ((UINT8)0x00)
#define SPI_FirstBit_LSB                ((UINT8)0x01)

#define IS_SPI_FIRST_BIT(BIT) (((BIT) == SPI_FirstBit_MSB) || \
                               ((BIT) == SPI_FirstBit_LSB))


#define CR1_SPE_Set    ((UINT8)0x40)
#define CR1_SPE_Reset  ((UINT8)0xBF)

#define SPI_CS_L(x)		    do{x->SPIPORT &= 0xF7;}while(0)
#define SPI_CS_H(x)		    do{x->SPIPORT |= 0x08;}while(0)

//#define SPI1_CS_L		    do{SPI1->SPIPORT &= 0xF7;}while(0)
//#define SPI1_CS_H		    do{SPI1->SPIPORT |= 0x08;}while(0)

//#define SPI2_CS_L		    do{SPI2->SPIPORT &= 0xF7;}while(0)
//#define SPI2_CS_H		    do{SPI2->SPIPORT |= 0x08;}while(0)

//#define SPI3_CS_L		    do{SPI3->SPIPORT &= 0xF7;}while(0)
//#define SPI3_CS_H		    do{SPI3->SPIPORT |= 0x08;}while(0)

#define SPISR_TXDMAE_MASK      (0x02)
#define SPISR_RXDMAE_MASK      (0x01)

/*******************************************************************************
* Function Name  : SPI_StructInit
* Description    : Fills each SPI_InitStruct member with its default value.
* Input          : - SPI_InitStruct : pointer to a SPI_InitTypeDef structure
*                    which will be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_StructInit(UINT8 mode,SPI_InitTypeDef* SPI_InitStruct);

/*******************************************************************************
* Function Name  : SPI_Init
* Description    : SPI初始化
*
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
*                    contains the configuration information for the specified
*                    SPI peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);

/*******************************************************************************
* Function Name  : SPI_Cmd
* Description    : Enables or disables the specified SPI peripheral.
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - NewState: new state of the SPIx peripheral.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);

/*******************************************************************************
* Function Name  : SPI_MasterSendData
* Description    : SPI作为主设备发送数据
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - Send: 发送数据缓冲
*                  - Len：发送数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_MasterSendData(SPI_TypeDef* SPIx, UINT8 *Send, UINT32 Len);

/*******************************************************************************
* Function Name  : SPI_MasterFixedSendData
* Description    : SPI作为主设备发送长度Len的相同Send数据
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - Send: 发送的数据
*                  - Len：发送数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_MasterFixedSendData(SPI_TypeDef* SPIx, UINT8 Send, UINT16 Len);

/*******************************************************************************
* Function Name  : SPI_MasterReceiveData
* Description    : SPI作为主设备接收来自从设备数据
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*
*                  - Len：需要接收数据的长度
*
* Output         : - Revc: 接收数据缓冲
* Return         : None
******************************************************************************/
void SPI_MasterReceiveData(SPI_TypeDef* SPIx, UINT8 *Recv, UINT16 Len);

/*******************************************************************************
* Function Name  : SPI_MasterSendData
* Description    : SPI作为从设备发送数据
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - Send: 发送数据缓冲
*                  - Len：发送数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_SlaveSendData(SPI_TypeDef* SPIx, UINT8 *Send, UINT16 Len);

/*******************************************************************************
* Function Name  : SPI_SlaveReceiveData
* Description    : SPI作为从设备接收来自主设备数据
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*
*                  - Len：需要接收数据的长度
*
* Output         : - Revc: 接收数据缓冲
* Return         : None
******************************************************************************/
void SPI_SlaveReceiveData(SPI_TypeDef* SPIx, UINT8 *Revc, UINT16 Len);


/*******************************************************************************
* Function Name  : SPI_ConfigGpio
* Description    : SPI配置成GPIO用途
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - SPI_Pin：SPI对应的PIN脚，取值SPI_MISO、SPI_MOSI、SPI_SCK、SPI_SS
*                  - SPI_Dir：设置GPIO方向   GPIO_OUTPUT：输出  GPIO_INPUT：输入
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_ConfigGpio(SPI_TypeDef* SPIx, SPI_PINx SPI_Pin, UINT8 SPI_Dir);

/*******************************************************************************
* Function Name  : SPI_ReadGpioData
* Description    : 获取SPI_Pin对应引脚的电平
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - SPI_Pin：SPI对应的PIN脚，取值SPI_MISO、SPI_MOSI、SPI_SCK、SPI_SS
*
* Output         : None
* Return         : Bit_SET:高电平  Bit_RESET：低电平
******************************************************************************/
UINT8 SPI_ReadGpioData(SPI_TypeDef* SPIx, SPI_PINx SPI_Pin);

/*******************************************************************************
* Function Name  : SPI_WriteGpioData
* Description    : 设置SPI_Pin对应引脚的电平
* Input          : - SPIx: where x can be 1, 2  to select the SPI peripheral.
*                  - SPI_Pin：SPI对应的PIN脚，取值SPI_MISO、SPI_MOSI、SPI_SCK、SPI_SS
*                  - bitVal：设置的电平，Bit_SET：设置为高电平  Bit_RESET：设置为低电平
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_WriteGpioData(SPI_TypeDef* SPIx, SPI_PINx SPI_Pin, UINT8 bitVal);

/*******************************************************************************
* Function Name  : SPI_MasterReceiveDataByEDMA
* Description    : SPI作为主设备发送并接收数据
* Input          : - SPIx: where x can be 1, 2 ,3 to select the SPI peripheral.
*				   - recvAddr: 接收数据缓冲首地址
*                  - Len：发送数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_MasterReceiveDataByEDMA(SPI_TypeDef* SPIx, UINT32 recvAddr, UINT32 Len);

/*******************************************************************************
* Function Name  : SPI_MasterOnlyReceiveDataByEDMA
* Description    : SPI作为主设备接收数据
* Input          : - SPIx: where x can be 1, 2 ,3 to select the SPI peripheral.
*                  - recvAddr: 接收数据缓冲首地址
*                  - Len：接收数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/

void SPI_MasterOnlyReceiveDataByEDMA(SPI_TypeDef* SPIx, UINT32 recvAddr, UINT32 Len);

/*******************************************************************************
* Function Name  : SPI_MasterSendDataByEDMA
* Description    : SPI作为主设备发送数据
* Input          : - SPIx: where x can be 1, 2 ,3 to select the SPI peripheral.
*                  - sendAddr: 发送数据缓冲首地址
*                  - Len：发送数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_MasterSendDataByEDMA(SPI_TypeDef* SPIx, UINT32 sendAddr, UINT32 Len);

/*******************************************************************************
* Function Name  : SPI_MasterOnlySendDataByEDMA
* Description    : SPI作为主设备发送数据
* Input          : - SPIx: where x can be 1, 2 ,3 to select the SPI peripheral.
*                  - sendAddr: 发送数据缓冲首地址
*                  - Len：发送数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_MasterOnlySendDataByEDMA(SPI_TypeDef* SPIx, UINT32 sendAddr, UINT32 Len);
/*******************************************************************************
* Function Name  : SPI_MasterOnlySendSameDataByEDMA
* Description    : SPI作为主设备发送数据，不会改变源buf数据
* Input          : - SPIx: where x can be 1, 2 ,3 to select the SPI peripheral.
*                  - sendAddr: 发送数据缓冲首地址
*                  - Len：发送数据的长度
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_MasterOnlySendSameDataByEDMA(SPI_TypeDef* SPIx, UINT32 sendAddr, UINT32 Len);

/*******************************************************************************
* Function Name  : SPI_EnableDMA
* Description    : SPI DMA功能的使能
* Input          : - SPIx: where x can be 1, 2, 3  to select the SPI peripheral.
*				   - enable: 是否使能
*
* Output         : None
* Return         : None
******************************************************************************/
void SPI_EnableDMA(SPI_TypeDef* SPIx,bool enable);

/*******************************************************************************
* Function Name  : SPI_StructInit_NFC
* Description    : Fills each SPI_InitStruct member with its default value.
* Input          : - SPI_InitStruct : pointer to a SPI_InitTypeDef structure
*                    which will be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_StructInit_NFC(UINT8 mode,SPI_InitTypeDef* SPI_InitStruct);

#endif /* SPI_DRV_H_ */
