#include <iostream>
#include "gpio.h"

using namespace std;

int main(int argc, char ** argv){
	int pin = 12;
	if(argc>0){
		pin = atoi((char *)*(argv+1));
	}
	cout << "using pin " << pin << endl;
	
	gpio led(pin);
	
	led.exportPin();
	led.setDirection("out");
	
	led.setValue(1);
	cout << "on\n";
	cin.get();
	led.setValue(0);
	cout << "off\n";
	
	led.unexportPin();
	
	
	return 0;
}

