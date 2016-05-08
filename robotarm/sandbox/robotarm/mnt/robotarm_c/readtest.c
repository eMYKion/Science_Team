#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

const int priority=90;
int returnVal;

int main()
  {
    wiringPiSetupGpio();
    pinMode(18, INPUT);
    if(piHiPri(priority)<0) {printf("failed to set task priority\n");} /*sets task priority*/
    returnVal = digitalRead(18);
    printf("Chan reads %i\n",digitalRead(18));
    return 0;
  }