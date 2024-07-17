#include "QuadDisplay2.h"
#include "mbed.h"

QuadDisplayMy::QuadDisplayMy(SPI *spi, PinName cs_pin) : 
        _dev_spi(spi), _cs_pin(cs_pin)
{
    assert (spi);
   _cs_pin = 0;  
}

void QuadDisplayMy::displayDigits(uint8_t *text)
{
    int i;
     _dev_spi->lock();
     _cs_pin = 0;  
     for (i=0;i<4;i++)
        _dev_spi->write(conv(text[i])); 
     _cs_pin = 1;
     _dev_spi->unlock(); 
}

uint8_t QuadDisplayMy::conv(uint8_t c)
{
    uint8_t a;
    switch (c){
       case char(0):
          a=QD_NONE;
          break; 
       case char(1):
          a=QD_DOT;
          break; 
       case char(2):
          a=QD_MINUS;
          break;           
       case char(3):
          a=QD_UNDERSCORE;
          break; 
       case char(4):
          a=QD_DEGREE;
          break; 
       case char(5):
          a=QD_UNDER_DEGREE;
          break;         
       case char(6):
          a=QD_UPPERSCORE;
          break; 
       case char(7):
          a=QD_COMMAS;
          break; 
       case char(8):
          a=QD_UNDERCOMMAS;
          break;                            
       
       case '-':     
          a=QD_MINUS;  
          break;                                              
       case '0':
          a=QD_0;
          break;
       case '1':
          a=QD_1; 
          break;
       case '2':
          a=QD_2; 
          break;          
       case '3':
          a=QD_3;
          break;
       case '4':
          a=QD_4;
          break; 
       case '5':
          a=QD_5;
          break;
       case '6':
          a=QD_6;
          break; 
       case '7':
          a=QD_7;
          break; 
       case '8':
          a=QD_8;
          break;
       case '9':
          a=QD_9;
          break;  
       case 'd':
       case 'D':         
          a=QD_d;
          break;            
       case 'P':
       case 'p':         
          a=QD_P;
          break;    
       case 'H':
       case 'h':         
          a=QD_H;
          break;                                
       default:
          a=QD_NONE;                                         
    }  
    return a^0xff;     
}
