#include<lpc21xx.h>
#include<string.h>
#include "can_header.h"
#define LED 1<<20;
#define u8 unsigned int
typedef struct can_tx
{
u8 id;
u8 rtr;
u8 dlc;
u8 data1;
u8 data2;
}tx;

void uart(void);
void tx_uart(unsigned char );
unsigned char rx_uart(void);
void uart(void);
void can_init(void);
void can_tx(tx);
int i=0;
unsigned char ch[20];
int main()
{
	IODIR0|=LED;
	IOSET0|=LED;
	tx m1;
	can_init();
	LCD_INIT();
	uart();
	m1.id=0x1AF;
	m1.rtr=0;
	m1.dlc=4;
	
	while(1)
	{
		while(i<12)
		{
			ch[i]=rx_uart();
			i++;
		}
		ch[i]='\0';
		i=0;
		tx_uart(ch);
		if(strcmp(tx_uart,"00000000000")==0)
		{
			IOCLR0=LED;
			m1.data1=1;
			m1.data2=0;
			tx(m1);
		}
		else
		{
			m1.data1=2;
			m1.data2=0;
			tx(m1);
			IOSET0=LED;
		}
	}
	
}


void uart(void)
{
	PINSEL0|=5<<16;
	U1LCR=0X83;
	U1DLL=32;
	U1DLM=0;
	U1LCR=0X03;
}

void tx_uart(unsigned char n)
{
	while(((U1LSR>>5)&1)==0);
	U1THRE=n;
}
unsigned char rx_uart(void)
{
	while((U1LSR&1)==0);
	return U1RBR;
}

void can_init(void)
{
	PINSEL1|=0x14000;
	VPBDIV=1;
	C2MOD=0X01;
	AFMR=0X02;
	C2BTR=0X001C001D;
	C2MOD=0X00;
}

void can_tx(tx m1)
{
	C2TID1=m1.id;
	C2TFI1=(m1.dlc<<16);
	if(m1.rtr==0)
	{
		C2TFI1&=~(1<<30);
		C2TDA1=m1.data1;
		C2TDB1=m1.data2;
	}
	else
		C2TFI1|=(1<<30);
	C2CMR=(1<<0)|(1<<5);
	while((C2GSR&(1<<3))==0);
}
