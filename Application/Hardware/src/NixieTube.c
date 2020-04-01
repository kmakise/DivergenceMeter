#include "stm32f0xx_hal.h"
#include "NixieTube.h"
#include "main.h"
#include "stdio.h"
#include "stdlib.h"

//PA3 - NT DATA
//PA4 - NT CLK
//PA5 - NT OUT

#define NT_DATA(a) GPIOA->BSRR = DS_Pin << 16 * !a
#define NT_CLK(a)  GPIOA->BSRR = SCK_Pin << 16 * !a
#define NT_OUT(a)  GPIOA->BSRR = RCK_Pin << 16 * !a


void NixieTubeDrive(uint8_t * num)
{
	uint16_t i;
	for(i = 0;i < 88;i++)
	{
		//data level out
		NT_DATA(num[i]);
		//clock out
		NT_CLK(1);
		NT_CLK(0);
	}
	//out put
	NT_OUT(1);
	NT_OUT(0);
}

const uint8_t TUBE[8][12] = 
{ 
//  0   1  2  3  4  5  6  7  8  9 . NA
	{85,86,87,78,79,77,80,81,83,84,82,89,},//0
	{74,75,76,67,68,66,69,70,72,73,71,89,},//1
	{63,64,65,56,57,55,58,59,61,62,60,89,},//2
	{52,53,54,45,46,44,47,48,50,51,49,89,},//3
	{41,42,43,34,35,33,36,37,39,40,38,89,},//4
	{30,31,32,23,24,22,25,26,28,29,27,89,},//5
	{19,20,21,12,13,11,14,15,17,18,16,89,},//6
	{ 8, 9,10, 1, 2, 0, 3, 4, 6, 7, 5,89,},//7
};

uint8_t NixieTubeRunning = 1;

void NixieTubeDisplay(uint8_t * num)
{
	uint8_t buf[89] = { 
	//0 1 2 3 4 5 6 7 8 9 10
	//5 3 4 6 7 . 8 9 0 1 2
		0,0,0,0,0,0,0,0,0,0,0,//7
		0,0,0,0,0,0,0,0,0,0,0,//6
		0,0,0,0,0,0,0,0,0,0,0,//5
		0,0,0,0,0,0,0,0,0,0,0,//4
		0,0,0,0,0,0,0,0,0,0,0,//3
		0,0,0,0,0,0,0,0,0,0,0,//2
		0,0,0,0,0,0,0,0,0,0,0,//1
		0,0,0,0,0,0,0,0,0,0,0,//0
	};
	uint8_t i = 0;
	if(NixieTubeRunning)
	{
		for(i = 0;i < 8;i++)
		{
			buf[TUBE[i][num[i]]] = 1;
		}
	}
	NixieTubeDrive(buf);
}
void NixieTubeControl(uint8_t cmd)
{
	if(cmd == ENABLE)
	{
		NixieTubeRunning = 1;
	}
	else if(cmd == DISABLE)
	{
		NixieTubeRunning = 0;
	}
}

void NixieTubeTest(uint8_t delay)
{
	uint8_t num[8] = {11,11,11,11,11,11,11,11};
	for(int i = 0;i < 8;i++)
	{
		num[i] = 10;
		NixieTubeDisplay(num);
		HAL_Delay(delay);
	}
	for(int i = 0;i < 10;i++)
	{
		for(int j = 0;j < 8;j++)
		{
			num[j] = i;
		}
		NixieTubeDisplay(num);
		HAL_Delay(delay*2);
	}
	for(int i = 0;i < 8;i++)
	{
		num[i] = 11;
		NixieTubeDisplay(num);
		HAL_Delay(delay);
	}
}

void NixieTubeFlash(uint8_t delay)
{
	uint8_t num[8];
	
	for(int i = 0;i < 30;i++)
	{
		for(int j = 0;j < 8;j++)
		{
			num[j] = rand()%10;
		}
		NixieTubeDisplay(num);
		HAL_Delay(delay);
	}
}


