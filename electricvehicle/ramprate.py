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
DELAY_MIN = 0.0001

idealDelay = 0

steppers = stepper(COIL_A1, COIL_A2, COIL_B1, COIL_B2, DELAY_MAX)

def increment():
    
    while(steppers._delay>DELAY_MIN):
        steppers._delay -=0.00005
        for  i in range(0,100):
            steppers.setPinsByOne(1)
            print(steppers._delay)
            time.sleep(steppers._delay)

try:
    increment()
except KeyboardInterrupt:
    pass
    
Quit()
