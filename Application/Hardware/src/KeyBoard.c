#include "stm32f0xx_hal.h"
#include "KeyBoard.h"


uint8_t getKey(void)
{
	//key map             
	uint32_t pin[3] = {Key1_Pin,Key2_Pin,Key3_Pin};
	uint8_t i;
	for(i = 0;i < 3;i++)
	{
		if((Key1_GPIO_Port->IDR & pin[i]) == 0)
		{
			return i+1;
		}
	}
	return 0;
}

uint16_t key_times(uint8_t key_state)
{
	static uint16_t time = 0;
	static uint32_t ts = 0;
	static uint8_t  hist = 0;
	if(key_state != 0)
	{
		if(hist != key_state)
		{
			ts = HAL_GetTick();
		}
		time = HAL_GetTick() - ts;
	}
	else
	{
		time = 0;
	}
	hist = key_state;
	
	return time / 50;
}

