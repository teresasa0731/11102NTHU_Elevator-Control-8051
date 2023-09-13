#include "i2c.h"
#include "8051.h"
// #include "stdutils.h"

// I2C Start Condition
void I2C_Start(void)
{
    SDA = 1;
    SCL = 1;
    Delay();
    SDA = 0;
    Delay();
    SCL = 0;
}

// I2C Stop Condition
void I2C_Stop(void)
{
    SDA = 0;
    SCL = 1;
    Delay();
    SDA = 1;
    Delay();
}

// Send ACK
void I2C_SendACK(__bit ack)
{
    SDA = ack;
    SCL = 1;
    Delay();
    SCL = 0;
    Delay();
}

// Recieve ACK
__bit I2C_RecvACK(void)
{
    SCL = 1;
    Delay();
    CY = SDA;
    SCL = 0;
    Delay();
    return CY;
}

// Send a Byte
void I2C_SendByte(uint8_t dat)
{
    uint8_t i;
    for (i=0; i<8; i++)
    {
        dat <<= 1;
        SDA = CY;
        SCL = 1;
        Delay();
        SCL = 0;
        Delay();
    }
    I2C_RecvACK();
}

// Recieve a Byte
uint8_t I2C_RecvByte(void)
{
    uint8_t i;
    uint8_t dat = 0;
    SDA = 1;
    for (i=0; i<8; i++)
    {
        dat <<= 1;
        SCL = 1;
        Delay();
        dat |= SDA;             
        SCL = 0;
        Delay();
    }
    return dat;
}

// I2C Data Transfer: Write mode
void Single_WriteI2C(uint8_t SlaveAddress, uint8_t REG_Address,uint8_t REG_data)
{
    I2C_Start();
    I2C_SendByte(SlaveAddress);
    I2C_SendByte(REG_Address);
    I2C_SendByte(REG_data);
    I2C_Stop();
}

// I2C Data Transfer: Read mode
uint8_t Single_ReadI2C(uint8_t SlaveAddress, uint8_t REG_Address)
{
    uint8_t REG_data;
    I2C_Start();
    I2C_SendByte(SlaveAddress);
    I2C_SendByte(REG_Address);
    I2C_Stop();

    I2C_Start();
		I2C_SendByte(SlaveAddress|0x01);  // R/W#: 1 means read mode
    REG_data=I2C_RecvByte();
    I2C_SendACK(1);
    I2C_Stop();
    return REG_data;
}

void Delay(void)
{
  char i = 0;
  for (i = 0; i < 5; i++);
}