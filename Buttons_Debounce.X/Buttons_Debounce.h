/***************************************************
Name:- Tommy Gartlan
Date last modified:- Jan 2016
Updated: Update to use xc8 compiler within Mplabx
 * version 2:-
Filename:- Buttons_Debounce.h
Program Description:- 
 * 
****************************************************/

void Find_Button_Press(void);

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

                     // test ? True:False)
#define Got_Button_E (Button_Press.Full !=0 ? 1:0)
