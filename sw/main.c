#include <reg52.h>
#include <stdio.h>


#define LCD_DATA_PORT P2
#define TIMER0_RELOAD_VALUE 65536 - (12000000 / 12 / 100) // 为10ms计算重载值

sbit RS = P1^0;
sbit RW = P1^1;
sbit EN = P1^2;
sbit StaStoBon = P3^3;
sbit RestBon = P3^4;
sbit CounBon = P3^2;
unsigned char counter = 0;
unsigned int ms_elapsed = 0;
unsigned int seconds = 0;
unsigned int minutes = 0;
unsigned int hours = 0;

unsigned int lap[3][4] = { 0 };

void delay_ms(unsigned int ms)
{
    unsigned int i, j;

    for(i = 0; i < ms; i++)
    {
        for(j = 0; j < 1000; j++) 
        {
            // 空循环，什么也不做
        }
    }
}

void lcd_write_command(unsigned char command) {
    RS = 0;
    RW = 0;
    LCD_DATA_PORT = command;
    EN = 1;
    delay_ms(1);
    EN = 0;
    delay_ms(1);
}

void lcd_write_data(unsigned char data1) {
    RS = 1;
    RW = 0;
    LCD_DATA_PORT = data1;
    EN = 1;
    delay_ms(1);
    EN = 0;
    delay_ms(1);
}

void lcd_init() {
    delay_ms(15);
    lcd_write_command(0x38); // 设置为16x4显示模式，8位数据总线，2行显示，5x8点阵字符
    lcd_write_command(0x0c); // 开启显示，关闭光标和光标闪烁
    lcd_write_command(0x06); // 光标右移，字符不移动
    lcd_write_command(0x01); // 清屏
}

void lcd_clear() {
    lcd_write_command(0x01);
}

void lcd_set_cursor(unsigned char row, unsigned char col) {  //设置光标位置
    unsigned char address;
    if (row == 0) {
        address = col;
    } else if (row == 1) {
        address = col + 0x40;
    } else if (row == 2) {
        address = col + 0x10;
    } else { // row == 3
        address = col + 0x50;
    }
    lcd_write_command(0x80 | address);
}

void lcd_write_string(char *str) {
    while (*str) {
        lcd_write_data(*str++);
    }
}

void timer0_init() {
	TMOD |= 0x01; // 设置定时器0为模式1（16位定时器/计数器）
    TH0 = (TIMER0_RELOAD_VALUE >> 8) & 0xFF; // 设置高字节重载值
    TL0 = TIMER0_RELOAD_VALUE & 0xFF; // 设置低字节重载值
    ET0 = 1; // 使能定时器0中断
    TR0 = 0; // 启动定时器0
	EA=1;
	
} 

void timer0_isr() interrupt 1{
		TH0 = (TIMER0_RELOAD_VALUE >> 8) & 0xFF; // 重新设置高字节重载值
    TL0 = TIMER0_RELOAD_VALUE & 0xFF; // 重新设置低字节重载值
		ms_elapsed++;
		if(ms_elapsed == 100){
			ms_elapsed = 0;				
			seconds++;
			if(seconds == 60){
				seconds = 0;
				minutes++;
				if(minutes == 60){
					minutes = 0;
					hours++;
				}
			}
		}
	
}
	
   



unsigned char i, j;
void key_scan_task(){
	if (StaStoBon == 0) { // Start/Stop button
		TR0 = !TR0;
    while (StaStoBon == 0);
  }
  if (RestBon == 0) { // Reset button
    counter = ms_elapsed = seconds = minutes = hours = 0; //秒表清零
		for (i = 0; i < 3; i++) {                             //计次清零
			for (j = 0; j < 4; j++) {
        lap[i][j] = 0;
			}
		}
    while (RestBon == 0);
  }
  if (CounBon == 0) { // Lap button
		if(counter < 3){
			if(TR0)         //停止跑秒不记次
			{
				lap[counter][0] = hours;
				lap[counter][1] = minutes;
				lap[counter][2] = seconds;
				lap[counter][3] = ms_elapsed;
				counter++;
			}
			
		}
		
		
    while (CounBon == 0);
  }
}

void display_task(){
	
	char tempbuff[16];
	sprintf(tempbuff,"  %02d:%02d:%02d:%02d",hours,minutes,seconds,ms_elapsed);
	lcd_set_cursor(0, 0);
	lcd_write_string(tempbuff);
}

void display1_task(){
	
	char tempbuff[16];
	sprintf(tempbuff,"1 %02d:%02d:%02d:%02d",lap[0][0],lap[0][1],lap[0][2],lap[0][3]);
	lcd_set_cursor(1, 0);
	lcd_write_string(tempbuff);
	sprintf(tempbuff,"2 %02d:%02d:%02d:%02d",lap[1][0],lap[1][1],lap[1][2],lap[1][3]);
	lcd_set_cursor(2, 0);
	lcd_write_string(tempbuff);
	sprintf(tempbuff,"3 %02d:%02d:%02d:%02d",lap[2][0],lap[2][1],lap[2][2],lap[2][3]);
	lcd_set_cursor(3, 0);
	lcd_write_string(tempbuff);

}

void main() {
		timer0_init();
    lcd_init();
	while(1){
		display_task();
		display1_task();
		key_scan_task();
	}
}
