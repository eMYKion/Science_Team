from lib.stepperlib import *
import sys,math, time
import RPi.GPIO as io

#all measurements are metric or arbitrary

Init()

COIL_A1 = 5
COIL_A2 = 6
COIL_B1 = 26
COIL_B2 = 13

MOTOR_RESOLUTION = 200
WHEEL_CIRCUMFERENCE = math.pi*0.09

VELOCITY_MIN = 50
VELOCITY_MAX = 1/0.0015

steppers = stepper(COIL_A1, COIL_A2, COIL_B1, COIL_B2, 1/VELOCITY_MIN)

#distance to ticks
ticksPerMeter = int(MOTOR_RESOLUTION/WHEEL_CIRCUMFERENCE)
startMaxVelocity =  int(ticksPerMeter * float(sys.argv[1]))
stopMaxVelocity = int(ticksPerMeter * float(sys.argv[2]))
stopTravel = int(ticksPerMeter * float(sys.argv[3]))

currentVel=0

def trapVelProfile(currTicks):
    global currentVel
    
    if(0<=currTicks<startMaxVelocity):
        
        if(startMaxVelocity - currTicks != 0):
            currentVel += (VELOCITY_MAX - currentVel) / (startMaxVelocity - currTicks)
        
    elif(startMaxVelocity<=currTicks<stopMaxVelocity):
        
        currentVel = VELOCITY_MAX        
    elif(stopMaxVelocity<=currTicks<stopTravel):
        
        if(stopTravel - currTicks != 0):
            currentVel += (VELOCITY_MIN - currentVel) / (stopTravel - currTicks)

    elif(stopTravel<=currTicks):
        print("done")
        pass

        
        
        
        
    
    
def increment(steps):
    print(steps)
    for e in range(0, steps):
        trapVelProfile(steppers._virtPos)
        steppers.setPinsByOne(1)
        steppers._delay = 1.0/currentVel
        print(str(steppers._virtPos)+', '+str(currentVel)+', '+str(steppers._delay))
        time.sleep(steppers._delay)
        

io.setup(18, io.IN)

#signal on
steppers._delay = 0.0001
for e in range(0, 3):
    steppers.progress(100)
    time.sleep(0.05)
    
steppers._delay = 1/VELOCITY_MIN
steppers._virtPos = 0


try:
    
    while(1):
        if(io.input(18)==0):
            break
    time.sleep(2)
    
    increment(stopTravel)
except KeyboardInterrupt:
    pass
    
Quit()
