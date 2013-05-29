//那个函数出错后可以在里面添加#define DEBUG来查错
/*
2011.10.5
冀博
调用mdns_socket函数可实现：
创建套接字，初始化本地地址和目的地址以及完成绑定套接字，
加入一个指定多播组IP地址，设置多播 超时，回环许可，地址重利用，接收数据超时时间，
向多播地址发送数据，
从指定多播地址处接收数据，
从一个多播组地址中退出，
关闭套接字。
2011.10.6号修改
冀博
1.修改mDNS_Init()函数实现自动获取动本地IP地址，将本地IP地址（src_ip是全局变量）
2.添加int mDNS_Get_IPAddr(char *ip_addr, int sock_fd)函数实现自动获取IP地址
3.修改mDNS_Join_Group把接收超时时间用select()函数实现不用err = setsockopt(sock_fd,SOL_SOCKET,SO_RCVTIMEO,tv,sizeof(struct timeval));实现
4.修改int mDNS_Recv(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len)在中调用select()函数实现接收
超时
-----------------------------------------------------------------------------------------
2011.10.12号
冀博
修改了mDNS_Init()函数实现了从文件中获取主机名
测试成功
-----------------------------------------------------------------------------------------------
2011.10.12号修改了在mDNS_Init()函数用对文件进行操作后，对文件进行关闭
-------------------------------------------------------------------------
2011.11.3号修改了mDNS_Init()函数，将其中的获得主机名函数写成一个函数分离出mDNS_Init()函数用mDNS_Get_DeNam()函数中
*/
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
#include "mdns_socket.h"
extern char NAME[MAX_DEVICENAME];//全局变量，存放设备名
extern char FILE_NAME[50];//全局变量，存放设备名的文件名

/*
time :2011.10.4
author:冀博
 函数功能：根据传入的套接子符，查询（eth0和wlan0网卡IP地址）自动获得本地IP地址
1. 函数名 int mDNS_Get_IPAddr(char *ip_addr,int sock_fd)
2. 函数形参 :
char 	*ip_addr :指向字符串形式的IP地址。
int 	 sock_fd :套接字
3.函数返回值：
执行成功返回  0
执行错误返回 -1.	
测试成功
*/
int mDNS_Get_IpAddr(char *ip_addr, int sock_fd)
{
		
        #define  DEBUG_mDNS_Get_IPAddr
	char  ip_temp[16];
	int  err;	
	struct ifreq ifr;
	struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
	memset(ip_temp,0,16);
	memcpy(ifr.ifr_name,"eth0",sizeof("eth0"));//先查询eth0网卡，如果IP地址没有，则再获取无线网卡，若还没有则返回0
	err = ioctl(sock_fd,SIOCGIFADDR,&ifr);
	if (!err){
			inet_ntop(AF_INET,&sin->sin_addr.s_addr,ip_temp,16);	
			#ifdef   DEBUG_mDNS_Get_IPAddr
			printf("\n**************DEBUG_mDNS_Get_IPAddr**********************************\n");    	
			printf("eht0 ip=%s\n",ip_temp);
			printf("\n**************Exit DEBUG_mDNS_Get_IPAddr**********************************\n");
			#endif
	}else{
		memcpy(ifr.ifr_name,"wlan0",sizeof("wlan0"));
		err = ioctl(sock_fd,SIOCGIFADDR,&ifr);
		if(!err){			
			inet_ntop(AF_INET,&sin->sin_addr.s_addr,ip_temp,16);	
			#ifdef   DEBUG_mDNS_Get_IPAddr	
			printf("\n**************DEBUG_mDNS_Get_IPAddr**********************************\n");    			
			printf("wlan0 ip=%s\n",ip_temp);
			printf("\n**************Exit DEBUG_mDNS_Get_IPAddr**********************************\n");
			#endif	
		} else {		
			return -1;
		}
	}
	strcpy(ip_addr,ip_temp);
	return  0; 
	
}
/*
2011.10.31
冀博
函数完成功能:从FILE_NAME中获得设备主机名，并将其写入全局变量中,（如果FILE_NAME文件不存在，则建立一个FILE_NAME文件）
1.函数名称：int  mDNS_Get_DeName(void)
2.函数形参：void 
3.函数返回值：
函数成功返回0
函数执行失败返回-1.
-------------------------------------------------------------------------
2011.11.03.
修改了代开函数使用O_CREAT与O_EXCL结合使用可以编写容错的程序
测试成功
*/
//从文件name.conf中获取主机名
int  mDNS_Get_DeName(void)
{
	#define DEBUG_mDNS_Get_DeName	
	int fd;	
	char name[63]="demon";//初始化设备名tiger(如果文件不存在，则将初始化名字写入文件中)
	int read_num=0;
	int write_num=0;
	int name_len;
	
	strcpy(FILE_NAME,"/etc/hostname");//初始化存放设备文件名地址为name.conf
	
	fd = open(FILE_NAME,O_RDWR | O_CREAT | O_EXCL,S_IRWXU);//打开文件，如果文件不存在，则建立，如果文件存在，则重新打开	
	if( -1 == fd){ //文件已经存在
		#ifdef DEBUG_mDNS_Get_DeName	
		printf("\n**************DEBUG_mDNS_Get_DeName**********************************\n"); 	
		printf("File  %s Exist!,Reopen it\n",FILE_NAME);
		#endif
		fd = open(FILE_NAME,O_RDWR);//重新打开	
	} else {  //文件不存在，创建并打开
		#ifdef DEBUG_mDNS_Get_DeName	
		printf("\n**************DEBUG_mDNS_Get_DeName**********************************\n"); 		
		printf("Open file   %s success,fd=%d\n",FILE_NAME,fd);
		#endif
		
	}

	read_num = read(fd,NAME,63);//获取主机名并将其写入全局变量中
	if(read_num == 0){            //如果文件中数据不存在，则将初始化主机名写入文件和全局变量中
		name_len = strlen(name)+1;
		write_num = write(fd,name,name_len);
		if(write_num < 0){
			#ifdef DEBUG_mDNS_Get_DeName	
			printf("write num failed:%s\n",strerror(errno));
			printf("\n**************Exit DEBUG_mDNS_Get_DeName**********************************\n");
			#endif
			return -1;
		} else {
			strcpy(NAME,name);//并把初始化名字给全局变量
			#ifdef DEBUG_mDNS_Get_DeName	
			printf("write device name:%s\n",name);
			printf("\n**************Exit DEBUG_mDNS_Get_DeName**********************************\n");
			#endif
		}
		
	} else if (read_num <0){
		#ifdef DEBUG_mDNS_Get_DeName	
		printf("read failed:%s\n",strerror(errno));
		printf("\n**************Exit DEBUG_mDNS_Get_DeName**********************************\n");
		#endif
		return -1;
	} else{
		NAME[read_num] = '\0';		
		#ifdef DEBUG_mDNS_Get_DeName	
		printf("read_num=%d\t%s\n",read_num,NAME);
		printf("\n**************Exit DEBUG_mDNS_Get_DeName**********************************\n");
		#endif
	}
	close(fd);	
	return 0;
}
/*
2011.10.5
冀博
函数完成功能：创建套接字，初始化本地地址和目的地址以及完成绑定套接字
1.函数名称：int  mDNS_Init(struct sockaddr_in *local_addr,struct sockaddr_in *dest_addr,int local_port,int dest_port,char *src_ip,char *dest_ip)
2.函数形参：
1>struct sockaddr_in  *local_addr:存放本地套接字地址
2>struct sockaddr_in  *dest_addr:存放目的套接字地址
3>int   local_port:存放本地端口号
4>int   dest_port:存放目的端口号
5>char  *src_ip:存放源IP地址
6>char  *dest_ip:存放目的IP地址
3.函数返回值：
函数成功返回套接字描述符
函数执行失败返回-1.具体错误可以通过在函数内部定义#define DEBUG来查看错误信息
-------------------------------------------------------------------------
2011.10月12号：
添加了从name.conf文件中读取主机名存放在全局变量中（NAME）,如果文件不存在，则新建name.conf文件并把初始化主机名写入文件中
2011.10.14号
修该：添加了对name.conf文件操作完成后进行关闭
----------------------------------------------------------------------------
冀博
2011.11.3号修改了函数添加了mDNS_Get_DeName()函数
测试成功
*/
int  mDNS_Init(struct sockaddr_in *local_addr,struct sockaddr_in *dest_addr,int local_port,int dest_port,char *src_ip,char *dest_ip)
{
	#define DEBUG_mDNS_Init
	int sock_fd; 
	int option_value = 1;      
	if((sock_fd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
		#ifdef DEBUG_mDNS_Init
		printf("socket() failed: %s\n",strerror(errno));
		#endif
		return -1;
	} else {
		#ifdef DEBUG_mDNS_Init
		printf("socket() sucessful\n");
		#endif
	}
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&option_value,sizeof(int));
	//获得本地IP地址
	if(mDNS_Get_IpAddr(src_ip,sock_fd) < 0){
		#ifdef DEBUG_mDNS_Init	
		printf("IP Address is NULL\n");
		#endif
		return -1;
	}
	//从文件name.conf中获取主机名
	if(mDNS_Get_DeName() < 0 ){
		#ifdef DEBUG_mDNS_Init	
		printf("mDNS_Get_DeName() is failed\n");
		#endif
		return -1;
	}
       //初始化本地地址
        memset(local_addr,0,sizeof(struct sockaddr_in));
        local_addr->sin_family = AF_INET;
        local_addr->sin_addr.s_addr =  htonl(INADDR_ANY);//tes inet_addr(src_ip); 使用源IP地址，则接收不到多播地址(因为他直接收目的地址为自己IP地址的，其他不接受)
	local_addr->sin_port = htons(local_port);
	//初始化目的地址
	memset(dest_addr,0,sizeof(struct sockaddr_in));
	dest_addr->sin_family = AF_INET;
	dest_addr->sin_addr.s_addr = inet_addr(dest_ip);
	dest_addr->sin_port = htons(dest_port); 
	if (bind(sock_fd,(struct sockaddr *)local_addr,sizeof(struct sockaddr_in)) < 0 ){
		#ifdef  DEBUG_mDNS_Init
		printf("bind() failed: %s\n",strerror(errno));
		#endif
		return -1;
	} else {
		#ifdef  DEBUG_mDNS_Init
		printf("bind() sucessful\n");
		#endif
	}
	return sock_fd;
}
/*
2011.10.5
冀博
函数完成功能：加入一个多播组地址，并设置多播超时时间，回环许可，地址重用。
1.函数名称：struct ip_mreq  mDNS_Join_Group(int sock_fd,char *src_ip,char *mcast_ip,int loop,int re_addr,int ip_ttl)
2.函数形参：
  int   sock_fd:调用mDNS_Init()函数后返回的套接字描述符
  char  *src_ip:加入多播组的网络接口IP地址
  char  *mucast_ip:  加入的多播组IP地址
  int  loop:回环许可，参数loop设置为0表示禁止回送，设置为1表示允许回送
  int  re_addr:重用地址，参数re_addr设置为0表示禁止地址重用，设置为1表示允许地址重用
  int  mucast_ttl:设置多播超时TTL，范围为0~225，每过一个路由器，TTL值减1
3.函数返回值：
  返回加入成功的多播组结构体
  失败则直接返回
具体错误可以通过在函数内部定义#define DEBUG来查看错误信息
*/
struct ip_mreq  mDNS_Join_Group(int sock_fd,char *src_ip,char *mcast_ip,int loop,int re_addr,int ip_ttl)
{
	//#define   DEBUG_mDNS_Join_Group
	struct ip_mreq mreq;
        int err;

	mreq.imr_multiaddr.s_addr = inet_addr(mcast_ip);
	mreq.imr_interface.s_addr = inet_addr(src_ip);
        
	//加入多播组
	err = setsockopt(sock_fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
	if(err < 0){
		#ifndef  DEBUG_mDNS_Join_Group
		printf("%s setsockopt():IP ADD MEMBURSHIP\n",strerror(errno));
		#endif
		mDNS_Close(sock_fd);
		exit(1);
	} else {
		#ifdef  DEBUG_mDNS_Join_Group
		printf("setsockopt() IP ADD MEMBURSHIP sucessful\n");
		#endif
	}
        
	//设置多播TTL
	err = setsockopt(sock_fd,IPPROTO_IP,IP_MULTICAST_TTL,&ip_ttl,sizeof(ip_ttl));
	if (err  < 0){
		#ifndef DEBUG_mDNS_Join_Group
		printf("IP_MULTICAST_TTL failed:%s\n",strerror(errno));
		#endif
		mDNS_Close(sock_fd);
		exit(1);
	} else {
		#ifdef DEBUG_mDNS_Join_Group
		printf("IP_MULTICAST_TTL sucessful\n");
		#endif
	}
	
	//重用地址
	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &re_addr, sizeof(re_addr));
   	if ( err  < 0) {
        	#ifndef DEBUG_mDNS_Join_Group
		printf("SO_REUSEADDR failed: %s\n", strerror(errno));
        	#endif
		mDNS_Close(sock_fd);
		exit(1);
    	} else {
		#ifdef DEBUG_mDNS_Join_Group
		printf("SO_REUSEADDR sucessful\n");
		#endif
	}
	//回环许可
 	err = setsockopt(sock_fd,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop));
	if(err < 0){
		 #ifndef DEBUG_mDNS_Join_Group
		 printf("setsocket():IP MULTICAST_LOOP:%s\n",strerror(errno));
		 #endif
		 mDNS_Close(sock_fd);
		 exit(1);
	} else {
		#ifdef DEBUG_mDNS_Join_Group
		printf("IP_MULTICAST_LOOP SUCESSFUL\n");
		#endif
	}
	return mreq;
}
/*
2011.10.5
冀博
函数完成功能：向多播地址发送数据
1.函数名称：int mDNS_Send(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len)
2.函数形参：
sock_fd:套接字描述符
buff:发送缓冲区
buffer_size:待发送数据的长度
flags:控制选项，一般设置为0或取下面的值
struct  sockaddr  *addr:用于指定目的地址
int  *addr_len :目的地址的长度
3.函数返回值:
 return send_num    发送的实际字节数
----------------------------------------------------------------------------------------------------------------------
2011.10.10
冀博
函数完成功能：延迟delaytime毫秒，然后向多播地址发送数据。
1.函数名称：int mDNS_Send(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len,unsigned long delaytime)
2.函数形参：
sock_fd:套接字描述符
buff:发送缓冲区
buffer_size:待发送数据的长度
flags:控制选项，一般设置为0或取下面的值
struct  sockaddr  *addr:用于指定目的地址
int  *addr_len :目的地址的长度
unsigned long delaytime:延迟多久发送数据。如果为零表不延迟(单位是毫秒)
3.函数返回值:
 return send_num    发送的实际字节数
*/
int mDNS_Send(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int addr_len,unsigned long delaytime)
{
	int send_num;
	usleep((delaytime*1000));//unsleep()参数是微妙
	send_num = sendto(sock_fd,buff,buffer_size,0,(struct sockaddr *)addr,addr_len);
	#ifdef   DEBUG_mDNS_Send
	printf("****** DEBUG_mDNS_Send**********\n");
	printf("delaytime:%ld\tsend_num:%d\n",delaytime*1000,send_num);
	printf("******Exit DEBUG_mDNS_Send**********\n");
	#endif
	return send_num;
}
/*
2011.10.5
冀博
函数完成功能：从指定地址接收数据
1.函数名称：int mDNS_Recv(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len)
2.函数形参：
int  sock_fd:套接字描述符
buff:指向接收缓冲区，接收到的数据将放在这个指针所指向的内存空间。
buffer_size指定了缓冲区的大小。
flags:控制选项,一般设置为0或取以下值
struct sockadrr  *addr:保存了接收数据报的源地址。
int  *addr_len:参数addr_len在调用recvfrom前为参数addr的长度，调用recvfrom后将保存addr的实际大小
3.函数返回值:
 return recv_num    接收的实际字节数
——————————————————————————————————————————————————————————————————————————
2011年10月6号修改
冀博
修改接收函数，在函数内部实现接收超时间
1.int mDNS_Recv(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len，struct timeval *tv)
2.函数形参：
int  sock_fd:套接字描述符
buff:指向接收缓冲区，接收到的数据将放在这个指针所指向的内存空间。
buffer_size指定了缓冲区的大小。
flags:控制选项,一般设置为0或取以下值
struct sockadrr  *addr:保存了接收数据报的源地址。
int  *addr_len:参数addr_len在调用recvfrom前为参数addr的长度，调用recvfrom后将保存addr的实际大小
struct  timeval *tv:存放接收数据的超时时间
3.函数返回值
	 >0:表示接收到的实际字符个数
	 0：表示超时
	 -1：表示select()出错
	 -2:表示接收失败
------------------------------------------------------------------------------	
2010.10.10号修改：
冀博
修改接收函数，在函数内部实现接收超时间
1.int mDNS_Recv(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len，int timeout )
2.函数形参：
int  sock_fd:套接字描述符
buff:指向接收缓冲区，接收到的数据将放在这个指针所指向的内存空间。
buffer_size指定了缓冲区的大小。
flags:控制选项,一般设置为0或取以下值
struct sockadrr  *addr:保存了接收数据报的源地址。
int  *addr_len:参数addr_len在调用recvfrom前为参数addr的长度，调用recvfrom后将保存addr的实际大小
int timeout:存放接收数据的超时时间(单位是ms)
3.函数返回值
	 >0:表示接收到的实际字符个数
	 0：表示超时
	 -1：表示select()出错
	 -2:表示接收失败
*/
int mDNS_Recv(int sock_fd,char *buff,int buffer_size,int flag,struct sockaddr *addr,int *addr_len,unsigned long timeout)
{
	int recv_num;
	fd_set  rfds;
	int retval;
	struct  timeval tv;
	if(timeout > 1000){
		tv.tv_sec 	= timeout/1000;
		tv.tv_usec 	= (timeout%1000)*1000;
	} else	{
		tv.tv_sec = 0;
		tv.tv_usec = timeout*1000;
	}
	//把可读文件描述符结合清空		
	FD_ZERO(&rfds);
	//把套接子符添加到可读文件描述符集合中
	FD_SET(sock_fd,&rfds);
	retval = select(sock_fd+1,&rfds,NULL,NULL,&tv);
	if(retval == -1){
		#ifndef  DEBUG_mDNS_Recv
		printf("select error:%s\n",strerror(errno));
		#endif
		return -1;
	} else if(0 == retval ){
		#ifdef   DEBUG_mDNS_Recv
                printf("NO data receive\n");
		#endif
		return 0;
	} else{
		 recv_num = recvfrom(sock_fd,buff,buffer_size,flag,(struct sockaddr *)addr,addr_len);
		 if(recv_num >0){
				#ifdef   DEBUG_mDNS_Recv			
				printf("receive sucess\n");
				#endif	
				return  recv_num;
			}else {
				#ifndef   DEBUG_mDNS_Recv			
				printf("receive failed\n");
				#endif	
				return -2;
			}
	}    
}
/*
2011.10.5
冀博
函数完成功能：退出一个多播组地址
1.函数名称：int  mDNS_Exit_Group(int sock_fd,struct ip_mreq *mreq)
2.函数形参：
  int sock_fd:套接字描述符，调用mDNS_Init()函数产生
  struct ip_mreq *mreq:mreq指向多播组的接口，调用mDNS_Join_Group()函数返回的mreq
3.返回值：
  0 成功退出一个多播组地址
  -1 退出失败
*/
int  mDNS_Exit_Group(int sock_fd,struct ip_mreq *mreq)
{
	int err;

	err = setsockopt(sock_fd,IPPROTO_IP,IP_DROP_MEMBERSHIP,mreq,sizeof(struct ip_mreq));
	if (err  < 0){
			#ifndef DEBUG_mDNS_Exit_Group
			printf("%s setsockopt() IP_DROP_MEMBERSHIP failed\n",strerror(errno));
			#endif
			mDNS_Close(sock_fd);
			return -1;
		 } else{
		 	#ifdef DEBUG_mDNS_Exit_Group
			printf("setsockopt IP_DROP_MEMBERSHIP sucessful\n");
			#endif
			return 0;
		 }
}

/*
2011.10.5
冀博
函数完成功能：关闭套接字描述符
1.函数名称：int  mDNS_Close(int sock_fd)
2.形参：int sock_fd 一个已创建的套机字描述符
3.函数返回值：
0 成功关闭所创建的套接字描述符
-1 关闭套接字描述符失败
具体错误可以通过在函数内部定义#define DEBUG来查看错误信息
*/
int  mDNS_Close(int sock_fd)
{
	int err;

	err = close(sock_fd);
	if(err < 0 ){
		#ifndef DEBUG_mDNS_Close
		printf("mDNS_Close failed:%s\n",strerror(errno));
		#endif
		return -1;
	} else {
		#ifdef DEBUG_mDNS_Close
		printf("mDNS_Close sucessful\n");
		#endif
		return 0;
	}
}
