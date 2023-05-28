#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int

uchar code Tab0[]=		//定义数码管数组，没有小数点
{
	0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f
};
uchar code Tab1[]=		//定义数码管数组，有小数点
{	
	0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef
};

sbit K1=P3^2;			//
sbit K2=P3^3;			//
sbit K3=P3^4;			//


void Display1(); 		//秒表显示函数
void Init(); 			//中断初始化函数
void KEY_MiaoBiao();	//检测秒表操作按键
void KEY_Time_Set();	//检测时间设置按键
void delay_ms(unsigned int ms);


typedef struct
{
	uchar Miao_Biao0;
	uchar Miao_Biao1;
	uchar Miao_Biao2;
	uchar Miao_Biao3;
	uchar Miao_Biao4;

}MIAOBIAO;

MIAOBIAO MiaoBiao;
void main()
{
	Init();										//中断初始化
	while(1)									//死循环
		{
			
			Display1();	 		//显示秒表
			KEY_MiaoBiao();		//扫描秒表操作
							
		}
}

void KEY_MiaoBiao()								//检测秒表按键操作
{
	
	if(K2==0)
	{
		delay_ms(10);
		if(K2==0)
		{
			TR0=~TR0;			//K2每按一次TR1取反，暂停或开始定时器1，达到暂停或开始秒表的目的
			while(!K2);
		}
	}
	if(K3==0)
	{
		delay_ms(10);
		if(K3==0)	 				//当K3按下时秒表所有数据清零，并停止定时器1
		{
			if(TR0 == 0)	//只有在跑秒停止时才可以清零
			{
				TR0=0;	 			//停止定时器1
				MiaoBiao.Miao_Biao0=0; 		//清零数据
				MiaoBiao.Miao_Biao1=0;
				MiaoBiao.Miao_Biao2=0;
				MiaoBiao.Miao_Biao3=0;
				MiaoBiao.Miao_Biao4=0;
				while(!K3);
			}
		}
	}
	if(K1==0)
	{
		delay_ms(10);
		if(K1==0)
		{
			if(TR1 == 0)		//停止跑秒时翻看计次记录
			{
								
			}
			else						//跑秒时计次
			{
							
			}
		}
						
	}
		
}


	

void Time1() interrupt 1  			//定时器1函数
{
	TH0 = 0xFC;  // 设置定时器初值
  TL0 = 0x18;

	MiaoBiao.Miao_Biao0++;
	if(MiaoBiao.Miao_Biao0==100)				//以下为秒表数据处理
		{
			MiaoBiao.Miao_Biao0=0;
			MiaoBiao.Miao_Biao1++;	 		//Miao_Biao1每加1次为100ms，
			if(MiaoBiao.Miao_Biao1==10)
				{
					MiaoBiao.Miao_Biao1=0;
					MiaoBiao.Miao_Biao2++;
					if(MiaoBiao.Miao_Biao2==60)	
						{
							MiaoBiao.Miao_Biao2=0;
							MiaoBiao.Miao_Biao3++;
							if(MiaoBiao.Miao_Biao3==60)
								{
									MiaoBiao.Miao_Biao3=0;
									MiaoBiao.Miao_Biao4++;
									if(MiaoBiao.Miao_Biao4==10)
										MiaoBiao.Miao_Biao4=0;
								}
						}
				}
		}
}

void Init()	 						//中断初始化函数
{
	TMOD |= 0x01;   // 定时器0工作在模式1：16位自动重载计数模式
  TH0 = 0xFC;     // 设置定时器初值，计算公式：65535-4500+1=0xFC18
  TL0 = 0x18;     // 时钟周期为1/11.0592MHz ≈ 90.5ns，计时4500个时钟周期等于1ms
  ET0 = 1;        // 开启定时器0中断
  TR0 = 0;        // 启动定时器0
  EA = 1;         // 开启总中断
}


void Display1()			//显示秒表
{
	P0=Tab0[MiaoBiao.Miao_Biao1%10];		//显示1/10秒的个位
	P1=0xdf;					//段选
	delay_ms(1);					//延时
	P0=0X00;					//消隐

	P0=Tab1[MiaoBiao.Miao_Biao2%10];		//显示秒的个位，需要加上小数点做分隔符
	P1=0xef;					//段选
	delay_ms(1);					//延时
	P0=0X00;					//消隐

	P0=Tab0[MiaoBiao.Miao_Biao2/10];		//显示秒的十位
	P1=0xf7;					//段选
	delay_ms(1);					//延时
	P0=0X00;					//消隐

	P0=Tab1[MiaoBiao.Miao_Biao3%10];		//显示分的个位，需要加上小数点做分隔符
	P1=0xfb;					//段选
	delay_ms(1);					//延时
	P0=0X00;					//消隐

	P0=Tab0[MiaoBiao.Miao_Biao3/10];		//显示分的十位
	P1=0xfd;					//段选
	delay_ms(1);					//延时
	P0=0X00;					//消隐

	P0=Tab1[MiaoBiao.Miao_Biao4%10];		//显示时的个位，需要加上小数点做分隔符
	P1=0xfe;					//段选
	delay_ms(1);					//延时
	P0=0X00;					//消隐
}


void delay_ms(unsigned int ms)  // 延时函数
{
  unsigned char i, j;
  while (ms--)
  {
    for (i = 0; i < 12; i++)
    {
      for (j = 0; j < 250; j++);
     }
  }
}
