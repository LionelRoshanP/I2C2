//Hear Beat Monito 8051
#include <at89x51.h> 
#include <string.h> 
#define lcdport P2  //Port 2 of 8952
sbit rs = P3^5; 	  // Port 3 pin 5  Register Select for writing data to lcd
sbit rw = P3^6; 		// Port 3 pin 6 bit data type R/W LCD
sbit en = P3^7; 	  // Port 3 pin 7 Enable pin for starting enable mode
unsigned char sec,sec100;
unsigned int bt,tick,r,bpm,bt1;	//global variables
void lcdinit(void);
void lcdcmd(unsigned char);
void lcddata(unsigned char);
void send_string(unsigned char *s);
void msdelay(unsigned int);

void lcdinit(void)
{
	lcdcmd(0x38);	//use two line display 5x7
	msdelay(10);
	lcdcmd(0x0f);	//display on and curson on blinking
	msdelay(10);
	lcdcmd(0x01);	//clear screen
	msdelay(10);
	lcdcmd(0x81);	//force cursor to 1 line
	lcdcmd(0x3c);	//activate second line
}
void extrint (void) interrupt 0 // external Interrupt to detect the heart pulse
{
	bt=tick; // number of ticks
	tick=0;  // reset for counting
}
void timer0 (void) interrupt 1 using 1 // Timer 0 for one second time
{
	TH0 = 0xdc;     //Register counter reset for Ssc/100 at crystal 11.0592MHz
	sec100++;      // incremented every hundreth of sec at crystal 11.0592MHz
	tick++;        // counts ever hundreth of a sec till the next interrupt occurs
	if(tick>=3500) // tick are limited to less than 255 for valid calculation
	{
		tick=0;  
	} 
	if(sec100 >=100) // 1 sec elapsed when  sec100 reaches 100 
	{
		sec++;
		sec100=0;
	}
}

void main()
{	
	
	
	P0=0xff;	// processor port init
	P1=0xff;
	P2=0xff;
	P3=0xff;

	
	EA = 1;		// cpu register commands
	TMOD = 0x21;
	IT0 = 1;
	EX0 = 1;
	ET0 = 1;
	TR0 = 1;
	lcdinit();
	
	send_string("Heart beat ");
	msdelay(5000);
	sec=0;
 	bpm=0;
	bt=0;	
	while(1)
	{
		if(sec>=1)
		{	
			sec=0;
			lcdcmd(0x01);
			lcdcmd(0x02);			
			send_string("Pulse ");
			if(bt>6)
			{
				bt1=6000/bt;
				r=(bt1/100);
				lcddata(r+0x30);
				bt1=bt1%100;
	
				r=(bt1/10);
				lcddata(r+0x30);
				bt1=bt1%10;

				r=bt1;
				lcddata(r+0x30);
				send_string(" Pulse ");
				//msdelay(500);
			}
			else
			{
				//send_string(" Reading ");
			}
		}
		
	}
}
void lcdcmd(unsigned char value)
{
	lcdport=value;
	msdelay(100);
	rs=0;
	msdelay(100);
	rw=0;
	msdelay(100);
	en=1;
	msdelay(100);
	en=0;
}
void lcddata(unsigned char value) //data to lcd
{
	rs=1;
	lcdport=value;
	en=1;
	rw=0;
	msdelay(100);
	en=0;
	rs=0;
}
void msdelay(unsigned int i)	//delay loop for mili seconds
{
	while(i --);
}
void send_string(unsigned char *s)	//send string to LCD
{
	unsigned char l,i;
	l = strlen(s); 
	for(i=1;i <=l;i++)
	{
		lcddata(*s); // Send one char at a time to lcd
		s++;
	}
}
