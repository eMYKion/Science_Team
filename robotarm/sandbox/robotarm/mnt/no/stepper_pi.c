/*run the stepper with c code*/
/*compile: gcc stepper_pi.c -o stepper_pi -lwiringPi*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <wiringPi.h>

#define STEP_PIN 19
#define DIR_PIN 12
#define ENABLE_PIN 6

const int priority=90;

long delaylist[1000000];

#define TRUE 1
#define FALSE 0
#define INITIAL_MAX_SPEED 3000
int max_speed = INITIAL_MAX_SPEED;
long double accel = 2000.0;
#define STEPPER_MOVE 23
int DIRECTION = 1;
#define min_speed 250
const long nanosecs_per_sec = 1e9;
long delaytime = 1000000;
long double current_speed = min_speed;
int steps;
int step;
int ramp_end;
int pos;
int DONE_ACCELERATING;
const struct timespec *deadline;

static inline void tsnorm(struct timespec *ts)
{
  while (ts->tv_nsec >= nanosecs_per_sec) {
    ts->tv_nsec -= nanosecs_per_sec;
    ts->tv_sec++;
  }
}

int main()
{
  struct timespec t;
    wiringPiSetupGpio();
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT); /*Don't do anything with this pin in current version*/
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, 0); /*Disable stepper driver*/
    //if(piHiPri(priority)<0) {printf("failed to set task priority\n");}
    piHiPri(priority);
  while (1)
    {
      ramp_end = -1;
      DONE_ACCELERATING = FALSE;
      pos = 0;
      delaytime = nanosecs_per_sec/min_speed;
      current_speed = min_speed;
      printf("Maximum speed\n");
      scanf("%d", &max_speed);
      printf("Step number\n");
      scanf("%d", &steps);
      steps = abs(steps);
      digitalWrite(ENABLE_PIN, 1);
      clock_gettime(0, &t);
      t.tv_nsec+=0.5*nanosecs_per_sec;
      tsnorm(&t);
      clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
      for(step=0; step<steps/2 && ! DONE_ACCELERATING; step++)  /* accelerate */
	{
	  current_speed = current_speed + accel*delaytime/nanosecs_per_sec;
	  if (current_speed>max_speed) /* done with acceleration */
	    {
	      current_speed = max_speed;
	      ramp_end = step;
	      DONE_ACCELERATING = TRUE;
              printf("maximum speed at:%i\n",ramp_end);
	    }
	  delaytime = nanosecs_per_sec/current_speed;
	  pos++;
	  delaylist[step]=delaytime;
	}

      if (ramp_end>-1) /* we reached max speed in less than half the steps*/
	{
	  while(step<(steps-ramp_end-1)) /*continue at max_speed till deccel time */
	    /* stopping to allow same number of steps for decel as for accel */
	    {
	      pos++;
	      delaylist[step] = delaytime;
	      step++;
	    }
	}

      while (step<steps) /* now decelerate, symmetrically with the accel */
	{
	  pos++;
	  delaylist[step] = delaylist[steps-step-1];
	  step++;
	}
      printf("Steps taken: %i\n",pos);
      printf("---------\n");
      for(step=0; step<steps; step+=steps/50)
	{
	  printf("Step %7i had delaytime %li\n",step, delaylist[step]);
	}
      clock_gettime(0, &t);
      for(step=0; step<steps; step++)
	{
	  /* Need a pin drive here*/
	  digitalWrite(STEP_PIN, ((step+1)%2));
	  /*printf("%i, %i\n", step, delaylist[step]);*/	  
	  t.tv_nsec+=delaylist[step];
	  tsnorm(&t);
	  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);

	}
      t.tv_nsec+=0.5*nanosecs_per_sec;
      tsnorm(&t);
      clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME,  &t, NULL);
      digitalWrite(ENABLE_PIN, 0);
    }
  return 1;
}
	      

