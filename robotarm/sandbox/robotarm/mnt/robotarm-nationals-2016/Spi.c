#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Spi.h"

# define CS0 8
# define MOSI 10
# define MISO 9
# define SCLK 11
# define SPI_MAX_SPEED_HZ 1e5
# define nanosecs_per_sec 1e9
# define NSEC_PER_LOOP 25.67
# define priority 90
# define spi_clk_delay nanosecs_per_sec/SPI_MAX_SPEED_HZ

int bit = 0;
int transferred_bit;
int adcval;






# define chan 4

int transfer_bits_base[] = {1,2,2,2, 2,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};
int bits_read[20];

//const struct timespec *deadline;

static inline void tsnorm(struct timespec *ts)
{
  while (ts->tv_nsec >= nanosecs_per_sec) {
    ts->tv_nsec -= nanosecs_per_sec;
    ts->tv_sec++;
  }
}

int spiReadADC(int channel)
  {
    struct timespec t;
    bit = 0;
    transfer_bits_base[1] = 1; /*set ADC to single-ended mode*/
    transfer_bits_base[2] = (channel&(100))>>2;
    transfer_bits_base[3] = (channel&(10))>>1;
    transfer_bits_base[4] = (channel&(1));
    bit = 0;
    digitalWrite(CS0, 0);
    clock_gettime(0, &t);
    while (bit<20)
      {
        digitalWrite(SCLK, 0);
	digitalWrite(MOSI, transfer_bits_base[bit]);
        clock_gettime(0, &t);
        digitalWrite(SCLK, 1);
        bits_read[bit] = digitalRead(MISO);
        clock_gettime(0, &t);
        bit++;
      }
    digitalWrite(SCLK, 1);
    digitalWrite(CS0, 1);
    transferred_bit = 8;
    adcval = 0;
    while (transferred_bit<20)
      {
        adcval = adcval + (bits_read[transferred_bit]<<(19-transferred_bit));
        transferred_bit++;
      }
    return adcval;
}

int spiReadADCDifferential(int channel) //input the channel in the pair that is positive Pairs: 0,1 ; 2,3 ; 4,5; 6,7 ;
  {
    struct timespec t;
    bit = 0;
    transfer_bits_base[1] = 0; /*set ADC to differential mode*/
    transfer_bits_base[2] = (channel&(100))>>2;
    transfer_bits_base[3] = (channel&(10))>>1;
    transfer_bits_base[4] = (channel&(1));
    bit = 0;
    digitalWrite(CS0, 0);
    clock_gettime(0, &t);
    while (bit<20)
      {
        digitalWrite(SCLK, 0);
	digitalWrite(MOSI, transfer_bits_base[bit]);
        clock_gettime(0, &t);
        digitalWrite(SCLK, 1);
        bits_read[bit] = digitalRead(MISO);
        clock_gettime(0, &t);
        bit++;
      }
    digitalWrite(SCLK, 1);
    digitalWrite(CS0, 1);
    transferred_bit = 7;
    adcval = 0;
    while (transferred_bit<20)
      {
        adcval = adcval + (bits_read[transferred_bit]<<(19-transferred_bit));
        transferred_bit++;
      }
    if (adcval>4096)
      {
        adcval = adcval - 8192;
      }
    return adcval;
}
void spiSetup(void){
	//wiringPiSetupGpio();
    pinMode(CS0, OUTPUT);
    pinMode(MOSI, OUTPUT);
    pinMode(SCLK, OUTPUT);
    pinMode(MISO, INPUT);
    digitalWrite(CS0, 1);
    digitalWrite(SCLK, 0);

}
