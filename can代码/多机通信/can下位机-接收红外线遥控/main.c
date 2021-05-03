/*************************************************
*��������: CEPARK CAN������-CAN�����ڵ����
*˵����������CAN���߿�����CANH---CANH,CANL---CANL����
*      ���������ֱ𰴶��жϰ�������Ӧ���������ʾ��һ��
*      ��ͨ��CAN���߷��͵���һ�����������ʾ��
*�汾��v2.00
*�����������������磬��ӭ���ѽ����Ľ�
*      ��Ծ��̳����ͬ����
*���ߣ�Э�ɵ���	��hnrain 
*ʱ�䣺2010/09/14
*���䣺hnrain1004@gmail.com	��HQL19982003@163.com  ��qq��87675298
*��վ��bbs.cepark.com ��д������������⣬��������������
*���ǵĿںţ��ò���can��ѧϰ�ߣ�Ҳ��������תcan

*�Ƽ��̲ģ�
������,�޼̾����ֳ����� CAN ԭ����Ӧ�ü�����.�������������պ����ѧ������ ���Ȿ�飬���Ų���
����������ǿ��80c51��Ƭ���ٳ���ʵս������������д��һЩcanͨ�ų����㷨�ܲ���ֵ��ѧϰ
��������������ôһ�����㣬���ǵı��˼�룬����....����ѧϰ��
****************************************************/ 
#include <reg52.h>
#include "sjapelican.h"
#include "config.h"
#include <intrins.h>


#define uchar unsigned char

uchar distemp = 0;

sbit IRIN = P1^7;         //���������������

uchar IRCOM[7];


/**********************************************************/
void delay(unsigned char x)    //x*0.14MS
{
 unsigned char i;
  while(x--)
 {
  for (i = 0; i<13; i++) {}
 }
}




void IR_IN() interrupt 0 
{//INT0����Ϊ��������

 unsigned char j,k,N=0;
     EX1 = 0;   
	  delay(15);
	 if (IRIN==1) 
     { EX1 =1;
	   return;
	  } 
                           //ȷ��IR�źų���
  while (!IRIN)            //��IR��Ϊ�ߵ�ƽ������9ms��ǰ���͵�ƽ�źš�
    {delay(1);}
	  
 for (j=0;j<4;j++)         //�ռ���������
 { 
  for (k=0;k<8;k++)        //ÿ��������8λ
  {
   while (IRIN)            //�� IR ��Ϊ�͵�ƽ������4.5ms��ǰ���ߵ�ƽ�źš�
     {delay(1);}
    while (!IRIN)          //�� IR ��Ϊ�ߵ�ƽ
     {delay(1);}
		 	   
     while (IRIN)           //����IR�ߵ�ƽʱ��
      {
    delay(1);
    N++;           
    if (N>=30)
	 { EX1=1;
	 return;}                  //0.14ms���������Զ��뿪��
      }                        //�ߵ�ƽ�������                
     IRCOM[j]=IRCOM[j] >> 1;                  //�������λ����0��
     if (N>=8) {IRCOM[j] = IRCOM[j] | 0x80;}  //�������λ����1��
     N=0;
  }//end for k
 }//end for j
   
   if (IRCOM[2]!=~IRCOM[3])
   { EX1=1;
     goto LOOP; }

   switch(IRCOM[2])
   {
   		case 0x19:
			distemp = 0;
			break;
		case 0x45:
			distemp = 1;
			break;
		case 0x46:
			distemp = 2;
			break;
		case 0x47:
			distemp = 3;
			break;
		case 0x44:
			distemp = 4;
			break;
		case 0x40:
			distemp = 5;
			break;
		case 0x43:
			distemp = 6;
			break;
		case 0x07:
			distemp = 7;
			break;
		case 0x15:
			distemp = 8;
			break;
		case 0x09:
			distemp = 9;
			break;
   }
   Txd_data = distemp;
    EX1 = 1;
	LOOP:; 
}

void Peli_RXD(void) interrupt 2
{//�������ݺ��������жϷ�������е���

    uint8 data Status;
    EA = 0;//��CPU�ж�

    Status = SJA_IR;
    if(Status & RI_BIT)
    {//IR.0 = 1 �����ж�
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
	
	//IE =1;
    IT0 = 1;//�ⲿ�ж�0�����ش���
    EX0 = 1;//���ⲿ�ж�0
    EA = 1; //�����ж�

}

void main(void)
{
	MCU_Init();
    Peli_Init();
//  mDelay(1);
    while(1)
    {  
	//	Txd_data =1;
		Peli_TXD();
			mDelay(2000);

	}

}
//SJA1000 �ĳ�ʼ��
void Peli_Init(void)
{
    uint8 bdata Status;
    do
    {//  .0=1---reset MODRe,���븴λģʽ���Ա�������Ӧ�ļĴ���
     //��ֹδ���븴λģʽ���ظ�д��
        SJA_MOD = RM_BIT |AFM_BIT;
	Status = SJA_MOD ;
    }
    while(!(Status & RM_BIT));

    SJA_CDR = CANMode_BIT|CLKOff_BIT;// CDR.3=1--ʱ�ӹر�, .7=0---basic CAN, .7=1---Peli CAN
    SJA_BTR0 = 0x03;
    SJA_BTR1 = 0x1c;//16M���񣬲�����125Kbps
    SJA_IER = RIE_BIT;// .0=1--�����ж�ʹ�ܣ�  .1=0--�رշ����ж�ʹ��
    SJA_OCR = NormalMode|Tx0PullDn|OCPOL1_BIT|Tx1PullUp;// ����������ƼĴ���
    SJA_CMR = RRB_BIT;//�ͷŽ��ջ�����

    SJA_ACR0 = 0x11;
    SJA_ACR1 = 0x22;
    SJA_ACR2 = 0x33;
    SJA_ACR3 = 0x44;//��ʼ����ʾ��

    SJA_AMR0 = 0xff;
    SJA_AMR1 = 0xff;
    SJA_AMR2 = 0xff;
    SJA_AMR3 = 0xff;//��ʼ������

    do
    {
	SJA_MOD = AFM_BIT;
	Status  = SJA_MOD;
     }
    while( Status & RM_BIT );

}//SJA1000 �ĳ�ʼ��


void Peli_TXD( void )
{
    uint8 data Status;
//��ʼ����ʾ��ͷ��Ϣ
    TX_buffer[0] = 0x88;//.7=0��չ֡��.6=0����֡; .3=1���ݳ���
    TX_buffer[1] = 0x01;//���ڵ��ַ
    TX_buffer[2] = 0x02;
    TX_buffer[3] = 0x03;
    TX_buffer[4] = 0x04;

//��ʼ���������ݵ�Ԫ
    TX_buffer[5]  = Txd_data;
    TX_buffer[6]  = 0xFF;
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
    SJA_TBSR12 = TX_buffer[12];

    SJA_CMR = TR_BIT;//��λ���ͽ�������
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

   DisBuff[0] = Rxd_data%10;     //ȡ��λ��
   DisBuff[1] = Rxd_data%100/10; //ȡʮλ��
   DisBuff[2] = Rxd_data%10;	   //��λ��
   DisBuff[3] = Rxd_data%100/10; //ǧλ��
  
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

