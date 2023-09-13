#include "adc.h"
#include "delay.h"
#include "param.h"

char SPI_WR(char data_in)
{
  char data_out = 0;
  unsigned char i;
  for(i=0;i<8;i++)  // 8 bit transaction
  {
    data_out <<= 1;

    CLK = 0;
    delay_ms(5);
    DIN = (data_in & 0x80) ? 1 : 0;   // Send data_in via DIN to ADC (e.g. start bit, mode selection)
    delay_ms(5);
    CLK = 1;
    delay_ms(5);

    if(DOUT == 1)   // Read DOUT
      data_out = (data_out | 0x01);   // Store into data_out
    data_in <<= 1;
  }
  return data_out;
}

unsigned int read_adc3202(unsigned char channel)
{
  
  unsigned int data_buff = 0;
  unsigned char spi_buff = 0;

  LOAD = 0;
  
  // Each read requires 24-bit transaction (3 byte transactions)
  SPI_WR(0x01);               // First byte transaction includes start bit
  if(channel == 1)            // Channel 1: 11100000, to IR sensor
    spi_buff = SPI_WR(0xe0);  // Second byte transaction includes mode selection and higher 4 bit ADC output
  else                        // Channel 0: 10100000, to variable resistor
    spi_buff = SPI_WR(0xa0);  // Second byte transaction includes mode selection and higher 4 bit ADC output
  data_buff = spi_buff & 0x0f;
  data_buff <<= 8;            // Store higher 4 bit ADC output
    
  spi_buff = SPI_WR(0x00);    // Third byte transaction includes lower 8 bit ADC output
  data_buff = data_buff | spi_buff;   // Store higher 4 bit ADC output

  LOAD = 1;  
  CLK = 0;
  DIN = 0;

  return data_buff;
}