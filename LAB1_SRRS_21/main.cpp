/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

//#include "mbed.h"
// Blinking rate in milliseconds
#include "mbed.h"
#include "QuadDisplay2.h"
#include "string.h"
#include "stdio.h"

//#include "ESP_8266.h"
#define BLINKING_RATE 500ms
#define TICKER_PERIOD 20ms
/****************************************************************
*                           Definitions 
****************************************************************/
const int buffer_size = 255;  
//--------------------------------------------------------------
typedef struct {
    volatile uint8_t in;       // Circular buffer pointers, volatile makes 
                                        //read-   modify-write atomic
    uint8_t event;      
    uint8_t app;
    uint8_t answer; 
    int len;
    char buffer[buffer_size];  
  } RxPc;  


/****************************************************************
*                          Prototypes   
*****************************************************************/
void button1_enabled_cb(void);
void actions();
void button1_onpressed_cb(void);
void Ini();
//----------------------------1
void IndTicker_isr();
//-----------------------------
void RunningSnake ();
//-----------------------------
void RunningFire ();
//-------------------------------
void button_frame_timeout_isr ();
//------------------------------
void HowManyClicks ();
//----------------------------------
void proximity1_1_isr ();
void proximity1_0_isr ();
void proximity2_1_isr ();
void proximity2_0_isr ();
void prox1_enabled_isr ();
void prox2_enabled_isr ();

void indication ();

void Rx_interrupt();

void Response ();
//-----------------------------------
/*****************************************************************
*                           Interface
******************************************************************/
DigitalOut led1(LED1);
InterruptIn button1(BUTTON1);
//------------------------------1
SPI spi(D4, D5, D3);
QuadDisplayMy display(&spi,D6);

DigitalOut relay1 (D12);
DigitalOut relay2 (D9);
DigitalOut relay3 (D8);
DigitalOut relay4 (D11);

InterruptIn proximity1(D5);
InterruptIn proximity2(D7);
//-----------------------------
UnbufferedSerial pc(USBTX, USBRX);

/***************************************************************** *
                            Time
******************************************************************/
Timeout button1_timeout; // Used for debouncing

//--------------------------1
Ticker IndTicker;
//--------------------------
Timeout button_frame_timeout;

Timeout prox1_timeout; 
Timeout prox2_timeout;


/*****************************************************************
*                             Global variables
******************************************************************/
volatile bool button1_pressed = false; // Used in the main loop
volatile bool button1_enabled = true; // Used for debouncing

//-------------------------------------------------------------------
uint8_t RunningStickCnt;
uint8_t RunningStickEn;
uint8_t text [4];
uint8_t text1 [4];

uint8_t text_switch;
//-------------------------------------------------------------------
uint8_t button_cnt;
uint8_t button_event;
uint8_t button_cnt_event;
uint8_t prox1;
uint8_t prox2;
uint8_t prox1_event;
uint8_t prox2_event; 
uint8_t action_event;
//---------------------------------
RxPc rxpc;
char tx_buffer[128];
uint8_t Rx_event;
//----------------------------------
Timeout Pc_ift;
/****************************************************************** *
                              Main 
******************************************************************/
int main()
{
    Ini();
    while(1) {
        //RunningSnake ();
        HowManyClicks ();
        //ThisThread::sleep_for (10ms);
        indication ();
        actions ();
        IndTicker.attach(&IndTicker_isr, 4*TICKER_PERIOD);
    }
}

/**********************************************************
*                              Functions  
***********************************************************/
void Ini()
{
// Attach ISR to handle button press event
    button1.fall(callback(button1_onpressed_cb));
    button1_enabled = true;
//------------------------1
    IndTicker.attach(&IndTicker_isr,BLINKING_RATE);
    display.displayDigits((uint8_t*)"4045");
    //---------------------------------------------
    RunningStickEn = 0;
    RunningStickCnt = 0;
    button_cnt = 0;
    button_event = 0;
    button_cnt_event = 0;
    //-------------------------------------------
    text_switch = 1;
    relay1 = relay2 = relay3 = relay4 = 0;
    //--------------------------------------------
    proximity1.rise(callback(proximity1_1_isr));  
    proximity1.fall(callback(proximity1_0_isr));        
    proximity2.fall(callback(proximity2_0_isr)); 
    proximity2.rise(callback(proximity2_1_isr));  

    prox1 = 1;
    prox2 = 1;
    prox1_event=0;
    prox2_event=0;
    action_event=0;
    //---------------------------------------------
        pc.baud(115200);
    pc.attach(&Rx_interrupt, SerialBase::RxIrq);
    Rx_event=0;
    //-----------------------------------------------
}


void RunningSnake ()
{
    RunningStickEn = 1;
    RunningStickCnt = 0;
    if (RunningStickEn) {
        switch (RunningStickCnt) {
            case 0:
                text [0] = text [1] = text [2] = text [3] = 0;
                text1 [0] = text1 [1] = text1 [2] = text1 [3] = 0;
            break;
//--------------------------------------------------------0
            case 1:
                text [0] = 6;
                text1 [0] = 7;
            break;

            case 2:
                text [0] = '1';
                text1 [0] = '1';
            break;

            case 3: 
                text [0] = 8;
                text [1] = 3;
                text1 [1] = 3;
                text [0] = 8;
            break;
//--------------------------------------------------------1
            case 4:
                text [0] = 0;
                text1 [0] = 0;
                text [1] = 8;
                text1 [1] = 3;
            break;

            case 5:
                text [1] = '1';
                text1 [1] = '1';
            break;

            case 6: 
                text [1] = 7;
                text [2] = 6;
                text1 [1] = 7;
                text [2] = 6;
            break;
//--------------------------------------------------------2
            case 7:
                text [1] = 0;
                text1 [1] = 0;
                text [2] = 6;
                text1 [2] = 7;
            break;

            case 8:
                text [2] = '1';
                text1 [2] = '1';
            break;

            case 9: 
                text [2] = 8;
                text [3] = 3;
                text1 [3] = 3;
                text [2] = 8;
            break;
//--------------------------------------------------------3
            case 10:
                text [2] = 0;
                text [2] = 0;
                text1 [3] = 3;
                text1 [3] = 8;
            break;

            case 11:
                text [3] = '1';
                text1 [3] = '1';
            break;

            case 12: 
                text [3] = 7;
                text1 [3] = 7;
            break;
        }

        if (text_switch) {
            display.displayDigits (text1);
            text_switch = 0;
        } 

        display.displayDigits(text);
    }
}

void indication ()  
{
    char text[5];

    if(button_cnt_event){ 

        button_cnt_event=0;
        sprintf(text,"P%1d  ", button_cnt);             
        text[2]=3;
        text[3]=3; 

        switch (button_cnt) {

            case 1:
                text[2]=-2; 
                text[3]=relay1?'1':'0';
                break;

            case 2:
                text [2] = -2;
                text [3] = relay2?'1':'0';
                break; 

            case 3:
                text [2] = -2;
                text [3] = relay3?'1':'0';
                break; 

            case 4:
                text [2] = -2;
                text [3] = relay4?'1':'0';
                break; 

            case 5:
                for(int i = 0; i < 4; i++)
                    text[i]=5; 
                break; 

            default:
                break;

        }                                      
        display.displayDigits((uint8_t*)text); 
    } 
}

void HowManyClicks () {

    switch (button_event) {
        case 1:
        RunningStickEn = 1;
        relay1 = !relay1;
            break; 

        case 2:
        RunningStickEn = 0;
        relay2 = !relay2;
            break;

        case 3:
        RunningStickEn = 1;
        RunningStickCnt = 0;
        relay3 = !relay3;
            break;

        case 4:
        relay4 = !relay4;
            break;

        case 5:
        relay1 = relay2 = relay3 = relay4 = 0;
            break;
        
    }
}
//--------------------------------------------------------
void actions()
{
    if (prox1_event == 2) {

        prox1_event = 3;

        if (relay1 == 0){
            relay1 = 1;
            relay2 = 1;
        }

        else {
            relay1 = 0;
            relay2 = 0;
        }
    }

    if (prox2_event == 2) {
        prox2_event = 3;

        if (relay3 == 0) {
            relay3 = 1;
            relay4 = 1;
        }

        else {
            relay3 = 0;
            relay4 = 0;
        }
    }
}
//----------------------------------------------------------

uint8_t execute(char *buffer)
{
char *endptr;
uint8_t answer;
   
answer=0;
switch(buffer[0]) {
case 'k':
endptr=strstr(&buffer[0],"k1");  
            	if(endptr!=NULL){
                		endptr +=2;
                		if(endptr[0]=='1')
                  			relay1=1;
                		else
                  			relay1=0;                    
            	}  
            	endptr=strstr(&buffer[0],"k2");  
            	if(endptr!=NULL){

            	} 
           	 endptr=strstr(&buffer[0],"k3");  
            	if(endptr!=NULL){

            	}   
            	endptr=strstr(&buffer[0],"k4");  
            	if(endptr!=NULL){

            	}  
            	endptr=strstr(&buffer[0],"k0");  
           	if(endptr!=NULL){

            	}                                                                                         
            	break; 
case 'S':
        		case 's':
            		answer=1;
           	 break;                         
    }  
    return answer;   
}   

void Response(uint8_t answer,char* tbuff)
{
switch(answer) {
case 1:
                 sprintf(tbuff,"P1=%1d P2=%1d L1...L4=%1d %1d %1d %1d\r\n",prox1,prox2,relay1?1:0,relay2?1:0,relay3?1:0,relay4?1:0);       
                 break;
            case 2:
                break;            
            case 3:
                break;
            case 4:
                break;
         }                                       
} 

void PcDataProcessing()
{ 
    switch(rxpc.buffer[0]) {
                case 'P': //------wifi sleep
            case 'p':
                    break;
                case 'W': //------wifi wake up
            case 'w':
                    break;
                case 'A': //----------AT commands-------------
                    break;
                default: 
                    rxpc.answer=execute(rxpc.buffer);
                    if (rxpc.answer==0)
                            pc.write(rxpc.buffer,rxpc.len);
                    else{
                        Response(rxpc.answer,tx_buffer);
    pc.write(tx_buffer,strlen(tx_buffer));
                }  
    } 
}

/**********************************************************
*                        Interrupt Service Routines 
***********************************************************/
//----------------------------ISR handling button pressed event
void button1_onpressed_cb(void)
{
    if (button1_enabled) {
// Disabled while the button is bouncing
        button1_enabled = false;
// To be read by the main loop
        button1_pressed = true; 
// Debounce time 300 ms
        button1_timeout.attach(callback(button1_enabled_cb), 300ms);

        button_cnt ++;
        button_cnt_event = 1;
        button_frame_timeout.detach ();
        button_frame_timeout.attach (callback (button_frame_timeout_isr), 300ms);
    }
}
//---------------------------Enables button when bouncing is over
void button1_enabled_cb(void)
{
    button1_enabled = true;
}
//----------------------------------1
void IndTicker_isr()
{
   led1 = !led1; 

   if (RunningStickEn) {

       if (RunningStickCnt == 12)
           RunningStickCnt = 0;

       else
       RunningStickCnt ++;
   }
}

void button_frame_timeout_isr ()
{
    button_event = button_cnt;
    button_cnt = 0;
}
           
//---------------------------------------------------
void proximity1_1_isr ()
{
    if (prox1_event == 3) {
        prox1_event = 4;
        prox1_timeout.attach (callback (prox1_enabled_isr), 30ms);
    }

    else if (prox1_event ==4) {
        prox1_timeout.detach ();
        prox1_timeout.attach (callback (prox1_enabled_isr), 30ms);
    }

}

void proximity1_0_isr ()
{
    if (prox1_event == 0) {
        prox1_event = 1;
        prox1_timeout.attach(callback(prox1_enabled_isr), 30ms);
    }

    else if (prox1_event == 1) {

        prox1_timeout.detach ();
        prox1_timeout.attach (callback (prox1_enabled_isr), 30ms);
    }
}

void proximity2_0_isr ()
{
    if (prox2_event ==0) {
        prox2_event = 1;
        prox2_timeout.attach (callback (prox2_enabled_isr), 30ms);
    }

    else if (prox2_event == 1) {
        prox2_timeout.detach ();
        prox2_timeout.attach (callback (prox2_enabled_isr), 30ms);
    }

}

void proximity2_1_isr ()
{
    if (prox2_event == 3) {
        prox2_event = 4;
        prox2_timeout.attach (callback (prox2_enabled_isr), 30ms);
    }

    else if (prox2_event == 4) {
        prox2_timeout.detach ();
        prox2_timeout.attach (callback (prox2_enabled_isr), 30ms);
    }
}
//-----------------------------------------
void prox1_enabled_isr ()
{
    if (prox1_event ==1) {

        if (proximity1 ==0) {
            prox1 = 0;
            prox1_event = 2;
        }
    }

    else if (prox1_event == 4) {

        if (proximity1 == 1) {

        }
    }
}


void prox2_enabled_isr()
{
    if (prox2_event == 2) {

        if (proximity2 == 0) {
            prox2 = 0;
            prox2_event = 2;
        }
    }

    else if (prox2_event == 4) {

        if (proximity2 == 1) {

        }

    }
}

void Pc_ift_isr()
{
Rx_event=1;
}  


void Rx_interrupt()
{
char c;
    	if (pc.read(&c, 1)==1) {
       		rxpc.buffer[rxpc.in]=c; 
       		Pc_ift.attach(Pc_ift_isr, 10ms); 
       		if(Rx_event==0 && rxpc.in<(buffer_size-1) )
            	rxpc.in++;
    	} 
}
//-------------------------------------------------


void RunningFire ()
{

}