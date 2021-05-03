/*************************************************
*��������: CEPARK CAN������-CAN���շ�ʵ��
*˵��������ܴ��ҵ���ֱ���1~4λ��
*      �����1��2λ��ʾ���͵����ݣ�3��4λ��ʾ���յ������ݡ�
*      ÿ��һ���жϰ�����������ֵ��һ��
*�Ƽ��̲ģ�
������,�޼̾����ֳ����� CAN ԭ����Ӧ�ü�����.�������������պ����ѧ������ ���Ȿ�飬���Ų���
����������ǿ��80c51��Ƭ���ٳ���ʵս������������д��һЩcanͨ�ų����㷨�ܲ���ֵ��ѧϰ
��������������ôһ�����㣬���ǵı��˼�룬����....����ѧϰ��
****************************************************/ 
#include <reg52.h>
#include "sjapelican.h"
#include "config.h"


void INT0_Data(void) interrupt 0
{   //INT0����Ϊ��������
    EA = 0;
    Txd_data++; //�洢�����������Ϊ�����͵�����
	Peli_TXD();
    EA = 1;
}

void Peli_RXD(void) interrupt 2
{   //�������ݺ��������жϷ�������е���

    uint8 data Status;
    EA = 0;//��CPU�ж�

    Status = SJA_IR;
    if(Status & RI_BIT)
    {   //IR.0 = 1 �����ж�
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
        Status = SJA_ALC;//�ͷ��ٲ���ʱ��׽�Ĵ���
        Status = SJA_ECC;//�ͷŴ�����벶׽�Ĵ���
    }
    SJA_IER = RIE_BIT;// .0=1--�����ж�ʹ�ܣ�

	Rxd_data = RX_buffer[5];

    EA = 1;//��CPU�ж�
}

void MCU_Init(void)
{
	//CPU��ʼ��
	SJA_RST = 0;//SJA1000��λ��Ч
	mDelay(10);	//��ʱ
    SJA_RST = 1;//CAN���߸�λ�ܽ�,��λ��Ч
    SJA_CS = 0;//CAN����Ƭѡ��Ч
    EX1 = 1;//�ⲿ�ж�1ʹ�ܣ�CAN���߽����ж�
    IT1 = 0;//CAN���߽����жϣ��͵�ƽ����
    IT0 = 1;//�ⲿ�ж�0�����ش���
    EX0 = 1;//���ⲿ�ж�0
    EA = 1; //�����ж�
}

void main(void)
{
	cs1602 = 0;	 //����1602ʹ�ܽ�������λ�����������ʾ
	MCU_Init();
    Peli_Init();
//  mDelay(1);
    while(1)
    {  
		LED_Disp_Seg7();
	}
}

void Peli_Init(void)
{   //SJA1000 �ĳ�ʼ��
    uint8 bdata Status;
    do
    {   // .0=1---reset MODRe,���븴λģʽ���Ա�������Ӧ�ļĴ���
        //��ֹδ���븴λģʽ���ظ�д��
        SJA_MOD = RM_BIT |AFM_BIT;
	    Status = SJA_MOD ;
    }
    while(!(Status & RM_BIT));

    SJA_CDR  = CANMode_BIT|CLKOff_BIT;// CDR.3=1--ʱ�ӹر�, .7=0---basic CAN, .7=1---Peli CAN
    SJA_BTR0 = 0x03;
    SJA_BTR1 = 0x1c;//16M���񣬲�����125Kbps
    SJA_IER  = RIE_BIT;// .0=1--�����ж�ʹ�ܣ�  .1=0--�رշ����ж�ʹ��
    SJA_OCR  = NormalMode|Tx0PullDn|OCPOL1_BIT|Tx1PullUp;// ����������ƼĴ���
    SJA_CMR  = RRB_BIT;//�ͷŽ��ջ�����

    SJA_ACR0  = 0x11;
    SJA_ACR1  = 0x22;
    SJA_ACR2  = 0x33;
    SJA_ACR3  = 0x44;//��ʼ����ʾ��

    SJA_AMR0  = 0xff;
    SJA_AMR1  = 0xff;
    SJA_AMR2  = 0xff;
    SJA_AMR3  = 0xff;//��ʼ������

    do
    {   //ȷ�������Խ���ģʽ
		SJA_MOD   = STM_BIT;
		Status  = SJA_MOD;
    }
    while( !(Status & STM_BIT) );

}  //SJA1000 �ĳ�ʼ��


void Peli_TXD( void )
{
    uint8 data Status;
    //��ʼ����ʾ��ͷ��Ϣ
    TX_buffer[0] = 0x88;//.7=0��չ֡��.6=0����֡; .3=1���ݳ���
    TX_buffer[1] = 0x01;//���ڵ��ַ
    TX_buffer[2] = 0x02;//
    TX_buffer[3] = 0x03;//
    TX_buffer[4] = 0x04;//

    //��ʼ���������ݵ�Ԫ
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
    while( Status & RS_BIT);  //SR.4=1 ���ڽ��գ��ȴ�
    
    do
    {
        Status = SJA_SR;
    }
    while(!(Status & TCS_BIT)); //SR.3=0,��������δ�����꣬�ȴ�

    do
    {
        Status = SJA_SR;
    }
    while(!(Status & TBS_BIT)); //SR.2=0,���ͻ������������ȴ�

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

    SJA_CMR = SRR_BIT;//��λ�Է��ͽ�������
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

   DisBuff[0]   = Txd_data%10;//ȡ��λ��
   DisBuff[1]   = Txd_data%100/10; //ȡʮλ��
   DisBuff[2]   = Rxd_data%10;	   //��λ��
   DisBuff[3]   = Rxd_data%100/10; //ǧλ��
  
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

