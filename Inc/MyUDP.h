/*
 * MyUDP.h
 *
 *  Created on: Aug 21, 2018
 *      Author: super
 */

#ifndef MYAPP_INC_MYUDP_H_
#define MYAPP_INC_MYUDP_H_


#include <AppSYS.h>
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F4&F7开发板
//UDP 测试代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/2/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//*******************************************************************************
//修改信息
//无
//////////////////////////////////////////////////////////////////////////////////

#define UDP_DEMO_RX_BUFSIZE     2000    //定义udp最大接收数据长度
#define UDP_DEMO_PORT           8089    //定义udp连接的端口
#define UDP_DEMO_PORT_REMOTE    8080    //定义udp连接的端口

//lwip控制结构体
typedef struct
{
    u8 mac[6];      //MAC地址
    u8 remoteip[4]; //远端主机IP地址
    u8 ip[4];       //本机IP地址
    u8 netmask[4];  //子网掩码
    u8 gateway[4];  //默认网关的IP地址

    vu8 dhcpstatus; //dhcp状态
                    //0,未获取DHCP地址;
                    //1,进入DHCP获取状态
                    //2,成功获取DHCP地址
                    //0XFF,获取失败.
}__lwip_dev;
extern __lwip_dev lwipdev;  //lwip控制结构体
extern struct udp_pcb *udppcb;     //定义一个TCP服务器控制块
extern char udp_demo_flag;
extern u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区 
extern u8 udp_send_buf[10];	//UDP发送数据缓冲区 

uint8_t udp_Create_and_Bind_DEV(void);
uint8_t udp_connecttion_create(void);
void udp_set_default_remoteip(void);
void udp_demo_set_remoteip(void);
void udp_cmd_senddata(struct udp_pcb *upcb);
void udp_pcm_senddata(struct udp_pcb *upcb);
u8 UDP_CONNECT_TRY(void);
u8 UDP_PHONE_RDY_TRY(void);
uint8_t udp_demo_test(void);
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,ip_addr_t *addr,u16_t port);
void udp_demo_senddata(struct udp_pcb *upcb);

#endif /* MYAPP_INC_MYUDP_H_ */
