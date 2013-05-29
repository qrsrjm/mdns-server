/**********************************************************************************************************************
*filename：mDNSCore.c
*time    :2011.09.28
*author  :冀博
*modifytime:2011.09.28
*fiel     :mDNSCore.c中实现了mDNSCore的函数过程
*modifier:冀博
*2011.0.9.29发先bug，在linux中的gcc编译器int 为4个字节，long 为8个字节，在ADS下开发int 为2个字节，long为4个字节
**解决方法：自己定义结构体来解决2个字节，4字节
bug:主机字节序和网络字节序没注意（引起错误在mDNS_Header()函数中）
**解决方法调用htons函数将16字节的主机字节序转化为网络字节序再存入包中
bug:单标签中每个名字前面的长度为字符行长度而不是数字型长度。
**解决方法，将len用sprintf用%c方式输入即可
2011.9.30号
修改：
1>mDNS_Service_Name(DomainLabel name,DomainLabel  service,DomainLabel domain，uint32 *length)
添加uint32 *length参数，将名字长度返回。
2>DomainName  mDNS_Host_Name(DomainLabel s,DomainLabel d，uint32 *length)
添加uint32 *length参数，将名字长度返回。
测试成功

2011.10.1
author:冀博
添加了查询包结构体QuesScet和修改了查询记录QuesInfo
修改了封装查询记录函数
添加了封装查询记录段函数
----------------------------------------------
修改了查询信息结构体QuesInfo,在其中添加了name长度

2011 10.2
修改了查询信息结构体QuesInfo,在其中添加了name长度
修改了查询段结构体
添加了 DomainName  mDNS_Name（）函数对mDNS_Host_Name()和mNS_Service_Name()函数进行统一封装
修改了QuesInfo   mDNS_Ques_Info(char *name,char *service,uint16 type,uint16 class)根据传入的主机名和服务名，类型，类产生一条查询记录，
name,service可以为你个空（用“ ”字符串空格表示空），但两者不能同时为空，name为空表示服务域名，service为空表示主机域名，两者都不空表示服务实体名。
添加了QuesSect  mDNS_Ques_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)，根据名字数组，域名数组，类型数组
类数组，和问题记录数目产生一个问题记录段。
最新

2011.10.6号修改
int  mDNS_Header(mDNSHeader *h,uint8 qr,uint8 aa,uint8 tc,uint16 qunum,uint16 ansum,uint16 authnum,uint16 addnum)
返回报头长度
--------------------------------------------------------------------------------------------------------------------
冀博
2011.10.9号修改了 mDNS_Header（）函数，把return head_len从测试中放到了最后，增加了memcpy(test,(uint8 *)h,12);
增加了uint32  mDNS_Probe_Packet(uint8 *buf,char *name[],char *service[],uint16 probe_num,uint16 QU_QM）函数测试成功(没有授权段记录)
增加了uint8  mDNS_Porbe(int sock_fd,char *hostname,struct sockaddr_in *addr)
--------------------------------------------------------------------------------------------------------------------
冀博
2011.10.11号添加了
unsigned long mDNS_Rand(uint32 x,uint32 y)函数用来在x~y范围内产生一个随机数字
uint32 mDNS_A_Data(char *ip,mDNSv4Addr *ipv4)函数用来求A资源记录数据内容
uint32 mDNS_SRV_Data(char *name,char *service,uint16 priority,uint16 weight,SRVData *srvdata)函数用来求SRV记录数据
RRInfo mDNS_RR(char *name,char *service,uint16 type,uint16 class)函数用来封装RR记录，目前只实现了A记录和SRV记录
AuthSect  mDNS_Auth_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)函数实现了授权段记录封装
测试成功：2011.10.11
----------------------------------------------------------------------------------------------------------------------
2011.10.12号修改了mDNS_Probe_Packet(uint8 *buf,char *name[],char *service[],uint16 probe_num,uint16 QU_QM）添加授权端记录
增加了void append_name(char *name)用来解决名字冲突后，在名字后面追加一个数字来解决名冲突
2011.10.12号测试成功
----------------------------------------------------------------------------------------------------------------------
冀博：
2011.10.13.
修改了RRInfo mDNS_RR(char *name,char *service,uint16 type,uint16 class)函数用来封装RR记录，目前只实现了A记录和SRV记录和PTR记录
增加了void Ip_Reverse(char *ip, char *reverse_ip)转换IP地址(郭猛，何盟实现)
增加了uint16 Ip_Domain(char *ip,DomainName *reverse)实现求IP地址的域名
增加了DomainName  mDNS_Reverse_Name(char *name,uint16 *name_leng)实现了求PTR逆向IP地址域名
修该了DomainName  mDNS_Service_Name(char *name,char *service,uint8 TCP_UDP,uint16 *length)函数将
把if(name != NULL)改成if( strcmp(name ," ") != 0)
增加了uint32 mDNS_PTR_Data(char *name,char *service, DomainName *dname)求PTR资源记录数据
将AuthSect  mDNS_Auth_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)函数修改为
RR_Sect  mDNS_RR_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)
------------------------------------------------------------------------------------------------------------------------
冀博
2011.10.14(实现了声明)
增加了
uint32  mDNS_Announce_Packet(uint8 *buf,char *name[],char *service[],uint16 answ_num,uint16 addi_num)；
uint8  mDNS_Announce(int sock_fd,struct sockaddr_in *addr)；
修该uint32  mDNS_Probe_Packet(uint8 *buf,char *name[],char *service[],uint16 probe_num,uint16 QU_QM)
增加了uint16 mDNS_TXT_Data(char *name,char *service,TXTData *txtdata)(实现了简单的TXT记录)
修改了RRInfo mDNS_RR(char *name,char *service,uint16 type,uint16 class)实现了简单TXT记录
修改了uint32  mDNS_Announce_Packet(uint8 *buf,char *name[],char *service[],uint16 answ_num,uint16 addi_num)
中uint16          answ_class[MAX_RR_NUM];
uint16          addi_class[MAX_RR_NUM];
以前的宏是 MAX_PROB_NUM 
增加了rotdash(void)函数，实现发送声明时候（一种字符效果）在，uint8  mDNS_Announce(int sock_fd,struct sockaddr_in *addr)函数中的
循环部分调用rotdash()函数
-------------------------------
2011.10.16号修改了一些bug
将mDNS_Probe()和mDNS_Announce（）移动到mdns_responser.c中
2010.10.16
增加了uint8  Mult_Name(const char *name,DomainName *domain) 函数实现球过个含.字符串域名
修改了DomainName  mDNS_Reverse_Name(char *name,uint16 *name_leng)实现了求PTR逆向IP地址域名
去除了增加了uint16 Ip_Domain(char *ip,DomainName *reverse)实现求IP地址的域名
------------------------------------------------------------------------------------------------
2011.11.4号修改将最后一个((uint16) (rd & 15)))该为 ((uint16) (rcode & 15)))
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
#include "mdns.h"
#include "mdns_core.h"
#include "../mdns_socket/mdns_socket.h"
extern  char NAME[MAX_DEVICENAME];//全局变量，用来存放主机名
extern  char SRC_IP[IPV4_LENGTH];//全局变量，用来存放源IP地址
extern  char  *TEMP_SERVICE[SERVICE_NUM]; //全局变量，用来从mdns_responser文件中吧SERVICE的数值展示存起来，供mdns_core里面的文件使用

/*功能		：根据传入的标量X（Service_Tcp_Udp ：UDP为0，TCP为1）来返回响应的协议域名的第二子字段的单标签域名
 1.带参宏	：#define  SERVICE_TCP_UDP(X)   ((X) == TCP ? TCP_LABEL  : UDP_LABEL)   
 2.形参： 	
		X：为标量，为UDP或TCP
 3.函数返回 	：
		DomainLabel型的协议域名的第二子字段的单标签域名即_tcp,或_udp
 author: 冀博
 time:2011.9.30
*/

#define  SERVICE_TCP_UDP(X)   ((X) == TCP ? TCP_LABEL  : UDP_LABEL)          

/*功能		:实现封装mDNS报头中的Flags位
 1.带参宏： #define  mDNS_Flages(qr,opcode,aa,tc,rd,ra,z,ad,cd,rcode)  \
        (((uint16 ) !!qr << 15) |  \
         ((uint16) (opcode & 15) << 11) | \
         ((uint16) !!aa << 10) | \
         ((uint16) !!tc << 9) | \
         ((uint16) !!rd << 8) | \
         ((uint16) !!ra << 7) | \
         ((uint16) !!ad << 5) | \
         ((uint16) !!cd << 4) | \
         ((uint16) (rcode & 15)))
 2.形参    qr: 具体见mDNS概要设计文档            opcode: 具体见mDNS概要设计文档
	  aa: 具体见mDNS概要设计文档            tc: 具体见mDNS概要设计文档
	  rd: 具体见mDNS概要设计文档            ra: 具体见mDNS概要设计文档
	  z: 具体见mDNS概要设计文档             ad: 具体见mDNS概要设计文
	  cd: 具体见mDNS概要设计文档            rcode:具体见mDNS概要设计文档
 3.函数返回 
	  返回mDNS报文头部的标志字节（主机字节序）
 author: 冀博
 time:2011年9月28号测试成功
*/
#define  mDNS_Flages(qr,opcode,aa,tc,rd,ra,z,ad,cd,rcode)  \
        (((uint16 ) !!qr << 15) |  \
         ((uint16) (opcode & 15) << 11) | \
         ((uint16) !!aa << 10) | \
         ((uint16) !!tc << 9) | \
         ((uint16) !!rd << 8) | \
         ((uint16) !!ra << 7) | \
         ((uint16) !!ad << 5) | \
         ((uint16) !!cd << 4) | \
         ((uint16) (rcode & 15)))

/*
功能	：响应包中TTL的设定
1.带参宏  
	#define mDNS_RR_TTL(X) (((X) == mDNSType_A || (X) == mDNSType_AAAA || (X) == mDNSType_SRV) ? HOST_NAME_TTL  : STAND_TTL)
2.形参 
	x  : 资源记录类型
3.返回值  ：
	X资源记录类型的TTL
4.说明： 
HostNameTTL:名字为主机名的，或者数据断中记录为主机名的资源记录TTL为120S（例如：A，AAAAA，reverse-mapping PTR和SRV）
StandardTTL:其他资源记录生存周期为75分钟（PTR(dns-sd)）
PTR记录统一做为共享资源记录，在函数mDNS_RR()中，在做细分
author: 冀博
time :2011.9.28
2011.9.29测试成功
*/
#define mDNS_RR_TTL(X) (((X) == mDNSType_A || (X) == mDNSType_AAAA || (X) == mDNSType_SRV) ? HOST_NAME_TTL  : STAND_TTL)
/*
功能：  响应包中唯一资源记录和共享资源记录的确定
1.带参宏  
#define mDNS_RR_CLASS(X) (((X) == mDNSType_A || (X) == mDNSType_AAAA || (X) == mDNSType_SRV) ? (mDNSClass_IN | mDNSClass_UniqueRRset) :(mDNSClass_IN))
2.形参 
	x  : 资源记录类型
3.返回值  ：
	该资源记录类型对应的类
4.说明： 
PTR记录统一做为共享资源记录，在函数mDNS_RR()中，在做细分
author: 冀博
time :2011.10.14
*/
#define mDNS_RR_CLASS(X) (((X) == mDNSType_A || (X) == mDNSType_AAAA || (X) == mDNSType_SRV) ? (mDNSClass_IN | mDNSClass_UniqueRRset) :(mDNSClass_IN))
/*
函数功能		：函数执行后实现一种字符效果
1>函数名		：static char rotdash(void)
2>函数形参	：
3>函数执行后实现一种字符效果
author: 冀博
time:2011年10.14号
2011年10.14号测试成功
*/
char rotdash(void)
{
    static const char *rd = "/-\\|\\-";
    static const char*c = NULL;

    if (!c || !*c)
        c = rd;
    
    return *(c++);
}
/*
函数功能		：根据传入的的x和y，产生一个从X～Y的随机数
1>函数名		：unsigned long mDNS_Rand(uint32 x,uint32 y)
2>函数形参	：
		uint32 	x:表示范围的下限
		uint32 	y:表示范围的上限
3>函数执行后返回从x~y之间的随机数.
author: 冀博
time:2011年10.11号
2011年10.11号测试成功
通用函数
*/
unsigned long mDNS_Rand(uint32 x,uint32 y)
{
	time_t t;
	srand((unsigned )time(&t));	
	return (rand()%(y-x+1)+x);
}


/*函数名：int Find_String( char const *table[],char * desired_word )
**函数行参：
         char const *table[] :指向指针数组表
         char *desired_word  :指向字符串
**函数返回值：
         函数执行成功返回该会desired_word在table中的下标值。
	 若没有找到则返回-1.
**time: 2011.11.13
**author:冀博
**2011.11.16号测试成功
*/
int Find_String( char const *table[],char * desired_word )
{
	char const **kwp;

	for( kwp = table;*kwp != NULL;kwp++){
		/*
		**如果这个单词与我们所查找的单词匹配，返回它在表中的位置	
		*/
		if( 0 ==  strncasecmp( desired_word, *kwp, strlen(desired_word) ) ){
			return kwp - table;
		}
	}
	return -1;
}
/*
函数功能		：根据service返回其在service_txt_table中的标号
1>函数名		：int  Find_TXT_Service(char *service)
2>函数形参	：
		char   	*service:服务名
3>函数返回值	：
		函数执行成功后，返回其service对应的标号;
		如果没有找到则返回-1.
		如果，service为空，返回-2.
author: 冀博
time:2011年11.7
通用函数
说明：
		service_txt_table[]中服务的顺序必须和mdns.h文件中的 TXT_Service枚举变量中的标号一致
*/
int  Find_TXT_Service(char *service)
{	
	//#define  DEBUG_Find_TXT_Service	
	int  ret ;
	if(NULL == service){//如果为空指针，返回-1
		printf("**********IN_DEBUG_Find_TXT_Service**************\n");	
		printf("service pointer is NULL\n");		
		printf("**********Exit_DEBUG_Find_TXT_Service**************\n");
		return -2; 
	} 
       static   char const *service_txt_table[]={
				   "http",
				   "device-info",
				   "webdav",
				   "telnet",
				   "ftp",
				   "smtp",
				   "lxi",//以下服务都是LXI标准制定的
				   "vxi-11",
				   "scpi-raw",
				   "scpi-telnet",
				   "ni-rt",//以下是NI自己定义的服务
				   "ni",
                                   "ni-sysapi",
				   NULL,
				};
        ret = Find_String( service_txt_table,service );
	#ifdef DEBUG_Find_TXT_Service
	printf("**********IN_DEBUG_Find_TXT_Service**************\n");			
	if(ret != -1){	
		printf("%dth IN service_txt_table[] %s\n",ret,service_txt_table[ret]);
	} else {
		printf("NO Find %s IN service_txt_table[]\n",service);	
	}
	printf("**********Exit_DEBUG_Find_TXT_Service**************\n");
	#endif	
	return ret;

}
/*
函数功能		：根据传入的service，返回其相应service的端口号
1>函数名		：uint16 Find_Service_Port(char *service)
2>函数形参	：
char   *service:服务名
3>函数返回值：
	函数执行成功后返回service的端口号；
	函数执行错误返回0
author: 冀博
time:2011年11.7
通用函数
说明service_port_table[]中服务的顺序要和switch()中的标号顺序一致
*/
uint16 Find_Service_Port(char *service)
{
//	#define DEBUG_Find_Service_Port	
	int  ret;
	if(NULL == service){//如果为空指针，返回-1
		printf("**********In DEBUG_Find_Service_Port**************\n");	
		printf("service pointer is NULL\n");	
		printf("**********Exit_Debug_Find_Service_Port**************\n");	
		return 0; 
	} 	
        static   char const *service_port_table[]={
				   "http",
				//   "device-info",srv记录里面没有该服务(SRV记录没有此服务，此服务只出现在TXT记录里)
				   "webdav",
				   "telnet",
				   "ftp",
				   "smtp",
				   "lxi",//以下服务都是LXI标准制定的
				   "vxi-11",
				   "scpi-raw",
				   "scpi-telnet",
			       //  "ni-rt",//以下是NI自己定义的服务
			       //  "ni",
                               //  "ni-sysapi",				
				   NULL,//用“ ”表示结束
				};      
	ret = Find_String( service_port_table,service );
	#ifdef DEBUG_Find_Service_Port
	printf("**********In DEBUG_Find_Service_Port**************\n");			
	if(ret != -1){	
		printf("%dth  IN service_port_table[] %s\n",ret,service_port_table[ret]);
	} else {
		printf("NO Find %s IN service_port_table[]\n",service);	
		return 0;
	}
	printf("**********Exit_Debug_Find_Service_Port**************\n");
	#endif
 	switch (ret){
		case 0 ://http
			return  80;
		case 1 ://webdav
			return  80;
		case 2 ://telenet;
			return  23;
		case 3://ftp;
			return  21;
		case 4: //smtp
			return  25;
		case 5://lxi
			return  80;
		case 6://vxi-11
			return  111;
		case 7: //scpi-raw
			return 5025;
		case 8://scpi-telnet
			return 5024;
		default:
			return 0;
	}

}
/*
函数功能		：根据传入的协议名，返回该协议是TCP协议还是UDP协议
1>函数名		：uint8  Find_TCP_Service(char *service)
2>函数形参	：
		char            *service:指向协议名
3>函数返回值：
		返回该协议是TCP还是UDP协议
		若service为空则返回-2.
author: 冀博
time:2011年11.7号
通用函数
*/
//若service为TCP,则返回TCP的标量，否则返回UDP的标量
uint8  Find_TCP_Service(char *service)
{
//	#define  DEBUG_Find_TCP_Service	
	int  ret;	
	
	if(NULL == service){//如果为空指针，返回-1
		printf("**********In DEBUG_Find_TCP_Service**************\n");	
		printf("service pointer is NULL\n");		
		printf("**********Exit_Debug_Find_TCP_Service**************\n");
		return -2; 
	}  

	static   char const *service_tcp_table[]={
				   "http",
				   "device-info",
				   "webdav",
				   "telnet",
				   "ftp",
				   "smtp",
				   "lxi",//以下服务都是LXI标准制定的
				   "vxi-11",
				   "scpi-raw",
				   "scpi-telnet",
				   "ni-rt",//以下是NI自己定义的服务
				   "ni",
                                   "ni-sysapi",
				   NULL,//用“ ”表示结束
				};
	     
	ret = Find_String( service_tcp_table,service );
	if (-1 == ret){
		#ifdef DEBUG_Find_TCP_Service
		printf("**********In DEBUG_Find_TCP_Service**************\n");	
		printf("%s is  UDP\n",service);
		printf("**********Exit_Debug_Find_TCP_Service**************\n");
		#endif
		return UDP;
	} else	{
		#ifdef DEBUG_Find_TCP_Service
		printf("**********In DEBUG_Find_TCP_Service**************\n");	
		printf("%s is  TCP\n",service);
		printf("**********Exit_Debug_Find_TCP_Service**************\n");
		#endif
		return TCP;
	} 
	
}


/*
函数功能		：用来解决名字冲突后，在名字后面追加一个数字来解决名冲突
1>函数名		：void Append_Name(char *name)
2>函数形参	：
		char *name:用来存放主机名
3>函数返回值为void
author: 冀博
time :2011.10.12
2011.10.12测试成功
*/
void Append_Name(char *name)
{
	
	#define DEBUG_Append_Name
	uint8   len=0;

	len = strlen(name);

	#ifdef DEBUG_Append_Name
	printf("*******DEBUG_Append_Name***********\n");
	printf("before name:%s\n",name);
	printf("before len:%d\n",len);
	printf("name[%d]=%c\n",len-1,name[len-1]);
	#endif
	if(name[len-1] >= '0'&& name[len-1]<'9'){
		name[len-1]++;
	}else if('9' == name[len-1]){
		name[len-1]='1';
		name[len]='0';
		name[len+1]='\0';
	}else {
		name[len]='1';
		name[len+1]='\0';
	}
	len =strlen(name);
	#ifdef DEBUG_Append_Name
	printf("after len=%d, name=%s\n",len,name);
	printf("********Exit DEBUG_Append_Name*********************\n");
	#endif
}

/*
功能		:用来封装mDNS报文中的报头 
1. 函数名	：void    mDNS_Header(mDNSHeader *h,uint8 qr,uint8 aa,uint8 tc,uint16 qunum,uint16 ansum,uint16 authnum,uint16 addnum)
2. 函数形参：
  		qr:0表示查询1表示响应；
  		aa:查询包中为0，响应包中为1，
  		tc：截尾位，在响应包中为0，在查询包中为1表示还有后续的查询包，0表示没有后续的查询
  		qunum：问题记录个数
  		ansum：答案记录个数
  		authnum：授权记录个数
  		addnum：附加断记录个数
3. 函数返回值 	：函数返回mDNS报头长度
author: 冀博
time 2011年9月28号测试成功
     2011年9月29号发现bug:字节序不对，将传入的16字节转换为网络字节序后存如mDNS报文中。
切记：调用htons将主机字节序变为网络字节序号、
2011.10.6号修改
modifer:冀博
返回报头的长度
htons()函数在window中也有
*/
uint8  mDNS_Header(mDNSHeader *h,uint8 qr,uint8 aa,uint8 tc,uint16 qunum,uint16 ansum,uint16 authnum,uint16 addnum)
{

//	#define DEBUG_mDNS_Header	

	h->id = 0;
	h->flags = htons(mDNS_Flages(qr,0,aa,tc,0,0,0,0,0,0));
	h->qunum = htons(qunum) ;// 变成网络字节序号"" 
	h->ansum = htons(ansum) ;
	h->authnum =htons(authnum);
	h->addnum =htons(addnum);
        #ifdef  DEBUG_mDNS_Header
	uint8 test[12];//mDNS报头长度为12，因此测试包长度最大为12
	uint8  i;
	memset(test,0,sizeof(test));//初始化为0
	memcpy(test,(uint8 *)h,12);
	printf("*************************DEBUG mDNS_Header*****************************\n");		
	printf("id=0x%x\tqr=0x%x\taa=0x%x\ttc=0x%x\n",0,qr,aa,tc);
	printf("qunum=0x%x\tansnum=0x%x\tauthnum=0x%x\taddnum=0x%x\n",qunum,ansum,authnum,addnum);	
	for(i=0;i<12;i++){	
		if(0 == i%5){
			printf("\n");		
		}			
		printf("test[%d]=0x%x\t",i,test[i]);
	}
	printf("\n");
	printf("*************************Exit DEBUG mDNS_Header*****************************\n");	
	#endif
	return  12;
}
/*
函数功能		：将IP地址逆转
1>函数名		：void reverse(char *ip, char *reverse_ip)
2>函数形参	：
char *ip:用来存放IP
char *reverse_ip:存放转换后的IP地址
3>函数返回值为void
author: 郭猛，何盟
time :2011.10.13
2011.10.13测试成功（冀博测试）
*/
void Ip_Reverse(char *ip, char *reverse_ip)
{
		
  //     #define	 DEBUG_mDNS_Ip_Reverse	
	char  *p1, *p2;
	char  *tmp;
	int nLen;

	if ((NULL == ip) || (NULL == reverse_ip)){
        	perror("ip");
 	}
	p1 = ip + strlen(ip) - 1;
	p2 = p1;
	tmp = reverse_ip;

	while (p1 != ip){
        	if ('.' == *p1){
			nLen = p2 - p1;
			memcpy(tmp, p1 + 1, nLen);
			tmp += nLen;
			*tmp++ = '.';
			p1--;
			p2 = p1;
		}
		else{
			p1--;
  		}
	}

	if (*p1 == '.'){
		nLen = p2 - p1;
		memcpy(tmp, p1 + 1, nLen);
		tmp += nLen;
		*tmp++ = '.';
	}
 	else{
		nLen = p2 - p1 + 1;
		memcpy(tmp, p1, nLen);
		tmp += nLen;
	}
	*tmp = '\0';
	#ifdef DEBUG_mDNS_Ip_Reverse
	printf("*************************DEBUG mDNS_Ip_Reverse*****************************\n");	
	printf("before:%s\nafeter:%s\n",ip,reverse_ip);
	printf("*************************Exit DEBUG mDNS_Ip_Reverse*****************************\n");
	#endif
}
/*
功能		：将名字转换为域名
1. 函数名  	：uint16  Transer_Name_Domain(const char *name,DomainName *domain) 
2. 函数形参	：
   		const    char *name	:存放含多个点的字符串
   		DomainName *domain	:存放转换后的域名
3：函数返回值     :
		函数执行成功返回值为域名的长度；
		函数执行失败返回0
author: 冀博
time  :2011年11月5号
*/
uint16  Transer_Name_Domain(const char *name,DomainName *domain) 
{ 
	//#define  DEBUG_Transer_Name_Domain    	
	uint8 	len;
	uint16 	name_length;
	int i = 0;
	for (;;) {
        	len  = strcspn(name, ".");
        	if (len > MAX_LABEL)
        	    return 0;
		else if( 0 == len){
		    break;
		} else {
        		domain->name[i] = len;
			memcpy(&(domain->name[1+i]),name,len);
			name += len;
			i += len+1;
			if( !*name)
			break;//没有末尾的dot
			name++;//跨过.
			if(!*name)
			break;
		}
      }
      domain->name[i]='\0';  //域名最后一个\0.
      name_length = i+1;//包括最后一个\0
      #ifdef DEBUG_Transer_Name_Domain 
      printf("*************************DEBUG_Transer_Name_Domain*****************************\n");
      uint8  test[MAX_NAME+1];//域名长度最大为256（加上最后一个.）
      uint8  j;
      memset(test,0,sizeof(test));//初始化为0。
      memcpy(test,domain->name,name_length);
      printf("Before name:%s\n",name);
      printf("After Transer_Name_Domain:%s\n",domain->name);
      printf("name length:%d\n",name_length);
      for(j=0;j < name_length;j++){
			if( 0 == j%5 ){
			printf("\n");
			}		
			printf("test[%d]=0x%x\t",j,test[j]);
		}
      printf("\n");
      printf("*************************Exit DEBUG_Transer_Name_Domain****************************\n");
      #endif
      if(name_length > MAX_NAME){
		printf("Domain Name is Too Long\n");
		return 0;
      }
      return name_length;//算结尾0
}

/*功能		：将传入的名字，服务转换成域名
1.函数  	:DomainName  mDNS_Name(char *name,char *service,uint16 *length)
2.形惨  	：
		 char *name :名字字符串
		 char *servic :应用协议名字符串（例如：http,fttp等）
	 	 uint16 *length:存放域名的长度
3.函数返回	：
		函数执行成功返回域名，以及域名的长度（存放在形参*length中）
	  	函数执行错误，抛出异常
author: 冀博
time  :2011年10月2号添加
*/
DomainName  mDNS_Name(char *name,char *service,uint16 *length)
{
//    	#define DEBUG_mDNS_Name	
	char temp_name[MAX_NAME+1];
	DomainName temp_domain_name;	
	uint16 name_leng;
	char  temp_service[MAX_NAME+1];
	uint16  len;//用于测试域名长度
	memset(&temp_service,0,sizeof(temp_service));//初始化为0。
	memset(&temp_name,0,sizeof(DomainName));//初始化为0
	if(  (strcmp(service ,MDNS_NULL) == 0) && (strcmp(name ,MDNS_NULL) != 0) ){
		if(0 == strcasecmp(SRC_IP,name) ) {	
			char tmp_ip[16];	
			Ip_Reverse(name,tmp_ip);
			sprintf(temp_name,"%s%s",tmp_ip,REVE_LOCAL_LABEL);
		} else{	
			len = strlen(name);//单标签长度
			if(len >= MAX_LABEL){
				printf("name lable is too long\n");
				exit(1);
			}			
			sprintf(temp_name,"%s%s",name,DOMAIN_LABEL);			
		}	
	  }  else if ( strcmp(service ,MDNS_NULL) != 0 ){
			uint8  tcp_udp; 
			
			len = strlen(service);//单标签长度
			if(len >= MAX_LABEL){
					printf("name lable is too long\n");
					exit(1);
				}		
			tcp_udp = Find_TCP_Service(service);	
			if(tcp_udp < 0){
				printf("Find_TCP_Service is Error\n");
				exit(1);			
			}
	        	sprintf(temp_service,"_%s%s%s",service,SERVICE_TCP_UDP(tcp_udp),DOMAIN_LABEL); 
			if( strcmp(name ,MDNS_NULL) != 0 ){
				len = strlen(name);//单标签长度				
				if(len >= MAX_LABEL){
					printf("name lable is too long\n");
					exit(1);
				}	
				sprintf(temp_name,"%s.%s",name,temp_service); 
			 } else {
				strcpy(temp_name,temp_service);
			 }	
	}
	name_leng =  Transer_Name_Domain(temp_name,&temp_domain_name);
	if(0 == name_leng ){
		printf("Transer_Name_Domain is wrong\n");
		exit(1);
	}	
	(*length) = name_leng; 
	#ifdef DEBUG_mDNS_Name
	uint8  test[MAX_NAME+1];//域名长度最大为256（加上最后一个.）
	uint16 i;
	memset(test,0,sizeof(test));//初始化为0
	printf("*************************Debug mDNS_Name*****************************\n");
	printf("Before name=%s\nservice=%s\n",name,service);	
        printf("domain_name string:%s\n",temp_name);
	printf("domain name:%s\n",temp_domain_name.name);	
	memcpy(test,(temp_domain_name.name),name_leng);
	printf("name length:%d\n",name_leng);
	for(i=0;i < name_leng;i++){
			if( 0 == i%5){
			printf("\n");
			}		
			printf("test[%d]=0x%x\t",i,test[i]);
		}
	printf("\n");
	printf("*************************Exit Debug mDNS_Name*****************************\n");
	#endif	
	return temp_domain_name;
}
/*
  函数功能	：根据根据名字，服务名，类型，类产生一个问题记录	            
1.函数名		：QuesInfo   mDNS_Ques_Info(char *name,char *service,uint16 type,uint16 class)
2.函数形参	：
		  char *name		:存放主机名
  		  char *service 	:用来存放服务协议名。 
 		  uint16 type:          :存放各问题记录的资源记录类型
  		  uint16 class:          :对应名字和服务器的类是QU，还是QM
3.函数返回值	：
		  函数执行成功返回问题记录
		  函数执行错误，抛出异常
author 		:冀博
time   		:2011.10月1号测试成功
modifer		:冀博
time   		:2011.10.2好修改。
说明：
		如果service为NULL，则表示问题名为主机域名
		如果service不为NLLL，则表示问题名为服务实体名
		如果name为空，则表示为服务域名。
		name和service都为空则直接退出
*/
QuesInfo   mDNS_Ques_Info(char *name,char *service,uint16 type,uint16 class)
{
//	#define  DEBUG_mDNS_Ques_Info

	QuesInfo  	tmp_que;
        DomainName      ques_name;//名字域名
	uint16          name_leng=0;

	memset(&tmp_que,0,sizeof(QuesInfo));     //初始化为0  
	memset(&ques_name,0,sizeof(DomainName)); //初始化为0
	if( (strcmp(name,MDNS_NULL) == 0) && strcmp(service,MDNS_NULL) == 0){
		#ifdef  DEBUG_mDNS_Ques_Info
		printf("name and service parament\n");
		#endif		
		exit(1);	
	}
	ques_name = mDNS_Name(name,service,&name_leng);
	//memcpy(tmp_que.name.name,ques_name.name,name_leng);
        //结构体可以直接赋值，但类型要一致
	tmp_que.name  = ques_name;
	tmp_que.type  = htons(type);
	tmp_que.class = htons(class);
	tmp_que.name_leng = name_leng;//把问题记录名字长度存入结构提中
	#ifdef  DEBUG_mDNS_Ques_Info
	uint16  tmp_type=0,tmp_class=0;
	QuesInfo  *p=NULL;
	uint8 test[MAX_NAME+4];//一条问题记录的最大长度（名字最大长度+4个字节）
	uint8 i;
	
	memset(test,0,sizeof(test));//初始化为0。
	tmp_type = type;
	tmp_class = class;
	printf("*************************Debug mDNS_Ques_Info*****************************\n");
	printf("Before:name:%s\tservice:%s\n",name,service);
	printf("Question name:%s\n",ques_name.name);
	printf("TYPE = %#x \tCLASS = %#x\n",tmp_type,tmp_class);
	p = &tmp_que;
	memcpy(test,(uint8 *)&(p->name),p->name_leng);
	memcpy(&test[p->name_leng],(uint8 *)&(p->type),2);
	memcpy(&test[p->name_leng+2],(uint8 *)&(p->class),2);
	printf("namelength:%d\n",p->name_leng+4);
	for(i=0;i < (p->name_leng+4);i++){
		if( 0 == i%5){
		printf("\n");
		}		
		printf("test[%d]=0x%x\t",i,test[i]);
		
	}
	printf("\n");
	printf("*************************Exit Debug mDNS_Ques_Info*****************************\n");
	#endif
	return tmp_que;
	
}
/*
功能：根据名字数组，域名数组，类型数组类数组，和问题记录数目产生一个问题记录段
1.函数名：QuesSect  mDNS_Ques_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)
2.函数形参：
  char *name[]		:存放主机名数组
  char *service[]	:用来存放服务协议名。 
			name和service之间的对应关系：name[i]和service[i]要对对应。
			如 char *name[]={  		 char  *service[]={
			  		 "xnms",			     " ",			
			  		 "xnms",			     "http",
			    		  " ",				     "http",				
			                };                               };  
			 第一个对应主机域名，第二个对应服务实体域名，第三个对应服务域名(注用“ ”字符串表示空)	
  char class[]          :对应名字和服务器的类是QU，还是QM
  uint16   num          :对应报头中问题段记录个数
3:函数返回值		:
			函数执行成功返回问题记录段；
			函数执行错误，则抛出异常  
author :冀博
时间    ：2011年10月2号        
*/
QuesSect  mDNS_Ques_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)
{

//	  #define DEBUG_mDNS_Ques_Sect
	  QuesSect    temp_quesarry;
	  uint16  j;
		
          memset(&temp_quesarry,0,sizeof(QuesSect)); //初始化为0
	  for(j=0 ; j < num ;j++){
		temp_quesarry.ques[j] = mDNS_Ques_Info(name[j],service[j],type[j],class[j]);  //一般类为1即Internet		
			
	  }
	  temp_quesarry.num =num;
          #ifdef  DEBUG_mDNS_Ques_Sect	
	  uint32  i;//数据最长能会很大	
	  uint8  test[MAX_DATA]; //mDNS报文所能存放的最大主体报文长度。
	  uint32 length=0;//数据最长能会很大
	  memset(test,0,sizeof(test));//初始化为0。
	  printf("*************************Debug mDNS_Ques_Sect*****************************\n");
	  printf("Quest Num:%d\n",temp_quesarry.num);
	  printf("Ques Sect name && service\n");
	  for( i = 0; i< num; i++){
	  	printf("name[%d]=%s\tservice[%d]=%s\n",i,name[i],i,service[i]);
          }
          for(i=0,length=0;i<temp_quesarry.num;i++){
		if(length >= MAX_DATA){ //防止数组越界
			printf("mDNS_Ques_Sect Datao Too Long\n");
			exit(1);
		}
		memcpy(&test[length],(uint8 *)(temp_quesarry.ques[i].name.name),temp_quesarry.ques[i].name_leng);
		memcpy(&test[length+temp_quesarry.ques[i].name_leng],(uint8 *)&(temp_quesarry.ques[i].type),2);
		memcpy(&test[length+temp_quesarry.ques[i].name_leng+2],(uint8 *)&(temp_quesarry.ques[i].class),2);
	        length += (temp_quesarry.ques[i].name_leng+4); 
          }
	  if(length >= MAX_DATA+12){
		printf("mDNS_Ques_Sect Datao Too Long\n");
		exit(1);
	}	  
	 for(i=0;i < length;i++){
			if(0 == i%5){
				printf("\n");
			}
		printf("test[%d]=0x%x\t",i,test[i]);
		}
	   printf("\n");
	   printf("*************************Exit Debug_mDNS_Ques_Sect*****************************\n");
	   #endif
	   return  temp_quesarry;
		
}
/*
函数功能：根据传入的主机名,服务名，试探记录个数和 QU_QM组装试探包，并最终返回试探包的长度
QU_QM的值为  ( mDNSClass_IN  | mDNSClass_UnicastResponse)表示QU查询
	    ( mDNSClass_IN ) 表示QM查询    
1.函数名 ：uint32  mDNS_Probe_Packet(uint8 *buf,char *name[],char *service[],uint16 probe_num,uint16 QU_QM)
1>uint8 *buf:将试探包中的内容放到buf中
2> char *name[]		:存放主机名数组
   char *service[]	:用来存放服务协议名。 
			name和service之间的对应关系：name[i]和service[i]要对对应。
			如 char *name[]={  		 char  *service[]={
			  		 "xnms",			     " ",			
			  		 "xnms",			     "http",
			    		  " ",				     "http",				
			                };                               };  
			 第一个对应主机域名，第二个对应服务实体域名，第三个对应服务域名(注用“ ”字符串表示空)	 
3> uint16 probe_num:试探包中问题记录的个数和授权段中记录的个数（有几条试探记录，就有几条授权记录）
4>QU_QM:主要用来说明是是QU查询还是QM查询（第一次是QU查询随后都是QM查询）              
说明：
试探包主要有查询段，和授权段组成
1>查询段主要进行查询主机名(eg:tiger.local)，和服务实体名tiger._http._tcp.local)《服务实体名，有函数内部定义，不提供接口目前本系统只提供web服务》，
因此有两条查询记录
2>授权段主要有A地址记录，和AAAA地址记录。A和AAAA地址记录中的数据段数据应该从系统动态获得。（目前展时不实现IPV6因此只实现A地址记录的试探和一个http服务的试探）
授权段即是一个A地址记录和相应服务的SRV记录
3>在试探包中，查询记录的资源记录类型多为ANY。试探包中的问题记录段是一个
在试探包中的授权段中是是A地址记录，和SRV地址记录

 char            *name[] ={
				   "tigerjohn",
				   "tigerjohn",
				   "tigerjohn"
				 };
	char            *service[] = {
				    " ",//用“ ”表示空
				   "http",
				   "webdav",
				};
2011.10.9号测试成功（没有授权段记录）
------------------------------------------------
冀博
2011.10.12号添加了授权段记录
测试成功
2011.10.13号修改了数据结构
--------------------------------
2011.10.14号修改了一些小细节

-------------------------------------
2011.11.6号
1>自动完成对问题记录段中问题记录类型和问题记录类的封装；以及授权段中资源记录类型和资源记录类的封装
2>试探包中只有A地址记录和SRV地址记录，没有逆向地址映射PTR
3>试探包中，问题段记录个数和授权段记录个数相等
函数执行成功返回试探包的长度，错误返回0
*/
uint32  mDNS_Probe_Packet(uint8 *buf,char *name[],char *service[],uint16 probe_num,uint16 QU_QM)
{

//	#define          DEBUG_mDNS_Probe_Packet	
	QuesSect   	ques_sec,*p_qsec;  //ques_sec指向问题段
	RR_Sect         auth_sec,*p_auth;  //auth_sec指向授权段
	uint8           probe_buf[MAX_DATA];   //mDNS报文最大长度为MAX_DATA+12
	uint32   	i;
	uint32 		length;		//试探包总体长度
	uint32          probe_len;   	//报文主体长度
	
	uint32          head_len;      //包头长度
       	uint16 		ques_type[MAX_PROBE_NUM] ;//PROBE_NUM=SERVICE_NUM+2//最大为SERVICE+2；SERVICE+A+AAAA
	uint16          ques_class[MAX_PROBE_NUM]; //试探问题记录类
	uint16          auth_type[MAX_PROBE_NUM];// 
	uint16          auth_class[MAX_PROBE_NUM]; //试探包中的数据都是共享资源记录
	
	mDNSHeader  	header; //试探包头
	//定义试探包头内容
	uint8  		q_r= 0;  
	uint8  		aa=0;
	uint8  		tc=0;
        uint8  		ques_num,answ_num ,auth_num,addi_num;
	ques_num = probe_num;	
	answ_num = 0;
	auth_num = probe_num;
	addi_num = 0;

	for(i = 0;i < ques_num;i++){//封装试探包中的问题记录类型和问题记录类
		ques_type[i]  =  mDNSType_ANY;//试探包中问题记录类型都为ANY	
		ques_class[i] = QU_QM;//第一次为QU，随后为QM
	}
	for(i = 0 ;i< probe_num;i++){ //主机不为空，服务为空的为A地址类型
		if( (strcmp(name[i],MDNS_NULL) != 0) && strcmp(service[i],MDNS_NULL) == 0 ){
			auth_type[i] = mDNSType_A;	
		} else if( (strcmp(name[i],MDNS_NULL) != 0) && strcmp(service[i],MDNS_NULL) != 0 ){
			auth_type[i] = mDNSType_SRV;
		}  else {
			return 0;//错误
		}
		auth_class[i] = mDNSClass_IN;//试探时候，授权记录中的资源记录都为共享资源记录
		
	}
        //试探包头
	head_len    =  mDNS_Header(&header,q_r,aa,tc,ques_num,answ_num,auth_num,addi_num);
        //封装问题记录段内容
	ques_sec =  mDNS_Ques_Sect(name,service,ques_type,ques_class,ques_num);
	p_qsec = &ques_sec;
        for(i=0,length=0;i<p_qsec->num;i++){
		memcpy(&probe_buf[length],(uint8 *)(p_qsec->ques[i].name.name),p_qsec->ques[i].name_leng);
		memcpy(&probe_buf[length+p_qsec->ques[i].name_leng],(uint8 *)&(p_qsec->ques[i].type),2);
		memcpy(&probe_buf[length+p_qsec->ques[i].name_leng+2],(uint8 *)&(p_qsec->ques[i].class),2);
	        length += (p_qsec->ques[i].name_leng+4); 
        }
	//封装授权记录段内容
	auth_sec =  mDNS_RR_Sect(name,service,auth_type,auth_class,auth_num);
	p_auth = &auth_sec;
	for(i=0;i<p_auth->num;i++){
		memcpy(&probe_buf[length],(uint8 *)(p_auth->rr_sect[i].name.name),p_auth->rr_sect[i].name_leng);
		memcpy(&probe_buf[length+p_auth->rr_sect[i].name_leng],(uint8 *)&(p_auth->rr_sect[i].type),2);
		memcpy(&probe_buf[length+p_auth->rr_sect[i].name_leng+2],(uint8 *)&(p_auth->rr_sect[i].class),2);
		memcpy(&probe_buf[length+p_auth->rr_sect[i].name_leng+2+2],(uint8 *)&(p_auth->rr_sect[i].ttl),4);
		memcpy(&probe_buf[length+p_auth->rr_sect[i].name_leng+2+2+4],(uint8 *)&(p_auth->rr_sect[i].data_leng),2);
		memcpy(&probe_buf[length+p_auth->rr_sect[i].name_leng+2+2+4+2],(uint8 *)(p_auth->rr_sect[i].data.rrdata.data),htons(p_auth->rr_sect[i].data_leng));
	        length += (10+p_auth->rr_sect[i].name_leng+htons(p_auth->rr_sect[i].data_leng)); 
          }	  
	probe_len= length;
	
	memcpy(buf,(uint8 *)&header,head_len);
	memcpy(&buf[head_len],probe_buf,probe_len);
	length = probe_len+head_len;	
	#ifdef   DEBUG_mDNS_Probe_Packet		
	printf("*************************Debug mDNS_Probe*****************************\n");  
	printf("probe num:%d\n",probe_num);
	printf("probe class:0x%0x\n",QU_QM);	
	printf("name &&service:\n");
	for(i=0;i<probe_num;i++){
		printf("name[%d]=%s\tservice[%d]=%s\n",i,name[i],i,service[i]);
	}
	
	for(i=0;i < length;i++){
			if(0 == i%5){
				printf("\n");
			}
		printf("test[%d]=0x%x\t",i,buf[i]);
		}
	 printf("\n");
	 printf("*************************Exit Debug mDNS_Probe*****************************\n");
	 #endif
	return length;
}
/*函数功能		：根据name,service组成域名，type为资源记录类型，class用来确定为唯一资源记录和共享资源记录（在试探包是都是共享资源记录）
组成一条资源记录
1.函数名字		：RRInfo mDNS_RR(char *name,char *service,uint16 type,uint16 class)
2.函数形参		:
			1>char *name:用来存放实体名（可以为空,用" "表示空）
			2>char *service:用来存放服务名（可以为空，用“ ”表示空）
			若name不为空，service为空，表示主机域名
			若name不为空，service也不为空，则表示服务实体名
			若name空，service为空，表示服务名。
			但是两个不能同时为空
			3>uint16 type:表示资源记录类型
			4>uint16 class:表示资源记录类，最高
3.函数返回值 		:
			函数执行成功后返回一条RR资源记录；
			函数执行错误，抛出异常
4.time:2011.10.11
5.author:冀博
*/
RRInfo mDNS_RR(char *name,char *service,uint16 type,uint16 class)
{	
		
//	#define         DEBUG_mDNS_RR	

	RRInfo          tmp_info;

	RData 		rr_data;//存放A记录的数据
	uint16          name_leng=0;
        uint16          data_len;
	
	memset(&tmp_info,0,sizeof(RRInfo));   //初始化为0    
	memset(&rr_data,0,sizeof(RData));

	if( (strcmp(name,MDNS_NULL) == 0) && strcmp(service,MDNS_NULL) == 0){//判断参数是否正确，即名字和服务不能同时为空
		#ifdef  DEBUG_mDNS_RR
		printf("name and service parament\n");
		#endif		
		exit(1);	
	}
	tmp_info.name = mDNS_Name(name,service,&name_leng);//求资源记录名字段
	tmp_info.name_leng = name_leng;//名字长度
	tmp_info.type = htons(type);   //封装type
	tmp_info.class = htons(class);//PTR的逆向地址映射的CLASS在传入时指定。
	//若为PTR中的逆向映射，TTL为120S，class为唯一资源记录
	if( (type == mDNSType_PTR) && (0 == strcasecmp(SRC_IP,name)) &&(0 == strcasecmp(MDNS_NULL,service)) ){
			  
		tmp_info.ttl  = htonl( HOST_NAME_TTL);//逆向地址PTR的TTL是HOST_NAME_TTL	  
	} else {
	
		tmp_info.ttl  = htonl(mDNS_RR_TTL(type));//封装TTL
	}
	switch(type){
		case mDNSType_A :
				#ifdef DEBUG_mDNS_RR
				printf("*************************DEBUG mDNS_AA_RR*****************************\n");  
				#endif
				data_len = mDNS_A_Data(SRC_IP,&(rr_data.rrdata.ipv4));//求A资源记录
				break;
		case mDNSType_AAAA:
				printf("Not Implement mDNSType_AAAA\n");
				//data_len = mDNS_AAAA_Data(SRC_IPV6,&(rr_data.rrdata.ipv6));//求AAAA资源记录
				exit(1);
				break;
	        case mDNSType_PTR:
				if( (0 == strcasecmp(MDNS_NULL,name)) &&(0 == strcasecmp(PTR_BROWSE,service)) ){//服务列举
					#ifdef DEBUG_mDNS_RR
					printf("PTR---->Browse Service :\n");
					#endif
				        static uint8 count = 0;
					if(count >= SERVICE_NUM){
						count = 0;
					}
					#ifdef DEBUG_mDNS_RR
					printf("TEMP_SERVICE[%d]=%s\n",count,TEMP_SERVICE[count]);
					#endif
                                        data_len = mDNS_PTR_Data(MDNS_NULL,TEMP_SERVICE[count],&(rr_data.rrdata.name));//求PTR资源记录数据（服务名）
					count++;	
				 }else if( (0 == strcasecmp(SRC_IP,name)) &&(0 == strcasecmp(MDNS_NULL,service)) ){
					  #ifdef DEBUG_mDNS_RR				 
					  printf("PTR---->Reverse Mapping:\n");
					  #endif
					  data_len = mDNS_PTR_Data(NAME,MDNS_NULL,&(rr_data.rrdata.name));//求PTR资源记录数据(主机名)
					
				 }else {  //其余情况都为查询ptr各种服务的服务实体名
					#ifdef DEBUG_mDNS_RR
					printf("PTR---->Find Service :\n");
					#endif
                                        data_len = mDNS_PTR_Data(NAME,service,&(rr_data.rrdata.name));//求PTR资源记录数据(服务实体名)
					
				  }
				  #ifdef DEBUG_mDNS_RR
				  printf("*************************DEBUG mDNS_PTR_RR*****************************\n");  
				  #endif
				  break;
		case mDNSType_SRV :
				#ifdef DEBUG_mDNS_RR
				printf("*************************DEBUG mDNS_SRV_RR*****************************\n");  
				#endif
				data_len = mDNS_SRV_Data(name,service,0,0,&(rr_data.rrdata.srv));//求SRV资源记录		
				break; 	
		case mDNSType_TXT  :
				#ifdef DEBUG_mDNS_RR
				printf("*************************DEBUG mDNS_TXT_RR*****************************\n");  
				#endif
				data_len = mDNS_TXT_Data(service,&(rr_data.rrdata.txt));//求TXT资源记录				
				break;
		default :
			printf("other mDNS Type Not Implement\n");
			exit(1);
	}
	tmp_info.data_leng = htons(data_len);	
	tmp_info.data = rr_data;	
	#ifdef DEBUG_mDNS_RR
	uint16 length=0 ;
	uint8  test[MAX_NUM_RDAT_SIZE];//RR最大资源记录为MAX_NUM_RDAT_SIZE
	uint32  i;
	memcpy(test,tmp_info.name.name,tmp_info.name_leng);
	memcpy(&test[tmp_info.name_leng],(uint8 *)&(tmp_info.type),2);
	memcpy(&test[tmp_info.name_leng+2],(uint8 *)&(tmp_info.class),2);	
	memcpy(&test[tmp_info.name_leng+2+2],(uint8 *)&(tmp_info.ttl),4);
	memcpy(&test[tmp_info.name_leng+2+2+4],(uint8 *)&(tmp_info.data_leng),2);
	memcpy(&test[tmp_info.name_leng+2+2+4+2],(uint8 *)(tmp_info.data.rrdata.data),data_len);	
	length = 10+name_leng+data_len;
	printf("length:%d\n",length);
	for(i=0;i < length;i++){
		if(0 == i%5){
			printf("\n");
		}
		printf("test[%d]=0x%x\t",i,test[i]);
	}
	printf("\n");
	printf("*************************Exit DEBUG mDNS_XX_RR*****************************\n");
	#endif
	return  tmp_info;	
}
/*
功能			：根据名字数组，域名数组，类型数组类数组，和问题记录数目产生一个RR资源记录段
1.函数名			：RR_Sect  mDNS_RR_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)
2.函数形参		：
  			char *name[]	:存放主机名数组
  			char *service[]	:用来存放服务协议名。 
			name和service之间的对应关系：name[i]和service[i]要对对应。
			如 char *name[]={  		 char  *service[]={
			  		 "xnms",			     " ",			
			  		 "xnms",			     "http",
			    		  " ",				     "http",				
			                };                               };  
			 第一个对应主机域名，第二个对应服务实体域名，第三个对应服务域名(注用“ ”字符串表示空)	
  			char type[]:存放各问题记录的资源记录类型
  			char class[]          :对应名字和服务器的类是QU，还是QM
  			uint16   num          :对应报头中问题段记录个数
3:函数返回值：
			函数返回一个RR资源记录段
			  
author :冀博
时间    ：2011年10月12号    
测试成功  
2011.10月13号修改了数据结构测试成功
*/
RR_Sect  mDNS_RR_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num)
{

  //       #define  DEBUG_mDNS_RR_Sect	

	  RR_Sect    temp_rrsect;
	  uint16  j;
		
          memset(&temp_rrsect,0,sizeof(RR_Sect)); //初始化为0
	  for(j=0 ; j < num ;j++){
		temp_rrsect.rr_sect[j] = mDNS_RR(name[j],service[j],type[j],class[j]);  //封装RR资源记录段	
	  }
	  temp_rrsect.num =num;
          #ifdef  DEBUG_mDNS_RR_Sect	
	  uint32  i;//数据最长能会很大	
	  uint8  test[MAX_DATA];
	  uint32 length=0;//数据最长能会很大
	  memset(test,0,sizeof(test));//初始化为0
	  printf("*************************DEBUG mDNS_RR_Sect*****************************\n");
	  printf("authsect num:%d\n",temp_rrsect.num);
          for(i=0,length=0;i<temp_rrsect.num;i++){
		memcpy(&test[length],(uint8 *)(temp_rrsect.rr_sect[i].name.name),temp_rrsect.rr_sect[i].name_leng);
		memcpy(&test[length+temp_rrsect.rr_sect[i].name_leng],(uint8 *)&(temp_rrsect.rr_sect[i].type),2);
		memcpy(&test[length+temp_rrsect.rr_sect[i].name_leng+2],(uint8 *)&(temp_rrsect.rr_sect[i].class),2);
		memcpy(&test[length+temp_rrsect.rr_sect[i].name_leng+2+2],(uint8 *)&(temp_rrsect.rr_sect[i].ttl),4);
		memcpy(&test[length+temp_rrsect.rr_sect[i].name_leng+2+2+4],(uint8 *)&(temp_rrsect.rr_sect[i].data_leng),2);
		memcpy(&test[length+temp_rrsect.rr_sect[i].name_leng+2+2+4+2],(uint8 *)(temp_rrsect.rr_sect[i].data.rrdata.data),htons(temp_rrsect.rr_sect[i].data_leng));
	        length += (10+temp_rrsect.rr_sect[i].name_leng+htons(temp_rrsect.rr_sect[i].data_leng)); 
          }	  
	 for(i=0;i < length;i++){
			if(0 == i%5){
				printf("\n");
			}
		printf("test[%d]=0x%x\t",i,test[i]);
		}
	   printf("\n");
	   printf("*************************Exit DEBUG mDNS_RR_Sect*****************************\n");
	   #endif
	   return   temp_rrsect;
		
}


/*函数功能：声明唯一资源记录和共享资源记录，（声明包有答案段和附加记录段组成） 
声明：（答案段）
1>SRV	      name[]=tigerjohn,		service[]="http"	;资源记录为对应服务的SRV数据。
2>A地址资源记录 name[]=tigerjohn,		service[]=" "   	;资源记录数据为本机IP地址
3>PTR         name[]="_service,     	service[]="dns-sd" 	;资源记录数据为服务名eg:_http._tcp.local.
(如果多条服务则有多条该PTR记录，name,servcie不变只是，资源记录数据为其他的服务即可)。
4>PTR	      name[]=" ",		service[]="http"	;资源记录为该服务对应的服务实体名eg:tigerjibo._http._tcp.local
5>逆向地址映射  name[]="192.168.1.101",	service[]= " "		;资源记录数据为主机域名eg:tigerjibo.local
6>TXT	      name[]=tigerjohn,		service[]="device-info" ;资源记录为该设备的一些键值信息
7>TXT	      name[]=tigerjohn,		service[]="http"        ;则资源记录为空，即只存一个'\0',资源数据长度为0
（附件段）：目前没有实现，主要是NSEC资源记录，即AAAA地址记录。
1.函数名 ：uint32  mDNS_Probe_Packet(uint8 *buf,char *name[],char *service[],uint16 answ_num,uint16 addi_num)
2>函数形参：
（1）char *name[]		:存放主机名数组
（2）char *service[]	:用来存放服务协议名。 
			name和service之间的对应关系：name[i]和service[i]要对对应。
			如 char *name[]={  		 char  *service[]={
			  		 "xnms",			     " ",			
			  		 "xnms",			     "http",
			    		  " ",				     "http",				
			                };                               };  
			 第一个对应主机域名，第二个对应服务实体域名，第三个对应服务域名(注用“ ”字符串表示空)	 
第一个为SRV，
第二个A    
第三个为PTR（_service）
第四个为PTR（HTTP）
第五个为PTR逆向地址映射
第六个为TXT（先不实现）
第七个为TXT（先不实现）
顺序是定死的。
（3）uint16  answ_num:答案段记录个数，
（4）uint16  ddi_num; 附件段记录个数（目前为0，暂时不实现）
3>函数返回响应包的长度。并把响应包内容放到uint8 buf中带回。
-------------------------------------------------------
author:冀博
time:2011.10,14
2011.10.14号测试成功
2011.10.15号添加了附加记录段
*/
uint32  mDNS_Announce_Packet(uint8 *buf,char *service[])
{
	
  //      #define   DEBUG_mDNS_Announce_Packet 	
	
	mDNSHeader  	header; //响应包头	
	RR_Sect  	answ_sect,*pwst;//答案段
	RR_Sect  	addi_sect,*padist;//附加记录段
	uint8   	answ_buf[MAX_DATA];//存放答案段数据
	uint8  		addi_buf[MAX_DATA];//存放附加段数据
	uint32   	i;
	uint32 		length = 0;     //数据最长能会很大，初始化为0
	uint32          answ_len = 0;   //答案段长度，初始化为0
	uint32          addi_len = 0;   //附加记录段长度，初始化为0
	uint32          head_len = 0;   //包头长度，初始化为0
	//定义试探包头内容
	uint8  		q_r = 1;       //表示为响应报问
	uint8  		aa  = 1;       //表示权威
	uint8  		tc  = 0;       //TC位为0
	//答案段中各个变量的定义
	uint16          answ_num = 0;     		//答案段记录个数，初始化为0
       	uint16 		answ_type[MAX_ANNOUNCE_NUM];	//存放答案段资源记录中各个记录的资源记录类型
	uint16          answ_class[MAX_ANNOUNCE_NUM];	//存放答案段资源记录中各个记录的资源记录类
	char            *answ_name[MAX_ANNOUNCE_NUM];   //answ_name[]和answ_service[]对应起来组成答案段中资源记录名的域名
	char            *answ_service[MAX_ANNOUNCE_NUM];
        //附加记录段中各个变量的定义
	uint16          addi_num;     			//附加记录段个数，，初始化为0
	uint16          addi_type[MAX_ANNOUNCE_NUM];	//附件记录段中各资源记录类型
	uint16          addi_class[MAX_ANNOUNCE_NUM];	//附加记录段中各资源类型类
	char            *addi_name[MAX_ANNOUNCE_NUM];   //addi_name[]和addi_service[]对应起来组成答案段中资源记录名的域名
	char            *addi_service[MAX_ANNOUNCE_NUM];
	
	uint16		count;//统计name和service的个数
	
	
        //封装答案段记录	      	
	//第一个为A地址记录，没有实现AAAA地址记录
	count = 0;
	answ_name[count]    	= NAME;
     	answ_service[count] 	= MDNS_NULL;
	answ_type[count]   	= mDNSType_A;
	answ_class[count]  	= mDNSClass_IN | mDNSClass_UniqueRRset;//A地址记录是唯一资源记录
        count++;
	//第二个为PTR逆向地址
	answ_name[count]    	= SRC_IP;
	answ_service[count] 	= MDNS_NULL;
	answ_type[count]   	= mDNSType_PTR;
	answ_class[count]  	= mDNSClass_IN | mDNSClass_UniqueRRset;//A逆向地址记录是唯一资源记录
	count++;
	
	//有多少个service，就有多少个services._dns-sd记录，用于服务列举
	for(i=0;i<SERVICE_NUM;i++,count++){ 
		answ_name[count] 	= MDNS_NULL;		
		answ_service[count]	= PTR_BROWSE;
		answ_type[count]    	= mDNSType_PTR;
		answ_class[count]   	= mDNSClass_IN ;//PTR服务列举
	}
	//服务：
        for(i=0;i<SERVICE_NUM;i++,count++){ 
		answ_name[count] 	= MDNS_NULL;		
		answ_service[count]	= TEMP_SERVICE[i];
		answ_type[count]    	= mDNSType_PTR;
		answ_class[count]   	= mDNSClass_IN ;//PTR服务列举
	}
	//有多少个service,就有多少个SRV记录
	for(i=0;i<SERVICE_NUM;i++,count++){
		answ_name[count] 	= NAME;		
		answ_service[count]	= TEMP_SERVICE[i];	
		answ_type[count]    	= mDNSType_SRV;
		answ_class[count]   	= mDNSClass_IN | mDNSClass_UniqueRRset ;//srv记录是唯一资源记录
	}
	//有多少个service,就有多少个TXT记录
	for(i=0;i<SERVICE_NUM;i++,count++){
		answ_name[count] 	= NAME;		
		answ_service[count]	= TEMP_SERVICE[i];	
		answ_type[count]    	= mDNSType_TXT;
		answ_class[count]   	= mDNSClass_IN;//srv记录是唯一资源记录
	}
	/*
	*此TXT记录可有可无
		name[count] 	=   NAME;		
		service[count]	=   "device-info"; //设备信息记录，该记录只在TXT记录里存在	
		answ_type[count]    = mDNSType_TXT;
		answ_class[count]   = mDNSClass_IN;//srv记录是唯一资源记录	
		count++;
	*/
	answ_num = count;


	//封装附加段记录/*目前附件段记录没有实现NSEC，AAAA*/
	//第一个为A地址记录，没有实现AAAA地址记录
	count = 0;
	addi_name[count]    	= NAME;
     	addi_service[count] 	= MDNS_NULL;
	addi_type[count]   	= mDNSType_A;
	addi_class[count]  	= mDNSClass_IN | mDNSClass_UniqueRRset;//A地址记录是唯一资源记录
        count++;
	//第二个为PTR逆向地址
	addi_name[count]    	= SRC_IP;
	addi_service[count] 	= MDNS_NULL;
	addi_type[count]   	= mDNSType_PTR;
	addi_class[count]  	= mDNSClass_IN | mDNSClass_UniqueRRset;//A逆向地址记录是唯一资源记录
	count++;
	
	//目前在附件段记录中先把PTR服务记录，SRV记录添加进去
	//服务：
        for(i=0;i<SERVICE_NUM;i++,count++){ 
		addi_name[count] 	= MDNS_NULL;		
		addi_service[count]	= TEMP_SERVICE[i];
		addi_type[count]    	= mDNSType_PTR;
		addi_class[count]   	= mDNSClass_IN ;//PTR服务列举
	}
	//有多少个service,就有多少个SRV记录
	for(i=0;i<SERVICE_NUM;i++,count++){
		addi_name[count] 	= NAME;		
		addi_service[count]	= TEMP_SERVICE[i];	
		addi_type[count]    	= mDNSType_SRV;
		addi_class[count]   	= mDNSClass_IN | mDNSClass_UniqueRRset ;//srv记录是唯一资源记录
	}
        addi_num = count;
	//封装声明报文头
	head_len    =  mDNS_Header(&header,q_r,aa,tc,0,answ_num,0,addi_num);
	
	//组装声明报文主体部分
	answ_sect =   mDNS_RR_Sect(answ_name,answ_service,answ_type,answ_class,answ_num);
	
	
	pwst = &answ_sect;
        for(i=0,length=0;i<pwst->num;i++){
		memcpy(&answ_buf[length],(uint8 *)(pwst->rr_sect[i].name.name),pwst->rr_sect[i].name_leng);
		memcpy(&answ_buf[length+pwst->rr_sect[i].name_leng],(uint8 *)&(pwst->rr_sect[i].type),2);
		memcpy(&answ_buf[length+pwst->rr_sect[i].name_leng+2],(uint8 *)&(pwst->rr_sect[i].class),2);
		memcpy(&answ_buf[length+pwst->rr_sect[i].name_leng+2+2],(uint8 *)&(pwst->rr_sect[i].ttl),4);
		memcpy(&answ_buf[length+pwst->rr_sect[i].name_leng+2+2+4],(uint8 *)&(pwst->rr_sect[i].data_leng),2);
		memcpy(&answ_buf[length+pwst->rr_sect[i].name_leng+2+2+4+2],(uint8 *)(pwst->rr_sect[i].data.rrdata.data),htons(pwst->rr_sect[i].data_leng));
	        length += (10+pwst->rr_sect[i].name_leng+htons(pwst->rr_sect[i].data_leng)); 
          }	
	answ_len = length;
	length = 0;
	#ifdef   DEBUG_mDNS_Announce_Packet
	printf("In mDNS_Announce mDNS_ADDI_Sect :\n");//这以前都正确
	
	#endif
	addi_sect =   mDNS_RR_Sect(addi_name,addi_service,addi_type,addi_class,addi_num); 
	
	padist = &addi_sect;
	for(i=0,length=0;i<pwst->num;i++){
		memcpy(&addi_buf[length],(uint8 *)(padist->rr_sect[i].name.name),padist->rr_sect[i].name_leng);
		memcpy(&addi_buf[length+padist->rr_sect[i].name_leng],(uint8 *)&(padist->rr_sect[i].type),2);
		memcpy(&addi_buf[length+padist->rr_sect[i].name_leng+2],(uint8 *)&(padist->rr_sect[i].class),2);
		memcpy(&addi_buf[length+padist->rr_sect[i].name_leng+2+2],(uint8 *)&(padist->rr_sect[i].ttl),4);
		memcpy(&addi_buf[length+padist->rr_sect[i].name_leng+2+2+4],(uint8 *)&(padist->rr_sect[i].data_leng),2);
		memcpy(&addi_buf[length+padist->rr_sect[i].name_leng+2+2+4+2],(uint8 *)(padist->rr_sect[i].data.rrdata.data),htons(padist->rr_sect[i].data_leng));
	        length += (10+padist->rr_sect[i].name_leng+htons(padist->rr_sect[i].data_leng)); 
          }		
	addi_len = length;
	length = 0;
	memcpy(buf,(uint8 *)&header,head_len);
	memcpy(&buf[head_len],answ_buf,answ_len);
	memcpy(&buf[head_len+answ_len],addi_buf,addi_len);	
	length = head_len+ answ_len+addi_len;
	#ifdef   DEBUG_mDNS_Announce_Packet 			
	printf("*************************Debug mDNS_Announce_Packet*****************************\n");  
	printf("answ num:%d\n",answ_num);
	printf("answ name && answ service:\n");
	for(i=0;i<answ_num;i++){
		printf("answ_name[%d]=%s\tansw_service[%d]=%s\n",i,answ_name[i],i,answ_service[i]);
	}
	printf("answ type && answ class:\n");	
	for(i=0;i<answ_num;i++){
		printf("answ_type[%d]=0x%x\tansw_class[%d]=0x%x\n",i,answ_type[i],i,answ_class[i]);
	}
	
	printf("addi_num=%d\n",addi_num);
	printf("answ name && answ service:\n");
	for(i=0;i<addi_num;i++){
		printf("addi_name[%d]=%s\taddi_servcie[%d]%s\n",i,addi_name[i],i,addi_service[i]);
	}
	printf("addi type && addi class\n");
	for(i=0;i<addi_num;i++){
		printf("addi_type[%d]=0x%x\taddi_class[%d]=0x%x\n",i,addi_type[i],i,addi_class[i]);
	}
	for(i=0;i < length;i++){
			if(0 == i%5){
				printf("\n");
			}
		printf("test[%d]=0x%x\t",i,buf[i]);
		}
	 printf("\n");
	 printf("*************************Exit Debug mDNS_Announce_Packet*****************************\n");
	 #endif
	return length;
	       
}


