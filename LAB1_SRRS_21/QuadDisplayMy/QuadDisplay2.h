#ifndef QUAD_DISPLAY
#define QUAD_DISPLAY

#include "mbed.h"
#include <assert.h>
/*
original
    ___7___
   /       \
  |         |
 2|         |6
   \___1___/
   /       \
  |         |
 3|         |5
   \___4___/
               *0
 ----my---------
 
    ___1___
   /       \
  |         |
 6|         |2
   \___7___/
   /       \
  |         |
 5|         |3
   \___4___/
               *0              
               
 */

#define QD_NONE  0//my 0
#define QD_DOT   1//my 1
#define QD_MINUS 0b10000000//my 2

#define QD_UNDERSCORE       0b00010000//my 3
#define QD_DEGREE           0b11000110//my 4
#define QD_UNDER_DEGREE     0b10111000//my 5
#define QD_UPPERSCORE       0b00000010//my 6
#define QD_COMMAS           0b00000100//my 7
#define QD_UNDERCOMMAS      0b00001000//my 8
             //76543210
#define QD_0 0b01111110
#define QD_1 0b00001100//my
#define QD_2 0b10110110//my
#define QD_3 0b10011110//my
#define QD_4 0b11001100//my
#define QD_5 0b11011010//my
#define QD_6 0b11111010//my
#define QD_7 0b00001110//my
#define QD_8 0b11111110//my
#define QD_9 0b11011110//my

#define QD_A 0b00000000
#define QD_a 0b00000000
#define QD_b 0b00000000
#define QD_C 0b01100000
#define QD_c 0b00000000
            //76543210
#define QD_d 0b10111100//my
#define QD_E 0b00000000
#define QD_f 0b00000000
#define QD_F QD_f
#define QD_H 0b11101100 //my
#define QD_h 0b00000000
#define QD_I QD_1
#define QD_J 0b00000000
#define QD_K QD_H
#define QD_L 0b00000000
#define QD_n 0b00000000
#define QD_o 0b00000000
#define QD_O QD_0
             //76543210
#define QD_P 0b11100110//my
#define QD_r 0b00000000
#define QD_S 0b00000000
#define QD_t 0b00000000
#define QD_u 0b00000000
#define QD_U 0b00000000
#define QD_Y 0b00000000


class QuadDisplayMy
{
private:
    SPI    *_dev_spi; 
    DigitalOut  _cs_pin; 
     uint8_t conv(uint8_t c);   
public:
    QuadDisplayMy(SPI *spi, PinName cs_pin);
    void displayDigits(uint8_t *text);
};

#endif
