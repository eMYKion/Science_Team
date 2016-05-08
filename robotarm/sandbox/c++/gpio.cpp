/*
*by Mayank Mali 2.8.2016
*/

#include <fstream>
#include <iostream>
#include <string>
#include "gpio.h"

#define ROOT_GPIO_ADDRESS "/sys/class/gpio/"
#define EXPORT_ADDRESS "/sys/class/gpio/export"
#define UNEXPORT_ADDRESS "/sys/class/gpio/unexport"
		
using namespace std;
		
gpio::gpio(int pin){
	this->_pinNumber = pin;
};

gpio::~gpio(){
	cout << "gpio" << this->_pinNumber << " destroyed"<<endl;
};
		
int gpio::exportPin(){
	ofstream exportfile(EXPORT_ADDRESS);
	if(exportfile<0){
		cout << "PIN " << this->_pinNumber <<" EXPORT FAIL" << endl;
		return -1;
	}
	
	exportfile << this->_pinNumber;
	
	exportfile.close();
	this->_isExported = true;
	
	return 0;
	
};

int gpio::unexportPin(){
	ofstream unexportfile(UNEXPORT_ADDRESS);
	if(unexportfile<0){
		cout << "PIN " << this->_pinNumber <<" UNEXPORT FAIL" << endl;
		return -1;
	}
	
	unexportfile << this->_pinNumber;
	
	unexportfile.close();
	this->_isExported = false;
	return 0;
};

bool gpio::isExported(){
	return this->_isExported;
};
int gpio::getNumber(){
	return this->_pinNumber;
};
		
int gpio::setDirection(const char *dir){
	if(this->_isExported){
		
		string tempaddr = ROOT_GPIO_ADDRESS + string("gpio") + to_string(this->_pinNumber) + "/direction";
		ofstream setdirfile(tempaddr.c_str());
		if(setdirfile < 0){
			cout << "PIN " << this->_pinNumber <<" DIRECTION FAIL" << endl;
			return -1;
		}
		setdirfile << dir;
		cout << dir << endl;
		this->_pinDirection = (char *)dir;
		setdirfile.close();
		
		return 0;
	}else{
		return -1;
	}
}

char * gpio::getDirection(){
	return (char *)this->_pinDirection;
};

int gpio::setValue(int val){
	
	if(this->_isExported){
		string tempaddr = ROOT_GPIO_ADDRESS + string("gpio") + to_string(this->_pinNumber)+"/value";
		ofstream setvalfile(tempaddr.c_str());
		if(setvalfile < 0){
			cout << "PIN " << this->_pinNumber <<" SET VALUE FAIL" << endl;
			return -1;
		}
		setvalfile << to_string(val);
		cout << "value being set is " << to_string(val) << endl;
		
		setvalfile.close();
		
		return 0;
	}else{
		cout << "PIN " << this->_pinNumber <<" SET VALUE FAIL" << endl;
		return -1;
	}
};

int gpio::getValue(){
	if(this->_isExported){
		string tempaddr = ROOT_GPIO_ADDRESS + string("gpio") + to_string(this->_pinNumber)+"/value";
		ifstream getvalfile(tempaddr.c_str());
		if(getvalfile < 0){
			cout << "PIN " << this->_pinNumber <<" GET VALUE FAIL" << endl;
			return -1;
		}
		int temp;
		getvalfile >> temp;
		
		getvalfile.close();
		
		return temp;
	}else{
		cout << "PIN " << this->_pinNumber <<" GET VALUE FAIL" << endl;
		return -1;
	}
};
