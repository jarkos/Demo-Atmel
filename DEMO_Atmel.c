#include "AT91SAM9263.h"
#include <string.h> /* do strlen */
#include <stdlib.h>

/* ############################################################################### */
// Zmienne pomocniczne
char maska[9]="11111111";
char maska2[9]="10101010";
char maska3[9]="01010101";
int podstawowa=100;
int i=0;
char buffor;
unsigned int at91c_pit_period = 0; // interrupt progres
unsigned int at91c_pit_temp   = 0; // temp for reading PITC_PISR

/* ############################################################################### */
/* WYSWIETLACZ 7SEG */
	// cyfry 0-9 , kropka , all
	unsigned int segments[] =
	{
		   /*0*/ AT91C_PIO_PB25 | AT91C_PIO_PB24 | AT91C_PIO_PB22 | AT91C_PIO_PB21 | AT91C_PIO_PB20 | AT91C_PIO_PB27 , 					//0
		   /*1*/ AT91C_PIO_PB24 | AT91C_PIO_PB22 ,								       					//1
		   /*2*/ AT91C_PIO_PB25 | AT91C_PIO_PB24 | AT91C_PIO_PB26 | AT91C_PIO_PB20 | AT91C_PIO_PB21 ,		       					//2
		   /*3*/ AT91C_PIO_PB25 | AT91C_PIO_PB24 | AT91C_PIO_PB26 | AT91C_PIO_PB22 | AT91C_PIO_PB21 ,		       					//3
		   /*4*/ AT91C_PIO_PB27 | AT91C_PIO_PB26 | AT91C_PIO_PB24 | AT91C_PIO_PB22 ,				       					//4
		   /*5*/ AT91C_PIO_PB25 | AT91C_PIO_PB27 | AT91C_PIO_PB26 | AT91C_PIO_PB22 | AT91C_PIO_PB21 ,		       					//5
		   /*6*/ AT91C_PIO_PB25 | AT91C_PIO_PB27 | AT91C_PIO_PB20 | AT91C_PIO_PB21 | AT91C_PIO_PB22 | AT91C_PIO_PB26 , 					//6
		   /*7*/ AT91C_PIO_PB25 | AT91C_PIO_PB24 | AT91C_PIO_PB22 ,						       					//7
		   /*8*/ AT91C_PIO_PB25 | AT91C_PIO_PB24 | AT91C_PIO_PB22 | AT91C_PIO_PB21 | AT91C_PIO_PB20 | AT91C_PIO_PB27 | AT91C_PIO_PB26 , 		//8
		   /*9*/ AT91C_PIO_PB25 | AT91C_PIO_PB24 | AT91C_PIO_PB26 | AT91C_PIO_PB27 | AT91C_PIO_PB22 | AT91C_PIO_PB21 ,					//9
		/*[DP]*/ AT91C_PIO_PB23 ,															//10
		 /*ALL*/ AT91C_PIO_PB25 | AT91C_PIO_PB24 | AT91C_PIO_PB22 | AT91C_PIO_PB21 | AT91C_PIO_PB20 | AT91C_PIO_PB27 | AT91C_PIO_PB26 | AT91C_PIO_PB23  //11
	};

/* ############################################################################### */

/* Porty do wyswietlacza / linijki led */

	unsigned int com_port[] =
	{
		 /*com1*/  AT91C_PIO_PB28 , 		   //0			
		 /*com2*/  AT91C_PIO_PB30 , 		   //1
	          /*ALL*/  AT91C_PIO_PB28 | AT91C_PIO_PB30, //2
		/*LED*/	   AT91C_PIO_PB16		   //3
	};
	
 
/* ############################################################################### */
// TImer 


void pit_interrupt() 
{  
   at91c_pit_temp = *(AT91C_PITC_PISR);
   at91c_pit_period++;
   volatile int reset = AT91C_BASE_PITC->PITC_PIVR; 
};

void pit_initialize(int pit_time) 
{
	// set handler to interrupt function in source vector register
	AT91C_BASE_AIC->AIC_SVR[AT91C_ID_SYS] = (unsigned int)pit_interrupt;
	// turn on PIT and set interrupt time
	AT91C_BASE_PITC->PITC_PIMR = AT91C_PITC_PITEN | AT91C_PITC_PITIEN | (AT91C_PITC_PIV & pit_time);
	// set interrupt for interrupt function in source vector register
	*(AT91C_AIC_IECR) = 2; 
};

/* Linijka LED */

unsigned int led[] =	
{
	/*0*/ AT91C_PIO_PB20,
	/*1*/ AT91C_PIO_PB21,
	/*2*/ AT91C_PIO_PB22,
	/*3*/ AT91C_PIO_PB23,
	/*4*/ AT91C_PIO_PB24,
	/*5*/ AT91C_PIO_PB25,
	/*6*/ AT91C_PIO_PB26,
	/*7*/ AT91C_PIO_PB27,
	/*8*/ AT91C_PIO_PB20 | AT91C_PIO_PB21 | AT91C_PIO_PB22 | AT91C_PIO_PB23 | AT91C_PIO_PB24 | AT91C_PIO_PB25 | AT91C_PIO_PB26 | AT91C_PIO_PB27
};
/* ############################################################################### */	
	void wylacz_linijke()
	{
		*AT91C_PIOB_ODR = com_port[3] | led[8];
	};

/* ############################################################################### */	

/* Do przelaczania miedzy lewym i prawym wyswietlaczem 7seg + sztuczne opoznienie*/

	void lewy()
	{
     	 *AT91C_PIOB_CODR = com_port[0];
	 *AT91C_PIOB_SODR = com_port[1]; 
	};	

	void prawy()
	{
     	 *AT91C_PIOB_CODR = com_port[1];
	 *AT91C_PIOB_SODR = com_port[0]; 
	};

	void delay()
	{
		at91c_pit_period = 0;
		pit_initialize(62500);// co 0.01 sek
	}
/* ############################################################################### */	

/* Funkcja gaszaca obie czesci wyswietlacza 7segmentowego */

void gaszenie()
{
	 *AT91C_PIOB_CODR = com_port[2] | segments[11];
};
	
/* ############################################################################### */	

/* Funkcja zapalajaca okreslona liczbe podana (np. 75) na wyswietlaczu 7seg */


void zapal(int liczba)
{

	gaszenie();
	wylacz_linijke();
	int liczbalewy=liczba/10;
	int liczbaprawy=liczba%10;
	while(1)
	{
		if(buffor==AT91C_BASE_DBGU->DBGU_RHR)
		{
			 *AT91C_PIOB_PER  = com_port[2] | segments[11];
			 *AT91C_PIOB_OER  = com_port[2] | segments[11];
			lewy();
			gaszenie();
			 *AT91C_PIOB_SODR = com_port[0] | segments[liczbaprawy];
			 *AT91C_PIOB_CODR = com_port[1];
			delay();
			prawy();
			gaszenie();
			 *AT91C_PIOB_SODR = com_port[1] | segments[liczbalewy];
			 *AT91C_PIOB_CODR = com_port[0];
			delay();
		}
		else
		{break;}
	}

*AT91C_PIOB_ODR = com_port[2] | segments[11];
};




/* ############################################################################### */	

/* Funkcja wylaczajaca wyswietlacz 7segmentowy */

void wylacz7seg()
{
	*AT91C_PIOB_ODR = com_port[2] | segments[11];
}

/* ############################################################################### */	

/* Funkcja zapalajaca odpowiednia kombinacje diod LED z linijki z maski binarnej */

        void linijka(char maska[])
        {

		wylacz7seg();
	 	   int diody[8];
	 	   int i=0;
	 	   for(i=0;i<8;i++)
		    {
	 	       diody[i] = maska[i] - '0';
		    }

		*AT91C_PIOB_PER= com_port[3] | led[8];
		*AT91C_PIOB_OER= com_port[3] | led[8];
		*AT91C_PIOB_CODR = com_port[3] | led[8];

		void zapal_diode(int nr_diody)
		{
			*AT91C_PIOB_SODR=led[nr_diody];
		};

		int j=0;
		    for(j=0;j<8;j++)
		    {
			if(diody[j]==1)
		    	{zapal_diode(j);}
		    }
        };
 

/* ############################################################################### */
//Miganie diodami LED0 i LED1

void migaj(int podstawowa)
{
at91c_pit_period = 0;
pit_initialize(62500); // PIT_VALUE = 6249999 - 1 sekunda, czyli 100 odliczone do 62500 to 1 SEK!
// LED1
*AT91C_PIOB_PER = AT91C_PIO_PB8;
*AT91C_PIOB_OER = AT91C_PIO_PB8;


// Obs³uga LED2
*AT91C_PIOC_PER = AT91C_PIO_PC29;
*AT91C_PIOC_OER = AT91C_PIO_PC29;

*AT91C_PIOC_CODR = AT91C_PIO_PC29;
*AT91C_PIOB_CODR = AT91C_PIO_PB8;

			do
			{
			*AT91C_PIOC_SODR = AT91C_PIO_PC29;
			*AT91C_PIOB_SODR = AT91C_PIO_PB8;	
			}while(at91c_pit_period<podstawowa);
			at91c_pit_period = 0;
			do
			{
			*AT91C_PIOC_CODR = AT91C_PIO_PC29;
			*AT91C_PIOB_CODR = AT91C_PIO_PB8;
			}while(at91c_pit_period<podstawowa);
			at91c_pit_period = 0;

};
/* ############################################################################### */
//Buzzer

void buzzer(int value)
{
	*(AT91C_PIOB_PER) = AT91C_PIO_PB29;
	*(AT91C_PIOB_OER) = AT91C_PIO_PB29;
	at91c_pit_period = 0;
	pit_initialize(value);
	int i=0;
	while(i<1000) // D³ugoœæ trwania dzwiêku
	{
	*(AT91C_PIOB_SODR) = AT91C_PIO_PB29;
	*(AT91C_PIOB_CODR) = AT91C_PIO_PB29;
	i++;
	while(at91c_pit_period < 1)
	{}
	at91c_pit_period = 0;
	}
	*(AT91C_PIOB_CODR) = AT91C_PIO_PB29;
};


/* ############################################################################### */
//OBS£UGA DBGU

void dbgu_print_ascii(const char *string);

void dbgu_read_ascii(void){
     AT91C_BASE_AIC->AIC_ICCR = (1<<AT91C_ID_SYS);
}

static void Open_DBGU (void)
{
	/* http://www.keil.com/dd/vtr/3817/8261.htm */
//	1. Wylacz przerwania do portu DBGU  
	AT91C_BASE_DBGU->DBGU_IDR = 0xFFFFFFFF;
//	2. Resetuj i wylacz odbiornik //RSTRXres reciever RXDIS res disable 
	AT91C_BASE_DBGU->DBGU_CR = AT91C_US_RSTRX | AT91C_US_RXDIS;
//	3. 	Resetuj i wylacz nadajnik //RSTTX res transmitter TXDIS transmitter disable 
	AT91C_BASE_DBGU->DBGU_CR = AT91C_US_RSTTX | AT91C_US_TXDIS;
//	4. Konfiguracja portów wejœcia/wyjœcia RxD i TxD DBGU  ASR/PDR
	AT91C_BASE_PIOC->PIO_ASR = AT91C_PC30_DRXD | AT91C_PC31_DTXD;
	AT91C_BASE_PIOC->PIO_PDR = AT91C_PC30_DRXD | AT91C_PC31_DTXD;
//	5. Konfiguracja szybkoœci transmisji portu szeregowego
//  Define the baud rate divisor register [BRGR = MCK / (115200 * CD)]
//	1e8/(16*115200) = 54
//	54 (DEC) = 0x36 (HEX)
	AT91C_BASE_DBGU->DBGU_BRGR = 0x36;
//	6. Konfiguracja trybu pracy (normalny bez parzystoœci) 8N1
	/* http://www.mikrocontroller.net/attachment/20627/USART-CODE.txt */
	AT91C_BASE_DBGU->DBGU_MR = AT91C_US_USMODE_NORMAL | AT91C_US_CHRL_8_BITS |  AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT;
//	7. Skonfiguruj przerwania jeœli s¹ wykonywane Open_DBGU_INT()

//	8. W³¹cz odbiornik,
	AT91C_BASE_DBGU->DBGU_CR = AT91C_US_RXEN;
//	9. W³¹cz nadajnik
	AT91C_BASE_DBGU->DBGU_CR = AT91C_US_TXEN;
}
/* ############################################################################### */
void dbgu_print_ascii(const char* string) {

    /* slajdy3.pdf, slajd 22 */

    int len = strlen(string), i = 0;

    for (i = 0; i < len; i++) 
	{
        while (!(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_TXRDY));
        AT91C_BASE_DBGU->DBGU_THR = string[i];
    	}

}

/* ############################################################################### */
void TestMigaj(int podstawowa)
{
//int a =100; // co  oko³o 1 sek 8*0,125sek przy 781250 


	while(1)
		{	
		if(buffor==AT91C_BASE_DBGU->DBGU_RHR)
			{
			migaj(podstawowa);
			}
		else {break;}
		*AT91C_PIOC_ODR = AT91C_PIO_PC29;
		*AT91C_PIOB_ODR = AT91C_PIO_PB8;
		}
}
/* ############################################################################### */

void WlaczDiody()
{
	// LED1
	*AT91C_PIOB_PER = AT91C_PIO_PB8;
	*AT91C_PIOB_OER = AT91C_PIO_PB8;

	// Obs³uga LED2
	*AT91C_PIOC_PER = AT91C_PIO_PC29;
	*AT91C_PIOC_OER = AT91C_PIO_PC29;
	while(1)
	{
		if(buffor==AT91C_BASE_DBGU->DBGU_RHR){
		*AT91C_PIOC_CODR = AT91C_PIO_PC29;
		*AT91C_PIOB_CODR = AT91C_PIO_PB8;
		}
		else {break;}
	}
}
/* ############################################################################### */
void WylaczDiody()
{
	// LED1
	*AT91C_PIOB_PER = AT91C_PIO_PB8;
	*AT91C_PIOB_OER = AT91C_PIO_PB8;

	// Obs³uga LED2
	*AT91C_PIOC_PER = AT91C_PIO_PC29;
	*AT91C_PIOC_OER = AT91C_PIO_PC29;

	while(1)
	{
		if(buffor==AT91C_BASE_DBGU->DBGU_RHR)
		{
			*AT91C_PIOC_SODR = AT91C_PIO_PC29;
			*AT91C_PIOB_SODR = AT91C_PIO_PB8;
		}
		else {
			*AT91C_PIOC_ODR = AT91C_PIO_PC29;
			*AT91C_PIOB_ODR = AT91C_PIO_PB8;
			break;

	      		}
	}
	
}
/* ############################################################################### */
void TestBuzzer(int wartosc)
{
while(1)
	{
	if(buffor==AT91C_BASE_DBGU->DBGU_RHR)
		{
		buzzer(wartosc);
		}
	else {break;}
	*(AT91C_PIOB_ODR) = AT91C_PIO_PB29;
	}
}

/* ############################################################################### */
void TestLinijka(char maska[])
{
	while(1)
	{
		if(buffor==AT91C_BASE_DBGU->DBGU_RHR){
		linijka(maska);
		}
		else{break;}
		wylacz_linijke();
	}
}
/* ############################################################################### */

void Menu()
{
	int wartosc_w=0;
	int wartosc_b=5000;
	int wartosc_migaj=100;
	int a;	
	char y[] = "######################### MENU ######################### \n\r1. Obsluga wyswietlacza\n\r\t'q' - Zwieksz liczbe o 1\n\r\t'w' - Zmniejsz liczbe o 1\n\r\t'a' - Zwieksz liczbe o 10\n\r\t's' - Zmniejsz liczbe o 10\n\r2. Miganie dwoch diod\n\r\t'r' - Zwieksz czestotliwosc\n\r\t't' - Zmniejsz czestotliwosc\n\r3. Wlaczenie dwoch diod\n\r4. Wylaczenie dwoch diod\n\r5. Obsluga buzzera\n\r\t'u' - Zwieksz czestotliwosc\n\r\t'i' - Zmniejsz czestotliwosc\n\r6. Obsluga linijki diod\n\r\t'b' - Dla maski 11111111 (wszystkie zapalone)\n\r\t'n' - Dla maski 10101010  (zapalona co druga)\n\r\t'm' - Dla maski 01010101 (zapalona co druga)\n\r7. Wylacz wszystko\n\n\rWYBIERZ OPCJE: \n\r";
	dbgu_print_ascii(y);
	char b;
		dbgu_read_ascii();		
while(1)
	{
		while(!((AT91C_BASE_DBGU->DBGU_CSR)&(AT91C_US_RXRDY))){}
		buffor=AT91C_BASE_DBGU->DBGU_RHR;
		b=buffor;
		do 
		{
			switch	(b)
			{	
				case 'q':		
					if( wartosc_w==99)  wartosc_w = 0;
					 wartosc_w=wartosc_w+1;			
					zapal( wartosc_w);
					dbgu_print_ascii(y);
					break;
				case 'w':		
					if( wartosc_w==0)  wartosc_w = 100;
					 wartosc_w=wartosc_w-1;			
					zapal( wartosc_w);
					dbgu_print_ascii(y);
					break;
				case 'a':		
					if( wartosc_w >= 90 && wartosc_w <= 99) zapal(wartosc_w=0);
					else
					{
						wartosc_w=wartosc_w+10;
						zapal( wartosc_w);
					}
					dbgu_print_ascii(y);		
					break;
				case 's':		
					if( wartosc_w >= 0 && wartosc_w <= 10) zapal(wartosc_w=0);
					else
					{
						wartosc_w=wartosc_w-10;
						zapal( wartosc_w);
					}
					dbgu_print_ascii(y);
					break;


				case '2':
					TestMigaj(podstawowa);
					dbgu_print_ascii(y);
					break;

				case 't': 
					if(wartosc_migaj<10)  wartosc_migaj = 10;
					wartosc_migaj=wartosc_migaj*2;			
					TestMigaj(wartosc_migaj);
					dbgu_print_ascii(y);
					break;

				case 'r': 
					if(wartosc_migaj<10)  wartosc_migaj = 10;
					wartosc_migaj=wartosc_migaj/2;			
					TestMigaj(wartosc_migaj);
					dbgu_print_ascii(y);
					break;

				case '3':
					WlaczDiody();
					dbgu_print_ascii(y);
					break;
				case '4':
					WylaczDiody();
					dbgu_print_ascii(y);
					break;

				case 'i':
					if(wartosc_b>=9000) TestBuzzer(wartosc_b = 9000);
					wartosc_b=wartosc_b+500; 
					TestBuzzer(wartosc_b);
					dbgu_print_ascii(y);
					break;

				case 'u': 
					if(wartosc_b<=1000) TestBuzzer(wartosc_b=1000);
					else
					{
					wartosc_b=wartosc_b-500;
					TestBuzzer(wartosc_b);
					}
					dbgu_print_ascii(y);
					break;

				case 'b': 
					linijka(maska);
					TestLinijka(maska);
					dbgu_print_ascii(y);
					break;
				case 'n': 
					linijka(maska2);
					TestLinijka(maska2);
					dbgu_print_ascii(y);
					break;
				case 'm': 
					linijka(maska3);
					TestLinijka(maska3);
					dbgu_print_ascii(y);
					break;
				default:Menu();
					break;
			}
		}while (a == '7'); 
	  }
}


/* ############################################################################### */
int main(void)
{

Open_DBGU();

Menu();
return 0;
}