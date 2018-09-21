#include <AppDelay.h>
#include <AppUSART.h>
#include "MyUDP.h"
#include "driver_gpio.h"
#include "dil_phone.h"

#define RETRY_TIMES 5
extern unsigned char ETH_PHONE_LISTEN;
extern unsigned char ETH_PHONE_LISTEN_RDY_FLAG;
extern unsigned char ETH_PHONE_REMOTE_RINGOFF_FLAG;
extern unsigned char ETH_PHONE_LOCAL_RINGOFF_FLAG;
extern unsigned char RETRY;
extern unsigned char res;

void phone_call_function(void);				//普通电话拨打函数
void phone_listen_function(void);			//普通电话接听函数
void ETH_phone_call_function(void);		//网络电话拨打函数
void ETH_phone_listen_function(void);	//网络电话接听函数
void phone_local_ring_off(void);			//本机挂断处理函数
void phone_remote_ring_off(void);			//远端挂断处理函数
u8  phone_CALL_Create(void);					//普通电话拨打准备
u8  ETH_phone_CALL_Create(void);			//网络电话拨打准备
void  ETH_phone_Idle_Scan(void);			//网络空闲扫描