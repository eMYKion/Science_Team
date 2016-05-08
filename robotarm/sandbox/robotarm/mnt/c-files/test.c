#include <stdio.h>

#define MAX_ACCEL 15000

double accelConstant = 1;

int tableSize = 7;
double accel_table[7][2] = {
	{0.0,500.0},
	{1000.0,5000.0},
	{2000.0,20000.0},
	{3000.0,20000.0},
	{4000.0,20000.0},
	{5000.0,15000.0},
	{6000.0,13000.0}
	
};

static double returnVal;

double getInterpolatedAccel(double vel);

void test(double vel){
	printf("v: %g is a: %g\n", vel, getInterpolatedAccel(vel));
}

int main(void){
	
	test(0);
	test(500);
	test(1000);
	test(1500);
	test(2000);
	test(2500);
	test(3000);
	test(3500);
	test(4000);
	test(4500);
	test(5000);
	test(5500);
	test(6000);
	test(6500);
	
	return 0;
}



double getInterpolatedAccel(double vel){
	
	if(vel < accel_table[0][0]){
		return accel_table[0][1];
	}else if(vel > accel_table[tableSize-1][0]){
		return accel_table[tableSize-1][1];
	}
	
	double lastVel;
	
	double accel_per_vel;
	
	int i;
	int n;
	
	for(i =0, n=tableSize;i<n;i++){
		
		lastVel = accel_table[i][0];
		
		//printf("i = %d\n", i);
		
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