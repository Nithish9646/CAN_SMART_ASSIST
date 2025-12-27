#include<lpc21xx.h>
#include<string.h>
#include "smart_header.h"
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


void uart_init(void);
void tx_uart(unsigned char );
unsigned char rx_uart(void);
void can_init(void);
void uart_string(unsigned char *);

void can_tx(tx);

unsigned char ch[12],i=0,n;

int main()
{
	tx m1;
	IODIR0|=LED;
	IOSET0|=LED;

	can_init();
	LCD_INIT();
	uart_init();

	m1.id=0x1AF;
	m1.rtr=0;
	m1.dlc=4;
	while(1)
	{
		while(i<12)
		{
		n=rx_uart();
			ch[i++]=n;
			
			tx_uart(n);

		}
		uart_string(ch);
		STRING(ch);

		i=0;

		if(strcmp("06003B4E2E5D",(const char *)ch)==0)
		{
			IOCLR0=LED;
			m1.data1=1;
			m1.data2=0;
			can_tx(m1);
		}
		else
		{
			m1.data1=2;
			m1.data2=0;
			can_tx(m1);
			IOSET0=LED;
		} 
	}
}

void uart_init(void)
{
	PINSEL0|=0x05;
	U0LCR=0X83;
	U0DLL=32;
	U0DLM=0;
	U0LCR=0X03;
}

void tx_uart(unsigned char n)
{
	while(((U0LSR>>5)&1)==0);
	U0THR=n;
}

unsigned char rx_uart(void)
{
	while((U0LSR&1)==0);
	return U0RBR;
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

void uart_string(unsigned char *p)
{
	while(*p)
	{
		tx_uart(*p++);
	}
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

