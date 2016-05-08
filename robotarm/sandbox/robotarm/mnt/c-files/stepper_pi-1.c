/*run the stepper with c code*/
/*compile: gcc stepper_pi.c -o stepper_pi -lwiringPi*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdbool.h>

#define STEP_PIN 19
#define DIR_PIN 12
#define ENABLE_PIN 6
#define START_PIN 4
#define STEPPER_MOVE 23
#define MIN_SPEED 100

#define MAX_ACCEL 200000


const int priority=90;

long delaylist[1000000];

long double accel = 2000;

int tableSize = 12;
double accel_table[12][2] = {
	{0.0,11000.0},
	{1000.0,11000.0},
	{2000.0,10000.0},
	{3000.0,10000.0},
	{4000.0,8000.0},
	{5000.0,8000.0},
	{6000.0,7000.0},
	{7000.0,5000.0},
	{8000.0,4000.0},
	{9000.0,4000.0},
	{10000.0,3000.0},
	{11000,2500.0}
	
};



const long nanosecs_per_sec = 1e9;
long delaytime = 1000000;
long double current_speed;
int steps;
int step;
int ramp_end;
int pos;
int isDoneAccelerating;
const struct timespec *deadline;

static double startPosMeters;
static double targetPosMeters;
static double farFenceMeters;

static bool isWaitingForStart = true;

static double motorResolution = 200;
const double PI = 3.141592;
static double gearRatio = 1;
static double motorDiameter = 0.0799;

static double metersPerStep;

static double accelConstant;

static double returnVal;

double getInterpolatedAccel(double vel);

static inline void tsnorm(struct timespec *ts){
	while (ts->tv_nsec >= nanosecs_per_sec) {
		ts->tv_nsec -= nanosecs_per_sec;
		ts->tv_sec++;
	}
}

void startMainSequence(void){
	isWaitingForStart = false;
}

void profile(int max_speed, int steps, int direction);

int main(int argc, char **argv){
	
	printf("main function started...\n");
	
	
	//metersPerStep = motorDiameter * PI/(gearRatio * motorResolution);
	metersPerStep = 0.0012655;
	
	printf("initializing pins...\n");
	wiringPiSetupGpio();
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
	pinMode(START_PIN, INPUT);
    pinMode(ENABLE_PIN, OUTPUT);
	
    digitalWrite(ENABLE_PIN, 0);
	
	
	printf("setting task priority...\n");
	//sets task priority
    if(piHiPri(priority)<0){
		printf("failed to set task priority\n");
	} 
	
	
	if(argc!=3){
		printf("Needed 2 command-line arguments!");
		return -1;
	}
	
	
	accelConstant = 0.21;
	sscanf(*(argv+1), "%lf", &startPosMeters);
	sscanf(*(argv+2), "%lf", &targetPosMeters);
	//sscanf(*(argv+4), "%lf", &farFenceMeters);
	
	
	//wait for start pin
	
	
	printf("waiting for start pin...\n");
	
	wiringPiISR(START_PIN, INT_EDGE_RISING, startMainSequence);
	
	
	while(true){
		delay(10);
		if(!isWaitingForStart){
			break;
		}
	}
	printf("start pin activated, running profiles\n");
	

	profile(6000, (int)((targetPosMeters - startPosMeters)/metersPerStep), 1);
	
    
	
	//done
	printf("program completed, exiting with return status 0\n");
	return 0;
}

void profile(int max_speed, int steps, int direction){
	
	digitalWrite(DIR_PIN, direction);
	
	struct timespec t;
	
	ramp_end = -1;
	isDoneAccelerating = false;
	pos = 0;
	delaytime = nanosecs_per_sec/MIN_SPEED;
	current_speed = MIN_SPEED;
	
	steps = abs(steps);
	digitalWrite(ENABLE_PIN, 1);
	clock_gettime(0, &t);
	t.tv_nsec += 0.5*nanosecs_per_sec;
	tsnorm(&t);
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
	
	 //accelerate
	for(step=0; step<steps/2 && ! isDoneAccelerating; step++){
		double temp = getInterpolatedAccel((double)current_speed);
		//printf("acceleration is %g\n", temp);
		current_speed = current_speed + (long double)(temp)*delaytime/nanosecs_per_sec;
		
		//done with acceleration 
		if (current_speed>max_speed){
			current_speed = max_speed;
			ramp_end = step;
			isDoneAccelerating = true;
			printf("maximum speed at:%i\n",ramp_end);
		}
		delaytime = nanosecs_per_sec/current_speed;
		pos++;
		delaylist[step]=delaytime;
	}

	// we reached max speed in less than half the steps
	if (ramp_end>-1){
		
		//continue at max_speed till deccel time 
		// stopping to allow same number of steps for decel as for accel 
		
		while(step<(steps-ramp_end-1)){
			pos++;
			delaylist[step] = delaytime;
			step++;
		}
	}
	
	// now decelerate, symmetrically with the accel 
	while (step<steps) {
		pos++;
		delaylist[step] = delaylist[steps-step-1];
		step++;
	}
	
	printf("Steps taken: %i\n",pos);
	printf("---------\n");
	
	
	clock_gettime(0, &t);
	for(step=0; step<steps; step++){
		// Need a pin drive here
		digitalWrite(STEP_PIN, ((step+1)%2));
		t.tv_nsec+=delaylist[step];
		tsnorm(&t);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
	}
	t.tv_nsec+=0.5*nanosecs_per_sec;
	tsnorm(&t);
	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
	digitalWrite(ENABLE_PIN, 0);
}

double getInterpolatedAccel(double vel){
	
	if(vel < accel_table[0][0]){
		returnVal = accel_table[0][1];
	}else if(vel > accel_table[tableSize-1][0]){
		returnVal = accel_table[tableSize-1][1];
	}
	
	double lastVel;
	
	double accel_per_vel;
	
	int i;
	int n;
	
	for(i =0, n=tableSize;i<n;i++){
		
		lastVel = accel_table[i][0];
		
		
		
		returnVal = -1;
		
		if(vel < lastVel){
			
			accel_per_vel = (accel_table[i][1] - accel_table[i-1][1])/(accel_table[i][0] - accel_table[i-1][0]);
			
			returnVal = accelConstant * (accel_table[i-1][1] + accel_per_vel * (vel - accel_table[i-1][0]));
			break;
			
		}else if(vel == lastVel){
			returnVal =  accelConstant * (accel_table[i][1]);
			break;
		}
	}
	
	if(returnVal > MAX_ACCEL){
		return MAX_ACCEL;
	}else{
		return returnVal;
	}
}