
#ifndef _MEAN_FILTER_
#define _MEAN_FILTER_
/*******************************************************************************
* Function Name  : mean_filter
* Description    : 一维均值滤波函数
* Input          : indata：输入数组指针
*                  outdata: 输出数据指针
*                  k: 滤波参数，均值计算窗口大小（k<n）
*                  n: 处理数据字符数目
*
* Output         : None
* Return         : None
******************************************************************************/
void mean_filter(unsigned char *indata,unsigned char *outdata,int k,int n);
#endif
