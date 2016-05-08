#ifndef STEPPER_H
#define STEPPER_H

class Stepper{
	private:
		int _gpioA1;
		int _gpioA2;
		int _gpioB1;
		int _gpioB2;
		

		
		int **_orderMatrix;
		
		int _virtualPosition;
		
	
	public:
		Stepper(int gpioA1, int gpioA2, int gpioB1, int gpioB2);
		~Stepper(void);
		
		void forward(void);
		void backward(void);
		
		int getPosition(void);
		void setPosition(int pos);
		
		void cleanup(void);
};

#endif
