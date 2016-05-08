#ifndef DCMOTOR_H
#define DCMOTOR_H

class dcmotor{
	private:
		
		int _gpioF;
		int _gpioB;
		int _gpioBreak;
	public:
		dcmotor(int gpioF, int gpioB, int gpioBreak);
		~dcmotor(void);
		
		void forward(int dutyCycle);
		void backward(int dutyCycle);
		void stop(void);
		void cleanup(void);

};

#endif
