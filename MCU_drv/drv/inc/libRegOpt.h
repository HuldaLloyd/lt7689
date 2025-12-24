
/*******************************************************************************
* Function Name  :CPM_PWRCR_OptBits
* Description    :对CPM PWRCR 寄存器进行位操作(bit),
* Input          :-bits		:输入位(bits)
*									-opt		:置位或清零
*									 0			:清零
*                  1			:置位
*
* Output         : None
* Return         : None
******************************************************************************/
extern unsigned char CPM_PWRCR_OptBits(unsigned int bits, unsigned char opt);

