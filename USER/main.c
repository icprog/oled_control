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
		USART1_Config();
		LED_GPIO_Config();
		KEY_GPIO_Config();
    TIM2_Config();
    TIM3_Config();
		clear_screen();    //clear all dots 
		display_128x64(bmp1);
		clear_screen();    //clear all dots 
		display_GB2312_string(0,0,"我们");
		delay_ms(100);
    while(1){
			Execute_Host_Comm();
			
		}       
}




