#ifndef SPI_H
#define SPI_H

#include <wiringPi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static inline void tsnorm(struct timespec *ts);

int spiReadADC(int channel);

int spiReadADCDifferential(int channel);

void spiSetup(void);

#endif