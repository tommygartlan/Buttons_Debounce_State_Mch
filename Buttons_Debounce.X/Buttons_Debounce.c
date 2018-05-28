/***************************************************
Name:- Tommy Gartlan
Date last modified:- MAy 2018

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



typedef enum {Waiting, Detected, WaitForRelease,Update} states;
//Function called every by timed interrupt in application file (every 10mS for example)
void Find_Button_Press(void)
{
	static states Button_State=Waiting;
	static unsigned char Button_Count=0;
    static Button_Type Temp_Press;      //stores temporary button values
    
	switch (Button_State){
		case(Waiting):
			if(Button_PORT)   //if any button press detected
			{
				Button_State = Detected;        //change state
				Button_Count = 0;		       //OnEntry/zero the count
	
				Temp_Press.Full=Button_PORT;  //OnEntry/record the temporary value
			}
			break;
		case(Detected):
			if (Temp_Press.Full == Button_PORT)
			{
				++Button_Count;    //Guarded state action, if same value increase button count
				if (Button_Count > MIN_BUTTON_COUNT)
				{
					Button_State = WaitForRelease;   //guarded state transition
				}
			}
			else
			{
				Button_State = Waiting;   //state transition
			}
			break;
		case(WaitForRelease):
			if (!Button_PORT)
			{
				Button_State = Update;   //state transition when all buttons released
			}
			break;
		case(Update):
			{
				Button_Press = Temp_Press;  //state action         HERE the BUTTON value is updated
				Button_State = Waiting;     //state transition
				Button_Count = 0;           //state action
				Temp_Press.Full=0;          //state action
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
	