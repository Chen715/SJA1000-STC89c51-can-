#ifndef __SJAPELICAN_H__
#define __SJAPELICAN_H__
#include <absacc.h>
#include "config.h"



#define     FrameNum      13//一帧字节数

uint8       RX_buffer[FrameNum];  //接收的数据
uint8       TX_buffer[FrameNum];  //接收的数据
uint8       Txd_data = 0;//CAN总线要发送的数据，也是要在数码管1-2位置显示的数据
uint8       Rxd_data = 0;//CAN总线要接收的数据，也是要在数码管3-4位置显示的数据
uint8				rxddata1 = 0;
						
						
sbit SJA_RST = P3^4;//SJA1000复位管脚
sbit SJA_CS  = P3^5;//SJA1000片选管脚

void MCU_Init(void);
void Peli_Init(void);//初始化CAN总线芯片
void Peli_TXD(void);//CAN发送子函数
void mDelay(uint16 mtime);//延时子函数


#define SJA_BASE_ADR     0xfe00	 //寄存器地址的基址

//SJA1000寄存器地址定义，作用在Peli模式，扩展帧方式
/**********************
模式控制寄存器及其位定义
************************/
#define SJA_MOD   XBYTE[SJA_BASE_ADR + 0x00]

#define RM_BIT     0x01             //复位模式请求位
#define LOM_BIT    0x02             //只听模式位
#define STM_BIT    0x04             //自检模式位
#define AFM_BIT    0x08             //验收滤波器模式位
#define SM_BIT     0x10             //睡眠模式位

/**********************
命令寄存器及其位定义
************************/
#define SJA_CMR    XBYTE[SJA_BASE_ADR + 0x01]
#define TR_BIT         0x01         //发送请求位
#define AT_BIT         0x02         //中止发送位
#define RRB_BIT        0x04         //释放接收缓冲器位
#define CDO_BIT        0x08         //清除数据溢出位
#define SRR_BIT        0x10         //自身接收请求位

/**********************
状态寄存器及其位定义
************************/
#define SJA_SR     XBYTE[SJA_BASE_ADR + 0x02]
#define RBS_BIT           0x01      //接收缓冲器状态位
#define DOS_BIT           0x02      //数据溢出状态位
#define TBS_BIT           0x04      //发送缓冲器状态位
#define TCS_BIT           0x08      //发送完成状态位
#define RS_BIT            0x10      //接收状态位
#define TS_BIT            0x20      //发送状态位
#define ES_BIT            0x40      //错误状态位
#define BS_BIT            0x80      //总线状态位

/**********************
中断寄存器及其位定义
************************/
#define SJA_IR     XBYTE[SJA_BASE_ADR + 0x03]
#define RI_BIT            0x01      //接收中断位
#define TI_BIT            0x02      //发送中断位
#define EI_BIT            0x04      //错误警告中断位
#define DOI_BIT           0x08      //数据溢出中断位
#define WUI_BIT           0x10      //唤醒中断位
#define EPI_BIT           0x20      //错误消极中断位
#define ALI_BIT           0x40      //仲裁丢失中断位
#define BEI_BIT           0x80      //总线错误中断位

/**********************
中断使能寄存器及其位定义
************************/
#define SJA_IER    XBYTE[SJA_BASE_ADR + 0x04]
#define RIE_BIT           0x01      //接收中断使能位
#define TIE_BIT           0x02      //发送中断使能位
#define EIE_BIT           0x04      //错误警告中断使能位
#define DOIE_BIT          0x08      //数据溢出中断使能位
#define WUIE_BIT          0x10      //唤醒中断使能位
#define EPIE_BIT          0x20      //错误消极中断使能位
#define ALIE_BIT          0x40      //仲裁丢失中断使能位
#define BEIE_BIT          0x80      //总线错误中断使能位


#define SJA_BTR0   XBYTE[SJA_BASE_ADR + 0x06]	  //总线定时器0寄存器
#define SJA_BTR1   XBYTE[SJA_BASE_ADR + 0x07]	  //总线定时器1寄存器
#define SAM_BIT           0x80                    //采样模式位；0==总线被采样1次；1== 总线被采样3次

/**********************
输出控制寄存器及其位定义
************************/
#define SJA_OCR    XBYTE[SJA_BASE_ADR + 0x08]
           /*OCMODE1 ，OCMODE0 */
#define BiPhaseMode       0x00       //双相输出模式
#define NormalMode        0x02       //正常输出模式
#define ClkOutMode        (0x01|0x02)//时钟输出模式
           /*TX1 的输出管脚配置*/
#define OCPOL1_BIT        0x20       //输出极性控制位
#define Tx1Float          0x00       //配置为悬空
#define Tx1PullDn         0x40       //配置为下拉
#define Tx1PullUp         0x80       //配置为上拉
#define Tx1PshPull        (0x40|0x80)//配置为推挽
            /*TX0 的输出管脚配置*/
#define OCPOL0_BIT        0x04       //输出极性控制位
#define Tx0Float          0x00       //配置为悬空
#define Tx0PullDn         0x08       //配置为下拉
#define Tx0PullUp         0x10       //配置为上拉
#define Tx0PshPull        (0x10|0x08)//配置为推挽

#define SJA_TEST   XBYTE[SJA_BASE_ADR + 0x09] //测试寄存器

/********************************
 * #define SJA_10   XBYTE[SJA_BASE_ADR + 0x0a]   寄存器功能保留
 ********************************/

/**********************
其他寄存器及其位定义
************************/
#define SJA_ALC      XBYTE[SJA_BASE_ADR + 0x0b]		//仲裁丢失捕捉寄存器
#define SJA_ECC      XBYTE[SJA_BASE_ADR + 0x0c]		//错误捕捉寄存器
#define SJA_EWLR     XBYTE[SJA_BASE_ADR + 0x0d]		//错误报警限制寄存器
#define SJA_RXERR    XBYTE[SJA_BASE_ADR + 0x0e]		//RX 错误计数器寄存器
#define SJA_TXERR    XBYTE[SJA_BASE_ADR + 0x0f]		//TX 错误计数器寄存器

/**********************
验收滤波器寄存器及其位定义
************************/
#define SJA_ACR0     XBYTE[SJA_BASE_ADR + 0x10]		//验收代码0寄存器
#define SJA_ACR1     XBYTE[SJA_BASE_ADR + 0x11]		//验收代码1寄存器
#define SJA_ACR2     XBYTE[SJA_BASE_ADR + 0x12]		//验收代码2寄存器
#define SJA_ACR3     XBYTE[SJA_BASE_ADR + 0x13]		//验收代码3寄存器

#define SJA_AMR0     XBYTE[SJA_BASE_ADR + 0x14]		//验收屏蔽0寄存器
#define SJA_AMR1     XBYTE[SJA_BASE_ADR + 0x15]		//验收屏蔽1寄存器
#define SJA_AMR2     XBYTE[SJA_BASE_ADR + 0x16]		//验收屏蔽2寄存器
#define SJA_AMR3     XBYTE[SJA_BASE_ADR + 0x17]		//验收屏蔽3寄存器

/**********************
TX缓冲器地址定义
************************/
#define SJA_TBSR0    XBYTE[SJA_BASE_ADR + 0x10]
#define SJA_TBSR1    XBYTE[SJA_BASE_ADR + 0x11]
#define SJA_TBSR2    XBYTE[SJA_BASE_ADR + 0x12]
#define SJA_TBSR3    XBYTE[SJA_BASE_ADR + 0x13]
#define SJA_TBSR4    XBYTE[SJA_BASE_ADR + 0x14]
#define SJA_TBSR5    XBYTE[SJA_BASE_ADR + 0x15]
#define SJA_TBSR6    XBYTE[SJA_BASE_ADR + 0x16]
#define SJA_TBSR7    XBYTE[SJA_BASE_ADR + 0x17]
#define SJA_TBSR8    XBYTE[SJA_BASE_ADR + 0x18]
#define SJA_TBSR9    XBYTE[SJA_BASE_ADR + 0x19]
#define SJA_TBSR10   XBYTE[SJA_BASE_ADR + 0x1a]
#define SJA_TBSR11   XBYTE[SJA_BASE_ADR + 0x1b]
#define SJA_TBSR12   XBYTE[SJA_BASE_ADR + 0x1c]

/**********************
RX缓冲器地址定义
************************/
#define SJA_RBSR0    XBYTE[SJA_BASE_ADR + 0x10]
#define SJA_RBSR1    XBYTE[SJA_BASE_ADR + 0x11]
#define SJA_RBSR2    XBYTE[SJA_BASE_ADR + 0x12]
#define SJA_RBSR3    XBYTE[SJA_BASE_ADR + 0x13]
#define SJA_RBSR4    XBYTE[SJA_BASE_ADR + 0x14]
#define SJA_RBSR5    XBYTE[SJA_BASE_ADR + 0x15]
#define SJA_RBSR6    XBYTE[SJA_BASE_ADR + 0x16]
#define SJA_RBSR7    XBYTE[SJA_BASE_ADR + 0x17]
#define SJA_RBSR8    XBYTE[SJA_BASE_ADR + 0x18]
#define SJA_RBSR9    XBYTE[SJA_BASE_ADR + 0x19]
#define SJA_RBSR10   XBYTE[SJA_BASE_ADR + 0x1a]
#define SJA_RBSR11   XBYTE[SJA_BASE_ADR + 0x1b]
#define SJA_RBSR12   XBYTE[SJA_BASE_ADR + 0x1c]

#define SJA_RMC      XBYTE[SJA_BASE_ADR + 0x1d]    //RX 信息计数器 寄存器
#define SJA_RBSA     XBYTE[SJA_BASE_ADR + 0x1e]    //RX 缓冲区起始地址 寄存器

/**********************
始终分频寄存器地址定义
************************/
#define SJA_CDR       XBYTE[SJA_BASE_ADR + 0x1f]   //时钟分频 寄存器
#define CLKOff_BIT        0x08                     //时钟关闭位，时钟输出管脚控制位
#define RXINTEN_BIT       0x20                     //用于接收中断的管脚TX1
#define CBP_BIT           0x40                     //CAN 比较器旁路控制位
#define CANMode_BIT       0x80                     //CAN 模式控制位

#endif



