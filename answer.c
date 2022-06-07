/* 包含头文件 */
#include <ioCC2530.h>
#include <stdio.h>
#include <stdlib.h>

#include "hal_i2c.h"
#include "sht.h"
#include "sht1x.h"
#include "sht3x.h" 

/*宏定义*/
#define LED1 P1_0 
#define LED2 P1_1
#define SW1 P1_2

/*定义变量*/
uint8 counter = 0; //统计定时器溢出次数
uint8 flag_auto = 0;//自动工作模式标志位，0为手动模式，1为自动模式
//int16 tem,hum;//直播时用的： 温湿度数值，tem为温度，hum为湿度
int8 tem;//温度数值
uint8 hum;//湿度数值

//uint8 buff[10];
unsigned char buff[4];
/*声明函数*/
void Delay1Ms(uint8 time);//延时函数，32MHz系统时钟下，约1ms延时函数
void InitCLK(void);//系统时钟初始化函数，为32MHz
void InitTime1(void);//定时器1初始化函数，数据溢出周期为50ms
void InitUart0(void);//串口0初始化函数
void UART0SendByte(unsigned char c);//UART0发送一个字节函数
void UART0SendData(unsigned char *str,int len);//UART0发送指定数量字节数据

/*定义函数*/
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
  T1CCTL0 |= 0x04;//设定定时器1通道0比较模式
  T1CTL = 0x0a;//设置定时器1为32分频、模模式，并开始运行
  TIMIF &= ~0x40;//不产生定时器1的溢出中断
  T1IE = 1;//使能定时器1中断 
}

void InitUart0(void)
{
  PERCFG = 0x00;	
  P0SEL = 0x3c;	
  U0CSR |= 0xC0;
  U0BAUD = 216;
  U0GCR = 11;
  U0UCR |= 0x80;
  UTX0IF = 0;  // 清零UART0 TX中断标志 
  URX0IF = 0;// 清零UART0 RX中断标志
  URX0IE = 1;//使能UART0 RX中断
}

void UART0SendByte(unsigned char c)
{
  U0DBUF = c;// 将要发送的1字节数据写入U0DBUF
  while (!UTX0IF) ;// 等待TX中断标志，即U0DBUF就绪
  UTX0IF = 0;// 清零TX中断标志
}

void UART0SendData(unsigned char *str,int len)
{
  for(int i=0;i<len;i++)
  {
    U0DBUF = str[i];		// 将要发送的1字节数据写入U0DBUF
    while (!UTX0IF) ;  // 等待TX中断标志，即U0DBUF就绪
    UTX0IF = 0;       // 清零TX中断标志UART0SendByte(*str++);   // 发送一字节
  }
}

/*主函数*/
void main(void)
{
  InitCLK();
  InitTime1();
  InitUart0();
  SHT_Init();//初始化温湿度
  
  /*.......答题区1开始：LED灯初始状态设置...........*/
  P1DIR |= 0x03;//设置P1_0和P1_1为输出
  LED1 = LED2 = 0;//设置LED1和LED2的初始状态
  /*.......答题区1结束...........*/
  //SHT_Init();//初始化温湿度
  /*.......答题区2开始：SW1按键中断输入功能初始化...........*/
  P1DIR &= ~0x04;//设置P1_2为输入
  P1INP &= ~0x04;//设置P1_2端口为“上拉/下拉”模式
  P2INP &= ~0x40;//设置所有P1端口为“上拉”
  PICTL |= 0x02;//设置P1_2端口中断触发方式为：下降沿触发
  IEN2 |= 0x10;//使能P1端口中断
  P1IEN |= 0x04;//使能P1_2端口中断
  /*.......答题区2结束...........*/
  
  EA = 1;//使能总中断
  
  while(1)
  {
    /*..答题区3开始：每隔2s采集温湿度数据并通过串口发送，每次采集LED2闪烁..*/
    if(counter >= 40)
    {
      counter = 0;
      if(flag_auto == 1)
      {
       /* call_sht11(&tem,&hum);直播时的用法
        
        buff[0] = tem/10;
        buff[1] = hum/10;
        */
      SHT_SmpSnValue(&tem, &hum);
       buff[0]= tem;
       buff[1]= hum;  
        UART0SendData(buff,2);
      //  if(hum > 500)
         if(hum >= 0x25)//25是依据当前环境自定义的一个阀值
        {
          LED2 = 1;
        }
        
     //   if(hum < 450)
        if(hum < 0x25)
        {
          LED2 = 0;
        }
      }
    }
    /*.......答题区3结束...........*/
    
    
  }
}

/*中断服务函数*/

#pragma vector = P1INT_VECTOR
__interrupt void P1_ISR(void)
{
/*.......答题区4开始：按键中断服务函数...........*/
  if(P1IF == 1)
  {
    if(P1IFG & 0x04)
    {
      LED2 = ~LED2;
      P1IFG &= ~0x04;
    }
    P1IF = 0;
  }
/*.......答题区4结束...........*/
}


#pragma vector = T1_VECTOR
__interrupt void T1_ISR(void)
{
  counter++;
  T1STAT &= ~0x01;  //清除通道0中断标志
}

#pragma vector = URX0_VECTOR
__interrupt void UART0_ISR(void)
{
    /*.......答题区5开始：接收数据0xAF时进入自动工作方式、点亮LED1、禁止SW1中断；
  接收数据0xBF时退出自动工作方式、熄灭LED1、使能SW1中断。...........*/
  if(U0DBUF == 0xaf)
  {
    flag_auto = 1;
    LED1 = 1;
    P1IEN &= ~0x04;//禁止P1_2端口中断
  }
  
  if(U0DBUF == 0xbf)
  {
    flag_auto = 0;
    LED1 = 0;
    P1IFG &= ~0x04;
    P1IEN |= 0x04;//使能P1_2端口中断
  }
  /*.......答题区5结束...........*/  
  URX0IF = 0;// 清零UART0 RX中断标志
}
