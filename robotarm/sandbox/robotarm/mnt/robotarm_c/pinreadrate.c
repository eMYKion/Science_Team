#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

# define CS0 8
# define MOSI 10
# define MISO 9
# define SCLK 11
# define SPI_MAX_SPEED_HZ 5e4
# define nanosecs_per_sec 1e9
# define priority 90
# define spi_clk_delay nanosecs_per_sec/SPI_MAX_SPEED_HZ
int bit = 0;
int transferred_bit;
int adcval;
long oldTime;
long loopCount;

double accum;

const struct timespec *deadline;

static inline void tsnorm(struct timespec *ts)
{
  while (ts->tv_nsec >= nanosecs_per_sec) {
    ts->tv_nsec -= nanosecs_per_sec;
    ts->tv_sec++;
  }
}

int main()
  {
    wiringPiSetupGpio();
    pinMode(18, INPUT);
    if(piHiPri(priority)<0) {printf("failed to set task priority\n");} /*sets task priority*/
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);
    for(loopCount=0;loopCount<1e8;loopCount++)
      {
      }
    clock_gettime(CLOCK_REALTIME, &stop);
    accum = (stop.tv_sec-start.tv_sec) + (stop.tv_nsec-start.tv_nsec)/nanosecs_per_sec;
    printf("%f", accum);
    return 0;
  }