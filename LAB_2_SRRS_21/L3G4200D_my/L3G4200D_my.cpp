#include "L3G4200D_my.h"

void GyroL3G4200D_Ini(DevI2C *gyro)
{
    char data_write[2];

    wait_us (20000);
    data_write[0]=CTRL_REG1;  // DR1 DR0 BW1 BW0 PD Zen Yen Xen
    data_write[1]=0x1f;       //  0   0   0   1  1   1   1   1    dr=100Hz, BW=25
    gyro->write(I2C_ADDR_GYRO,data_write, 2,0); // 1-no stop
}

//-----------------------------------------------
void GyroL3G4200D_GetAxis(DevI2C *gyro,int16_t* g)
{
    char data_write[2];
    char  buffer[6];
    
    data_write[0]=OUT_X_L|0x80;
    gyro->write(I2C_ADDR_GYRO,data_write, 1,1); // 1-no stop
    gyro->read(I2C_ADDR_GYRO,buffer, 6,0);
    g[0]=*((int16_t*)&buffer[0]);
    g[1]=*((int16_t*)&buffer[2]);
    g[2]=*((int16_t*)&buffer[4]); 
}

