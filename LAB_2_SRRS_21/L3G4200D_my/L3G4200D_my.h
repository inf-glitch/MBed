#ifndef __L3G4200D_MY_H
#define __L3G4200D_MY_H

#include "mbed.h"
#include "DevI2C.h"

#define I2C_ADDR_READ 0xd1
#define I2C_ADDR_WRITE 0xd0
#define I2C_ADDR_GYRO 0xd0

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define STATUS_REG 0x27
#define OUT_X_L   0x28
#define OUT_X_H   0x29
#define OUT_Y_L   0x2a
#define OUT_Y_H   0x2b
#define OUT_Z_L   0x2c
#define OUT_Z_H   0x2d

void GyroL3G4200D_Ini(DevI2C *gyro);
void GyroL3G4200D_GetAxis(DevI2C *gyro,int16_t* g);
#endif
