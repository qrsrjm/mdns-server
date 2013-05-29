
/**********************************************************************************************************************
*filename	：dns-sd.c
*time    	:2011.11.17
*author  	:冀博
*describe     	:目前只实现了dns-sd资源记录中的A记录，TXT记录，PTR记录，SRV记录
*modifier:冀博   
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


uint16 mDNS_A_Data(char *ip,mDNSv4Addr *ipv4);
uint16 mDNS_TXT_Data(char *service,TXTData *txtdata);
uint16  mDNS_PTR_Data(char *name,char *service, DomainName *dname);
uint16 mDNS_SRV_Data(char *name,char *service,uint16 priority,uint16 weight,SRVData *srvdata);

/*函数功能：获取A资源记录数据(根据IP地址转换为A地址记录数据数组端内容)
1.函数名字：uint32 mDNS_A_Data(char *ip,mDNSv4Addr *ipv4)
2.函数形参:
	char *ip:ip为字符数组
	mDNSv4Addr *ipv4:存放IPV4资源数据。
3.函数返回值：
	 函数成功后返回资源数据长度。
	 错误返回0.
4.time:2011.10.11
5.author:冀博
6.2011.10.11测试成功
--------------------------
2011.10.14
修改了返回类型为
uint16 mDNS_A_Data(char *ip,mDNSv4Addr *ipv4)
该函数移植可能不兼容
*/
uint16 mDNS_A_Data(char *ip,mDNSv4Addr *ipv4)
{		

//	#define  DEBUG_mDNS_A_Data	
	struct in_addr in;
	in.s_addr  = 0;
	if(ip ==NULL)
	return 0;
	if(  INADDR_NONE == (in.s_addr = inet_addr(ip)) ){
		#ifdef  DEBUG_mDNS_A_Data
		printf("*************************DEBUG mDNS_A_Data*****************************\n");	
		printf("inet_addr() failed\n");
		printf("*************************Exit DEBUG mDNS_A_Data*****************************\n");
		#endif
		return 0;
	} else {
		
		ipv4->inter = in.s_addr;
		#ifdef  DEBUG_mDNS_A_Data
		printf("original ip address:%s\n",ip);
		printf("in.s_addr:0x%x\n",in.s_addr);
		uint8 i;
		uint8 test[4];
		memcpy(test,ipv4,4);	
		for(i=0;i<4;i++){
			printf("test[%d]=0x%0x\t",i,test[i]);
		}
		printf("\n");	
		printf("*************************Exit DEBUG mDNS_A_Data*****************************\n");
		#endif
	}
	return 4;
}

/*函数功能：获取TXT资源记录数据
1.函数名字：uint16 mDNS_TXT_Data(char *service, TXTData *txtdata)
2.函数形参:
	1>char 		*service:存放服务名
	2>TXTData 	*txtdata:存放TXT资源记录数据
3.函数返回值：
 	函数成功后返回资源数据长度
	函数执行错误抛出异常，退出
4.time:2011.10.14
5.author:冀博
2011.10.14号测试成功
*/

uint16 mDNS_TXT_Data(char *service,TXTData *txtdata)
{
//	#define  DEBUG_mDNS_TXT_Data	
	int  txt_flg;	//不能定义为无符号型，Find_Txt_Service（）函数错误时，返回-1.
	uint16	txt_leng;

	txt_flg = Find_TXT_Service(service);
	if ( txt_flg < 0){
		printf("mDNS TXT_Data() is Error\n");
		exit(1);	
	}
	switch(txt_flg){
		case  HTTP:
			    txt_leng = mDNS_TXT_Http(txtdata);
			    break;
		case  DEVICE_INFO://没有使用里面是空
			     txt_leng = mDNS_TXT_Device_Info(txtdata);
			    break;			
		case  WEBDAV:
                            txt_leng = mDNS_TXT_Webdav(txtdata);
			    break;
		case  TELNET:
			    printf("NO Implement:%s","WEBDAV");
			    exit(1);
			    break;
		case  FTP:
			    printf("NO Implement:%s","WEBDAV");
			    exit(1);
			    break;
		case  SMTP:
			    printf("NO Implement:%s","WEBDAV");
			    exit(1);
			    break;
		case  LXI  :
			    txt_leng = mDNS_TXT_LXI(txtdata);
			    break;
		case  VXI_11:
			    txt_leng = mDNS_TXT_VXI_11(txtdata);
			    break;
		case  SCPI_RAW:
			    txt_leng = mDNS_TXT_SCPI_RAW(txtdata);
			    break;
		case  SCPI_TELNET:
			    txt_leng = mDNS_TXT_SCPI_TELNET(txtdata);
			    break;
		case   NI_RT:
			    txt_leng = mDNS_TXT_NI_RT(txtdata);
			    break;
		case   NI:
			    txt_leng = mDNS_TXT_NI(txtdata);
			    break;
		case   NI_SYSAPI:
			   txt_leng = mDNS_TXT_NI_SYSAPI(txtdata); 
			    break;
		default:
			printf("NO SUCH TXT SERVICE\n");
			exit(1);
	}	
	#ifdef DEBUG_mDNS_TXT_Data
	printf("*************************DEBUG mDNS_TXT_Data*****************************\n");
	uint8 test[MAX_TXT_DATA];
	uint8 i;
	memset(test,0,sizeof(test));//初始化为0
	memcpy(test,txtdata->c,txt_leng);
	printf("TXT SERVICE:%s\t %d\n",service,txt_flg);
	printf("TXT Data leng=%d\n",txt_leng);
	for(i=0;i < txt_leng;i++){
		if(0 == i%5){
			printf("\n");
		}
		printf("test[%d]=0x%x\t",i,test[i]);
	}
	printf("\n");
	printf("*************************Exit DEBUG mDNS_TXT_Data*****************************\n");
	#endif	
	return txt_leng;
}

/*函数功能：获取PTR资源记录数据
1.函数名字：uint32 mDNS_PTR_Data(char *name,char *service, DomainName *name)
2.函数形参:
1>char 		*name:存放名字
2>char 		*service:存放服务名
3>DomainName    *name:存放域名
3.函数返回值：
	 函数成功后返回资源数据长度。
	 超过PTR数据最大长度，则抛出异常
4.time:2011.10.12
5.author:冀博
----------------------------------------------------------------
2011.10.14修改了数据返回类型
uint16 mDNS_PTR_Data(char *name,char *service, DomainName *dname)
说明：PTR：逆向地址映射：存放的是主机域名
	  服务发现   ：存放的主机实体域名
	  服务列举   ：存放的是服务域名
*/
uint16  mDNS_PTR_Data(char *name,char *service, DomainName *dname)
{
//	#define DEBUG_mDNS_PTR_Data	

	uint16 name_leng;

	*dname = mDNS_Name(name,service,&name_leng);	
 
	#ifdef DEBUG_mDNS_PTR_Data
	printf("*************************DEBUG mDNS_PTR_Data*****************************\n");
	uint8 test[MAX_NAME+1];
	uint8 i;
	memset(test,0,sizeof(test));//初始化为0
	memcpy(test,dname->name,name_leng);
	printf("before:\tname:%s\tservice:%s\n",name,service);
	printf("After:\tdomain%s\n",dname->name);
	printf("PTR Data leng=%d\n",name_leng);
	for(i=0;i < name_leng;i++){
		if(0 == i%5){
			printf("\n");
		}
		printf("test[%d]=0x%x\t",i,test[i]);
	}
	printf("\n");
	printf("*************************Exit DEBUG mDNS_PTR_Data*****************************\n");
	#endif	
	return name_leng;
}

/*函数功能	：获取SRV资源记录数据
1.函数名字	：uint16  mDNS_SRV_Data(char *name,char *service,uint16 priority,uint16 weight)
2.函数形参	:
		1>char 		*name:存放名字
		2>char 		*service:存放服务名
		3>uint16 	priority:SRV记录数据的优先级
		4>uint16	weight:存放SRV记录数据的优先级
3.函数返回值：
 		函数成功后返回资源数据长度。
4.time:2011.10.11
5.author:冀博
2011.10.11号测试成功
*/
uint16 mDNS_SRV_Data(char *name,char *service,uint16 priority,uint16 weight,SRVData *srvdata)
{
//	  #define   DEBUG_mDNS_SRV_Data
	  uint16  	  name_leng;
	  uint8           i;
	  
	  srvdata->priority = htons(priority);
	  srvdata->weight = htons(weight);
	  srvdata->ipprot = htons(Find_Service_Port(service));//调用Find_Service_Port来获取该服务的端口号
	  srvdata->target = mDNS_Name(name,MDNS_NULL,&name_leng);//target为主机名
	
	  #ifdef  DEBUG_mDNS_SRV_Data
	  uint8 test[STAN_RDAT_SIZE];//STAN_RDAT_SIZE:最大一个SRV记录
	
	  memset(test,0,sizeof(test));//初始化为0。

	  printf("*************************DEBUG_mDNS_SRV_Data*****************************\n");
	  printf("before:name=%s\tservice:%s\n",name,service);	
	  printf("priority = 0x%0x \tweight = 0x%x\tipport = 0x%x\n",srvdata->priority, srvdata->weight,srvdata->ipprot);
	  printf("srv data leng:%d\n",6+name_leng);	
	  memcpy(test,(uint8 *)&srvdata->priority,2);
	  memcpy(&test[2],(uint8 *)&(srvdata->weight),2);
	  memcpy(&test[2+2],(uint8 *)&(srvdata->ipprot),2);
	  memcpy(&test[2+2+2],(srvdata->target.name),name_leng);
	  for(i=0;i < (6+name_leng);i++){
		if( 0 == i%5){
		printf("\n");
		}		
		printf("test[%d]=0x%x\t",i,test[i]);
		
	}
	printf("\n");
	printf("*************************Exit DEBUG_mDNS_SRV_Data*****************************\n");
	#endif
	return (6+name_leng);
}

