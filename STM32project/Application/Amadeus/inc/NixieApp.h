#ifndef __NIXIEAPP_H
#define __NIXIEAPP_H

typedef enum
{
	Mode_N_T,				//普通时间显示模式
	Mode_D_T,				//世界线时间显示模式
	Mode_N_D,				//普通日期显示模式
	Mode_D_D,				//世界线日期显示模式
	Mode_TEM,				//温度显示模式
	Mode_D_M,				//世界线显示模式 参考 Steins;Gate
	
}ModeTypedef;

typedef enum
{
	Normal,
	Menu,						//主菜单
	TimeConfig,			//时间设置
	DateConfig,			//日期设置
	
}StateTypedef;

typedef struct
{
	volatile uint8_t keymap;
	volatile uint16_t keytimes;
}KeyTypedef;

void Nixie_Setup(void); //应用配置
void Nixie_Loop(void);	//应用轮询
void Nixie_Interrupt(void);//应用中断 每1ms一次

#endif /*__NIXIE_H*/
