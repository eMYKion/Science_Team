#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

# define CS0 8
# define MOSI 10
# define MISO 9
# define SCLK 11
# define SPI_MAX_SPEED_HZ 1e6
# define nanosecs_per_sec 1e9
# define priority 90
# define spi_clk_delay nanosecs_per_sec/SPI_MAX_SPEED_HZ

int bit = 0;
int transferred_bit;
int adcval;
long oldTime;
long loopCount;

# define chan 4

int transfer_bits_base[] = {0,0,0,0,1,2,2,2, 2,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};
int bits_read[24];

//const struct timespec *deadline;

static inline void tsnorm(struct timespec *ts){
  while (ts->tv_nsec >= nanosecs_per_sec) {
    ts->tv_nsec -= nanosecs_per_sec;
    ts->tv_sec++;
  }
}

int readadc(int channel){
    struct timespec t;
    bit = 0;
    transfer_bits_base[5] = 1; /*set ADC to single-ended mode*/
    transfer_bits_base[6] = (channel&(100))>>2;
    transfer_bits_base[7] = (channel&(10))>>1;
    transfer_bits_base[8] = (channel&(1));
    bit = 0;
    digitalWrite(CS0, 0);
    clock_gettime(0, &t);
    t.tv_nsec+=spi_clk_delay/2;
    tsnorm(&t);
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
	
    while (bit<24){
        digitalWrite(SCLK, 0);
		digitalWrite(MOSI, transfer_bits_base[bit]);
        t.tv_nsec+=spi_clk_delay/2;
		tsnorm(&t);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
        digitalWrite(SCLK, 1);
        bits_read[bit] = digitalRead(MISO);
        t.tv_nsec+=spi_clk_delay/2;
		tsnorm(&t);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
		bit++;
    }
    digitalWrite(SCLK, 1);
    digitalWrite(CS0, 1);
    transferred_bit = 12;
    adcval = 0;
    while (transferred_bit<24){
		adcval = adcval + (bits_read[transferred_bit]<<(23-transferred_bit));
		transferred_bit++;
    }
    return adcval;
}

void setup(void){
	wiringPiSetupGpio();
	pinMode(CS0, OUTPUT);
	pinMode(MOSI, OUTPUT);
	pinMode(SCLK, OUTPUT);
	pinMode(MISO, INPUT);
	digitalWrite(CS0, 1);
	digitalWrite(SCLK, 0);
}
    
        
