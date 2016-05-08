#include "dcmotor.h"
#include <wiringPi.h>
#include <softPwm.h>


dcmotor::dcmotor(int gpioF, int gpioB, int gpioBreak){
	this->_gpioF = gpioF;
	this->_gpioB = gpioB;
	this->_gpioBreak = gpioBreak;
	
	pinMode(gpioF, OUTPUT);
	pinMode(gpioB, OUTPUT);
	pinMode(gpioBreak, OUTPUT);
	softPwmCreate(gpioF, 0, 100);
	softPwmCreate(gpioB, 0, 100);

}

dcmotor::~dcmotor(void){

}
		
void dcmotor::forward(int dutyCycle){
	digitalWrite(this->_gpioBreak, 0);
	softPwmWrite(this->_gpioF, dutyCycle);
	softPwmWrite(this->_gpioB, 0);
}

void dcmotor::backward(int dutyCycle){
	digitalWrite(this->_gpioBreak, 0);
	softPwmWrite(this->_gpioB, dutyCycle);
	softPwmWrite(this->_gpioF, 0);
}

void dcmotor::stop(void){
	
	softPwmWrite(this->_gpioF, 0);
	softPwmWrite(this->_gpioB, 0);
	digitalWrite(this->_gpioBreak, 1);
}

void dcmotor::cleanup(void){
	softPwmWrite(this->_gpioF,0);
	softPwmWrite(this->_gpioB,0);
	digitalWrite(this->_gpioBreak,0);
	pinMode(this->_gpioF, INPUT);
	pinMode(this->_gpioB, INPUT);
	pinMode(this->_gpioBreak, INPUT);
}





