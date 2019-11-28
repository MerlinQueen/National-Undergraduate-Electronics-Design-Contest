#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "mytime.h"
#include "wkup.h"
#include "oled.h"
#include "FDC2214.h"
#include "beep.h"
#include "key.h"
#include  "math.h"
#include "stdio.h"
#include "spi.h"
#include "flash.h"

#define N1 20
#define N2 24
double paper_num;
float res5, res6;
//float temp[4];

float cap_num_hard[500];    //电容采样数据处理前数组存储
float cap_num_end[30];    //电容采样数据处理后数组存储
//double cap_save1[15];

double y20[N1] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20};
double x20[N1];
float paper_17;

//double y60[N2] = {14,15,16,17,20,21,25,26,27,29,30,31,35,36,37,40,41,45,46,50,51,55,56,60,61}; 
//double y60[N2] =  {11,13,15,17,19,20,21,23,25,27,29,30,31,33,35,37,39,40,41,43,45,47,49,50};    //   N = 24
double y60[N2] =   {15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,33,35,40,45,50,55,60};               //   N2 = 28个点
//double y60[N2]=  {15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,35,40,45,50,55,60,65,70}; 				//  N2 =24 个样本点

double x60[N2];


union 
{

	double k2[N2] ;
	u8  ku[N2*8];
} kn;

	
int mode_flag = 0;
double multiterm[6];      // 存放 多项式的系数  multiterm[0]  表示常数项,   multiterm[5]  表示 5次项的系数

double a[N1][N1], b[N1][N1], c[N1][N1], d[N1], kd[N1],Sum = 0.0, Sum1;   // k2[N2],    k1[N1], 
double a2[N2][N2], b2[N2][N2], c2[N2][N2], d2[N2], Sum2 = 0.0, Sum12;


double  check_paper_num(double x);
float end_ping_jun(void);
float end_zhong_zhi(void);
void cap_num_hard_end(void);
void lcd_init_display(void);
void mode_check(void);


void arryans(double a[][N1], double b[N1], double x[N1], int n);
void arrytrans(double a[][N1], double b[][N1], int n, int m);
void arrymul(double a[][N1], double b[][N1], double c[][N1], int m, int l, int n);
void arrymuls(double a[][N1], double b[], double c[], int m, int n);
void arrycon(double a[], double b[][N1], int n, int m);
void arryzeng(double a[][N1], double b[], double c[][N1], int m, int n);
void arryans2(double a2[][N2], double b2[N2], double x2[N2], int n);
void arrytrans2(double a2[][N2], double b2[][N2], int n, int m);
void arrymul2(double a2[][N2], double b2[][N2], double c2[][N2], int m, int l, int n);
void arrymuls2(double a2[][N2], double b2[], double c2[], int m, int n);
void arrycon2(double a2[], double b2[][N2], int n, int m);

void nihe(void)
{
	int  n, m;
	n = 6;
	m = N1;                       //m是初始数据的长度
	arrycon(x20, a, m, n);          //a[][]是A
	arrytrans(a, b, m, n);        //b[][]是A'
	arrymul(b, a, c, n, m, n);    //c[][]是A'*A
	arrymuls(b, y20, d, n, m);      //d[][]是A'*y
	arryans(c, d, kd, n);          //k[]是解，即最后的系数，按升幂排序
	
}

void nihe2(void)
{
	int  n, m;
	n = 7;
	m = N2;                       //m是初始数据的长度
	arrycon2(x60, a2, m, n);          //a[][]是A
	arrytrans2(a2, b2, m, n);        //b[][]是A'
	arrymul2(b2, a2, c2, n, m, n);    //c[][]是A'*A
	arrymuls2(b2, y60, d2, n, m);      //d[][]是A'*y
	arryans2(c2, d2, kn.k2, n);          //k[]是解，即最后的系数，按升幂排序

}

void arryans(double a[][N1], double b[N1], double x[N1], int n) //解一个n阶的线性方程组
{
	int i, j, k, mi;
	double mx, tmp;

	for (i = 0; i < n - 1; i++) {
		for (j = i + 1, mi = i, mx = fabs(a[i][i]); j < n; j++) //找主元素
			if (fabs(a[j][i]) > mx) {
				mi = j;
				mx = fabs(a[j][i]);
			}
		if (i < mi) { //交换两行
			tmp = b[i];
			b[i] = b[mi];
			b[mi] = tmp;
			for (j = i; j < n; j++) {
				tmp = a[i][j];
				a[i][j] = a[mi][j];
				a[mi][j] = tmp;
			}
		}
		for (j = i + 1; j < n; j++) { //高斯消元
			tmp = -a[j][i] / a[i][i];
			b[j] += b[i] * tmp;
			for (k = i; k < n; k++)
				a[j][k] += a[i][k] * tmp;
		}
	}
	x[n - 1] = b[n - 1] / a[n - 1][n - 1]; //求解方程
	for (i = n - 2; i >= 0; i--) {
		x[i] = b[i];
		for (j = i + 1; j < n; j++)
			x[i] -= a[i][j] * x[j];
		x[i] /= a[i][i];
	}
}

void arryans2(double a[][N2], double b[N2], double x[N2], int n) //解一个n阶的线性方程组
{
	int i, j, k, mi;
	double mx, tmp;

	for (i = 0; i < n - 1; i++) {
		for (j = i + 1, mi = i, mx = fabs(a[i][i]); j < n; j++) //找主元素
			if (fabs(a[j][i]) > mx) {
				mi = j;
				mx = fabs(a[j][i]);
			}
		if (i < mi) { //交换两行
			tmp = b[i];
			b[i] = b[mi];
			b[mi] = tmp;
			for (j = i; j < n; j++) {
				tmp = a[i][j];
				a[i][j] = a[mi][j];
				a[mi][j] = tmp;
			}
		}
		for (j = i + 1; j < n; j++) { //高斯消元
			tmp = -a[j][i] / a[i][i];
			b[j] += b[i] * tmp;
			for (k = i; k < n; k++)
				a[j][k] += a[i][k] * tmp;
		}
	}
	x[n - 1] = b[n - 1] / a[n - 1][n - 1]; //求解方程
	for (i = n - 2; i >= 0; i--) {
		x[i] = b[i];
		for (j = i + 1; j < n; j++)
			x[i] -= a[i][j] * x[j];
		x[i] /= a[i][i];
	}
}


void arrytrans(double a[][N1], double b[][N1], int m, int n) //矩阵转置,m行n列
{
	int i, j;

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			b[j][i] = a[i][j];
}

void arrytrans2(double a[][N2], double b[][N2], int m, int n) //矩阵转置,m行n列
{
	int i, j;

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			b[j][i] = a[i][j];
}

void arrymul(double a[][N1], double b[][N1], double c[][N1], int m, int l, int n) //两个矩阵的乘法
{
	int i, j, k; //c[m][n]=a[m][l]*b[l][n]

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			c[i][j] = 0;
			for (k = 0; k < l; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
}

void arrymul2(double a[][N2], double b[][N2], double c[][N2], int m, int l, int n) //两个矩阵的乘法
{
	int i, j, k; //c[m][n]=a[m][l]*b[l][n]

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			c[i][j] = 0;
			for (k = 0; k < l; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
}

void arrycon(double a[], double b[][N1], int m, int n) //矩阵一维变二维
{
	int i, j, k;

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			b[i][j] = 1;
			for (k = 0; k < j; k++)
				b[i][j] *= a[i];
		}
}
void arrycon2(double a[], double b[][N2], int m, int n) //矩阵一维变二维
{
	int i, j, k;

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			b[i][j] = 1;
			for (k = 0; k < j; k++)
				b[i][j] *= a[i];
		}
}

void arrymuls(double a[][N1], double b[], double c[], int m, int n)
{	//由于一维数组按行存放，故乘列向量单独写
	int i, j;

	for (i = 0; i < m; i++) {
		c[i] = 0;
		for (j = 0; j < n; j++)
			c[i] += a[i][j] * b[j];
	}
}
void arrymuls2(double a[][N2], double b[], double c[], int m, int n)
{	//由于一维数组按行存放，故乘列向量单独写
	int i, j;

	for (i = 0; i < m; i++) {
		c[i] = 0;
		for (j = 0; j < n; j++)
			c[i] += a[i][j] * b[j];
	}
}



//学习模式函数
void mode_learn(void)
{
	u16 i;
	u8 j;
	char flag = 0;

	//float res7;
	OLED_Clear();//清屏
	for (j = 0; j < N1; j++)
	{



		//OLED_ShowString(2,0,"first 1-15",16);
		/****************/
		OLED_ShowCHinese(20, 0, 13); //
		OLED_ShowCHinese(36, 0, 7); //
		OLED_ShowCHinese(52, 0, 8); //
		OLED_ShowCHinese(68, 0, 9); //学习模式

		OLED_ShowCHinese(2, 3, 23); //已
		OLED_ShowCHinese(18, 3, 11); //放
		OLED_ShowCHinese(34, 3, 12); //入

		OLED_ShowCHinese(2, 6, 3); //3
		OLED_ShowCHinese(18, 6, 4); //4
		OLED_ShowCHinese(34, 6, 5); //5
		OLED_ShowString(50, 6, ":", 16); //电容值
		/*已放入*/
		OLED_ShowString(50, 3, ":", 16);
		OLED_ShowNum(66, 3, ((int)(y20[j])), 3, 16);
		OLED_ShowCHinese(98, 3, 1); //入


		while (flag == 0)
		{

			for (i = 0; i < 500; i++)
			{
				res5 = FDC2214_read_CHx(0);
				cap_num_hard[i] = res5;
			}
			cap_num_hard_end();
			res5 = end_ping_jun();
			OLED_ShowNum(70, 6, (res5 * 1000), 6, 16);
			//res7 = (1 / res5) * 100;
			if (KEY3 == 0)
			{
				delay_ms(5);
				if (KEY3 == 0)
				{
					BEEP = 1;
					flag = 1;
					x20[j] = (1 / res5) * 100;
					while (!KEY3);
					OLED_ShowString(111, 0, "OK", 16);
					delay_ms(300);
				}
				delay_ms(30);
				OLED_ShowString(111, 0, "  ", 16);
				BEEP = 0;
			}
		}


		flag = 0;
		if (j == 17)
			paper_17 = res5;
	}

	x60[0]=x20[14];
	x60[1]=x20[15];
	x60[2]=x20[16];
	x60[3]=x20[17];
	x60[4]=x20[18];
	x60[5]=x20[19];
	
	for (j = 6; j < N2; j++)
	{

		OLED_ShowCHinese(20, 0, 13); //
		OLED_ShowCHinese(36, 0, 7); //
		OLED_ShowCHinese(52, 0, 8); //
		OLED_ShowCHinese(68, 0, 9); //学习模式

		OLED_ShowCHinese(2, 3, 23); //已
		OLED_ShowCHinese(18, 3, 11); //放
		OLED_ShowCHinese(34, 3, 12); //入

		OLED_ShowCHinese(2, 6, 3); //3
		OLED_ShowCHinese(18, 6, 4); //4
		OLED_ShowCHinese(34, 6, 5); //5
		OLED_ShowString(50, 6, ":", 16); //电容值
		/*已放入*/
		OLED_ShowString(50, 3, ":", 16);
		OLED_ShowNum(66, 3, ((int)(y60[j])), 3, 16);
		OLED_ShowCHinese(98, 3, 1); //入


		while (flag == 0)
		{

			for (i = 0; i < 500; i++)
			{
				res5 = FDC2214_read_CHx(0);
				cap_num_hard[i] = res5;
			}
			cap_num_hard_end();
			res5 = end_ping_jun();		
			OLED_ShowNum(70, 6, (res5 * 1000), 6, 16);	
			if (KEY3 == 0)
			{
				delay_ms(5);
				if (KEY3 == 0)
				{
					BEEP = 1;
					flag = 1;
					x60[j] = (1 / res5) * 100;
					//l++;
					while (!KEY3);
					OLED_ShowString(111, 0, "OK", 16);

					delay_ms(300);
				}
				delay_ms(30);
				OLED_ShowString(111, 0, "  ", 16);
				BEEP = 0;
			}
		}
		flag = 0;

	}

	
	nihe();
	nihe2();
	SPI_Flash_Write((u8*)kn.ku,100,N2*8);// 写入外部flash
	mode_flag = 0;
	OLED_Clear();

}



//判断纸张数目函数
double check_paper_num(double bb)
{
	double a, b, c, d, e, f, g;
	double yy, xx;
	//SPI_Flash_Read(kn.ku,100,N2*8);
	delay_ms(10);
	xx = (1 / bb) * 100;
	
	if( bb > paper_17 )
	//if( xx <((x20[9]+x60[0])/2) )
	{
		a = kd[5];
		b = kd[4];
		c = kd[3];
		d = kd[2];
		e = kd[1];
		f = kd[0];
		yy = a * xx * xx * xx * xx * xx + b * xx * xx * xx * xx + c * xx * xx * xx + d * xx * xx + e * xx + f;
	}
	else
	{
		a = kn.k2[6];
		b = kn.k2[5];
		c = kn.k2[4];
		d = kn.k2[3];
		e = kn.k2[2];
		f = kn.k2[1];
		g = kn.k2[0];
		yy = a * xx * xx * xx * xx * xx *xx+ b * xx * xx * xx * xx*xx + c * xx * xx * xx *xx+ d * xx * xx*xx + e * xx*xx+ f*xx+g;
	}

	return yy;

}
//初始界面函数
void mode_start(void)
{



	OLED_ShowCHinese(25, 0, 0); //
	OLED_ShowCHinese(41, 0, 1); //
	OLED_ShowCHinese(57, 0, 21); //
	OLED_ShowCHinese(73, 0, 22); //
	OLED_ShowCHinese(89, 0, 18); //纸张计数器
	OLED_ShowString(2, 4, "Key1:", 16);
	OLED_ShowCHinese(50, 4, 13); //
	OLED_ShowCHinese(66, 4, 7); //
	OLED_ShowCHinese(82, 4, 8); //
	OLED_ShowCHinese(98, 4, 9); //学习模式
	OLED_ShowString(2, 6, "Key2:", 16);


	OLED_ShowCHinese(50, 6, 14); //
	OLED_ShowCHinese(66, 6, 2); //
	OLED_ShowCHinese(82, 6, 8); //
	OLED_ShowCHinese(98, 6, 9); //学习模式




}

//测试模式函数
void mode_check(void)
{
	u16 i;
	u8 flag = 1;
	OLED_Clear();
	while (flag)
	{

		for (i = 0; i < 500; i++)
		{
			res5 = FDC2214_read_CHx(0);
			cap_num_hard[i] = res5;
		}
		cap_num_hard_end();
		res5 = end_zhong_zhi();

		paper_num = check_paper_num(res5);



		if (res5 <= 30)
		{
			BEEP = 1;
			delay_ms(2000);
			BEEP = 0;
			//OLED_ShowString(2,6,"kailu",16);
			OLED_ShowCHinese(2, 1, 15);  //
			OLED_ShowCHinese(16, 1, 16); // 短路
			OLED_ShowNum(40, 3, (0), 2, 16);    // 显示零张
			OLED_ShowCHinese(60, 3, 1);  //张
		
		}
		else
		{
			BEEP = 0;
			OLED_ShowCHinese(2, 1, 19);  //
			OLED_ShowCHinese(16, 1, 20); // 正常
		}





		if (KEY1 == 0)
		{
			delay_ms(5);
			{
				if (KEY1 == 0)
				{
					while (!KEY1);
					flag = 0;
					mode_flag = 0; ///////////////////////////
					OLED_Clear();
				}
			}
		}

		if (KEY3 == 0)
		{
			delay_ms(5);
			{
				if (KEY3 == 0)
				{
					while (!KEY3);
					TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设                        //////插入蜂鸣器及定时器

				}
			}
		}
	}

}
//冒泡排序函数
void mao_pao_pai_xu()               // 冒泡排序，对cap_num_hard[300]进行排序后期进行求平均和中值
{
	int i, j;
	float temp;
	for (i = 0; i < 499; i++)
	{
		for (j = 499 - i; j > 0; j--)
		{
			if (cap_num_hard[j] < cap_num_hard[j - 1])
			{
				temp = cap_num_hard[j];
				cap_num_hard[j] = cap_num_hard[j - 1];
				cap_num_hard[j - 1] = temp;
			}
		}
	}
}
//电容值处理函数
void cap_num_hard_end()
{
	u8 i = 0;
	mao_pao_pai_xu();
	for (i = 0; i < 30; i++)
	{
		cap_num_end[i] = cap_num_hard[i + 235];
	}
}
//最终电容值求中值
float end_zhong_zhi()
{
	float zhongzhi;
	zhongzhi = (cap_num_end[14] + cap_num_end[15]) / 2;
	return zhongzhi;
}
//最终电容值求平均
float end_ping_jun()
{
	u8 i;
	float add = 0;
	for (i = 0; i < 30; i++)
	{
		add = cap_num_end[i] + add;
	}
	return add / 30;
}




int main(void)
{

	delay_init();	    	 //延时函数初始
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	//uart_init(115200);	 	//串口初始化为9600
	//LED_Init();		  		//初始化与LED连接的硬件接口
	//WKUP_Init();			//初始化WK_UP按键，同时检测是否正常开机？
	FDC2214_Init();
	OLED_Init();//初始化LCD
	SPI_Flash_Init(); 
	BEEP_Init();
	KEY_Init();
	Time_Init( 7199, 29999);  //1.5s定时
	TIM_Cmd(TIM4, DISABLE);  //失能TIMx外设
	//paper_17 = 0.0;
	OLED_Clear();//清屏
	SPI_Flash_Read(kn.ku,100,N2*8);
	while (1)
	{
		if (KEY3 == 0)
		{
			delay_ms(5);
			if (KEY3 == 0)
			{
				while (!KEY3);
				mode_flag = 1;
			}
		}
		if (KEY1 == 0)
		{
			delay_ms(5);
			if (KEY1 == 0)
			{	while (!KEY1)
					mode_flag = 2;
			}
		}
		switch (mode_flag)
		{
		case 1: mode_learn(); break;
		case 2: mode_check(); break;
		case 0: mode_start(); break;
		}
	}


}


void TIM4_IRQHandler(void)   //TIM4中断
{
	int paper_num_int;
	float error;
	 float d;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源
	{
		paper_num_int = paper_num;
		error = paper_num - paper_num_int;
		//附加,测量距离, 
		 d = paper_num *0.085;
	     OLED_ShowNum(10, 6, (d*1000), 2, 16);
		 OLED_ShowString(60, 6, "uM", 16);
		if (error >= 0.5)
		{
			OLED_ShowNum(40, 3, (paper_num_int + 1), 2, 16);
			OLED_ShowCHinese(60, 3, 1); //张
		}
		else
		{
			OLED_ShowNum(40, 3, paper_num_int, 2, 16);
			OLED_ShowCHinese(60, 3, 1); //张
		}	
		BEEP = 1;
		delay_ms(300);
		BEEP = 0;
		TIM_Cmd(TIM4, DISABLE);  //不使能TIMx外设
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源


	}
}

