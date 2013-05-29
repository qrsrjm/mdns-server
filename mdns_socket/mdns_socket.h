#ifndef mdns_init
#define mdns_init
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<net/if.h>
/**********************************************************************************************************************
*                          全局变量(全局变量第一个字母大写)
**********************************************************************************************************************/
/*
全局变量
char NAME[63]:用来存放主机名字符串,主机名最大为63字节，最多能存放21个汉字
char SRC_IP[16]:用来存放本地IP地址
char FILE_NAME[50]:用来存放设备名文件的文件名
*/
#define   MAX_DEVICENAME    63  //设备名最大不能超过单标签的长度
#define   MAX_FILE_NAME     50  //存放设备名字的文件名最大长度
#define   IPV4_LENGTH       16  //ipv4字符串最大长度为16字节 
char  NAME[MAX_DEVICENAME] ;
char  SRC_IP[IPV4_LENGTH];
char  FILE_NAME[MAX_FILE_NAME ];

/**********************************************************************************************************************
*                          函数声明
**********************************************************************************************************************/
extern int      mDNS_Get_IpAddr(char *ip_addr, int sock_fd);
extern int      mDNS_Get_DeName(void);
extern int  	mDNS_Init(struct sockaddr_in *local_addr,struct sockaddr_in *dest_addr,int local_port,int dest_port,char *src_ip,char *dest_ip);
extern struct 	ip_mreq  mDNS_Join_Group(int sock_fd,char *src_ip,char *mcast_ip,int loop,int re_add,int ip_ttl);
extern int 	mDNS_Send(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int addr_len,unsigned long delaytime);
extern int 	mDNS_Recv(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len,unsigned long timeout);
extern int  	mDNS_Exit_Group(int sock_fd,struct ip_mreq *mreq);
extern int  	mDNS_Close(int sock_fd);








#endif
