#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

# define nanosecs_per_sec 1e9
# define priority 90
# define CS0 8
# define MOSI 10
# define MISO 9
# define SCLK 11

long int loopCount=0;
double accum;

int main()
  {
    wiringPiSetupGpio();
    pinMode(MOSI, OUTPUT);
    pinMode(SCLK, OUTPUT);
    if(piHiPri(priority)<0) {printf("failed to set task priority\n");} /*sets task priority*/
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);
    while (loopCount<1e6)
      {
        digitalWrite(SCLK, 0);
	digitalWrite(MOSI, loopCount%2);
        digitalWrite(SCLK, 1);
        /*digitalRead(MISO);*/
        loopCount++;
      }
    clock_gettime(CLOCK_REALTIME, &stop);
    accum = (stop.tv_sec-start.tv_sec) + (stop.tv_nsec-start.tv_nsec)/nanosecs_per_sec;
    printf("%f", accum*2);
    return 0;
  }