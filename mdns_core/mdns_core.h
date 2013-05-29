/**********************************************************************************************************************
*Filename	：mDNSCore.h
*Time    	:2011.09.28
*Author  	:冀博
*Describe:	: mDNSCore层的全局变量和函数声明
*
*modifytime:2011.09.28
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
最新
**********************************************************************************************************************/
//函数名中间用下划线链接，变量不用下划线链接()
#ifndef  MDNS_CORE
#define  MDNS_CORE
#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "mdns.h"
#include "mdns_core.h"
#include "../mdns_socket/mdns_socket.h"
/**********************************************************************************************************************
*                          全局变量(全局变量第一个字母大写)
**********************************************************************************************************************/
char  *TEMP_SERVICE[SERVICE_NUM]; //全局变量，用来从mdns_responser文件中吧SERVICE的数值展示存起来，供mdns_core里面的文件使用

/**********************************************************************************************************************
*                          函数声明
**********************************************************************************************************************/
extern   char rotdash(void);
extern   unsigned long mDNS_Rand(uint32 x,uint32 y);
extern   int  Find_TXT_Service(char *service);
extern   uint16 Find_Service_Port(char *service);
extern   uint8  Find_TCP_Service(char *service);
extern   void Append_Name(char *name);
extern   uint8  mDNS_Header(mDNSHeader *h,uint8 qr,uint8 aa,uint8 tc,uint16 qunum,uint16 ansum,uint16 authnum,uint16 addnum);
extern   void Ip_Reverse(char *ip, char *reverse_ip);
extern   uint16  Transer_Name_Domain(const char *name,DomainName *domain);
extern   DomainName  mDNS_Name(char *name,char *service,uint16 *length);
extern   QuesInfo   mDNS_Ques_Info(char *name,char *service,uint16 type,uint16 class);
extern   QuesSect  mDNS_Ques_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num);
extern   uint32  mDNS_Probe_Packet(uint8 *buf,char *name[],char *service[],uint16 probe_num,uint16 QU_QM);
extern   uint16 mDNS_A_Data(char *ip,mDNSv4Addr *ipv4);
extern   uint16 mDNS_TXT_Data(char *service,TXTData *txtdata);
extern   uint16  mDNS_PTR_Data(char *name,char *service, DomainName *dname);
extern   uint16 mDNS_SRV_Data(char *name,char *service,uint16 priority,uint16 weight,SRVData *srvdata);
extern   RRInfo mDNS_RR(char *name,char *service,uint16 type,uint16 class);
extern   RR_Sect  mDNS_RR_Sect(char *name[],char *service[],uint16 type[],uint16 class[],uint16 num);
extern   uint32  mDNS_Announce_Packet(uint8 *buf,char *service[]);







#endif 

