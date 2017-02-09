#include"HeadType.h"	
#include "usart.h"
#include "TIM.h"

void delay_ms1(u16 ms)
{
	u16  i;
	u32  j;
	for(i=ms;i>0;i--)
	{
		for(j=5000;j>0;j--)
		;
	}
}
//配置时钟在target宏定义
int main(void)
{
		delay_init();
		initial_lcd();
		clear_screen(0x00);    //clear all dots
		AT24CXX_Init();
		USART1_Config();
		LED_GPIO_Config();
		KEY_GPIO_Config();
    TIM2_Config();
    TIM3_Config();	
		delay_ms(100);
    while(1){
			  dispose_key();
				if(Key_SetParamFlag == 0){
					Communication_Process();
				}else{
					dispose_menu();
				}

		}       
}




