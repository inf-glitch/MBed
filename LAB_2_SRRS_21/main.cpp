/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#include "XNucleo53L0A1.h" 

#include "L3G4200D_my.h"

#include "sMotor.h" 

#include "stdio.h"
#include <cstdint>
/****************************************************************
 *                        Definitions                           *                                              
*****************************************************************/

/*****************************************************************
 *                         Prototypes                            *       
 *****************************************************************/
void button1_enabled_cb ( void );
void Diff_Timeout_Cb ();

/****************************************************************
 *                        Definitions                           *                                              
*****************************************************************/
const int buffer_size = 255;  

typedef struct 
{    
    volatile uint8_t in;    
    uint8_t event;      
    uint8_t app;
    uint8_t answer; 
    int len;
    char buffer[buffer_size];  
} RxPc;  



/*****************************************************************
 *                         Prototypes                            *       
 *****************************************************************/
void button1_enabled_cb(void);
void button1_onpressed_cb(void);
void Ini();

void sensors_task();
void polling_sensors_isr();
void duration_messure(uint32_t *dt);
void proximityR_isr();

/*****************************************************************
*                     Interface                                  *               
******************************************************************/

DigitalOut led1(LED1);
InterruptIn button1(BUTTON1);
UnbufferedSerial pc(USBTX, USBRX);
sMotor motor(A5, A4, A3, A1);
InterruptIn proximity(A0);

/*****************************************************************
*                         Threads                                *
******************************************************************/

Thread sensor_daemon; 

/*****************************************************************  
*                     Time                                       *
******************************************************************/

Timeout button1_timeout;
Ticker polling_sensors; 
Timer duration_timer;
Timeout Pc_ift;
Timeout Diff_Timeout;
/*****************************************************************
*                     Global variables                           *              
******************************************************************/
volatile bool button1_pressed = false;
volatile bool button1_enabled = true; 
uint8_t mode = 0;

//----------------------------------

RxPc rxpc;
char text[5];
char tx_buffer[128];
uint8_t Rx_event;

DevI2C *device_i2c;
static XNucleo53L0A1 *board = NULL;

uint8_t SensorsEn=1; 
uint32_t distance_c; 
uint32_t distance_l; 
uint32_t distance_r; 
uint32_t task_duration;
uint32_t task_duration_c;
int16_t G[3]; 
int steps;
int direction;
int speed; 

uint8_t diff_flag;

uint8_t a;
/***************************************************************** 
*                      Main                                      *
******************************************************************/
int main()
{
    Ini();
   	while (true) 
    {
    	if (button1_pressed)
        {
            mode++;
            if (mode > 4)
            {
                mode = 0;    
            }
            switch (mode)
            {
                case 0:
                    steps = 0;
                    break;
                case 1:
                    steps = 1;
                    direction = 0;
                    break;
                case 2:
                    steps = 1;
                    direction = 1;
                    break;
                case 3:
                    break;
                default:
                    break;
            }
            button1_pressed = false; 
        }   
        if (steps)
        {
            switch (mode)
            {
                case 0:
                    break;

                case 1:
                    motor.step(16, 0, speed);
                    break;

                case 2:
                    motor.step(16, 0, speed);
                    break;

                case 3:
                    motor.step(steps, direction, speed);
                    break;

                default:
                    steps = 0;
                    break;
            }
        }   
        else
        {
            wait_us(2000);
        }
    }
}
/***********************************************************
*                           Functions                      *
***********************************************************/
void Ini()
{ 
 	button1.fall(callback(button1_onpressed_cb)); 
    device_i2c = new DevI2C(D14, D15);
    board = XNucleo53L0A1::instance(device_i2c, A2, D8, D2);
    GyroL3G4200D_Ini(device_i2c);

    int status = board->init_board();
    if (status)
    {
        sprintf(text,"Hi--");
    }
    else
    {
        sprintf(text,"HiOK");
    }
    board->display->display_string(text); 
    GyroL3G4200D_Ini(device_i2c);   
    proximity.mode(PullUp);
    proximity.rise(&proximityR_isr);
    diff_flag = 0;

    wait_us(2000000); 
   	sensor_daemon.start(sensors_task);  
   	SensorsEn = 1; 
   	polling_sensors.attach(&polling_sensors_isr, 320ms);
    speed = 2000;


}


/***********************************************************
*         Interrupt Service Routines                       *
***********************************************************/
//----------------------------ISR handling button pressed event
void button1_onpressed_cb(void)
{
    if (button1_enabled) 
    {
        button1_enabled = false;
        button1_pressed = true;
        button1_timeout.attach(callback(button1_enabled_cb), 30ms); 
    }
}

void button1_enabled_cb(void)
{
 	button1_enabled = true;
}

 void polling_sensors_isr() 
{ 

 	if (SensorsEn)
    {
    	sensor_daemon.flags_set(0x1); 
 	}     

}

void proximityR_isr()
{

}

void duration_messure(uint32_t *dt)
{
    duration_timer.stop();
    *dt = duration_timer.elapsed_time().count();
    duration_timer.reset();
}

void sensors_task() 
{
 	int status; 
 	char text[5];  
    uint32_t dt;

    while (true) 
    { 
        ThisThread::flags_wait_any(0x1,true); 
        SensorsEn = 0;   
    	led1 = !led1;    

        switch(mode)
        {
            case 0: 
                duration_timer.start();
                status = board -> sensor_centre -> get_distance(&distance_c);  
                if (status != VL53L0X_ERROR_NONE) 
                {
                    distance_c = 8888;
                }
                duration_messure(&task_duration_c);
                sprintf(text,"c%3d",distance_c);
                board -> display -> display_string(text);
                break;
            case 1:
                status = board -> sensor_left -> get_distance(&distance_l);  
                if (status != VL53L0X_ERROR_NONE) 
                {
                    distance_l = 8888;
                }
                sprintf(text,"l%3d",distance_l);
                board->display->display_string(text);  
                break;
            case 2: 
                status = board -> sensor_right -> get_distance(&distance_r);  
                if (status != VL53L0X_ERROR_NONE) 
                {
                    distance_r = 8888;
                }
                sprintf(text,"r%3d",distance_r);
                board -> display -> display_string(text);  
                break;
            case 3:  
                duration_timer.start(); 

                status = board -> sensor_left -> get_distance(&distance_l);  
                if (status != VL53L0X_ERROR_NONE) 
                    distance_l = 8888;

                status = board -> sensor_right -> get_distance(&distance_r);  
                if (status != VL53L0X_ERROR_NONE) 
                    distance_r = 8888;

                a = abs ( ( int ) distance_r - ( int ) distance_l );

                sprintf(text,"d%3d", a );
                board -> display -> display_string(text);
                duration_messure(&task_duration);

                if ( distance_l != 8888 && distance_r != 8888 ) {
                    if ( a > 10 ) {
                        if ( distance_r > distance_l ) {
                            direction = 1;
                            if ( diff_flag == 0 ) {
                                diff_flag = 1;
                                Diff_Timeout.attach ( Diff_Timeout_Cb, 960ms );
                            }
                        }
                    }
                }  
            case 4:      
                dt = task_duration/1000;
                sprintf(text,"%4d", dt);
                board -> display -> display_string(text); 
                break;
           
            default:  
                sprintf(text,"----"); 
                board -> display -> display_string(text);                    
                break; 
        }
        GyroL3G4200D_GetAxis(device_i2c, G);                         
        SensorsEn=1; 
   	}          
} 


void Diff_Timeout_Cb () {
    sprintf ( tx_buffer, "RS\r\n" );
    if ( diff_flag == 1 ) {
        sprintf ( tx_buffer, "RS\r\n" );
        pc.write(tx_buffer,strlen(tx_buffer));
    }

    else {
        sprintf(tx_buffer,"LS\r\n");
        pc.write(tx_buffer,strlen(tx_buffer));
    }

}