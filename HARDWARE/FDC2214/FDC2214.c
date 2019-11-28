#include "FDC2214.h"
#include "myiic.h"
#include "delay.h"
#include "FDC2X2X_CMD.h"
#include "usart.h" 

 u32 data_Fdc2214_H;
 u32 data_Fdc2214_L;
extern uint16_t buf[4];
float FDC2214_read_CHx(u8 CHx)	//读Channel x 转化结果
{
	u8 reg_addr1 = 0;	//MSB地址
	u8 reg_addr = 0;	//LSB地址
	u32 data_H = 0;			//接收到的数据
	u32 data1_L = 0; 
	float   conver = 0;   //最终的转换的结果
	switch(CHx)
	{
		case 0:
		{
			reg_addr1 = LDC13xx16xx_CMD_DATA_MSB_CH0;
			reg_addr 	= LDC13xx16xx_CMD_DATA_LSB_CH0;	
		}break;
		case 1:
		{
			reg_addr1 = LDC13xx16xx_CMD_DATA_MSB_CH1;
			reg_addr 	= LDC13xx16xx_CMD_DATA_LSB_CH1;
		}break;
		case 2:
		{
			reg_addr1 = LDC13xx16xx_CMD_DATA_MSB_CH2;
			reg_addr 	= LDC13xx16xx_CMD_DATA_LSB_CH2;
		}break;
		case 3:
		{
			reg_addr1 = LDC13xx16xx_CMD_DATA_MSB_CH3;
			reg_addr 	= LDC13xx16xx_CMD_DATA_LSB_CH3;
		}break;
		default: return 0;	//输入错误
		
	}
	
	data_H = FDC_read_16bit(reg_addr1);	//读取寄存器值  读高位  
	data1_L = FDC_read_16bit(reg_addr);	//读取寄存器值   读低位   

	conver = ( ((data_H & 0x0FFF)<<16)|(data1_L) ); 	//融合结果 
	//return conver;
	data_Fdc2214_H = data_H;
	data_Fdc2214_L = data1_L;
	
//***注意Init中各通道寄存器0x10-0x17的时钟分频***//

	conver = (conver * 40000000.0F)/268435456.0F;//得到F(sensorx)
	
	conver = conver * 2.0F *3.1415926F;
	conver = 1.0F/((conver * conver) * 0.000018F);
	conver = conver - 0.000000000033F;
	conver = conver * 1000000000000.0F;//F转换pF;
	
	
	return conver;
		


}
void FDC2214_Init(void)	//初始化FDC2214和I2C
{
	IIC_Init();							//IIC初始化
	LDC_ADDR = 0;	//LDC的7bit地址为0X2A，注意是7bit 即 0101 010_  最低位为读写位
	LDC_SD = 0;		//退出Shutdown Mode
	delay_ms(500);	//延时500ms
	
		//根据时间要求（100 SPS）计算的参考计数分辨率要求
	FDC_write_16bit(LDC13xx16xx_CMD_REF_COUNT_CH0,0Xffff);
	FDC_write_16bit(LDC13xx16xx_CMD_REF_COUNT_CH1,0Xffff);
	FDC_write_16bit(LDC13xx16xx_CMD_REF_COUNT_CH2,0Xffff);
	FDC_write_16bit(LDC13xx16xx_CMD_REF_COUNT_CH3,0Xffff);
	
	FDC_write_16bit(LDC13xx16xx_CMD_SETTLE_COUNT_CH0,0X0400);
	FDC_write_16bit(LDC13xx16xx_CMD_SETTLE_COUNT_CH1,0X0400);
	FDC_write_16bit(LDC13xx16xx_CMD_SETTLE_COUNT_CH2,0X0400);
	FDC_write_16bit(LDC13xx16xx_CMD_SETTLE_COUNT_CH3,0X0400);
	
	FDC_write_16bit(LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH0,0X1001);
	FDC_write_16bit(LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH1,0X1001);
	FDC_write_16bit(LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH2,0X1001);
	FDC_write_16bit(LDC13xx16xx_CMD_CLOCK_DIVIDERS_CH3,0X1001);
	
	FDC_write_16bit(LDC13xx16xx_CMD_ERROR_CONFIG,0X0001);
	
	FDC_write_16bit(LDC13xx16xx_CMD_MUX_CONFIG,0xC20D);
	
	FDC_write_16bit(LDC13xx16xx_CMD_DRIVE_CURRENT_CH0,0x8c40);
	FDC_write_16bit(LDC13xx16xx_CMD_DRIVE_CURRENT_CH1,0x8c40);
	FDC_write_16bit(LDC13xx16xx_CMD_DRIVE_CURRENT_CH2,0x8c40);
	FDC_write_16bit(LDC13xx16xx_CMD_DRIVE_CURRENT_CH3,0x8c40);

	FDC_write_16bit(LDC13xx16xx_CMD_CONFIG,0x1701);		//bit13 清零 退出睡眠模式
	


}





