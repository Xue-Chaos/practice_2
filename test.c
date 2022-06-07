/* ����ͷ�ļ� */
#include <ioCC2530.h>
#include <stdio.h>
#include <stdlib.h>

#include "hal_i2c.h"
#include "sht.h"
#include "sht1x.h"
#include "sht3x.h" 

/*�궨��*/
#define LED1 P1_0 
#define LED2 P1_1
#define SW1 P1_2

/*�������*/
uint8 counter = 0; //ͳ�ƶ�ʱ���������
uint8 flag_auto = 0;//�Զ�����ģʽ��־λ��0Ϊ�ֶ�ģʽ��1Ϊ�Զ�ģʽ
int16 tem,hum;//ֱ��ʱ�õģ� ��ʪ����ֵ��temΪ�¶ȣ�humΪʪ��
int8 tem;//�¶���ֵ
uint8 hum;//ʪ����ֵ

//uint8 buff[10];
unsigned char buff[2];
/*��������*/
void Delay1Ms(uint8 time);//��ʱ������32MHzϵͳʱ���£�Լ1ms��ʱ����
void InitCLK(void);//ϵͳʱ�ӳ�ʼ��������Ϊ32MHz
void InitTime1(void);//��ʱ��1��ʼ�������������������Ϊ50ms
void InitUart0(void);//����0��ʼ������
void UART0SendByte(unsigned char c);//UART0����һ���ֽں���
void UART0SendData(unsigned char *str,int len);//UART0����ָ�������ֽ�����

/*���庯��*/
void Delay1Ms(uint8 time)
{
  unsigned int i,j;
  for(i=0;i<=time;i++)
  {
    for(j=0;j<1100;j++);
  }
}

void InitCLK(void)
{
  CLKCONCMD &= 0x80;
  while(CLKCONSTA & 0x40);
}

void InitTime1(void)
{
  T1CC0L = 50000 & 0xff;
  T1CC0H = (50000 &0xff00)>>8;
  T1CCTL0 |= 0x04;//�趨��ʱ��1ͨ��0�Ƚ�ģʽ
  T1CTL = 0x0a;//���ö�ʱ��1Ϊ32��Ƶ��ģģʽ������ʼ����
  TIMIF &= ~0x40;//��������ʱ��1������ж�
  T1IE = 1;//ʹ�ܶ�ʱ��1�ж� 
}

void InitUart0(void)
{
  PERCFG = 0x00;	
  P0SEL = 0x3c;	
  U0CSR |= 0xC0;
  U0BAUD = 216;
  U0GCR = 11;
  U0UCR |= 0x80;
  UTX0IF = 0;  // ����UART0 TX�жϱ�־ 
  URX0IF = 0;// ����UART0 RX�жϱ�־
  URX0IE = 1;//ʹ��UART0 RX�ж�
}

void UART0SendByte(unsigned char c)
{
  U0DBUF = c;// ��Ҫ���͵�1�ֽ�����д��U0DBUF
  while (!UTX0IF) ;// �ȴ�TX�жϱ�־����U0DBUF����
  UTX0IF = 0;// ����TX�жϱ�־
}

void UART0SendData(unsigned char *str,int len)
{
  for(int i=0;i<len;i++)
  {
    U0DBUF = str[i];		// ��Ҫ���͵�1�ֽ�����д��U0DBUF
    while (!UTX0IF) ;  // �ȴ�TX�жϱ�־����U0DBUF����
    UTX0IF = 0;       // ����TX�жϱ�־UART0SendByte(*str++);   // ����һ�ֽ�
  }
}

/*������*/
void main(void)
{
  InitCLK();
  InitTime1();
  InitUart0();
  SHT_Init();//��ʼ����ʪ��
  
  /*.......������1��ʼ��LED�Ƴ�ʼ״̬����...........*/
  P1 &= ~0x03;
  
  /*.......������1����...........*/
   
  /*.......������2��ʼ��SW1�����ж����빦�ܳ�ʼ��...........*/
  
  
  
  /*.......������2����...........*/
  
  EA = 1;//ʹ�����ж�
  
  while(1)
  {
    /*..������3��ʼ��ÿ��2s�ɼ���ʪ�����ݲ�ͨ�����ڷ��ͣ�ÿ�βɼ�LED2��˸..*/
     
      
    
    
    /*.......������3����...........*/
    
    
  }
}

/*�жϷ�����*/

#pragma vector = P1INT_VECTOR
__interrupt void P1_ISR(void)
{
  /*.......������4��ʼ�������жϷ�����...........*/
    
    
  
  
  /*.......������4����...........*/
}


#pragma vector = T1_VECTOR
__interrupt void T1_ISR(void)
{
  counter++;
  T1STAT &= ~0x01;  //���ͨ��0�жϱ�־
}

#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
    /*.......������5��ʼ����������0xAFʱ�����Զ�������ʽ������LED1����ֹSW1�жϣ�
  ��������0xBFʱ�˳��Զ�������ʽ��Ϩ��LED1��ʹ��SW1�жϡ�...........*/
    
  
  
  
  
  
  /*.......������5����...........*/  
  URX0IF = 0;// ����UART0 RX�жϱ�־
}
