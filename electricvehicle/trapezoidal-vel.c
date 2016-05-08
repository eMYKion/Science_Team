//all in metric [meters] or arbitrary

#define GEAR_RATIO 1
#define ENCODER_RESOLUTION 720
#define WHEEL_DIAMETER 0.09
#define DIGITS 5
#define MAX_VELOCITY 50;
#define MIN_VELOCITY 0;

double static power = MIN_VELOCITY;
double static targetTicks;



double getStopDistanceMeters(void){
	nxtDisplayString(1, "%d%d%d.%d%d" 0,0,0,0,0);
	int digits[]={0,0,0,0,0};
	//  000.00

	for(int digit=0;digit < DIGITS ; digit+=1){
		while(1){
			nxtDisplayString(1, "%d%d%d.%d%d" digits[0],digits[1],digits[2],digits[3],digits[4]);
			if(nNxtButtonPressed == 1){
				break;
			}else if(nNxtButtonPressed == 3){
				digits[digit]=(digits[digit]+1)%10;
			}
			wait1Msec(50);
		}
	}

	double sum =0.0;
	for(int i = 0; i < DIGITS; i+=1){
		sum+= (double)digits[i]*pow(10,2-i)
	}

	return sum



}

double trapezoidalVelocity(double currTicks){

	if(currTicks<START_MAX_VELOCITY_TICKS){
		//constant accelaration
		power+=(MAX_VELOCITY - power)/(START_MAX_VELOCITY_TICKS - currTicks);

	}else if(START_MAX_VELOCITY_TICKS<=currTicks <STOP_MAX_VELOCITY_TICKS){

		power = MAX_VELOCITY;

	}else if(STOP_MAX_VELOCITY_TICKS<=currTicks <targetTicks){
		//constant negative accelaration
		power += (MIN_VELOCITY - power)/(STOP_MAX_VELOCITY_TICKS- currTicks);
	}else if(targetTicks<=currTicks){
		power = null;
	}

	return power;
}

task main()
{
	double targetMeters = getStopDistanceMeters();
	targetTicks = targetMeters*(PI*WHEEL_DIAMETER/(GEAR_RATIO*ENCODER_RESOLUTION));
	nxtDisplayString(1, "%g", targetTicks);

	nMotorEncoderTarget = (int)targetTicks

	while(power!=null){
		motor[motorA] = power;
		trapezoidalVelocity(nMotorEncoder[motorA]);

	}








}
