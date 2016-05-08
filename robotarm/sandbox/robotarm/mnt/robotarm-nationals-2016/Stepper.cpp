#include "Stepper.h"
#include <wiringPi.h>



Stepper::Stepper(int gpioA1, int gpioA2, int gpioB1, int gpioB2){
	
	this->_gpioA1 = gpioA1;
	this->_gpioA2 = gpioA2;
	this->_gpioB1 = gpioB1;
	this->_gpioB2 = gpioB2;
	
	
	pinMode(gpioA1, OUTPUT);
	pinMode(gpioA2, OUTPUT);
	pinMode(gpioB1, OUTPUT);
	pinMode(gpioB2, OUTPUT);
	

	
	this->_orderMatrix = new int *[4];
	
	for(int i=0;i<4;i++){
		this->_orderMatrix[i] = new int[4];
	}

	//please replace this mess with some kind of loop and static matrix
	this->_orderMatrix[0][0] = 1;
	this->_orderMatrix[0][1] = 0;
	this->_orderMatrix[0][2] = 1;
	this->_orderMatrix[0][3] = 0;

	this->_orderMatrix[1][0] = 0;
	this->_orderMatrix[1][1] = 1;
	this->_orderMatrix[1][2] = 1;
	this->_orderMatrix[1][3] = 0;

	this->_orderMatrix[2][0] = 0;
	this->_orderMatrix[2][1] = 1;
	this->_orderMatrix[2][2] = 0;
	this->_orderMatrix[2][3] = 1;

	this->_orderMatrix[3][0] = 1;
	this->_orderMatrix[3][1] = 0;
	this->_orderMatrix[3][2] = 0;
	this->_orderMatrix[3][3] = 1;

	this->_virtualPosition=0;
}

Stepper::~Stepper(void){
	
}

void Stepper::forward(void){
		this->_virtualPosition++;
	
		int index = (this->_virtualPosition % 4 + 4)%4;
	
		digitalWrite(this->_gpioA1, this->_orderMatrix[index][0]);
		digitalWrite(this->_gpioA2, this->_orderMatrix[index][1]);
		digitalWrite(this->_gpioB1, this->_orderMatrix[index][2]);
		digitalWrite(this->_gpioB2, this->_orderMatrix[index][3]);
		
}

void Stepper::backward(void){
		this->_virtualPosition--;
	
		int index = (this->_virtualPosition % 4 + 4)%4;
	
		digitalWrite(this->_gpioA1, this->_orderMatrix[index][0]);
		digitalWrite(this->_gpioA2, this->_orderMatrix[index][1]);
		digitalWrite(this->_gpioB1, this->_orderMatrix[index][2]);
		digitalWrite(this->_gpioB2, this->_orderMatrix[index][3]);
		
}
		
void Stepper::cleanup(void){
	digitalWrite(this->_gpioA1, 0);
	digitalWrite(this->_gpioA2, 0);
	digitalWrite(this->_gpioB1, 0);
	digitalWrite(this->_gpioB2, 0);
	
	pinMode(this->_gpioA1, INPUT);
	pinMode(this->_gpioA2, INPUT);
	pinMode(this->_gpioB1, INPUT);
	pinMode(this->_gpioB2, INPUT);
}

int Stepper::getPosition(void){
	return this->_virtualPosition;
}

void Stepper::setPosition(int pos){
	this->_virtualPosition = pos;
}


