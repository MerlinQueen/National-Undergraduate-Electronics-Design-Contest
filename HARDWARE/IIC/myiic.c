#include "myiic.h"
#include "delay.h"
#include "FDC2X2X_CMD.h"


//初始化IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOE, ENABLE );
  //GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11); 	//PB10,PB11 输出高
	
	  //GPIOA0,A1初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	IIC_SCL=1;
	IIC_SDA=1;
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
	delay_us(4);
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;//u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(4);//delay_us(1);	   
	IIC_SCL=1;delay_us(4);//delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)//if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(4);//delay_us(2);
	IIC_SCL=1;
	delay_us(4);//delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(4);//delay_us(2);
	IIC_SCL=1;
	delay_us(4);//delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(4);//delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(4);//delay_us(2); 
		IIC_SCL=0;	
		delay_us(4);//delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
	delay_us(4);
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(4);//delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(4);//delay_us(2);//delay_us(1); 
    }					 
    if (ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}





void FDC_write_16bit(u8 regaddr,u16 data)	//写data到寄存器，regaddr为寄存器的地址
{
	u16 data_temp = 0;
	u16 data_temp1 = 0;
	u8 Ack1,Ack2,Ack3,Ack4;
	Ack1=Ack2=Ack3=Ack4=2;
	
	IIC_Start();
	IIC_Send_Byte(LDC1314_ADDR_WRITE);	//发送设备地址，写
	Ack1 = IIC_Wait_Ack();
	
	IIC_Send_Byte(regaddr);	//寄存器地址
	Ack2 = IIC_Wait_Ack();
	
	IIC_Send_Byte(data>>8);	//发送高八位
	Ack3 = IIC_Wait_Ack();
	
	IIC_Send_Byte(data&0xff);	//发送低八位
	Ack4 = IIC_Wait_Ack();
	
	IIC_Stop();//产生一个停止条件


}



u16 FDC_read_16bit(u8 regaddr)	//读寄存器regaddr的值，regaddr为寄存器的地址
{
	u16 data_temp = 0;
	u16 data_temp1 = 0;
	u8 Ack1,Ack2,Ack3,Ack4;
	Ack1=Ack2=Ack3=Ack4=2;
	
	IIC_Start();
	IIC_Send_Byte(LDC1314_ADDR_WRITE);	//发送设备地址，写
	Ack1 = IIC_Wait_Ack();
	IIC_Send_Byte(regaddr);	//寄存器地址
	Ack2 = IIC_Wait_Ack();
	
	IIC_Start();
	IIC_Send_Byte(LDC1314_ADDR_READ);//发送设备地址，读
	Ack3 = IIC_Wait_Ack();
	data_temp1 = IIC_Read_Byte(0);//IIC读取高位字节
	data_temp = IIC_Read_Byte(0);//IIC读取低位字节
	IIC_Stop();//产生一个停止条件
	
	//printf (" Ack1 %d\r\n Ack2 %d\r\n Ack3 %d, \r\n",Ack1,Ack2,Ack3);
	//printf ("data_temp1 %d,data_temp %d \r\n",data_temp1,data_temp);
	
	data_temp = data_temp + (data_temp1<<8) ;	//数据合并
	
	//printf("%d \r\n",data_temp); //串口发送看结果无误
	return data_temp;



}























