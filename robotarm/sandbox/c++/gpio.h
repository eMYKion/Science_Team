#ifndef GPIO_H
#define GPIO_H

class gpio{
	private:
		int _pinNumber;
		bool _isExported;
		char *_pinDirection;
		
		
	
	public:
		
		gpio(int pin);
		~gpio();
		
		int exportPin();
		int unexportPin();
		bool isExported();
		int getNumber();
		
		int setDirection(const char *dir);
		char *getDirection();
		int setValue(int val);
		int getValue();
		
		
		
};

#endif