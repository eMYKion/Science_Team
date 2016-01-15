from lib.stepperlib import *
import sys, math, time

#all measurements are metric or arbitrary

Init()

COIL_A1 = 5
COIL_A2 = 6
COIL_B1 = 26
COIL_B2 = 13

MOTOR_RESOLUTION = 200
WHEEL_CIRCUMFERENCE = math.pi*0.09

DELAY_MAX = 0.005
DELAY_MIN = 0.0015

steppers = stepper(COIL_A1, COIL_A2, COIL_B1, COIL_B2, DELAY_MAX)

#distance to ticks
ticksPerMeter = int(MOTOR_RESOLUTION/WHEEL_CIRCUMFERENCE)
startMinDelay =  int(ticksPerMeter * float(sys.argv[1]))
stopMinDelay = int(ticksPerMeter * float(sys.argv[2]))
stopTravel = int(ticksPerMeter * float(sys.argv[3]))

def setTrapezoidalDelay(ticks):
    
    if(0<=ticks<startMinDelay):
        
        if(startMinDelay - ticks == 0):
            pass
        else:
            steppers._delay += (DELAY_MIN - steppers._delay) / (startMinDelay - ticks)
        
    elif(startMinDelay<=ticks<stopMinDelay):
        
        steppers._delay = DELAY_MIN
        
    elif(stopMinDelay<=ticks<stopTravel):
        
        if(stopMinDelay - ticks == 0):
            pass
        else:
            steppers._delay += (DELAY_MAX - steppers._delay) / (stopTravel- ticks)

    elif(stopTravel<=ticks):
        pass

        
        
        
        
    
    
def increment(steps):
    
    for e in range(0, steps):
        setTrapezoidalDelay(steppers._virtPos)
        steppers.setPinsByOne(1)
        print(str(steppers._virtPos)+', '+str(steppers._delay))
        time.sleep(steppers._delay)
        
    

try:
    
    increment(stopTravel)
except KeyboardInterrupt:
    pass
    
Quit()
