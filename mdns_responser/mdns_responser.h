#ifndef  MDNS_RESPONSER
#define  MDNS_RESPONSER
#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "../mdns_socket/mdns_socket.h"
#include "../mdns_core/mdns.h"
#include "../mdns_core/mdns_core.h"

/**********************************************************************************************************************
*                          全局变量(全局变量第一个字母大写)
**********************************************************************************************************************/
char  *SERVICE[SERVICE_NUM]={
			 	"http",	
				"lxi",
				"ni-rt",
				"ni",
				"webdav",
				"ni-sysapi",				
			};//SERVICE表示不能出现device-info,否则个数不对，ANNOUNCE_NUM不对
/**********************************************************************************************************************
*                          函数声明
**********************************************************************************************************************/
extern   uint8  mDNS_Announce(int sock_fd,struct sockaddr_in *addr);
extern   uint8  mDNS_Probe(int sock_fd,char *hostname,struct sockaddr_in *addr);
#endif
