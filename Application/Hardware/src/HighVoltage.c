#include "main.h"
#include "stm32f0xx_hal.h"
#include "HighVoltage.h"

void HV_Control(uint8_t cmd)
{
		if(cmd)
		{
			HVPS_EN_GPIO_Port->BRR = HVPS_EN_Pin;
		}
		else
		{
			HVPS_EN_GPIO_Port->BSRR = HVPS_EN_Pin;
		}
}

