/*run the stepper with c code*/
#include <stdio.h>
#include <time.h>
#include <wiringPi.h>

#define STEP_PIN 17
#define DIR_PIN 23

#define TASK_PRIORITY 90


#define TRUE 1
#define FALSE 0
#define INITIAL_MAX_SPEED 3000
int max_speed = INITIAL_MAX_SPEED;
int accel = (INITIAL_MAX_SPEED/2);
#define STEPPER_MOVE 23
int DIRECTION = 1;
#define min_speed 10
#define nanosecs_per_sec 1e9
long delay = nanosecs_per_sec/min_speed;
double current_speed = min_speed;
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

void main()
{
  struct timespec t;
  wiringPisetup();
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  int PiHiPri(TASK_PRIORITY);
  while (1)
    {
      ramp_end = 0;
      DONE_ACCELERATING = FALSE;
      pos = 0;
      delay = nanosecs_per_sec/min_speed;
      current_speed = min_speed;
      printf("Maximum speed\n");
      scanf("%d", &max_speed);
      printf("Step number\n");
      scanf("%d", &steps);
      accel = max_speed/2;
      steps = abs(steps);
      long delaylist[steps];

      for(step=0; step<steps/2 && ! DONE_ACCELERATING; step++)  /* accelerate */
	{
	  current_speed = current_speed + accel*delay/nanosecs_per_sec;
	  if (current_speed>max_speed) /* done with acceleration */
	    {
	      current_speed = max_speed;
	      ramp_end = step;
	      DONE_ACCELERATING = TRUE;
	    }
	  delay = nanosecs_per_sec/current_speed;
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
      printf("maximum speed at:%i\n",ramp_end);
      printf("Steps taken: %i\n",pos);
      printf("---------\n");
      for(step=0; step<steps; step+=steps/50)
	{
	  printf("Step %7i had delay %i\n",step, delaylist[step]);
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

	};

    }
}
	      

