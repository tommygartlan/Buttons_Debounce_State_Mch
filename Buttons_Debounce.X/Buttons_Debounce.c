/***************************************************
Name:- Tommy Gartlan
Date last modified:- Jan 2017

 * Functions for Debouncing Button Block
 * The C file for the DEbounce button library
 * At the moment it depends on 'Button_Press' being declared
 * in the application.
 * 
 * Note to myself:-Could modify this to pass the address of 
 * Button_Press to the function or to pass a copy and the 
 * function returns a bit_mask.
****************************************************/


/****************************************************
		Libraries included
*****************************************************/
#include <xc.h>
#include "Buttons_Debounce.h"

#define Button_PORT PORTB

#define MIN_BUTTON_COUNT 10


typedef enum {Waiting, Detected, WaitForRelease,Update} states;


extern Bit_Mask Button_Press;   //must be declared in Application


void Find_Button_Press(void)
{
	static states Button_State=Waiting;
	static unsigned char Button_Count=0;
    static Bit_Mask Temp_Press;
    
	

	switch (Button_State){
		case(Waiting):
			if(Button_PORT)   //if button detected
			{
				Button_State = Detected;
				Button_Count = 0;					//zero the count
			
				Temp_Press.Full=Button_PORT;  //record the value
			}
			break;
		case(Detected):
			if (Temp_Press.Full == Button_PORT)
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
			if (!Button_PORT)
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
	