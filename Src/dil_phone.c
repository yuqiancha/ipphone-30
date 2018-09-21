//////////////////////////////////////////////////////////////////////////////////	 
//IP phone
//phone 驱动代码	   
//创建日期:2018-9-14 10:05:31
//编写：Wang Qingwu
//版本：V1.0
//修改：
////////////////////////////////////////////////////////////////////////////////// 	
#include "driver_gpio.h"
#include "dil_phone.h"
#include "MyUDP.h"
#include <AppDelay.h>
#define KEYDEBOUNCE 0x05             //消抖动，按键扫描次数。如果连续5次都是扫描的都是相同键值，则认为是有效键值，否则是误触发
unsigned char phone_num_valid_flag=0;
unsigned char ETH_phone_num_valid_flag=0;
unsigned char PHONE_NUM_DIL_VALID_FLAG=0;
unsigned char ETH_PHONE_NUM_DIL_VALID_FLAG=0;
unsigned char ETH_PHONE_VALID_CONNECT_FLGA=0;
unsigned char num_dil[4];
unsigned char num_cnt=0;
unsigned char dil=0,flag=0;
unsigned char dil_num_dil[4];
unsigned char dil_num_cnt=0;
static unsigned char first_num_flag=0;
unsigned char ipaddr_last_byte=0;
///
unsigned char g_uiCurrKey;            //当前按键值
unsigned char g_uiLastKey;            //上次按键值
unsigned char g_uiKeyScanCount;       //按键扫描计数，作用：消抖动
unsigned char g_uiKeyDown;            //键被按下，返回的键值。       作用：单次按键，单次返回有效键值；按住不放，也只返回被按下的一个键值
unsigned char g_uiKeyRelease;         //键被释放后，返回的键值。     作用：只有按下的按键被释放后，才返回按下的键值
unsigned char g_uiKeyContinue;        //键连续按键，重复返回的键值。 作用：只要按住不放，就会重复地返回相同键值
unsigned char g_uiPreKeyValue;  
unsigned char LastRead_Vaild_Key=17;
void phone_num(void)
{	
	//电话号码采集
	dil=HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8);
	if(dil)
	{
		num_cnt=0;
		phone_num_valid_flag=0;
	}

	if(DD_VAILD)
	{
		if(flag==1)
		{
			if(num_cnt<4)
			{
				num_dil[num_cnt++]=DDD0+(DDD1<<1)+(DDD2<<2)+(DDD3<<3);
			}	
		}
		flag=0;
	}
	else
	{
		flag=1;
	}
	
	if(num_cnt==4)
	{
		if(num_dil[0]==2)
		{
			if((num_dil[1]==6)&&(num_dil[2]==7)&&(num_dil[3]==9))
			{
				phone_num_valid_flag=1;
				ETH_phone_num_valid_flag=0;
				JDQ2_ENABLE;//接通话音通道，不代表具体含义
				JDQ8_ENABLE;
				HAL_Delay(500);
				JDQ8_DISABLE;
				HAL_Delay(500);
				JDQ8_ENABLE;
				//JDQ1_ENABLE;
				HAL_Delay(500);
				JDQ8_DISABLE;
				num_cnt=0;
			}
			else{;}
		}
		else if(num_dil[0]==1)
		{
			phone_num_valid_flag=0;
			ETH_phone_num_valid_flag=1;
		}
	}
}

unsigned char Read44KeyValue(void)  //矩阵键盘的返回值，返回值对应相关功能，4*4扫描
{
	unsigned char  KeyValue = 17;  //KeyValue是最后返回的按键数值
	//将K4-K7全部输出为;
	K4_RESET;
	K5_RESET;
	K6_RESET;
	K7_RESET;
	if(((K0_Value+(K1_Value<<1)+(K2_Value<<2)+(K3_Value<<3))) == 0xf)  //若回采的值全都为1，如果没有按键按下，返回值为-1
	{
		return -1;
	}
	else  //有按键按下
	{
		delay_ms(20);  //延时5ms去抖动
		if(((K0_Value+(K1_Value<<1)+(K2_Value<<2)+(K3_Value<<3))) == 0xf)  //若回采的值全都为1，如果没有按键按下，返回值为-1
		{
			return -1;  //所以还是返回 -1
		}
	}
	
	K4_RESET;
	K5_SET;
	K6_SET;
	K7_SET;
	switch((K0_Value+(K1_Value<<1)+(K2_Value<<2)+(K3_Value<<3)))  //对应的 PG4 - PG7 的值进行判断，输出不同的按键值
	{
		case 0x07:KeyValue = 13;
		break;
		case 0x0b:KeyValue = 14;
		break;
		case 0x0d:KeyValue = 15;
		break;
		case 0x0e:KeyValue = 16;
		break;
	}
	K4_SET;
	K5_RESET;
	K6_SET;
	K7_SET;
	switch((K0_Value+(K1_Value<<1)+(K2_Value<<2)+(K3_Value<<3)))  //对应的 PG4 - PG7 的值进行判断，输出不同的按键值
	{
		case 0x07:KeyValue = 12;
		break;
		case 0x0b:KeyValue = 9;
		break;
		case 0x0d:KeyValue = 6;
		break;
		case 0x0e:KeyValue = 3;
		break; 
	}
	K4_SET;
	K5_SET;
	K6_RESET;
	K7_SET;
	switch((K0_Value+(K1_Value<<1)+(K2_Value<<2)+(K3_Value<<3)))  //对应的 PG4 - PG7 的值进行判断，输出不同的按键值
	{
		case 0x07:KeyValue = 11;//11
		break;
		case 0x0b:KeyValue = 8;
		break;
		case 0x0d:KeyValue = 5;
		break;
		case 0x0e:KeyValue = 2;
		break; 
	}
	K4_SET;
	K5_SET;
	K6_SET;
	K7_RESET;
	switch((K0_Value+(K1_Value<<1)+(K2_Value<<2)+(K3_Value<<3)))  //对应的 PG4 - PG7 的值进行判断，输出不同的按键值
	{
		case 0x07:KeyValue = 10;
		break;
		case 0x0b:KeyValue = 7;
		break;
		case 0x0d:KeyValue = 4;
		break;
		case 0x0e:KeyValue = 1;
		break; 
	}

	return KeyValue;
}

//P0口的低八位作为按键
//没有按键时，返回的是0xff，
void Int_Key_Scan(void)
{
	static unsigned char LastReadKey;        //上次从IO口读取的键值 ,注意是静态变量
	unsigned char CurrReadKey;               //当前从IO口读取的键值
//	unsigned char CurrReadKey_temp[5];
//	unsigned char cnt=0;
//---------------------- 需要修改的地方 --------------------------------------//
//	
//	if(g_uiKeyScanCount < KEYDEBOUNCE)
//	{
//		CurrReadKey_temp[KEYDEBOUNCE++] =Read44KeyValue();         //获取当前的键值   
//		delay_ms(5);
//	}
//	for(cnt=0;cnt<KEYDEBOUNCE;cnt++)
//	{
//		CurrReadKey_temp[cnt]=CurrReadKey_temp
//	}
//	
	
	CurrReadKey=Read44KeyValue();         //获取当前的键值  
//-----------------------------------------------------------------------//
	if((CurrReadKey>=0)&&(CurrReadKey<=16))
	{
		if(LastReadKey==0xff){
			g_uiKeyDown = 0x00;                    //有效按键按下	
			LastReadKey=CurrReadKey;
		}
		else
		{
			g_uiKeyDown = 0xff;                    //有效按键	
			LastRead_Vaild_Key=CurrReadKey;
		}
	}
	else                               //如果当前读取的键值与上次从IO口读取的键值不同，说明按键已经变化
	{
		g_uiKeyDown = 0xff;         //
		LastReadKey = CurrReadKey;			//
	}
	
	if(((LastRead_Vaild_Key>=0)&&(LastRead_Vaild_Key<=16))&&(CurrReadKey==0xff))
	{
		g_uiKeyRelease = 0x00;                    //按键释放
		
	}
	else
	{
		g_uiKeyRelease = 0xff;                    //按键没有释放
	}
}
//拨号采集，扫描矩阵键盘，获取 按键值
void dil_phone_num(void)
{	
	//电话号码采集
	unsigned char key_num=17;//非16个按键值
	Int_Key_Scan();
	if(g_uiKeyRelease==0x00)//按键释放时，记录按键值
	{	
		g_uiKeyRelease=0xff;
		key_num=LastRead_Vaild_Key;//Read44KeyValue();
		LastRead_Vaild_Key=17;
	}
	else
	{
		key_num=17;
	}
	
	if(((key_num>=0)&&(key_num<=9))){//按下号码为0-9
		if(first_num_flag==0){
			first_num_flag=1;
			dil_num_cnt=0;
			dil_num_dil[dil_num_cnt++]=key_num;
			HT9200B_CTRL(key_num);//拨号
		}
		else if(dil_num_cnt<4){
			dil_num_dil[dil_num_cnt++]=key_num;
			HT9200B_CTRL(key_num);//拨号
		}
	}
	
	if(dil_num_cnt==4)
	{
		first_num_flag=0;
		dil_num_cnt=0;
		if(dil_num_dil[0]==5)//如果第一个号码为0，则说明为普通电话
		{
			if((dil_num_dil[1]==6)&&(dil_num_dil[2]==7)&&(dil_num_dil[3]==8))//拨打对方号码
			{
				PHONE_NUM_DIL_VALID_FLAG=1;
				ETH_PHONE_NUM_DIL_VALID_FLAG=0;
			}
		}
		else
		{
			ipaddr_last_byte=dil_num_dil[1]*100+dil_num_dil[2]*10+dil_num_dil[3];
			if((ipaddr_last_byte>0)&&(ipaddr_last_byte<=255))//映射IP地址，准备发送
			{
				PHONE_NUM_DIL_VALID_FLAG=0;
				ETH_PHONE_NUM_DIL_VALID_FLAG=1;
				ETH_PHONE_VALID_CONNECT_FLGA=1;
			}
		}
	}
	//普通电话需接通JDQ8,同时将检测到的号码拨号到C1，D1线上。需要拨号模块
}

void HT9200B_CTRL(u8 num)
{
	switch (num)
	{
		case 1: DTMFD3_RESET;
						DTMFD2_RESET;
						DTMFD1_RESET;
						DTMFD0_SET;
						break;
		case 2: DTMFD3_RESET;
						DTMFD2_RESET;
						DTMFD1_SET;
						DTMFD0_RESET;
						break;
		case 3: DTMFD3_RESET;
						DTMFD2_RESET;
						DTMFD1_SET;
						DTMFD0_SET;
						break;
		case 4: DTMFD3_RESET;
						DTMFD2_SET;
						DTMFD1_RESET;
						DTMFD0_RESET;
						break;
		case 5: DTMFD3_RESET;
						DTMFD2_SET;
						DTMFD1_RESET;
						DTMFD0_SET;
						break;
		case 6: DTMFD3_RESET;
						DTMFD2_SET;
						DTMFD1_SET;
						DTMFD0_RESET;
						break;
		case 7: DTMFD3_RESET;
						DTMFD2_SET;
						DTMFD1_SET;
						DTMFD0_SET;
						break;
		case 8: DTMFD3_SET;
						DTMFD2_RESET;
						DTMFD1_RESET;
						DTMFD0_RESET;
						break;
		case 9: DTMFD3_SET;
						DTMFD2_RESET;
						DTMFD1_RESET;
						DTMFD0_SET;
						break;
		case 11: DTMFD3_SET;//0
						DTMFD2_RESET;
						DTMFD1_SET;
						DTMFD0_RESET;
						break;				
		case 10: DTMFD3_SET;//*
						DTMFD2_RESET;
						DTMFD1_SET;
						DTMFD0_SET;
						break;
		case 12: DTMFD3_SET;//#
						DTMFD2_SET;
						DTMFD1_RESET;
						DTMFD0_RESET;
						break;
		case 13: DTMFD3_SET;//A
						DTMFD2_SET;
						DTMFD1_RESET;
						DTMFD0_SET;
						break;
		case 14: DTMFD3_SET;//B
						DTMFD2_SET;
						DTMFD1_SET;
						DTMFD0_RESET;
						break;
		case 15: DTMFD3_SET;//C
						DTMFD2_SET;
						DTMFD1_SET;
						DTMFD0_SET;
						break;
		case 16: DTMFD3_RESET;//D
						DTMFD2_RESET;
						DTMFD1_RESET;
						DTMFD0_RESET;
						break;
		default:break;
	}
	HT9200B_nCE_ENABLE;
	delay_ms(10);//时间需调整
	HT9200B_nCE_DISABLE;
	delay_ms(10);//时间需调整
	HT9200B_nCE_ENABLE;
}



