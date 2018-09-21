/*
 * MyUDP.c
 *
 *  Created on: Aug 21, 2018
 *      Author: super
 */


#include <AppUSART.h>
#include "MyUDP.h"
#include "AppUSART.h"
#include <AppDelay.h>
#include "stdio.h"
#include "string.h"
#include "lwip/udp.h"
struct udp_pcb *udppcb;  	//定义一个TCP服务器控制块
//UDP接收数据缓冲区
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];   //UDP接收数据缓冲区
u8 udp_send_buf[10];	//UDP发送数据缓冲区 
__lwip_dev lwipdev;  //lwip控制结构体

//UDP发送数据内容
const char *tcp_demo_sendbuf="UDP demo! 2018-8-21 14:51:40";

//UDP 测试全局状态标记变量
//bit7:没有用到
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上;1,连接上了.
//bit4~0:保留
char udp_demo_flag;
//struct udp_pcb *udppcb;     //定义一个TCP服务器控制块

//设置远端IP地址
void udp_set_default_remoteip(void)
{
	//前三个IP保持和DHCP得到的IP一致
	lwipdev.remoteip[0]=192;
	lwipdev.remoteip[1]=168;
	lwipdev.remoteip[2]=1; 
	lwipdev.remoteip[3]=105; 

//	广播地址
//	lwipdev.remoteip[0]=255;
//	lwipdev.remoteip[1]=255;
//	lwipdev.remoteip[2]=255; 
//	lwipdev.remoteip[3]=255;
	
} 
//设置远端IP地址
void udp_demo_set_remoteip(void)
{
	//前三个IP保持和DHCP得到的IP一致
	lwipdev.remoteip[0]=192;
	lwipdev.remoteip[1]=168;
	lwipdev.remoteip[2]=1; 
	lwipdev.remoteip[3]=104; 
} 

////初始化，UDP绑定设备和端口号，本地ip地址///
uint8_t udp_Create_and_Bind_DEV(void)
{
	err_t err;
	u8 res=0;	
	udppcb=udp_new();//
	if(udppcb)//创建成功
	{
		err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//绑定本地IP地址与端口号
		if(err==ERR_OK) //绑定完成
		{
				udp_recv(udppcb,udp_demo_recv,NULL);//注册接收回调函数
				udp_demo_flag |= 1<<5;          //标记已经连接上
		}
		else
		{ 
			res=1;
		}
	}
	else{ 
		res=1;
	}
	return res;
}
////UDP连接关系创建///
uint8_t udp_connecttion_create(void)
{
	u8 res=0;	
	err_t err;
	ip_addr_t rmtipaddr;   //远端ip地址
	
	IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
	err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP客户端连接到指定IP地址和端口号的服务器
	if(err==ERR_OK)
	{
		res=0;
	}
	else
	{ 
		res=1;
	}
	return res;
}


//UDP发送短指令测试数据，最长为10个字节
void udp_cmd_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,4,PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(uint8_t*)udp_send_buf,4);//strlen((char*)USART1_RX_BUF)); //将tcp_demo_sendbuf中的数据打包进pbuf结构中
		udp_send(upcb,ptr); //udp发送数据
		pbuf_free(ptr);//释放内存
	}
}
 
//UDP_CONNECT_TRY初始化
//返回值:0,成功;
//    其他,失败
u8 UDP_CONNECT_TRY(void)
{      
	udp_send_buf[0]=0x7e;
	udp_send_buf[1]=0x7e;
	udp_send_buf[2]=0x01;
	udp_send_buf[3]=0x55;//测试链路连接情况
	udp_cmd_senddata(udppcb);
	delay_ms(100);
	
	if(udp_demo_flag&1<<6)//是否收到数据,接收到话音数据,转发到FPGA
	{
		udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
		if((udp_demo_recvbuf[0]==0x7e)&&(udp_demo_recvbuf[1]==0x7e)&&(udp_demo_recvbuf[2]==0x01)&&(udp_demo_recvbuf[3]==0xaa))
		{
			return 0;   //成功
		}
		else return 1;  //失败
	}
	else
	{
		return 1;  //失败
	}
}

//UDP_PHONE_RDY_TRY初始化
//返回值:0,成功;
//    其他,失败
u8 UDP_PHONE_RDY_TRY(void)
{      
	udp_send_buf[0]=0x7e;
	udp_send_buf[1]=0x7e;
	udp_send_buf[2]=0x02;//
	udp_send_buf[3]=0x55;//测试是否准备好
	udp_cmd_senddata(udppcb);
	delay_ms(100);
	
	if(udp_demo_flag&1<<6)//是否收到数据,接收到话音数据,转发到FPGA
	{
		udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
		if((udp_demo_recvbuf[0]==0x7e)&&(udp_demo_recvbuf[1]==0x7e)&&(udp_demo_recvbuf[2]==0x02)&&(udp_demo_recvbuf[3]==0xaa))
		{
			return 0;   //成功
		}
		else return 1;  //失败
	}
	else
	{
		return 1;  //失败
	}
}


//UDP测试
uint8_t udp_demo_test(void)
{
	err_t err;
	ip_addr_t rmtipaddr;   //远端ip地址
	ip_addr_t myipaddr;   //本地ip地址
	u8 res=0;
	udp_demo_set_remoteip();//设定远端IP
	udppcb=udp_new();
	if(udppcb)//创建成功
	{
			IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
			err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT_REMOTE);//UDP客户端连接到指定IP地址和端口号的服务器
			if(err==ERR_OK)
			{
					err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//绑定本地IP地址与端口号
					if(err==ERR_OK) //绑定完成
					{
							udp_recv(udppcb,udp_demo_recv,NULL);//注册接收回调函数
							udp_demo_flag |= 1<<5;          //标记已经连接上
					}else res=1;
			}else res=1;
	}else res=1;
	MX_LWIP_Process();
	return res;
}

//UDP回调函数
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,ip_addr_t *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL) //接收到不为空的数据时
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //数据接收缓冲区清零
		for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
		{
		//判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，如果大于
			//的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len))
			{
				memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//拷贝数据
			}
			else
			{
				memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			}
			data_len += q->len;
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出
		}
		upcb->remote_ip=*addr;              //记录远程主机的IP地址
		upcb->remote_port=port;             //记录远程主机的端口号
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff;      //IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1
		udp_demo_flag|=1<<6;    //标记接收到数据了
		pbuf_free(p);//释放内存
	}
	else
	{
		udp_disconnect(upcb);
		udp_demo_flag &= ~(1<<5);   //标记连接断开
	}
}

//UDP服务器发送数据
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,200,PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(uint8_t*)USART2_RX_BUF,200);//strlen((char*)USART1_RX_BUF)); //将tcp_demo_sendbuf中的数据打包进pbuf结构中
		udp_send(upcb,ptr); //udp发送数据
		pbuf_free(ptr);//释放内存
	}
}

//UDP发送pcm数据
void udp_pcm_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,USART2_REC_LEN/2,PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(uint8_t*)USART2_RX_BUF,USART2_REC_LEN/2);//strlen((char*)USART1_RX_BUF)); //将tcp_demo_sendbuf中的数据打包进pbuf结构中
		udp_send(upcb,ptr); //udp发送数据
		pbuf_free(ptr);//释放内存
	}
}

//关闭UDP连接
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb);
	udp_remove(upcb);           //断开UDP连接
	udp_demo_flag &= ~(1<<5);   //标记连接断开
}

