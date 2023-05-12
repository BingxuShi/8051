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

void Delay(uchar x);	//延时函数
void Display1(); 		//秒表显示函数
void Init(); 			//中断初始化函数
void KEY_MiaoBiao();	//检测秒表操作按键
void KEY_Time_Set();	//检测时间设置按键

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
					Delay(10);
					if(K2==0)
						{
							TR1=~TR1;			//K2每按一次TR1取反，暂停或开始定时器1，达到暂停或开始秒表的目的
							while(!K2);
						}
				}
			if(K3==0)
				{
					Delay(10);
					if(K3==0)	 				//当K3按下时秒表所有数据清零，并停止定时器1
						{
							if(TR1 == 0)	//只有在跑秒停止时才可以清零
							{
								TR1=0;	 			//停止定时器1
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
					Delay(10);
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


	

void Time1() interrupt 3  			//定时器1函数
{
	TH1=(65536-50000)/256;			//重装初值
	TL1=(65536-50000)%256;

	MiaoBiao.Miao_Biao0++;
	if(MiaoBiao.Miao_Biao0==2)				//以下为秒表数据处理
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
	EA=1;		 					//开总中断
	TMOD=0X11;			  			//定时器工作方式选择，定时器1选择第1种工作方式
	
	TH1=(65536-50000)/256;			//定时器1装初值，定时50ms
	TL1=(65536-50000)%256;
	ET1=1;				  			//开定时器1开关
	TR1=0;							//关定时器1小开关
}


void Display1()			//显示秒表
{
	P0=Tab0[MiaoBiao.Miao_Biao1%10];		//显示1/10秒的个位
	P1=0xdf;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[MiaoBiao.Miao_Biao2%10];		//显示秒的个位，需要加上小数点做分隔符
	P1=0xef;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab0[MiaoBiao.Miao_Biao2/10];		//显示秒的十位
	P1=0xf7;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[MiaoBiao.Miao_Biao3%10];		//显示分的个位，需要加上小数点做分隔符
	P1=0xfb;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab0[MiaoBiao.Miao_Biao3/10];		//显示分的十位
	P1=0xfd;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[MiaoBiao.Miao_Biao4%10];		//显示时的个位，需要加上小数点做分隔符
	P1=0xfe;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐
}

void Delay(uchar x)		  		//延时
{
	uchar i,j;
	for(i=x;i>0;i--)
		for(j=110;j>0;j--);
}


