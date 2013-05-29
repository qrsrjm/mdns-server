/*
filename:mast_ip.c
time    :2011.10.9
author  :冀博
describe:测试mdns_socket和mdns_core中的试探包测试（mdns_socket中使用的是select()函数实现异步IO）
1.实现向多播地址224.0.0.251：5353发送包头数据接收数据，5次，
2.客户端接受后向其发送tigerjibo
说明：此试探包还没有实现授权端记录的封装
2011.10.9号测试成功
发送试探包进行试探
2011.10.10号
修改了底层函数mDNS_Recv,mDNS_Send,分别是修改了timeout,和增加了delaytime(延迟时间)
增加了随机函数
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>
#include "../mdns_socket/mdns_socket.h"
#include "../mdns_core/mdns.h"
#include "../mdns_core/mdns_core.h"
#define  LOCAL_PORT 		5353
#define  DEST_PORT 		5353
#define  DEST_IP_ADDR 		"224.0.0.251"

#define  LOOP                   0//回换许可 0：N； 1：Y
#define  MAUST_TTL              255 //路由跳数
#define  RE_ADDR                0  //重用地址许可 0：N；1：Y
#define  MCAST_INTERVAL   	1


int main(int argc,char **argv)
{
	#define DEBUG
	int sock_fd;
	struct sockaddr_in local_addr;
	struct sockaddr_in dest_addr;
	struct ip_mreq mreq;//指向多播结构体变量
	int 	ret;
        //动态的获取IP地址
	sock_fd = mDNS_Init(&local_addr,&dest_addr,LOCAL_PORT,DEST_PORT,SRC_IP,DEST_IP_ADDR);//自动获取本地IP地址，初始化套接子地址，绑定套接子

	if(sock_fd <0){
		#ifdef DEBUG
		printf("mDNS_Init() failed\n");
		#endif
		exit(1);
	} else {
		#ifdef DEBUG
		printf("mDNS_Init() sucessful\n");
		#endif
	}
	//加入多播组
	mreq = mDNS_Join_Group(sock_fd,SRC_IP,DEST_IP_ADDR,LOOP,RE_ADDR,IP_TTL);
	//试探唯一资源记录
	ret = mDNS_Probe(sock_fd,NAME,(struct sockaddr_in *)&dest_addr);
	if(2 == ret ){
		printf("probe sucessful:%d\n",ret);
	}else if(1 == ret){
		printf("probe conflict:%d\n",ret);//进行冲突解决，并将新的名字写入FILE_NAME文件和全局变量中（在声明的时候是从全局变量中取主机名，如果冲突后不写入全局变量则会出错）
	}else{
		printf("probe failed:%d\n",ret);
	}
	while(1){
		ret = mDNS_Announce(sock_fd,(struct sockaddr_in *)&dest_addr);
		if(!ret){
			printf("announce failed\n");
			break;
		}
	}	
	//退出多播组
	if(mDNS_Exit_Group(sock_fd,&mreq)<0) {
		printf("DOPR MEMBERSHIP failed %s\n",strerror(errno));
		exit(1);
	} else {
		printf("Exit Gruop\n");
	}
	return 0;
} 
