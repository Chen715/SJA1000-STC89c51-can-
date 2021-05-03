/*************************************************
*功能描述: CEPARK CAN开发板-CAN两个节点测试
*说明：将两块CAN总线开发板CANH---CANH,CANL---CANL相连
*      连接无误后分别按动中断按键。相应的数码管显示加一。
*      并通过CAN总线发送到另一块数码管上显示。
*版本：v2.00

温度传感器的接口 P17

红外线传感器接口 P32

****************************************************/ 
#include <reg52.h>
#include "sjapelican.h"
#include "config.h"
#include"delay.h"
#include "18b20.h"



bit ReadTempFlag;//定义读时间标志


void INT0_Data(void) interrupt 0
{//INT0按键为计数按键
    EA = 0;
    Txd_data++; //存储计数结果，并为待发送的数据
	Peli_TXD();
    EA = 1;
}

void Peli_RXD(void) interrupt 2
{//接收数据函数，在中断服务程序中调用

    uint8 data Status;
    EA = 0;//关CPU中断

    Status = SJA_IR;
    if(Status & RI_BIT)
    {//IR.0 = 1 接收中断
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


/*------------------------------------------------
                    定时器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 //TH0=0x00;	      //给定初值
 //TL0=0x00;
 //EA=1;            //总中断打开
 ET0=1;           //定时器中断打开
 TR0=1;           //定时器开关打开
}
/*------------------------------------------------
                 定时器中断子程序
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
 static unsigned int num;
 TH0=(65536-2000)/256;		  //重新赋值 2ms
 TL0=(65536-2000)%256;
 
 num++;
 if(num==300)        //
   {
    num=0;
    ReadTempFlag=1; //读标志位置1
	}
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
int temp;
float temp3;
int temp1=1234;
	
		MCU_Init();
		Init_Timer0();
    Peli_Init();
    mDelay(500);
    while(1)
    {  
			if(ReadTempFlag==1)
			 {
				ReadTempFlag=0;
				temp=ReadTemperature();
				temp =temp*6.25;//乘100倍
				Txd_data = temp/100; rxddata1 =temp%100; 
				 
				 
				}	
			 Peli_TXD();//发送数据
	    mDelay(200);
		}
			

	

}
//SJA1000 的初始化
void Peli_Init(void)
{
    uint8 bdata Status;
    do
    {//  .0=1---reset MODRe,进入复位模式，以便设置相应的寄存器
     //防止未进入复位模式，重复写入
        SJA_MOD = RM_BIT |AFM_BIT;
	Status = SJA_MOD ;
    }
    while(!(Status & RM_BIT));

    SJA_CDR = CANMode_BIT|CLKOff_BIT;// CDR.3=1--时钟关闭, .7=0---basic CAN, .7=1---Peli CAN
    SJA_BTR0 = 0x03;
    SJA_BTR1 = 0x1c;//16M晶振，波特率125Kbps
    SJA_IER = RIE_BIT;// .0=1--接收中断使能；  .1=0--关闭发送中断使能
    SJA_OCR = NormalMode|Tx0PullDn|OCPOL1_BIT|Tx1PullUp;// 配置输出控制寄存器
    SJA_CMR = RRB_BIT;//释放接收缓冲器

    SJA_ACR0 = 0x11;
    SJA_ACR1 = 0x22;
    SJA_ACR2 = 0x33;
    SJA_ACR3 = 0x44;//初始化标示码

    SJA_AMR0 = 0xff;
    SJA_AMR1 = 0xff;
    SJA_AMR2 = 0xff;
    SJA_AMR3 = 0xff;//初始化掩码

    do
    {
	SJA_MOD = AFM_BIT;
	Status  = SJA_MOD;
     }
    while( Status & RM_BIT );

}//SJA1000 的初始化


void Peli_TXD( void )
{
    uint8 data Status;
//初始化标示码头信息
    TX_buffer[0] = 0x88;//.7=0扩展帧；.6=0数据帧; .3=1数据长度
    TX_buffer[1] = 0x01;//本节点地址
    TX_buffer[2] = 0x02;
    TX_buffer[3] = 0x03;
    TX_buffer[4] = 0x04;

//初始化发送数据单元
    TX_buffer[5]  = Txd_data;
    TX_buffer[6]  = rxddata1;
    TX_buffer[7]  = 0x33;
    TX_buffer[8]  = 0x44;
    TX_buffer[9]  = 0x55;
    TX_buffer[10] = 0x66;
    TX_buffer[11] = 0x77;
    TX_buffer[12] = 0x88;

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
    SJA_TBSR12 = TX_buffer[12];

    SJA_CMR = TR_BIT;//置位发送接收请求
}

void mDelay(uint16 mtime)
{
	for(; mtime > 0; mtime--)
	{
		uint8 j = 244;
		while(--j);
	}
}
