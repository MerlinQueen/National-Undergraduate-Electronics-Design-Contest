#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PCin(5)   	
//#define KEY1 PAin(15)	 
//#define WK_UP  PAin(0)	 
 
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����2 
#define KEY2    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//��ȡ����0
#define KEY1    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//��ȡ����1
#define KEY3    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//��ȡ����2  
#define KEY4    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//��ȡ����2  

#define KEY1_PRES	1		//KEY0  
#define KEY2_PRES	2		//KEY1 
#define KEY3_PRES	4		//KEY0 
#define WKUP_PRES	3		//WK_UP  
#define KEY4_PRES   5

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��					    
#endif