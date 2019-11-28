#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 

//IO方向设置
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}	//PB11输入模式
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;} //PB11输出模式
//IO操作函数	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //SDA 
#define LDC_ADDR   PAin(0)  //FDC地址选择//硬件电路J7焊盘不焊接默认地址为高电平
#define LDC_SD   	PAin(1)  //FDC关断 

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号 

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

void FDC_write_16bit(u8 regaddr,u16 data);	//写data到寄存器，regaddr为寄存器的地址
u16 FDC_read_16bit(u8 regaddr);	//读寄存器regaddr的值，regaddr为寄存器的地址

#endif


















