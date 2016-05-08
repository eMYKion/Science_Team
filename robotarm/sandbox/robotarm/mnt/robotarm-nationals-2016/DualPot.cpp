#include "DualPot.h"
#include "Spi.h"
#include "Potentiometer.h"

DualPot::DualPot(int ch): Potentiometer(ch){
	
}

int DualPot::getValue(void){
	return spiReadADCDifferential(this->_chPin) + this->_internalOffset;
}

DualPot::~DualPot(void){

}

