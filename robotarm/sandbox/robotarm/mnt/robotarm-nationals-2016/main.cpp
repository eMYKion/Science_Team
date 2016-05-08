/*
*	written by Mayank Mali and Peter Wilson
*/
#include <iostream>
#include "Potentiometer.h"
#include "DualPot.h"
#include "Spi.h"
#include "dcmotor.h"
#include "Stepper.h"
#include <wiringPi.h>
#include <softPwm.h>

#define ELBOW_PIN_FORWARD 20
#define ELBOW_PIN_BACKWARD 25
#define ELBOW_PIN_BRAKE 12

#define SHOULDER_PIN_FORWARD 17
#define SHOULDER_PIN_BACKWARD 27
#define SHOULDER_PIN_BRAKE 19

#define ELBOW_POT_CHANNEL 2
#define SHOULDER_POT_CHANNEL 0
#define BASE_POT_CHANNEL 4

#define STEPPER_COIL_A1 5
#define STEPPER_COIL_A2 6
#define STEPPER_COIL_B1 26
#define STEPPER_COIL_B2 13

DualPot *elbowPot;
DualPot *shoulderPot;
DualPot *basePot;

dcmotor *elbowDc;
dcmotor *shoulderDc;
Stepper *baseStepper;

using namespace std;

void setupPots(void);

void setupMotors(void);

void printStatus(void);

int main(void){

	wiringPiSetupGpio();//needs to be first	
	spiSetup();

	setupMotors();
	setupPots();

	printStatus();
	
	while(true){
		printStatus();
		int elbow_error = elbowPot->getValue();
		if(elbow_error > 5){
			elbowDc->forward(50);
		}else if(elbow_error < -5){
			elbowDc->backward(50);
		}else{
			elbowDc->stop();
		}
		
	}

	elbowDc->cleanup();
	
	return 0;
}

void setupPots(void){
	
	elbowPot = new DualPot(ELBOW_POT_CHANNEL);
	shoulderPot = new DualPot(SHOULDER_POT_CHANNEL);
	basePot = new DualPot(BASE_POT_CHANNEL);
	
	elbowPot->setValue(0);
	shoulderPot->setValue(0);
	basePot->setValue(0);
}

void setupMotors(void){
	elbowDc = new dcmotor(ELBOW_PIN_FORWARD, ELBOW_PIN_BACKWARD, ELBOW_PIN_BRAKE);
	shoulderDc = new dcmotor(SHOULDER_PIN_FORWARD, SHOULDER_PIN_BACKWARD, SHOULDER_PIN_BRAKE);
	//baseStepper = new Stepper(STEPPER_COIL_A1, STEPPER_COIL_A2, STEPPER_COIL_B1, STEPPER_COIL_B2);
}

void printStatus(void){
	
	cout << "el:" << elbowPot->getValue() << " sh:" << shoulderPot->getValue() << " basePot:" << basePot->getValue() << endl;
	
}

/*
IOinit(IO_BCM) #initializes to BCM mode

el = IOUnit(EL_IN_CHANNEL_MASTER, EL_IN_CHANNEL_SLAVE, EL_PIN_FORWARD, EL_PIN_BACKWARD, EL_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
//no input, backward and forward out enabled, brake enabled, backward and forward PWM enabled

sh = IOUnit(SH_IN_CHANNEL_MASTER, SH_IN_CHANNEL_SLAVE, SH_PIN_FORWARD, SH_PIN_BACKWARD, SH_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
//no input, backward and forward out enabled, brake enabled, backward and forward PWM enabled

def signum(num):
  return 1 if(num>=0) else -1


def setTolSlope():
  el._tolerance = 8
  el._tol_max = 108
  el._slope = 100/(el._tol_max - el._tolerance)
  el._intercept = -el._tolerance*el._slope

  sh._tolerance = 2
  sh._tol_max = 42
  sh._slope = 100/(sh._tol_max - sh._tolerance)
  sh._intercept = -sh._tolerance*sh._slope
  


def getOffset():
  for x in range (0, 10):
    sh._offset += sh.diff_readadc()
    el._offset += el.diff_readadc()
  sh._offset = round(sh._offset/10, 3)
  el._offset = round(el._offset/10, 3)

def updateElShPots():
  el._potValSlave = el.inADCSlave()
  el._diff = el.diff_readadc() - el._offset
  sh._potValSlave = sh.inADCSlave()
  sh._diff = sh.diff_readadc()- sh._offset

setTolSlope()

el.pwmFStart()
el.pwmBStart()
sh.pwmFStart()
sh.pwmBStart()


getOffset()


#==============STEPPER======================
BASE_IN_CHANNEL_MASTER = 4
base = IOUnit(BASE_IN_CHANNEL_MASTER)

COIL_A1 = 5
COIL_A2 = 6
COIL_B1 = 26
COIL_B2 = 13

TICKS_PER_POT = 0.192 + 0.03

io.setup(COIL_A1, io.OUT)
io.setup(COIL_A2, io.OUT)
io.setup(COIL_B1, io.OUT)
io.setup(COIL_B2, io.OUT)

STEP = [[1, 0, 1, 0],
        [0, 1, 1, 0],
        [0, 1, 0, 1],
        [1, 0, 0, 1]]

stepperPos = 0
for e in range(0, 5):
  stepperPos += TICKS_PER_POT*base.inavgADCMaster()
stepperPos/=5
stepperPos = int(stepperPos)
basepot = stepperPos

#stepper motor stuff
  

speed = 0
start_speed = 40.0
accel = 400.0
deccel = 10.0
max_speed = 200.0
delay = 0.05

stepper_stop_tol = 2
stepper_decel_tol = 30
deccel = (max_speed)/(stepper_decel_tol-stepper_stop_tol)

def step(direction):
    global stepperPos
    global stepper_dir
    stepper_dir = direction
    if direction == 0:
        io.output(COIL_A1, 0)
        io.output(COIL_A2, 0)
        io.output(COIL_B1, 0)
        io.output(COIL_B2, 0)
        print('release')
    elif(stepper_dir == -1 or 1):
        stepperPos += direction
        io.output(COIL_A1, STEP[stepperPos%4][0])
        io.output(COIL_A2, STEP[stepperPos%4][1])
        io.output(COIL_B1, STEP[stepperPos%4][2])
        io.output(COIL_B2, STEP[stepperPos%4][3])
    else:
        print('error')

        

def stepperspeed(old_speed):
  global basepot
  global error
  global speed
  global delay
  basepot = TICKS_PER_POT*base.inavgADCMaster()/3.0 + basepot/3.0*2
  error = round(stepperPos - basepot, 3)
  
  if old_speed == 0:
    old_speed = start_speed*signum(error)
    
  if(error > stepper_decel_tol):
    speed += accel/abs(old_speed)
  elif (error > stepper_stop_tol):
    speed -=deccel#/abs(old_speed)
    if (speed<start_speed):
      speed = start_speed
  elif(error < -stepper_decel_tol):
    speed -= accel/abs(old_speed)
  elif(error < -stepper_stop_tol):
    speed += deccel#/abs(old_speed)
    if (speed > -start_speed):
      speed = -start_speed
  else:
    speed = 0
    
  if(speed < -max_speed):
    speed = -max_speed
  elif(speed > max_speed):
    speed = max_speed


print (sh._offset, el._offset)

stringFormat = "SH_M %4.2f SH_S %4.2f SH_D %4.2f || EL_M %4.2f EL_S %4.2f EL_D %4.2f"


oldTime = time.time()


def run_main():
  try:
    global oldTime
    global speed
    global basepot
    global oldLoopTime
    global stepperProfile
    oldLoopTime=0
    while 1:
      newTime = time.time()
      timeElapsed = newTime - oldTime
      #print((newTime-oldLoopTime)*1000)
      oldLoopTime=newTime
      

      updateElShPots()
      
      #=============STEPPER================
      
      if(speed!=0):
        delay = 1/abs(speed)
        if(timeElapsed >= delay):
          oldTime = newTime
          step(int(-speed/abs(speed)))
          stepperspeed(speed)
        else:
          pass
      else:
        stepperspeed(speed)
      #print(basepot, error, stepperPos, speed)
      #print(stringFormat % (sh._potValMaster,sh._potValSlave,sh._diff,el._potValMaster,el._potValSlave,el._diff))
      
      if (el._diff > el._tolerance):
        el.brakeoff()
      	#forward
        el.pwmBChangeDutyCycle(0)
        #print('EL FORWARD')

        
        if(el._diff>el._tol_max):#max
          el.pwmFChangeDutyCycle(100)
        else:
          el.pwmFChangeDutyCycle(el.dutyFunction())
          
      elif(el._diff < -1*el._tolerance):
      	#backward
        #print('EL BACKWARD')
        el.brakeoff()
        el.pwmFChangeDutyCycle(0)

        if(el._diff<-1*el._tol_max):
          el.pwmBChangeDutyCycle(100)
        else:
          el.pwmBChangeDutyCycle(el.dutyFunction())
      else:
      	#still
        #print('EL STILL')
        el.brakeon()
        pass
        
      if (sh._diff > sh._tolerance):
        sh.brakeoff()
      	#forward
        sh.pwmBChangeDutyCycle(0)
        #print('sh FORWARD')

        
        if(sh._diff>sh._tol_max):#max
          sh.pwmFChangeDutyCycle(100)
        else:
          sh.pwmFChangeDutyCycle(sh.dutyFunction())
          
      elif(sh._diff < -1*sh._tolerance):
      	#backward
        #print('sh BACKWARD')
        sh.brakeoff()
        sh.pwmFChangeDutyCycle(0)

        if(sh._diff<-1*sh._tol_max):
          sh.pwmBChangeDutyCycle(100)
        else:
          sh.pwmBChangeDutyCycle(sh.dutyFunction())
      else:
      	#still
        #print('sh STILL')
        sh.brakeon()
        pass


  except KeyboardInterrupt:
    global stepper_run
    el.pwmFStop()
    el.pwmBStop()
    sh.pwmFStop()
    sh.pwmBStop()
    IOquit()

run_main()

*/
