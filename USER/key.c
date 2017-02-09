#include"HeadType.h"	

#if selectboard
#define	DEVICE1_KEY_IO					GPIO_Pin_3
#define	DEVICE1_KEY_PORT				GPIOA
#define	DEVICE1_KEY_RCC				  RCC_APB2Periph_GPIOA
#else
#define	DEVICE1_KEY_IO					GPIO_Pin_13
#define	DEVICE1_KEY_PORT				GPIOC
#define	DEVICE1_KEY_RCC				  RCC_APB2Periph_GPIOC
#endif 
#define READ_DEVICE1_KEY    		GPIO_ReadInputDataBit(DEVICE1_KEY_PORT,DEVICE1_KEY_IO) //返回的是一个字节，读的是一个位


#define KEY_SHORT_TIME 		    15
#define KEY_LONG_TIME			    100
#define KEY_LONGLONG_TIME			400
u8 Key_ScanNum;
u8 Key_SetParamFlag;
Menu_Option Menu;
u16  timeflag;
u8   AdrrOK_Flag;
u16  Menu_Exit_Time;
u32 Menu_Valid_Time = MENU_VALID_TIME;
//=============================================================================
//函数名称: KEY_GPIO_Config
//功能概要:LED灯引脚配置
//参数名称:无
//函数返回:无
//注意    :无
//=============================================================================
void KEY_GPIO_Config(void)
{	
	//定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure 
	GPIO_InitTypeDef  GPIO_InitStructure;
	//使能GPIOC的外设时钟
	RCC_APB2PeriphClockCmd(DEVICE1_KEY_RCC,ENABLE);
	//选择要用的GPIO引脚		
	GPIO_InitStructure.GPIO_Pin = DEVICE1_KEY_IO;
	///设置引脚模式为推免输出模式			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
	//设置引脚速度为50MHZ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	//调用库函数，初始化GPIO
	GPIO_Init(DEVICE1_KEY_PORT, &GPIO_InitStructure);

}

u8 Key_Scan(void)
{
	 u8 key_num;
	 static u8 key1_triggerstate;
	 static u16 key1_timercount;
	 static u8 longlongkey_flag = 0;
	  key_num = 0;
	 //key1
		if(READ_DEVICE1_KEY == READLOW){
			if(longlongkey_flag == 0){
				if(key1_triggerstate == 0){
					key1_triggerstate = 1;
				}else{
					key1_timercount++;
					if(key1_timercount >= KEY_LONGLONG_TIME){
						key_num |=0xFF;
						longlongkey_flag = 1;
				  }
				}
			}
		}else{
			if(longlongkey_flag == 1){
				longlongkey_flag = 0;
				key1_timercount = 0;
				key1_triggerstate =0;
			}else{
				if(key1_triggerstate == 1){
					if(key1_timercount <KEY_SHORT_TIME){
						key_num &=0xfe;
					}else if((key1_timercount >=KEY_SHORT_TIME)&&(key1_timercount <KEY_LONG_TIME)){
							key_num |=0x01;
					}else{
							key_num |=0x11;
					}
					key1_triggerstate =0;				
				}
				key1_timercount = 0;
			}
		}
	
	 return key_num;
}

void dispose_key(void )
{
	if(Key_ScanNum == 0xFF){
		if(Key_SetParamFlag == 0){
			Key_SetParamFlag = 1;
			clear_screen(0x00);
			Menu = MENU_READPARAM;
			Menu_Exit_Time = MENU_EXIT_TIME;
		}else{
			Menu = MENU_SAVEPARAM;
		}
		Key_ScanNum = 0;
	}
}

void dispose_menu(void)
{
	static u8 slaveaddrtemp1,slaveaddrtemp2;
	u8 slavedisplay[4];
	static u8 param_flash_flag = 0;
	switch(Menu){
	case MENU_RESERVE:break;
	case MENU_READPARAM: slaveaddrtemp1 = AT24CXX_ReadOneByte(0x00);slaveaddrtemp2 = AT24CXX_ReadOneByte(0x01);
			display_GB2312_string(4,0,"修改前设备地址：");
			display_GB2312_string(0,0,"请修改当前地址：");
		if((slaveaddrtemp1 == slaveaddrtemp2)&&(slaveaddrtemp1 < 128)){
//					slaveaddr=slaveaddrtemp1;
					slavedisplay[0] = slaveaddrtemp1/100+ 0x30;
					slavedisplay[1] = slaveaddrtemp1%100/10 + 0x30;
					slavedisplay[2] = slaveaddrtemp1%10 + 0x30;
					slavedisplay[3] = '\0';
					display_GB2312_string(2,48,slavedisplay);
					display_GB2312_string(6,48,slavedisplay);
		}else{
				slaveaddrtemp1=0;
				slavedisplay[0] = slavedisplay[1] = slavedisplay[2] ='0';
				slavedisplay[3] = '\0';
			  display_GB2312_string(2,48,slavedisplay);
				display_GB2312_string(6,48+8,"0");
		}
		Menu = MENU_SETPARAM;
		timeflag = 60;
		param_flash_flag = 0;
		break;
	case MENU_SETPARAM:
		if(Key_ScanNum > 0){
			if(Key_ScanNum == 0x01){
					slaveaddrtemp1++;
					if(slaveaddrtemp1 > 50){
						slaveaddrtemp1 = 0;
					}
				}else if(Key_ScanNum == 0x11){
						if(slaveaddrtemp1 > 0){
							slaveaddrtemp1--;
						}else{
							slaveaddrtemp1 = 50;
						}
				}
			slavedisplay[0] = slaveaddrtemp1/100+ 0x30;
			slavedisplay[1] = slaveaddrtemp1%100/10 + 0x30;
			slavedisplay[2] = slaveaddrtemp1%10 + 0x30;
			slavedisplay[3] = '\0';
			display_GB2312_string(2,48,slavedisplay);
			Key_ScanNum = 0;
			Menu_Exit_Time = MENU_EXIT_TIME;
		}
		if(timeflag == 0){
			if(param_flash_flag == 0){
				display_GB2312_string(2,48,slavedisplay);
				param_flash_flag = 1;
			}else{
				display_GB2312_string(2,48,"   ");
				param_flash_flag = 0;
			}
			 timeflag = 60;
		}
		if(Menu_Exit_Time == 0){//菜单设置无动作20秒后自动保存退出菜单
				Menu = MENU_SAVEPARAM;
		}
		break;
	case MENU_SAVEPARAM:
		 clear_screen(0x00);
		 if(slaveaddrtemp1 != 0){
				AT24CXX_WriteOneByte(0x00,slaveaddrtemp1);AT24CXX_WriteOneByte(0x01,slaveaddrtemp1);
				if((slaveaddrtemp1 == AT24CXX_ReadOneByte(0x00))&&(slaveaddrtemp1 == AT24CXX_ReadOneByte(0x01))){
					slaveaddr=slaveaddrtemp1;					
					display_GB2312_string(0,16,"地址设置完成");
					display_GB2312_string(2,0,"修改后设备地址：");
					display_GB2312_string(4,48,slavedisplay);
					delay_ms(1500);
					delay_ms(1500);
					delay_ms(1500);
					clear_screen(0xFF);
					AdrrOK_Flag = 1;
				}else{
					display_GB2312_string(0,16,"保存地址错误");
					display_GB2312_string(2,8,"请重新设置地址");
					display_GB2312_string(4,0,"如果错误一直出现");
					display_GB2312_string(6,8,"请更换存储芯片");
				}
		}else{
			AT24CXX_WriteOneByte(0x00,slaveaddrtemp1);AT24CXX_WriteOneByte(0x01,slaveaddrtemp1);
			display_GB2312_string(0,16,"地址设置错误");
			display_GB2312_string(2,16,"当前地址:0");
			display_GB2312_string(4,8,"该地址设备异常");
			display_GB2312_string(6,16,"1<=地址<=128");
		}
		Menu = MENU_END;
		break;
	case MENU_END:Key_SetParamFlag = 0; Menu = MENU_RESERVE;break;
	}
	
}




