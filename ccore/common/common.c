#include "common.h"
#include "type.h"

UINT32 enter_critical_sr_save()
{
	register UINT32 __regPriMask __asm("primask");
	return (__regPriMask);
}

void exit_critical_sr_restore(UINT32 primask)
{
	register UINT32 __regPriMask __asm("primask");
	__regPriMask = (primask);
}

UINT32 Read_VEC()
{
	UINT32 vec_num;
	register UINT32 _reg_IPSR __asm("ipsr");

	vec_num = (_reg_IPSR & 0x1FF) - 16;

	return vec_num;
}
void assert_failed(UINT8 *file, UINT32 line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
	while (1)
		;
}