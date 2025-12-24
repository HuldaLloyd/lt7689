#include "pwm.h"
#include "pwm_drv.h"
#include "eport_drv.h"
#include "LT768_Lib.h"
#include "spi_drv.h"

void LT_BacklightSet(UINT16 duty)
{

	LT768_PWM1_Duty(duty);//×î´ó799
}
