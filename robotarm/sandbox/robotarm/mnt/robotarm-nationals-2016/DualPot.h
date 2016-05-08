#ifndef DUALPOT_H
#define DUALPOT_H

#include "Potentiometer.h"

class DualPot: public Potentiometer{
	public:
		DualPot(int ch);
		~DualPot(void);
		int getValue(void);//returns differential of two potentiometers
};


#endif 
