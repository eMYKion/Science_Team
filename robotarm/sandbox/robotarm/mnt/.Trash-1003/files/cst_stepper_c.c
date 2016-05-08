/*run the stepper with c code*/
#include <stdio.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define INITIAL_MAX_SPEED 3000
int max_speed = INITIAL_MAX_SPEED;
int accel = (INITIAL_MAX_SPEED/2);
#define STEPPER_MOVE 23
int DIRECTION = 1;
#define min_speed 10
double delay = 1.0/min_speed;
double current_speed = min_speed;
int steps;
int step;
int ramp_end;
int pos;
int DONE_ACCELERATING;

void main()
{
  while (1)
    {
      ramp_end = 0;
      DONE_ACCELERATING = FALSE;
      pos = 0;
      delay = 1.0/min_speed;
      current_speed = min_speed;
      printf("Maximum speed\n");
      scanf("%d", &max_speed);
      printf("Step number\n");
      scanf("%d", &steps);
      accel = max_speed/2;
      steps = abs(steps);
      double delaylist[steps];

      for(step=0; step<steps/2 && ! DONE_ACCELERATING; step++)  /* accelerate */
	{
	  current_speed = current_speed + accel*delay;
	  if (current_speed>max_speed) /* done with acceleration */
	    {
	      current_speed = max_speed;
	      ramp_end = step;
	      DONE_ACCELERATING = TRUE;
	    }
	  delay = 1.0/current_speed;
	  pos++;
	  delaylist[step]=delay;
	}

      if (ramp_end>0) /* we reached max speed in less than half the steps*/
	{
	  while(step<(steps-ramp_end-1)) /*continue at max_speed till deccel time */
	    /* stopping to allow same number of steps for decel as for accel */
	    {
	      pos++;
	      delaylist[step] = delay;
	      step++;
	    }
	}

      while (step<steps) /* now deccelerate, symmetrically with the accel */
	{
	  pos++;
	  delaylist[step] = delaylist[steps-step-1];
	  step++;
	}

      printf("Steps taken: %i\n",pos);
      printf("---------\n");
      for(step=0; step<steps; step+=steps/20)
	{
	  printf("Step %7i had delay %f \n",step, delaylist[step]);
	}

      for(step=0; step<=steps; step++) ;
    }
}
	      

