/******************************************************************************
	* @file    AmadeusMain.c
	* @author  ÕÅ¶«
	* @version V1.0.0
	* @date    2019-4-8
	* @brief   nixie tube clock main function
	******************************************************************************/
/*Iniclude -----------------------------------*/
#include "stm32f0xx_hal.h"
//hardware
#include "oled.h"
#include "HighVoltage.h"
#include "NixieTube.h"
#include "KeyBoard.h"
#include "DS3231.h"

#include "AmadeusMain.h"
#include "NixieApp.h"
#include "stdio.h"
#include "main.h"

extern TIM_HandleTypeDef htim3;   //main.c
extern TIM_HandleTypeDef htim14;  //main.c

void Sys_setup(void)
{
	OLED_Init();
	OLED_Clear();
	
	Nixie_Setup();
	
	Time_Handle();
	
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start(&htim14);
	HAL_TIM_Base_Start_IT(&htim14);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
}

void Sys_loop(void)
{
	Nixie_Loop();
}

void Sys_Interrupt(void)
{
	Nixie_Interrupt();
}





