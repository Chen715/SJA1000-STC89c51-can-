/*************************************************
*功能描述: CEPARK CAN开发板-CAN自收发实验
*说明：数码管从右到左分别是1~4位。
*      数码管1、2位显示发送的数据，3、4位显示接收到的数据。
*      每按一次中断按键发送数据值增一。
*推荐教材：
饶运涛,邹继军《现场总线 CAN 原理与应用技术》.北京：北京航空航天大学出版社 （这本书，看着不错）
周立功《增强型80c51单片机速成与实战》还有周立功写的一些can通信程序算法很不错，值得学习
本程序，融入了那么一点点点点点，他们的编程思想，哈哈....仅供学习用
****************************************************/ 
#include <reg52.h>
#include "sjapelican.h"
#include "config.h"


void INT0_Data(void) interrupt 0
{   //INT0按键为计数按键
    EA = 0;
    Txd_data++; //存储计数结果，并为待发送的数据
	Peli_TXD();
    EA = 1;
}

void Peli_RXD(void) interrupt 2
{   //接收数据函数，在中断服务程序中调用

    uint8 data Status;
    EA = 0;//关CPU中断

    Status = SJA_IR;
    if(Status & RI_BIT)
    {   //IR.0 = 1 接收中断
        RX_buffer[0] =  SJA_RBSR0;
        RX_buffer[1] =  SJA_RBSR1;
        RX_buffer[2] =  SJA_RBSR2;
        RX_buffer[3] =  SJA_RBSR3;
        RX_buffer[4] =  SJA_RBSR4;
        RX_buffer[5] =  SJA_RBSR5;
        RX_buffer[6] =  SJA_RBSR6;
        RX_buffer[7] =  SJA_RBSR7;
        RX_buffer[8] =  SJA_RBSR8;
        RX_buffer[9] =  SJA_RBSR9;
        RX_buffer[10] =  SJA_RBSR10;
        RX_buffer[11] =  SJA_RBSR11;
        RX_buffer[12] =  SJA_RBSR12;

        SJA_CMR = RRB_BIT;
        Status = SJA_ALC;//释放仲裁随时捕捉寄存器
        Status = SJA_ECC;//释放错误代码捕捉寄存器
    }
    SJA_IER = RIE_BIT;// .0=1--接收中断使能；

	Rxd_data = RX_buffer[5];

    EA = 1;//打开CPU中断
}

void MCU_Init(void)
{
	//CPU初始化
	SJA_RST = 0;//SJA1000复位有效
	mDelay(10);	//延时
    SJA_RST = 1;//CAN总线复位管脚,复位无效
    SJA_CS = 0;//CAN总线片选有效
    EX1 = 1;//外部中断1使能；CAN总线接收中断
    IT1 = 0;//CAN总线接收中断，低电平触发
    IT0 = 1;//外部中断0负边沿触发
    EX0 = 1;//打开外部中断0
    EA = 1; //打开总中断
}

void main(void)
{
	cs1602 = 0;	 //屏蔽1602使能脚能让四位数码管正常显示
	MCU_Init();
    Peli_Init();
//  mDelay(1);
    while(1)
    {  
		LED_Disp_Seg7();
	}
}

void Peli_Init(void)
{   //SJA1000 的初始化
    uint8 bdata Status;
    do
    {   // .0=1---reset MODRe,进入复位模式，以便设置相应的寄存器
        //防止未进入复位模式，重复写入
        SJA_MOD = RM_BIT |AFM_BIT;
	    Status = SJA_MOD ;
    }
    while(!(Status & RM_BIT));

    SJA_CDR  = CANMode_BIT|CLKOff_BIT;// CDR.3=1--时钟关闭, .7=0---basic CAN, .7=1---Peli CAN
    SJA_BTR0 = 0x03;
    SJA_BTR1 = 0x1c;//16M晶振，波特率125Kbps
    SJA_IER  = RIE_BIT;// .0=1--接收中断使能；  .1=0--关闭发送中断使能
    SJA_OCR  = NormalMode|Tx0PullDn|OCPOL1_BIT|Tx1PullUp;// 配置输出控制寄存器
    SJA_CMR  = RRB_BIT;//释放接收缓冲器

    SJA_ACR0  = 0x11;
    SJA_ACR1  = 0x22;
    SJA_ACR2  = 0x33;
    SJA_ACR3  = 0x44;//初始化标示码

    SJA_AMR0  = 0xff;
    SJA_AMR1  = 0xff;
    SJA_AMR2  = 0xff;
    SJA_AMR3  = 0xff;//初始化掩码

    do
    {   //确保进入自接收模式
		SJA_MOD   = STM_BIT;
		Status  = SJA_MOD;
    }
    while( !(Status & STM_BIT) );

}  //SJA1000 的初始化


void Peli_TXD( void )
{
    uint8 data Status;
    //初始化标示码头信息
    TX_buffer[0] = 0x88;//.7=0扩展帧；.6=0数据帧; .3=1数据长度
    TX_buffer[1] = 0x01;//本节点地址
    TX_buffer[2] = 0x02;//
    TX_buffer[3] = 0x03;//
    TX_buffer[4] = 0x04;//

    //初始化发送数据单元
    TX_buffer[5]  = Txd_data;
    TX_buffer[6]  = 0x22;
    TX_buffer[7]  = 0x33;
    TX_buffer[8]  = 0x44;//
    TX_buffer[9]  = 0x55;//
    TX_buffer[10] = 0x66;//
    TX_buffer[11] = 0x77;//
    TX_buffer[12] = 0x88;//

    do
    {
        Status = SJA_SR;
    }
    while( Status & RS_BIT);  //SR.4=1 正在接收，等待
    
    do
    {
        Status = SJA_SR;
    }
    while(!(Status & TCS_BIT)); //SR.3=0,发送请求未处理完，等待

    do
    {
        Status = SJA_SR;
    }
    while(!(Status & TBS_BIT)); //SR.2=0,发送缓冲器被锁。等待

    SJA_TBSR0  = TX_buffer[0];
    SJA_TBSR1  = TX_buffer[1];
    SJA_TBSR2  = TX_buffer[2];
    SJA_TBSR3  = TX_buffer[3];
    SJA_TBSR4  = TX_buffer[4];
    SJA_TBSR5  = TX_buffer[5];
    SJA_TBSR6  = TX_buffer[6];
    SJA_TBSR7  = TX_buffer[7];
    SJA_TBSR8  = TX_buffer[8];
    SJA_TBSR9  = TX_buffer[9];
    SJA_TBSR10 = TX_buffer[10];
    SJA_TBSR11 = TX_buffer[11];
    SJA_TBSR12  = TX_buffer[12];

    SJA_CMR = SRR_BIT;//置位自发送接收请求
}

void mDelay(uint16 mtime)
{
	for(; mtime > 0; mtime--)
	{
		uint8 j = 244;
		while(--j);
	}
}

void LED_Disp_Seg7()
{

   LedCtrl = LedCtrl | 0xf0;

   DisBuff[0]   = Txd_data%10;//取个位数
   DisBuff[1]   = Txd_data%100/10; //取十位数
   DisBuff[2]   = Rxd_data%10;	   //百位数
   DisBuff[3]   = Rxd_data%100/10; //千位数
  
   LedPort = LED_Disp[DisBuff[0]];
   LedCtrl = LedCtrl & 0xbf;
   mDelay(5);
   LedCtrl = LedCtrl | 0xf0;

   LedPort = LED_Disp[DisBuff[1]];
   LedCtrl = LedCtrl & 0x7f;
   mDelay(5);
   LedCtrl = LedCtrl | 0xf0;

   LedPort = LED_Disp[DisBuff[2]];
   LedCtrl = LedCtrl & 0xef;
   mDelay(5);
   LedCtrl = LedCtrl | 0xf0;

   LedPort = LED_Disp[DisBuff[3]];
   LedCtrl = LedCtrl & 0xdf;
   mDelay(5);
   LedCtrl = LedCtrl | 0xf0;
}

