/***************************************************
Name:- Tommy Gartlan
Date last modified:- Jan 2016
Updated: Update to use xc8 compiler within Mplabx
 * version 2:-
Filename:- Button_app.c
Program Description:- Simple application to show the use of the button_debounce
 * header and C file, that includes a state machine to debounce the button.
 * Button_Press can be treated in it's entirety as Button_Press.Full
 * or individual button as in Button_Press.B0
 * LEDS connected to PORTC
 * Buttons connected to PORTB
 * Note the state machine toggles the corresponding LED if a button is
 * pressed. It will only be set to 1 after debouncing. The application clears the 
 * value after use.
 * The state machine needs to be continuously called. In this case every 10mS
******************************************************************************/


/****************************************************
		Libraries included
*****************************************************/
#include <xc.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include "../LCD_library/lcdlib_2016.h"
#include <plib/timers.h>
#include "Buttons_Debounce.h"

Button_Type Button_Press;         //Create the Button variable

/************************************************
			Function Prototypes
*************************************************/
void Initial(void);
void delay_s(unsigned char secs);

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
        
        Find_Button_Press();       //NOTE.....I evaluate the debounce state machine every 10mS, i.e check the buttons every 10mS
        
        WriteTimer0(40960); 
        INTCONbits.TMR0IF = 0;  // clear this interrupt condition
        
        count_test++;
        if(count_test == 100){
            PORTCbits.RC7 = ~PORTCbits.RC7;   //check the timer overflow rate
            count_test = 0;                   //Toggle every 1 second (heartbeat))
        }
    }
}



void main(void)
{
	Button_Press.Full = 0x00;
	
    Initial();

	while(1)
	{
 		if(Got_Button_E)   //if some button has been pressed
		{
			if(Button_Press.B0)  //if its button 0
				PORTCbits.RC0 = ~PORTCbits.RC0;
            
            if  (Button_Press.B1)  //if its button 0
				PORTCbits.RC1 = ~PORTCbits.RC1;   
            
            if  (Button_Press.B2)  //if its button 0
				PORTCbits.RC2 = ~PORTCbits.RC2;    
            
            if  (Button_Press.B3)  //if its button 0
				PORTCbits.RC3 = ~PORTCbits.RC3;  
            
            //Clear the button press event
			Button_Press.Full=0x00;   //clear all button events since only one can happen at time.
		}
		
		
	}
}

void Initial(void){
    
    ADCON1 = 0x0F;
	TRISB = 0xFF; //Buttons
	TRISC = 0x00;   //LEDS
    
    //Test the LEDS
	LATC = 0x00;
	delay_s(1);
	LATC = 0xff;
	delay_s(1);
	LATC = 0x00;
    //test the switches
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

	
void delay_s(unsigned char secs)
{
    unsigned char i,j;
    for(j=0;j<secs;j++)
    {
        for (i=0;i<25;i++)
                __delay_ms(40);  //max value is 40 since this depends on the _delay() function which has a max number of cycles
        
    }
}