#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

class Potentiometer{
	protected:
		int _chPin;
		int _internalOffset;
		
	public:
		Potentiometer(int ch);
		~Potentiometer(void);
		virtual int getValue(void);
		void setValue(double val);
};

#endif
