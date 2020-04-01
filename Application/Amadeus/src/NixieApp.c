/******************************************************************************
	* @file    NixieApp.c
	* @author  张东
	* @version V1.0.0
	* @date    2019-4-9
	* @brief   nixie tube clock applicetion file
	******************************************************************************/

/*Iniclude --------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
//hardware
#include "oled.h"
#include "HighVoltage.h"
#include "NixieTube.h"
#include "KeyBoard.h"
#include "DS3231.h"
//data space
#include "bmp.h"
#include "stdio.h"
#include "stdlib.h"
#include "NixieApp.h"
/*Golbal data space -------------------------------------------------*/
ModeTypedef   g_mode  = Mode_N_T; //golbal nixie tube display mode
StateTypedef  g_state = Normal;   //golbal state machine status
KeyTypedef    g_keyEvent;					//golbal key event
uint8_t worldline[24][8] = 
{
	//	=======α线=======
	//[本篇]
  {0,10,0,0,0,0,0,0,},//% 传说中的基准线 

  {0,10,5,7,1,0,2,4,},//% α最初
  {0,10,5,7,1,0,1,5,}, //% 六合
  {0,10,5,7,1,0,4,6,}, //% α最后 
  {0,10,5,2,3,2,9,9,},//0.523307}, //% 萌郁抢走IBN
  {0,10,4,5,6,9,0,3,},//0.456914}, //% (以下同)琉华子(女)
  {0,10,4,0,9,4,2,0,},//0.409431}, //% (以下同) 秋叶原不萌
  {0,10,3,3,7,1,8,7,}, //% 铃羽失败
	//------------------
  {0,10,3,3,4,5,8,1,}, //% 外传小说(α线)

	//=======β线=======
	//[本篇] 
  {1,10,1,3,0,4,2,6,}, //% 最最初
  {1,10,1,3,0,2,0,5,}, //% 回来后 /助手END
  {1,10,1,3,0,2,1,2,}, //% 时间旅行后(TE)
  {1,10,1,3,0,2,3,8,}, //% 真由里END
	//------------------ 
  {1,10,1,2,9,8,4,8,}, //% 广播剧β

	//=======Ω线=======
	//[本篇] 
  {0,10,2,7,5,3,4,9,}, //%(显示:.275349) 菲莉丝END

	//=======γ线=======
  {2,10,6,1,5,0,7,4,}, //% 广播剧γ

	//=======S;G线=======
	//[本篇] 
  {1,10,0,4,8,5,9,6,}, //% Steins;Gate
	//-----------------
  {1,10,0,4,8,7,2,8,}, //% 8 BIT 

	//======δ线=======
	//[比翼恋理的Darling]
  {3,10,1,3,0,2,3,8,}, //% 真由里线(立FLAG + Dmail"再好好细心考虑Dmail的实验") 
  {3,10,1,8,2,8,7,9,}, //% 菲莉丝线(Dmail"停上开发12号机") 
  {3,10,3,7,2,3,2,9,}, //% 铃羽线(FLAG没立好 + Dmail"再好好细心考虑Dmail的实验") 
  {3,10,4,0,6,2,8,8,}, //% 最初/助手线(不发Dmail)
  {3,10,6,0,0,1,0,4,}, //% 萌郁线(Dmail"停止开发新未来道具") 
  {3,10,6,6,7,2,9,3,}, //% 琉华子线(Dmail"不要戴上12号机") 
};
/*Function nodes ----------------------------------------------------*/

/*Nixie Display ---------------------------------*/
//Nixie display mode 
void NixieDisplay(ModeTypedef mode)
{
	uint8_t num[8];
	uint8_t str[30];
	switch(mode)
	{
		case Mode_N_T: //普通时间显示模式
		{
			num[0] = TimeValue.hour / 10;
			num[1] = TimeValue.hour % 10;
			num[2] = 11;
			num[3] = TimeValue.minute / 10;
			num[4] = TimeValue.minute % 10;
			num[5] = 11;
			num[6] = TimeValue.second / 10;
			num[7] = TimeValue.second % 10;
			break;
		}
		case Mode_D_T: //世界线时间显示模式
		{
			num[0] = (TimeValue.minute % 2);
			num[1] = 10;
			num[2] = TimeValue.hour / 10;
			num[3] = TimeValue.hour % 10;
			num[4] = TimeValue.minute / 10;
			num[5] = TimeValue.minute % 10;
			num[6] = TimeValue.second / 10;
			num[7] = TimeValue.second % 10;
			break;
		}
		case Mode_N_D: //普通日期显示模式
		{
			num[0] = TimeValue.year / 10 % 10;
			num[1] = TimeValue.year % 10;
			num[2] = 11;
			num[3] = TimeValue.month / 10;
			num[4] = TimeValue.month % 10;
			num[5] = 11;
			num[6] = TimeValue.date / 10;
			num[7] = TimeValue.date % 10;
			break;
		}
		case Mode_D_D: //世界线日期显示模式
		{
			num[0] = (TimeValue.date%2);
			num[1] = 10;
			num[2] = TimeValue.year / 10 % 10;
			num[3] = TimeValue.year % 10;
			num[4] = TimeValue.month / 10;
			num[5] = TimeValue.month % 10;
			num[6] = TimeValue.date / 10;
			num[7] = TimeValue.date % 10;
			break;
		}
		case Mode_TEM: //temp 模式
		{
			DS3231_Read_Temp(str);
			num[0] = 11;
			num[1] = 11;
			num[2] = 11;
			num[3] = str[0] - 0x30;
			num[4] = str[1] - 0x30;
			num[5] = 10;
			num[6] = str[3] - 0x30;
			num[7] = str[4] - 0x30;
			break;
		}
		case Mode_D_M: //世界线显示模式 参考 steins;gate
		{
			for(int i = 0;i < 8;i++)
			{
				num[i] = worldline[TimeValue.minute % 23 + 1][i];
			}
			break;
		}
		default:g_mode = Mode_N_T;break;
	}
	NixieTubeDisplay(num);
}

// nixie tube cathodic protection
void NixieFlash(void)
{
	static uint8_t cmd = 0;
	if(TimeValue.second == 0)
	{
		if(cmd == 0)
		{
			cmd = 1;
			NixieTubeFlash(5);
		}
	}
	else
	{
		cmd = 0;
	}
}

/*OLED Display ----------------------------------*/
// OLED Divergence Meter main information view
void OLED_MainInfoView(ModeTypedef mode)
{
	uint8_t str[30];  //display string temp
	uint8_t temp[20]; //temp string  
	uint8_t title[6][9] = 
	{
		{"Mode:N-T"},{"Mode:D-T"},
		{"Mode:N-D"},{"Mode:D-D"},
		{"Mode:TEM"},{"Mode:D-M"},
	};
	//mode display 
	OLED_ShowString(0,0,title[(uint8_t)g_mode],0);
	//time display
	sprintf((char *)str,"T>%02d:%02d:%02d ",TimeValue.hour,TimeValue.minute,TimeValue.second);
	OLED_ShowString(0,2,str,0);
	//date display
	sprintf((char *)str,"D>20%02d/%02d/%02d ",TimeValue.year,TimeValue.month,TimeValue.date);
	OLED_ShowString(0,4,str,0);
	//temp
	DS3231_Read_Temp(temp);
	sprintf((char *)str,"C>%sC   ",temp);
	OLED_ShowString(0,6,str,0);
	//key function display
	sprintf((char *)str,"Mode Reset Menu");
	OLED_ShowString(0,7,str,0);
}
/*key event -------------------------------------*/
//change golbla nixie dsiplay mode
void keyModeEvent(void)
{
	//set new display mode by modeKey
	if(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5)
	{
		g_mode = (g_mode + 1) % (Mode_D_M + 1);
		while(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5);
	}
	//display mode reset
	if(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5)
	{
		g_mode = Mode_N_T;
		while(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5);
	}
	//menu state
	if(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5)
	{
		g_state = Menu;
		while(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5);
		OLED_Clear();
	}
}
/*LED Hart attack ---------------------------------------------------*/
void LED_HartAttack(void)
{
	static uint8_t cmd = 0;
	if(TimeValue.second % 5 == 0)
	{
		if(cmd == 0)
		{
			cmd = 1;
			TIM3->CCR1 = 1000;
			HAL_Delay(50);
			TIM3->CCR1 = 0;
		}
	}
	else
	{
		cmd = 0;
	}
}

void LED_Breathing(void)
{
	static uint16_t pluse = 0;
	static uint16_t div = 0;
	static uint8_t  flag = 0;
	div++;
	if(div > 5)
	{
		div = 0;
		if(flag == 0)
		{
			pluse++;
			if(pluse > 1000)
			{
				flag = 1;
			}
		}
		else
		{
			pluse--;
			if(pluse < 2)
			{
				flag = 0;
			}
		}
		TIM3->CCR2 = pluse;
	}
}

/* intterrupt updater -----------------------------------------------*/
// time value structure data update
void timeValueUpdate(void)
{
	static uint16_t div = 0;
	div++;
	if(div > 1)
	{
		div = 0;
		Time_Handle();
	}
}
//golbal key event update to golbla data space 
void keyEventUpdater(void)
{
	static uint16_t div = 0;
	div++;
	if(div > 100)
	{
		div = 0;
		
		g_keyEvent.keymap = getKey();
		g_keyEvent.keytimes =  key_times(g_keyEvent.keymap);
	}
}
/*State Function Nodes Master ---------------------------------------*/
void stateMenu(void)
{
	static uint8_t cursor = 0;
	uint8_t str[3][11] = 
	{
		{"TimeConfig"},
		{"DateConfig"},
		{"Back      "},
	};
	//OLED Display
	OLED_ShowString(0,0,(uint8_t *)"Menu",0);
	for(int i = 0;i < 4;i++)
	{
		OLED_ShowString(14,i+2,(uint8_t *)str[i],0);
	}
	switch(cursor)
	{
		case 0:
		{
			OLED_ShowString(0,2,(uint8_t *)"->",0);
			OLED_ShowString(0,3,(uint8_t *)"  ",0);
			OLED_ShowString(0,4,(uint8_t *)"  ",0);
			break;
		}
		case 1:
		{
			OLED_ShowString(0,2,(uint8_t *)"  ",0);
			OLED_ShowString(0,3,(uint8_t *)"->",0);
			OLED_ShowString(0,4,(uint8_t *)"  ",0);
			break;
		}
		case 2:
		{
			OLED_ShowString(0,2,(uint8_t *)"  ",0);
			OLED_ShowString(0,3,(uint8_t *)"  ",0);
			OLED_ShowString(0,4,(uint8_t *)"->",0);
			break;
		}
		default:cursor =  0;break;
	}
	
	OLED_ShowString(0,7,(uint8_t *)"<-    ->     !",0);
	
	//key event
	if(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5)
	{
		cursor = (cursor + 1)%3;
		while(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5);
	}
	//display mode reset
	if(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5)
	{
		cursor = (cursor == 0)?2:(cursor - 1);
		while(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5);
	}
	//menu state
	if(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5)
	{
		while(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5);
		switch(cursor)
		{
			case 0:g_state = TimeConfig;break;
			case 1:g_state = DateConfig;break;
			case 2:g_state = Normal;break;
			default:break;
		}
		OLED_Clear();
		cursor = 0;
	}
}
void OLED_title(uint8_t page)
{
	switch(page)
	{
		case 0:OLED_ShowString(0,4,(uint8_t * )"    ^       ",0);OLED_ShowString(0,5,(uint8_t * )"  back    save",0);break;
		case 1:OLED_ShowString(0,4,(uint8_t * )"     ^      ",0);OLED_ShowString(0,5,(uint8_t * )"  back    save",0);break;
		case 2:OLED_ShowString(0,4,(uint8_t * )"       ^    ",0);OLED_ShowString(0,5,(uint8_t * )"  back    save",0);break;
		case 3:OLED_ShowString(0,4,(uint8_t * )"        ^   ",0);OLED_ShowString(0,5,(uint8_t * )"  back    save",0);break;
		case 4:OLED_ShowString(0,4,(uint8_t * )"          ^ ",0);OLED_ShowString(0,5,(uint8_t * )"  back    save",0);break;
		case 5:OLED_ShowString(0,4,(uint8_t * )"           ^",0);OLED_ShowString(0,5,(uint8_t * )"  back    save",0);break;
		case 6:OLED_ShowString(0,4,(uint8_t * )"            ",0);OLED_ShowString(0,5,(uint8_t * )"->back    save",0);break;
		case 7:OLED_ShowString(0,4,(uint8_t * )"            ",0);OLED_ShowString(0,5,(uint8_t * )"  back  ->save",0);break;
	}
}
void stateTimeConfig(void)
{
	static uint8_t cursor = 0;
	static uint8_t cmd = 0;
	static Time_Typedef timeVal;
	uint8_t str[30];
	
	//load time structure 
	if(cmd == 0)
	{
		cmd = 1;
		memcpy(&timeVal,&TimeValue,sizeof(Time_Typedef));
	}
	//OLED Display
	OLED_title(cursor);
	OLED_ShowString(0,0,(uint8_t *)"TimeConfig",0);
	sprintf((char*)str,"    %02d:%02d:%02d",timeVal.hour,timeVal.minute,timeVal.second);
	OLED_ShowString(0,3,(uint8_t *)str,0);

	OLED_ShowString(0,7,(uint8_t *)"->     +     !",15);
	
	//key event
	if(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5)
	{
		while(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5);
		cursor = (cursor + 1)%8;
	}
	//display mode reset
	if(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5)
	{
		while(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5);
		switch(cursor)
		{
				case 0:timeVal.hour = ((timeVal.hour/10) == 2)?timeVal.hour%10:timeVal.hour+10;break;
				case 1:timeVal.hour = ((timeVal.hour%10) == 9)?(timeVal.hour/10)*10:timeVal.hour+1;break;
				case 2:timeVal.minute = ((timeVal.minute/10) == 5)?timeVal.minute%10:timeVal.minute+10;break;
				case 3:timeVal.minute = ((timeVal.minute%10) == 9)?(timeVal.minute/10)*10:timeVal.minute+1;break;
				case 4:timeVal.second = ((timeVal.second/10) == 5)?timeVal.second%10:timeVal.second+10;break;
				case 5:timeVal.second = ((timeVal.second%10) == 9)?(timeVal.second/10)*10:timeVal.second+1;break;
		}
	}
	//menu state
	if(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5)
	{
		while(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5);
		if(cursor == 6 || cursor == 7)
		{
			switch(cursor)
			{
				case 6:g_state = Menu;break;
				case 7:g_state = Normal;DS3231_Time_Init(&timeVal);break;
			}
			OLED_Clear();
			cursor = 0;
			cmd = 0;
		}
	}
	
	// time value check
	if(timeVal.hour > 23)
	{
		timeVal.hour = 20;
	}
	if(timeVal.minute > 59)
	{
		timeVal.minute = 59;
	}
	if(timeVal.second > 59)
	{
		timeVal.second = 59;
	}
}
void stateDateConfig(void)
{
	static uint8_t cursor = 0;
	static uint8_t cmd = 0;
	static Time_Typedef timeVal;
	uint8_t str[30];
	
	//load time structure 
	if(cmd == 0)
	{
		cmd = 1;
		memcpy(&timeVal,&TimeValue,sizeof(Time_Typedef));
	}
	
	//OLED Display
	if(cursor > 1)
	{
		OLED_title(cursor + 1);
	}
	else
	{
		OLED_title(cursor);
	}
	
	OLED_ShowString(0,0,(uint8_t *)"DateConfig",0);
	sprintf((char*)str,"  20%02d:%02d:%02d",timeVal.year,timeVal.month,timeVal.date);
	OLED_ShowString(0,3,(uint8_t *)str,14);


	OLED_ShowString(0,7,(uint8_t *)"->     +     !",15);
	//key event
	if(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5)
	{
		while(g_keyEvent.keymap == 1 && g_keyEvent.keytimes < 5);
		cursor = (cursor + 1)%7;
	}
	//display mode reset
	if(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5)
	{
		while(g_keyEvent.keymap == 2 && g_keyEvent.keytimes < 5);
		switch(cursor)
		{
				case 0:timeVal.year = ((timeVal.year/10) == 9)?timeVal.year%10:timeVal.year+10;break;
				case 1:timeVal.year = ((timeVal.year%10) == 9)?(timeVal.year/10)*10:timeVal.year+1;break;
				case 2:timeVal.month = (timeVal.month < 12)? timeVal.month + 1:1;break;
				case 3:timeVal.date = ((timeVal.date/10) == 3)?timeVal.date%10:timeVal.date+10;break;
				case 4:timeVal.date = ((timeVal.date%10) == 9)?(timeVal.date/10)*10:timeVal.date+1;break;
		}
	}
	//menu state
	if(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5)
	{
		while(g_keyEvent.keymap == 3 && g_keyEvent.keytimes < 5);
		if(cursor == 5 || cursor == 6)
		{
			switch(cursor)
			{
				case 5:g_state = Menu;break;
				case 6:g_state = Normal;DS3231_Time_Init(&timeVal);break;
			}
			OLED_Clear();
			cursor = 0;
			cmd = 0;
		}
	}
	
	//date value check
	if(timeVal.date > 31)
	{
		timeVal.date = 31;
	}
	if(timeVal.month == 2 && timeVal.date > 29)
	{
		timeVal.date = 29;
	}
	if(timeVal.month == 4||timeVal.month == 6||timeVal.month == 9||timeVal.month == 11)
	{
		if(timeVal.date > 30)
		{
			timeVal.date = 30;
		}
	}
}
/*Setup , loop  Interrupt funcation ---------------------------------*/

void Nixie_Setup(void)
{
	OLED_DrawBMP(0,0,128,8,(uint8_t *)BMP_Start);
	HAL_Delay(1000);
	OLED_Clear();
	HV_Control(ENABLE);
	OLED_DrawBMP(0,0,128,8,(uint8_t *)BMP_For);
	NixieTubeTest(50);
	OLED_Clear();
	TIM3->CCR2 = 500;
}

void Nixie_Loop(void)
{
	//golbal state machine 
	switch(g_state)
	{
		case Normal://Nixie display mode 
		{
			//display
			OLED_MainInfoView(g_mode);
			//operation
			keyModeEvent();
			break;
		}
		case Menu:// menu
		{
			stateMenu();
			break;
		}
		case TimeConfig://time value setting
		{
			stateTimeConfig();
			break;
		}
		case DateConfig://date value setting
		{
			stateDateConfig();
			break;
		}
		default:g_state =  Normal;
	}
	
	NixieFlash();
	NixieDisplay(g_mode);
	timeValueUpdate();
	LED_HartAttack();
}

void Nixie_Interrupt(void)
{
	keyEventUpdater();
	LED_Breathing();
}