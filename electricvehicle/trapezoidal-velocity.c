//all in metric [meters] or arbitrary

#define GEAR_RATIO 8.0/40.0
#define ENCODER_RESOLUTION 360.0
#define WHEEL_DIAMETER 0.056
#define METERS_PER_TICK 0.00244346095//experimentally determined
//#define METERS_PER_TICK PI*WHEEL_DIAMETER/(2*PI)*(2*PI)/(GEAR_RATIO*ENCODER_RESOLUTION)

#define DIGITS 5

const float MAX_VELOCITY  =100.0;
const float MIN_VELOCITY = 0.0;
const float START_MAX_VELOCITY_TICKS = 0.5/METERS_PER_TICK;
const float STOP_MAX_VELOCITY_TICKS  =8.5/METERS_PER_TICK;
const float OVERSHOOT_TICKS = 150;


static float power;
static float targetMeters;
static float targetTicks;
static int counts;

static int maxFlag = 0;



float getStopDistanceMeters(void){
	nxtDisplayString(1, "%d%d%d.%d%d", 0,0,0,0,0);
	int digits[]={0,0,0,0,0};
	//  000.00

	int digit = 0;
	while(digit<DIGITS){
		nxtDisplayString(1, "%d%d%d.%d%d", digits[0],digits[1],digits[2],digits[3],digits[4]);
		if(nNxtButtonPressed == 1){
			digit+=1;
		}else if(nNxtButtonPressed == 2){
			digit-=1;
		}else if(nNxtButtonPressed == 3){
			digits[digit]=(digits[digit]+1)%10;
		}
		wait1Msec(300);
	}



	float sum =0.0;
	for(int i = 0; i < DIGITS; i+=1){
		sum+= (float)digits[i]*pow(10,DIGITS-1-i);
	}
	sum /=100.0;
	return sum;



}

float trapezoidalVelocity(float currTicks){

	if(currTicks < START_MAX_VELOCITY_TICKS){

		power += (MAX_VELOCITY-power)/(START_MAX_VELOCITY_TICKS-currTicks);
		//constant accelaration
		nxtDisplayString(1, "ACCELERATING");

	}else if(START_MAX_VELOCITY_TICKS<=currTicks && currTicks<STOP_MAX_VELOCITY_TICKS){
	maxFlag = 1;

		power = MAX_VELOCITY;
		nxtDisplayString(1, "MAX VELOCITY");

	}else if(STOP_MAX_VELOCITY_TICKS<=currTicks && currTicks <targetTicks){
		counts+=1;
		maxFlag = 2;
		//constant negative accelaration

		power +=(MIN_VELOCITY+8 - power)/(targetTicks+OVERSHOOT_TICKS-currTicks);


		nxtDisplayString(1, "DECELERATING");
	}else if(targetTicks+OVERSHOOT_TICKS<=currTicks){
		power  = -5;

		nxtDisplayString(1, "OVERSHOT");
	}

	return power;
}

task main()
{
	//get distances in meters and encoder ticks


	targetMeters = getStopDistanceMeters() + 0.06;
	targetTicks = targetMeters/METERS_PER_TICK;
	nxtDisplayString(2, "Meters: %f", targetMeters);
	nxtDisplayString(3, "Ticks: %f", targetTicks);
	nxtDisplayString(4, "Press to RUN...");
	bool run= false;
	while(!run){
		if(nNxtButtonPressed==3){
			run = true;
		}
		wait1Msec(200);
	}

	nMotorPIDSpeedCtrl[motorA] = mtrNoReg;//we get to control motor power entirely
	bFloatDuringInactiveMotorPWM = false;//brake vs coast or float
	nSyncedMotors = synchAB;
	nSyncedMotors = synchAC;
	//motors synchronized to A

	nMotorEncoder[motorA] = 0;
	nMotorEncoderTarget[motorA] = (int)targetTicks;
	power = MIN_VELOCITY;

	//constantly update power based on trapezoidal-velocity profile
	int currEnc;
	int time;

	while(1){
		motor[motorA] = power;
		currEnc = nMotorEncoder[motorA];
		trapezoidalVelocity((float)currEnc);

		if(abs(nMotorEncoder[motorA] - (int)targetTicks)<1 && power < 0){
			motor[motorA] = 0;
			break;
		}
		if(maxFlag == 1){
			ClearTimer(T1);
		}else if(maxFlag == 2 && counts==1){
			time = time1[T1];
		}


		nxtDisplayString(5, "%5.2f %d", power, currEnc);
	}

	nxtDisplayString(6, "time: %d", time );
	wait1Msec(6000);
	//end of task


}
