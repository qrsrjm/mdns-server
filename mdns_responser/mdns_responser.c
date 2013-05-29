
/**********************************************************************************************************************
*filename：mdns_responser.c
*time    :2011.10.16
*author  :冀博
*time:2011.10.16
*describe:mdns_responser.c提供了上层接口
*                  函数
**********************************************************************************************************************/
#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include "../mdns_socket/mdns_socket.h"
#include "../mdns_core/mdns.h"
#include "../mdns_core/mdns_core.h"
#include "mdns_responser.h"
extern  char NAME[MAX_DEVICENAME];//全局变量，用来存放主机名
extern  char SRC_IP[IPV4_LENGTH];//全局变量，用来存放源IP地址
extern  char FILE_NAME[MAX_FILE_NAME ];//全局变量,存放设备名字的文件名的长度
extern  char  *TEMP_SERVICE[SERVICE_NUM]; //全局变量，用来从mdns_responser文件中吧SERVICE的数值展示存起来，供mdns_core里面的文件使用
/*
函数个功能：根据传入的主机名进行试探域名和服务实体名（系统中服务只有http）进行试探，返回试探结果
1.函数名：	uin8  mDNS_Probe(int sock_fd,char *hostname,struct sockaddr_in *addr)
2.函数形参：	
1>int 			sock_fd		:套接字描述符
2>char  		*hostname	:主机名
3>struct sockaddr_in	*addr           :指向发送地址
3.函数返回值：
没有冲突返回2，有冲突返回1,错误返回0
4.time	：	2011.10.9
5.author:	冀博
2011.10.10号修改，填加了第一次发送前要随机延迟0～250ms,接收超时的时间参数为unsigned long,而不用结构体struct timeval了
-------------------------------------------------------------------------------------------------------
2011.10.16修该了返回数值
试探主机民
和所有服务实体名的SRV记录
------------------------------------------------------------
2011。11.6:自动完成对A地址记录的封装和所有SRV记录的封装(有多少个服务就有多少个SRV记录)
*/


uint8  mDNS_Probe(int sock_fd,char *hostname,struct sockaddr_in *addr)
{
	
//	#define DEBUG_mDNS_Probe
	uint8 	send_buf[MAX_DATA+12];
	uint8 	recv_buf[MAX_DATA+12];
	uint8 	probe_test[MAX_DATA+12];//mDNS报文最大长度为mDNS报文主体最大长度+12（mDNS包头长度）
	int 	send_num,recv_num,addr_len;//不能定义为无符号型
	uint32  length;
	uint32   i;
	uint8    j;
	uint32   delaytime;
	uint32   timeout = 250;//单位是毫秒
	char 		*name[MAX_PROBE_NUM];//PROBE_NUM=SERVICE_NUM+1,目前没有实现AAAA地址记录
	char		*service[MAX_PROBE_NUM]={MDNS_NULL};
	for(i = 0,j=0;i < PROBE_NUM;i++){
		name[i]=hostname;
		if(i>=1){
			service[i] = SERVICE[j++];
		}	
	}	
        addr_len =  sizeof(struct sockaddr_in);
	i=0;
	while(1){
		i++;
		if(i==4) break;
		#ifdef   DEBUG_mDNS_Probe
		printf("%dth begin send:\n",i);
		#endif
		if( i==1){
			length   =  mDNS_Probe_Packet(probe_test,name,service,PROBE_NUM,(mDNSClass_IN  | mDNSClass_UnicastResponse) );
			memcpy(send_buf,probe_test,length);	
			delaytime = mDNS_Rand(0,250);	
			#ifdef   DEBUG_mDNS_Probe
			printf("delaytime=%ld\n",delaytime);
			#endif
			send_num = mDNS_Send(sock_fd,send_buf,length,0,(struct sockaddr *)addr,addr_len,delaytime);//第一此延迟0～250ms
		}  else {
			length   =  mDNS_Probe_Packet(probe_test,name,service,PROBE_NUM,(mDNSClass_IN));
			memcpy(send_buf,probe_test,length);
			send_num = mDNS_Send(sock_fd,send_buf,length,0,(struct sockaddr *)addr,addr_len,0);//随后不延迟
		}
		if( send_num  < 0){
			#ifndef   DEBUG_mDNS_Probe
			printf("sendto() failed:%s\n",strerror(errno));
			#endif
			return 0;
		} else{
			#ifdef   DEBUG_mDNS_Probe			
			printf("***Num%dth send num=%d\n",i,send_num);
			int z;			
			for(z=0;z<send_num;z++){
			if(0 == z%5)printf("\n");
			printf("*send_buf[%d]=0x%x\t",z,send_buf[z]);
			}
			printf("\n");
			#endif
		}
		#ifdef   DEBUG_mDNS_Probe
		printf("%dth begin recv:\n",i);
		#endif
		recv_num = mDNS_Recv(sock_fd,recv_buf,sizeof(recv_buf),0,(struct sockaddr *)addr,&addr_len,timeout);
		if (0 == recv_num ){
			#ifdef   DEBUG_mDNS_Probe
			printf("No Data in time\n");
			#endif
			continue;
		} else if(-1 == recv_num || -2 == recv_num){
			#ifndef   DEBUG_mDNS_Probe
			printf("mDNS_Recv failed\n");
			#endif			
			return 0;
		} else if(recv_num > 0){
			#ifdef   DEBUG_mDNS_Probe			
			printf("***Num%dth reveive%s\trecve_num=%d*******\n",i,recv_buf,recv_num);
			#endif			
			break;
		}
		
	}
	if( i==4){
		return 2;
	} else {
		return 1;
	}
	
}
/*函数功能:完成对试探中验证过的唯一记录和共享记录进行声明。从name.conf中获取试探成功后的主机名，进行声明。
发送8此查询
1.函数名：	uint8  mDNS_Announce(int sock_fd,struct sockaddr_in *addr)
2.函数形参：	
1>int 			sock_fd		:套接字描述符
2>char  		*hostname	:主机名
3>struct sockaddr_in	*addr           :指向发送地址
3.函数返回值：
执行成功后，返回1.执行错误返回0.
4.time	：	2011.10.14
5.author:	冀博
2011.10.14号测试成功
------------------------------
2010.10.16修该了int 		send_num,read_num,addr_len;以前为uint32型
*/
//把SERVICE[]中的服务复制到TEMP_SERVICE[]中
uint8  mDNS_Announce(int sock_fd,struct sockaddr_in *addr)
{
//	#define  DEBUG_mDNS_Announce		
	uint8 		send_buf[MAX_DATA+12];	
	uint8   	announce_buf[MAX_DATA+12];
	int 		send_num,read_num,addr_len;//这个不能定义无符号型,因为返回值错误时，无符号不能表示出来
	uint32  	i;
	uint8   	count;//发送声明的次数
	uint32  	length;
	uint32  	delaytime;//发送延迟

	
	for(i=0;i<SERVICE_NUM;i++){//用来从mdns_responser文件中吧SERVICE的数值展示存起来，供mdns_core里面的文件使用
		TEMP_SERVICE[i] = SERVICE[i];
	}
	
        addr_len =  sizeof(struct sockaddr_in);
	count=1;
	delaytime = 1 ;//起始为1秒，以后间隔为指数次方
	while(count <= 5){ //等于8最后的几次间隔太长
		#ifndef  DEBUG_mDNS_Announce//不定义的时候七作用
		//fprintf(stderr, "Announcing ... %c           \r", rotdash());//实现发送声明的效果	
		#endif	
		#ifdef   DEBUG_mDNS_Announce
		printf("**********%dth Send Announce******** \n",count);
		printf("delaytime=%d\n",delaytime);
		for(i=0;i< SERVICE_NUM;i++){
			printf("service[%d]=%s\n",i,SERVICE[i]);
		}
		#endif
		length   =  mDNS_Announce_Packet(announce_buf,SERVICE);
		memcpy(send_buf,announce_buf,length);
		
		send_num = mDNS_Send(sock_fd,send_buf,length,0,(struct sockaddr *)addr,addr_len,delaytime*1000);//第一次延迟1秒
		if( send_num  < 0){
			#ifndef   DEBUG_mDNS_Announce
			printf("sendto() failed:%s\n",strerror(errno));
			#endif
			return 0;
		} else {
			#ifdef   DEBUG_mDNS_Announce	
			uint8 j;					
			printf("***Num%dth send num=%d\n",count,send_num);
			for(j = 0; j < send_num;j++){
				if(0 == j%5)printf("\n");
				printf("send_buf[%d]=0x%x\t",j,send_buf[j]);
			}
			printf("\n");
			#endif			
		}
		delaytime *= 2;
		count++;
	}
	return 1;
}

