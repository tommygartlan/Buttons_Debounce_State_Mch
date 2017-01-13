/***************************************************
Name:- Tommy Gartlan
Date last modified:- Jan 2016
Updated: Update to use xc8 compiler within Mplabx
 * version 2:-
Filename:- st_mch_debounce2.c
Program Description:- Using a simple state machine to debounce a switch.
 * This is base on an example I did with Glen Dimplex however I have used
 * a union as opposed to type casting, s this did not work in this compiler
 * so version 1 has errors
 * This version works well
 * buttons connected to portb
 * leds connected to portc
 * button 0 turns on led 0, any other button turns it off.
 * note can't use buttons 6,7 if using the debugger!
 
 * Version 2: this evolves from version1b. Here I use timer0 to generate an
 * interrupt every 10ms to check the switches. Good exercise for the students to
 * develop this from version 1b
 * Great this works. Use of the Timer0 library is handy here to write value
 * Version 2b: making some small changes
****************************************************/


/****************************************************
		Libraries included
*****************************************************/
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "../LCD_library/lcdlib_2016.h"
#include <plib/timers.h>

/************************************************
			Function Prototypes
*************************************************/
void Initial(void);
void delay_s(unsigned char secs);
void Find_Button_Press(void);
/*************************************************
					Clock
*************************************************/
#define _XTAL_FREQ 19660800

unsigned char count_test =0;
void __interrupt myIsr(void)
{
    //Timer overflows every 10mS
    // only process timer-triggered interrupts
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        
        Find_Button_Press();       //check the buttons every 10mS
        WriteTimer0(40960); 
        INTCONbits.TMR0IF = 0;  // clear this interrupt condition
        
        count_test++;
        if(count_test == 100){
            PORTCbits.RC7 = ~PORTCbits.RC7;   //check the timer overflow rate
            count_test = 0;                   //Toggle every 1 second (heartbeat))
        }
    }
}



typedef union{
    unsigned char Full;
    struct{
        unsigned char B0 :1;
        unsigned char B1 :1;
        unsigned char B2 :1;
        unsigned char B3 :1;
        unsigned char B4 :1;
        unsigned char B5 :1;
        unsigned char B6 :1;
        unsigned char B7 :1;
    };
    
}Bit_Mask;

Bit_Mask Temp_Press;
Bit_Mask Button_Press;	

#define MIN_BUTTON_COUNT 10

#define Got_Button_E (Button_Press.Full !=0 ? 1:0)
typedef enum {Waiting, Detected, WaitForRelease,Update} states;



void main(void)
{
	Button_Press.Full = 0x00;
	Temp_Press.Full = 0x00;
    Initial();

	while(1)
	{
		//Find_Button_Press();
        //NOTE:-  The structure is really like logging a legitimate button press
        //as oppose to showing the state of the button. That's why we clear the event
        //when it is processed.
		if(Got_Button_E)   //if some button has been pressed
		{
			if(Button_Press.B0)  //if its button 0
				LATC = 0x01;
			else                  //otherwise an other button..just testing
				LATC = Button_Press.Full;
            
            //Clear the button press event
			Button_Press.Full=0x00;   //clear all button events since only one can happen at time.
		}
		
		//__delay_ms(10); //how often the buttons are checked
		
	}
}

void Initial(void){
    
    ADCON1 = 0x0F;
	TRISB = 0xFF; //Buttons
	TRISC = 0x00;   //LEDS

	LATC = 0x00;
	delay_s(1);
	LATC = 0xff;
	delay_s(1);
	LATC = 0x00;
    
	for(unsigned int i =0; i<1000;i++)
	{
		LATC = PORTB;  //test switches here
		__delay_ms(10);	
	}

	LATC = 0xff;
	delay_s(1);
	LATC = 0x00;
	delay_s(1); 
    //0n, 16bit, internal clock(Fosc/4), prescale by 2)
    OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_2);
    WriteTimer0(40960);  //65,536 - 24,576  //overflows every 10mS
    ei();
    
}

void Find_Button_Press(void)
{
	static states Button_State=Waiting;
	static unsigned char Button_Count=0;
	

	switch (Button_State){
		case(Waiting):
			if(PORTB)   //if button detected
			{
				Button_State = Detected;
				Button_Count = 0;					//zero the count
			
				Temp_Press.Full=PORTB;  //record the value
			}
			break;
		case(Detected):
			if (Temp_Press.Full == PORTB)
			{
				++Button_Count;
				if (Button_Count > MIN_BUTTON_COUNT)
				{
					Button_State = WaitForRelease;
				}
			}
			else
			{
				Button_State = Waiting;
			}
			break;
		case(WaitForRelease):
			if (!PORTB)
			{
				Button_State = Update;
			}
			break;
		case(Update):
			{
				Button_Press = Temp_Press;
				Button_State = Waiting;
				Button_Count = 0;
				Temp_Press.Full=0;
			}
			break;
		default:
			{
				Button_State = Waiting;
				Button_Count = 0;
				Temp_Press.Full=0;
				Button_Press.Full=0;
			}
	}//end switch
}//end function
	
void delay_s(unsigned char secs)
{
    unsigned char i,j;
    for(j=0;j<secs;j++)
    {
        for (i=0;i<25;i++)
                __delay_ms(40);  //max value is 40 since this depends on the _delay() function which has a max number of cycles
        
    }
}