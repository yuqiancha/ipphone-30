#include <AppDelay.h>
#include <AppUSART.h>
#include "MyUDP.h"
#include "driver_gpio.h"
#include "dil_phone.h"
#include "phone_process.h"

unsigned char res=1;
unsigned char ETH_PHONE_LISTEN=0;
unsigned char ETH_PHONE_LISTEN_RDY_FLAG=0;
unsigned char ETH_PHONE_REMOTE_RINGOFF_FLAG=0;
unsigned char ETH_PHONE_LOCAL_RINGOFF_FLAG=0;
unsigned char RETRY=0;

//普通电话拨打函数
void phone_call_function(void)
{
	//条件，FPGA检测到话音通道上有信号变化，打开通道，准备接听	
	JDQ2_ENABLE;//打开继电器2，接通话音通道
	//
}	
//普通电话接听函数
void phone_listen_function(void)
{
	SEL_BRCnN_RESET;//非广播模式
	PCM3_ETHnP_RESET;//普通电话模式
	ENABLE_RESET;
	delay_ms(1);
	ENABLE_SET;
}	
//网络电话拨打函数
void ETH_phone_call_function(void)
{
	//告知FPGA当先为网络电话
	//尝试连接
	//1、映射为ip地址，建立连接
//	if(ETH_PHONE_VALID_CONNECT_FLGA)//
//	{
//		ETH_PHONE_VALID_CONNECT_FLGA=0;//尝试建立连接
//		res=udp_connecttion_create();
//	}
//	res=1;
	//通知FPGA，设定工作方式
	SEL_BRCnN_RESET;//非广播模式
	PCM3_ETHnP_SET;//网络电话模式
	ENABLE_RESET;
	delay_ms(1);
	ENABLE_SET;
	
	while(res)//尝试连接,如果失败的话就重试5次
	{
		res=udp_connecttion_create();
		delay_ms(1000);
		RETRY++;
		if(RETRY>RETRY_TIMES){RETRY=0;break;} //连接失败
	}
	res=1;
	RETRY=0;
	while(UDP_CONNECT_TRY())//尝试链路,如果失败的话就重试5次
	{
		RETRY++;
		if((RETRY>RETRY_TIMES)||(HAND_ONnOFF_Value==0)){RETRY=0;break;} //连接失败
	}
	RETRY=0;
	while(UDP_PHONE_RDY_TRY())//询问是否准备好,如果失败的话就重试5次
	{
		RETRY++;
		if((RETRY>RETRY_TIMES)||(HAND_ONnOFF_Value==0)){RETRY=0;break;} //连接失败
	}
	while(HAND_ONnOFF_Value)//进入通话模式
	{
		////网络数据发送
		if(udp_demo_flag&1<<6)//是否收到数据,接收到话音数据,转发到FPGA
		{
			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
			if((udp_demo_recvbuf[0]==0x7e)&&(udp_demo_recvbuf[1]==0x7e)&&(udp_demo_recvbuf[2]==0x03)&&(udp_demo_recvbuf[3]==0x55))
			{
				//对方已挂机，出提示音，提示对方挂机
				;
			}
			else//对方没有挂断电话，则直接转发，可增加包头以识别报文类型
			{
				HAL_UART_Transmit_DMA(&huart2, (uint8_t *) udp_demo_recvbuf,200);	
			}
		}		
		if (USART2_RX_STA & 0x8000)
		{
			udp_pcm_senddata(udppcb);
			USART2_RX_STA = 0;
		}
	}
}	

//网络电话接听函数
void ETH_phone_listen_function(void)
{
	//检测到摘机后，发送一次准备好报文
	if(ETH_PHONE_LISTEN_RDY_FLAG)
	{
		ETH_PHONE_LISTEN_RDY_FLAG=0;
		ETH_PHONE_REMOTE_RINGOFF_FLAG=0;
		udp_send_buf[0]=0x7e;
		udp_send_buf[1]=0x7e;
		udp_send_buf[2]=0x02;
		udp_send_buf[3]=0xaa;//摘机后，回告，准备好
		udp_cmd_senddata(udppcb);
		delay_ms(100);//等待对方处理
	}
	else
	{
		//网络数据发送
		if(udp_demo_flag&1<<6)//是否收到话音数据，转发至fpga
		{
			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
			if((udp_demo_recvbuf[0]==0x7e)&&(udp_demo_recvbuf[1]==0x7e)&&(udp_demo_recvbuf[2]==0x03)&&(udp_demo_recvbuf[3]==0x55))//挂断
			{
				ETH_PHONE_LISTEN=0;
				//对方挂断标志
				ETH_PHONE_REMOTE_RINGOFF_FLAG=1;
			}
			else//拨打方没有挂断情况下，继续转发
			{
				HAL_UART_Transmit_DMA(&huart2, (uint8_t *) udp_demo_recvbuf,200);	
			}					
		}
		//话音数据
		if (USART2_RX_STA & 0x8000)//接收到话音数据，通过udp转发拨打方
		{
			udp_pcm_senddata(udppcb);
			USART2_RX_STA = 0;
		}
	}
}	

//本机挂断处理函数
void phone_local_ring_off(void)
{
	//if(PHONE_NUM_DIL_VALID_FLAG)
	{
		JDQ2_DISABLE;
		PCM2_MUSIC2_RESET;
		ETH_PHONE_LOCAL_RINGOFF_FLAG=0;
		ETH_PHONE_LISTEN=0;
		ETH_PHONE_REMOTE_RINGOFF_FLAG=0;
	}	
	
	//如果是网络电话，则发送网络报文
	if(ETH_phone_num_valid_flag||ETH_PHONE_NUM_DIL_VALID_FLAG)
	{
		ETH_phone_num_valid_flag=0;
		udp_send_buf[0]=0x7e;
		udp_send_buf[1]=0x7e;
		udp_send_buf[2]=0x03;
		udp_send_buf[3]=0x55;//挂断报文
		udp_cmd_senddata(udppcb);
	}
	delay_ms(100);//等待对方处理
}
//对方挂断处理函数
void phone_remote_ring_off(void)
{
	{
		JDQ2_DISABLE;
		JDQ8_DISABLE;
		PCM2_MUSIC2_SET;//嘟**嘟**嘟**
		ENABLE_RESET;
		delay_ms(1);
		ENABLE_SET;
	}	
	delay_ms(100);//等待对方处理
}

//UDP_PHONE_RDY_TRY初始化
//返回值:0,成功;
//    其他,失败
u8 phone_CALL_Create(void)
{
	//通知FPGA
	if(!HAND_ONnOFF_Value)//未摘机，即无人接听
	{
		SEL_BRCnN_RESET;
		PCM3_ETHnP_RESET;
		PCM1_MUSIC1_SET;//铃**铃**铃**
		ENABLE_RESET;
		delay_ms(1);
		ENABLE_SET;
		return 1;  //无人接听
	}
	else
	{
		SEL_BRCnN_RESET;
		PCM3_ETHnP_RESET;
		PCM1_MUSIC1_RESET;//停止播放，铃**铃**铃**
		ENABLE_RESET;
		delay_ms(1);
		ENABLE_SET;
		return 0;   //有人接听
	}
}

u8  ETH_phone_CALL_Create(void)
{
	//网络电话相关
	//通知FPGA
	if(!HAND_ONnOFF_Value)//未摘机，即无人接听
	{
		SEL_BRCnN_RESET;
		PCM3_ETHnP_RESET;
		PCM1_MUSIC1_SET;//铃**铃**铃**
		ENABLE_RESET;
		delay_ms(1);
		ENABLE_SET;
		ETH_PHONE_LISTEN=0;//
		ETH_PHONE_LISTEN_RDY_FLAG=0;
		return 1;  //无人接听
	}
	else
	{
		SEL_BRCnN_RESET;
		PCM3_ETHnP_RESET;
		PCM1_MUSIC1_RESET;//停止播放，铃**铃**铃**
		ENABLE_RESET;
		delay_ms(1);
		ENABLE_SET;
		ETH_PHONE_LISTEN=1;//
		ETH_PHONE_LISTEN_RDY_FLAG=1;
		return 0;   //有人接听
	}
	
//	
//	if(udp_demo_flag&1<<6)//是否收到数据
//	{				
//		udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
//		//1、确认报文内容，是否为链路测试报文
//		if((udp_demo_recvbuf[0]==0x7e)&&(udp_demo_recvbuf[1]==0x7e)&&(udp_demo_recvbuf[2]==0x01)&&(udp_demo_recvbuf[3]==0x55))//确认为对方呼叫
//		{
//			//回告链路好报文
//			//1、建立连接
//			
//			if(res==0)//建立通道，发送回告报文
//			{
//				//2、回送报文
//				udp_send_buf[0]=0x7e;
//				udp_send_buf[1]=0x7e;
//				udp_send_buf[2]=0x01;
//				udp_send_buf[3]=0xaa;
//				udp_cmd_senddata(udppcb);
//			}
//			else
//			{
//				res=udp_connecttion_create();
//			}	
//		}
//		//2、确认报文内容，是否为询问准备好报文
//		else if((udp_demo_recvbuf[0]==0x7e)&&(udp_demo_recvbuf[1]==0x7e)&&(udp_demo_recvbuf[2]==0x02)&&(udp_demo_recvbuf[3]==0x55))//，确认为对方呼叫
//		{
//			//回告为准备好报文，并向FPGA发出提示音使能指令，出提示音，外放喇叭响起，提示用户接听电话，摘机或按下免提键
//			//2、回送报文
//			udp_send_buf[0]=0x7e;
//			udp_send_buf[1]=0x7e;
//			udp_send_buf[2]=0x02;
//			udp_send_buf[3]=0x55;
//			udp_cmd_senddata(udppcb);
//			ETH_PHONE_LISTEN=1;//响铃，提示摘机
//			ETH_PHONE_LISTEN_RDY_FLAG=1;
//		}
//	}
}

void  ETH_phone_Idle_Scan(void)//网络空闲扫描
{
	//网络电话相关
	if(udp_demo_flag&1<<6)//是否收到数据
	{				
		udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
		//1、确认报文内容，是否为链路测试报文
		if((udp_demo_recvbuf[0]==0x7e)&&(udp_demo_recvbuf[1]==0x7e)&&(udp_demo_recvbuf[2]==0x01)&&(udp_demo_recvbuf[3]==0x55))//确认为对方呼叫
		{
			ETH_phone_num_valid_flag=1;//网络拨号有效标志
			//回告链路好报文
			//1、建立连接
			if(res==0)//建立通道后，发送链路ok回告报文
			{
				//2、回送报文
				udp_send_buf[0]=0x7e;
				udp_send_buf[1]=0x7e;
				udp_send_buf[2]=0x01;
				udp_send_buf[3]=0xaa;
				udp_cmd_senddata(udppcb);
			}
			else
			{
				res=udp_connecttion_create();//创建连接关系
			}	
		}
	}
}
	