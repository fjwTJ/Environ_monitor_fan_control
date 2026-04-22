#include "oled_use.h"

uint8_t threshold_value = 40;


// OLED 上电后的启动动画界面
void OLED_INITSHOW(void)
{
	OLED_Init();
	OLED_CLS();
	OLED_ON();
	OLED_ShowChar(12+0,2,'w',16);
	OLED_ShowChar(12+8,2,'a',16);
	OLED_ShowChar(12+16,2,'i',16);
	OLED_ShowChar(12+24,2,'t',16);
	
	OLED_ShowChar(12+40,2,'a',16);
	
	OLED_ShowChar(12+56,2,'m',16);
	OLED_ShowChar(12+64,2,'o',16);
	OLED_ShowChar(12+64+8,2,'m',16);
	OLED_ShowChar(12+64+16,2,'e',16);
	OLED_ShowChar(12+64+24,2,'n',16);
	OLED_ShowChar(12+64+32,2,'t',16);
	
	HAL_Delay(100);
	OLED_ShowChar(20,5,'o',8);
	HAL_Delay(100);
	OLED_ShowChar(20+16,5,'o',8);
	HAL_Delay(100);
	OLED_ShowChar(20+32,5,'o',8);
	HAL_Delay(100);
	OLED_ShowChar(20+48,5,'o',8);
	HAL_Delay(100);
	OLED_ShowChar(20+64,5,'o',8);
	HAL_Delay(100);
	OLED_ShowChar(20+80,5,'o',8);
}

// OLED 信息页：显示固定身份信息
void OLED_INMATIONSHOW(void)
{
	OLED_ShowCN(16,0,0);
	OLED_ShowCN(16+16,0,1);
	OLED_ShowCN(16+16+16,0,2);
	OLED_ShowCN(16+16+16+16,0,3);
	OLED_ShowCN(16+16+16+16+16,0,4);
	OLED_ShowCN(16+16+16+16+16+16,0,5);
	
	OLED_ShowCN(8,3,6);
	OLED_ShowCN(8+16,3,7);
	OLED_ShowChar(8+16+16,3,':',16);
	OLED_ShowCN(16+16+16+16,3,25);
	OLED_ShowCN(16+16+16+16+16,3,26);
	OLED_ShowCN(16+16+16+16+16+16,3,27);
	
	OLED_ShowCN(8,6,8);
	OLED_ShowCN(8+16,6,9);
	OLED_ShowChar(8+16+16,6,':',16);
	OLED_ShowNum(8+16+16+16,6,20021105,8,16);
}

// OLED 实时数据页
void OLED_READSHOW(void)
{
	OLED_ShowCN(32,0,10);
	OLED_ShowCN(32+16,0,11);
	OLED_ShowCN(32+16+16,0,12);
	OLED_ShowCN(32+16+16+16,0,13);
	
	// 报警阈值
	OLED_ShowCN(0,3,29);
	OLED_ShowCN(16,3,30);
	OLED_ShowCN(16+16,3,31);
	OLED_ShowCN(16+16+16,3,32);
	OLED_ShowChar(16+16+16+16,3,':',16);
	OLED_ShowNum(16+16+16+16+8,3,00,2,16);
	
	// 气体浓度
	OLED_ShowCN(0,6,25);
	OLED_ShowCN(16,6,26);
	OLED_ShowCN(16+16,6,27);
	OLED_ShowCN(16+16+16,6,28);
	OLED_ShowChar(16+16+16+16,6,':',16);
	OLED_ShowNum(16+16+16+16+8,6,00,2,16);
	
}

