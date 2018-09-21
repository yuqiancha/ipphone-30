
#include "sys.h"
#include "MyUDP.h"
#include <AppDelay.h>
//////////////////////////////////////////////////////////////////////////////////	 
//IP phone
//GPIO �����������	   
//��������:2018-9-6 10:37:09
//��д��Wang Qingwu
//�汾��V1.0
//�޸ģ�
////////////////////////////////////////////////////////////////////////////////// 	

//����ķ�ʽ��ͨ��λ��������ʽ��ȡIO
//#define KEY0        PHin(3) //KEY0����PH3
//#define KEY1        PHin(2) //KEY1����PH2
//#define KEY2        PCin(13)//KEY2����PC13
//#define WK_UP       PAin(0) //WKUP����PA0


//����ķ�ʽ��ͨ��ֱ�Ӳ���HAL�⺯����ʽ��ȡIO
//#define KEY0        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3)  //KEY0����PH3
//#define KEY1        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2)  //KEY1����PH2
//#define KEY2        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) //KEY2����PC13
//#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP����PA0

#define DDD0        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)  //DDD0-PA8
#define DDD1        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)  //DDD1-PA11
#define DDD2        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12) //DDD2-PA12
#define DDD3       	HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_10)  //DDD3-PC10
#define DD_VAILD    HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)  //DDD3-PC111

#define DIL_VAILD    HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)  //DIL-PD8

#define nRESET_IP178_0 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)   ///PB0��1����ΪIP178��λ��
#define nRESET_IP178_1 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)   ///PB0��1����ΪIP178��λ��

#define KEY_LED1_ENABLE			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_RESET);		//PD0��0,��
#define KEY_LED1_DISABLE		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,GPIO_PIN_SET);			//PD0��1����

#define DEBUG_LED_ENABLE		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);	//PD13��0
#define DEBUG_LED_DISABLE		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);		//PD13��1
#define HT9200B_nCE_ENABLE	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET); 	//PD14��0
#define HT9200B_nCE_DISABLE	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET); 		//PD14��1

#define DTMFD0_RESET HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_RESET);	//PD15��0,DTMF/D0
#define DTMFD1_RESET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);	//PC6��1,DTMF/D1
#define DTMFD2_RESET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);	//PC7��1,DTMF/D2
#define DTMFD3_RESET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	//PC8��1,DTMF/D3

#define DTMFD0_SET HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,GPIO_PIN_SET);	//PD15��0,DTMF/D0
#define DTMFD1_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);	//PC6��1,DTMF/D1
#define DTMFD2_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);	//PC7��1,DTMF/D2
#define DTMFD3_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	//PC8��1,DTMF/D3

#define LCD_RS_0	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,GPIO_PIN_RESET);		//PE0��0
#define LCD_RS_1	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,GPIO_PIN_SET);			//PE0��1
#define LCD_RW_0	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_RESET);		//PE1��0
#define LCD_RW_1	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,GPIO_PIN_SET);			//PE1��1
#define LCD_E_0		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET);		//PE2��0
#define LCD_E_1		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET);			//PE2��1
#define LCD_D4_0	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET);		//PE3��0
#define LCD_D4_1	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET);			//PE3��1
#define LCD_D5_0	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);		//PE4��0
#define LCD_D5_1	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);			//PE4��1
#define LCD_D6_0	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET);		//PE5��0
#define LCD_D6_1	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);			//PE5��1
#define LCD_D7_0	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);		//PE6��0
#define LCD_D7_1	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET);			//PE6��1

#define JDQ1_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET);		//PE7��0
#define JDQ2_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_RESET);		//PE8��0
#define JDQ3_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_RESET);		//PE9��0
#define JDQ4_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_RESET);	//PE10��0
#define JDQ5_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,GPIO_PIN_RESET);	//PE11��0
#define JDQ6_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,GPIO_PIN_RESET);	//PE12��0
#define JDQ7_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_RESET);	//PE13��0
#define JDQ8_DISABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_RESET);	//PE14��0

#define JDQ1_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET);			//PE7��1
#define JDQ2_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_SET);			//PE8��1
#define JDQ3_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_SET);			//PE9��1
#define JDQ4_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_SET);		//PE10��1
#define JDQ5_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,GPIO_PIN_SET);		//PE11��1
#define JDQ6_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,GPIO_PIN_SET);		//PE12��1
#define JDQ7_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_SET);		//PE13��1
#define JDQ8_ENABLE	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_SET);		//PE14��1

///////////�������////////////////////////////
//����
#define K0_Value   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) //K0-PB9
#define K1_Value   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8) //K1-PB8
#define K2_Value   HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2) //K2-PC2
#define K3_Value   HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_3) //K3-PC3

//���
#define K8_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);		//PD2��0
#define K7_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);		//PD3��0
#define K6_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_RESET);		//PD4��0
#define K5_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);		//PD6��0
#define K4_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_RESET);		//PD7��0
#define K8_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);			//PD2��1
#define K7_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);			//PD3��1
#define K6_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,GPIO_PIN_SET);			//PD4��1
#define K5_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET);			//PD6��1
#define K4_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,GPIO_PIN_SET);			//PD7��1

//////���� ժ�����//
#define HAND_ONnOFF_Value   HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_1) //HAND_ONnOFF-PD1

/////�����FPGA,ͨ��ģʽѡ����ʾ��ѡ��ģʽѡ��/////
#define PCM3_ETHnP_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,GPIO_PIN_RESET);	//PCM3��0
#define PCM2_MUSIC2_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);	//PCM2��0
#define PCM1_MUSIC1_RESET		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);	//PCM1��0
#define SEL_BRCnN_RESET			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);	//MCU_BAKIO11��0
#define ENABLE_RESET				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);	//MCU_BAKIO12��0
	
#define PCM3_ETHnP_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,GPIO_PIN_SET);	//PCM3��1
#define PCM2_MUSIC2_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_SET);	//PCM2��1
#define PCM1_MUSIC1_SET			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET);	//PCM1��1
#define SEL_BRCnN_SET				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);	//MCU_BAKIO11��1 
#define ENABLE_SET					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);	//MCU_BAKIO12��1

void phone_num(void);
unsigned char Read44KeyValue(void);
void dil_phone_num(void);
void HT9200B_CTRL(u8 num);