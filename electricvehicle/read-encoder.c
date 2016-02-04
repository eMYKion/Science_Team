
task main()
{
	nMotorEncoder[motorA] = 0;
	while(1){
		nxtDisplayString(1, "%d", nMotorEncoder[motorA]);
	}
}
