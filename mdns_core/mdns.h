/**********************************************************************************************************************
*Filename：mdns.h
*Time    :2011.10.1
*Author  :冀博
*describe:mDNS中的宏和自定义的结构体
*modifer 冀博
*Time    2011.10.2
*修改了    宏定义中的名字
*修改了    QuesSect问题记录段结构体
2011.10.3
1.修改了QuesSect结构体中的字段，将QuesInfo	question[MAX_QUEST_NUM]改成了 QuesInfo	ques[MAX_QUEST_NUM];（字段名太长，变成时候不方便）
2.添加了A,SRV,TXT,SRV数据记录的结构体 ， 以及 RRInfo和答案段，附件记录段，授权段数据结构
----------------------------------------------------------------------------------
2011.10.12
修改了typedef struct{
	DomainName      name;
	uint16   	type;
	uint16		class;
	uint32          ttl;
	uint16          name_leng;//名字长度
	uint16 		data_leng;
	RData           data;
}RRInfo;
修该了其中的uint16 name_leng; uint16 data_leng;
添加了
#define  MAX_PROB_NUM  定义试探包中最大记录个数(2011.10.12)
-----------------------------------------------------
2011.10.13号修改了数据结构
去除了答案段，授权段，附加记录端，统一改成
typedef struct{
	uint16   	num;  //答案段记录个数
	RRInfo  	rr_sect[MAX_QUEST_NUM];
}RR_Sect;
添加#define  MAX_RR_NUM   	20 //定义查询包中记录个数
----------------------------------------------------------
2011.10.14号修改了#define  REVE_LOCAL_LABEL  "\x7in-addr\x4arpa" 改成了 #define  REVE_LOCAL_LABEL  "\x7in-addr\4arpa（\x4会把后面的a看成是0x4a而不是\4,a）。
#define 	mDNS_REVE_LOCAL_LABEL	        "\x3254\x3169\x7in-addr\4arpa"（同理）
----------------------------------------------------------
2011.11.03修改了QuesInfo结构体中的nameleng改成了name_leng
	  修改了RData资源记录中的rdataleng改成了rdata_leng;
2011.11.5号修改了 TCP_LABEL..等
***********************************************************************************************************************/

/**********************************************************************************************************************
                  宏
***********************************************************************************************************************/
#ifndef  MDNS
#define  MDNS

#define  MAX_DATA   		8960 //报文主体长度不能超过1440,但是规定的最大包为9000，即8940个ipv4(20)+udp(8)+dns(12)+8960
#define  STAN_RDAT_SIZE         264  //标准的RR数据为264（256+8），可以放的下一个最大的SRV记录
#define  MAX_NUM_RDAT_SIZE      8192  //最大RR数据为8K。
#define  MAX_TXT_DATA           255    //TXT记录的数据长度最大为255字节。


#define  MAX_NAME   		255 //域名字符串长度不能超过255（不算结尾的.）
#define  MAX_LABEL   		63 //单标签中字符长度不能超过63
#define  STAND_TTL 		4500UL //标准ttl为75分钟
#define  HOST_NAME_TTL 		120UL  //与主机名相关的资源记录名为120S

#define  SERVICE_NUM            6// 6//定义服务个数，目前只为http
#define  PROBE_NUM              SERVICE_NUM+1//试探包个数为服务+1，加A地址记录
#define  MAX_PROBE_NUM          SERVICE_NUM+2//试探包最大个数为服务+2，加A地址记录和AAAA地址记录
//2指的是A地址记录和PTR逆向地址记录，如果要在程序中加device-info在要在mDNS_Announce_Packet（）函数中认为加上，然后此处改为 
//4*SERVICE_NUM+4;(A,和逆向地址是必须有的)
#define  ANNOUNCE_NUM            4*SERVICE_NUM+2
#define  MAX_ANNOUNCE_NUM        4*SERVICE_NUM+4//最大为A，AAAA，逆向PTR，service._device-info,(AAAA,和service._device-info没有实现)
#define  MAX_RR_NUM              4*SERVICE_NUM+4
//SRV查询
//TXT查询
//服务的PTR查询
//加1跳服务列举查询.service_dns_sd.udp
//加1条对A地址和1条AAAA，和1条逆向地址映射，和device-info(ptr)记录的查询
#define  MAX_QUEST_NUM   	 3*SERVICE_NUM+5//问题段记录最大为对所有声明包中的记录进行查询（服务查询只查询一次）


#define  	MDNS_NULL   			" "       		//用" "表示空
#define  	DOMAIN_LABEL  			".local"                 //域名单标签
#define 	TCP_LABEL        		"._tcp"                  //协议但标签中的第二个标签域名
#define 	UDP_LABEL        		"._udp"                  //协议但标签中的第二个标签域名
#define 	REVE_LOCAL_LABEL     	        ".in-addr.arpa"        //前面加上IPV4地址的方向即逆向IPv4地址
#define 	mDNS_REVE_LOCAL_LABEL	        ".254.169.in-addr.arpa"//mDNS文档定义（若使用本地IP地址动态分配时使用该地址否则不用）
#define         PTR_BROWSE		         "services._dns-sd"   //用于PTR服务列举
#define         TXT_VERSION                      "txtvers=1"          //txt数据段中的版本号

/**********************************************************************************************************************
                     结构体
***********************************************************************************************************************/
/**********************************************************************************************************************
*                  结构体宏定义枚举变量*
**********************************************************************************************************************/
/*
	定义mDNS包中的资源类标量，常用class
	以及定义了类中的最高位：mDNSClass_UniqueRRset(用在响应包中表死唯一资源记录）
			    mDNSClass_UnicastResponse(用在查询包中表示要求单播响应)
*/
typedef unsigned char    uint8;
typedef unsigned short   uint16;
typedef unsigned int     uint32;


typedef  enum {				//From RFC1035 
	mDNSClass_IN			=   1,//Internet
	mDNSClass_CS 			=   2,//CSNET
	mDNSClass_CH    		=   3,//CHAOS
	mDNSClass_HS    		=   4,//Hesiod
	mDNSClass_NONE  		=   254,//Used in DNS UPDATE
	mDNSClass_Mask  		=   0x7FFF,//Multicast DNS uses the bottom 15 bits to identify the record class...
	mDNSClass_UniqueRRset   	=   0x8000,//indicate the unique RR record
	mDNSClass_UnicastResponse       =   0x8000,//indicate the means "unicast response accetable"
}mDNSClassValues;
/*
	定义mDNS包中的资源记录类型标量

*/
typedef  enum { 			//From FRC 1035

	mDNSType_A  			=   1,	   //1  Address
	mDNSType_PTR                    =   12,    //12 Domain name pointer
	mDNSType_HINFO			=   13,    //13 Host Infromation
	mDNSType_TXT	                =   16,    //16 TXT

	mDNSType_AAAA			=    28,   //AAAA地址记录
	mDNSType_SRV                    =    33,   //Service record
	mDNSType_ANY			=    255,
	
}mDNSTypeValues;

typedef enum {                  //用来表示协议段中的第二个标签的数值
	UDP   =   0,
	TCP   =   1,
}Service_Tcp_Udp;
typedef enum {                  //要和Find_Txt_Service中service_txt_table中的服务的顺序一致
	 HTTP                =  0,
	 DEVICE_INFO         =  1,	     		
	 WEBDAV              =  2,             
	 TELNET              =  3,	      	   
	 FTP                 =  4,	             		   
	 SMTP                =  5,		     	   
	 LXI                 =  6,//以下服务都是LXI标准制定的
	 VXI_11              =  7,	     		  
	 SCPI_RAW            =  8,	     		   
	 SCPI_TELNET         =  9,	     		
	 NI_RT               =  10,//以下是NI自己定义的服务
	 NI                  =  11,		      	  
         NI_SYSAPI           =  12,                  
	
}Txt_Service;
/*
	mDNS报文头部，具体含义见mDNS概要设计文档
	验证
*/
typedef struct  {
	uint16    id;
	uint16    flags;
	uint16    qunum;
	uint16    ansum;
	uint16    authnum;
	uint16    addnum;
}mDNSHeader;

/*
mDNS报文主体，具体含义见mDNS概要设计文档
*/
typedef  struct 
{
	mDNSHeader  header;
	uint8       packdata[MAX_DATA];//发送报文前判断数据是否超过最大数值
}mDNSPacket;

/*
	mDNS中域名  验证
*/

typedef  struct{
	uint8  name[MAX_NAME+1];
}DomainName;

/*
	mDNS中单标签  验证
*/
typedef  struct {
	uint8   label[MAX_LABEL+1];
}DomainLabel;



/*
验证
	查询记录(具体含义见mDNS概要设计文档)//进行了修改
*/
typedef struct {
	DomainName  	name;			
	uint16      	type;
	uint16      	class;
	uint16      	name_leng;//名字长度，注名子长度+4即是查询记录的长度，在程序中判断域名长度是否超过（MAX_NAME+1）
}QuesInfo;
/*
  查询段（一个查询段中可能有多条查询记录）只存在与查询包中
*/
typedef struct {
	uint16   		num;
	QuesInfo		ques[MAX_QUEST_NUM];
}QuesSect;
/*            
	A地址记录 具体含义见mDNS概要设计文档和DNS-SD文档
*/
typedef union{
	uint8 	 b[4];
	uint32   inter;
}mDNS32;                        //主要用来定义IPv地址
typedef  union{
	uint8   b[16];
	uint16  w[8];
	mDNS32  l[4];
}mDNS128;                     //主要用于定义IPV6地址

typedef mDNS32   	mDNSv4Addr;   //用mDNSv4定义一个IPV4地址
typedef mDNS128  	mDNSv6Addr;  //用mDNSv6定义个IPv6地址
typedef  struct {            //结构体mDNSAAr用来定义地址。
	uint32 type;
	union {
		mDNSv6Addr v6;
		mDNSv4Addr v4;
	}ip;
}mDNSAddr;   
/***************************************/
/*
	SRV资源记录内容，具体含义见mDNS概要设计文档和DNS-SD文档
*/
typedef struct {//SRV

	uint16 		priority;
	uint16  	weight;
     	uint16  	ipprot;
	DomainName  	target;
	
}SRVData;
/*
	TXT资源记录内容，具体含义见mDNS概要设计文档和DNS-SD文档
*/
typedef struct {
	uint8 c[MAX_TXT_DATA];
}TXTData;

/*
	RR资源记录段数组结构体
*/
typedef union{                        
	uint8  		data[STAN_RDAT_SIZE]; 
	mDNSv4Addr      ipv4;                     //存放A地址记录
	mDNSv6Addr      ipv6;                     //存放AAAA地址记录
	DomainName      name;                    //PTR资源记录
        TXTData         txt;                     //txt资源记录
	SRVData         srv;                     //srv资源记录
	
}RDataBody;
/*
             RR记录中数据(进行了修改)
*/
typedef struct 
{
	uint16  	rdata_leng;  //Rdatabody长度，
	RDataBody       rrdata;  
}RData;
/*
	RR记录  具体含义见mDNS概要设计文档
*/
typedef struct{
	DomainName      name;
	uint16   	type;
	uint16		class;
	uint32          ttl;
	uint16          name_leng;//名字长度
	uint16 		data_leng;//判断数据是否超过STAN_RDAT_SIZE
	RData           data;
}RRInfo;

/*
    RRSect(RR资源记录段)
*/
typedef struct{
	uint16   	num;  //RR段记录个数
	RRInfo  	rr_sect[MAX_RR_NUM];
}RR_Sect;


#endif
