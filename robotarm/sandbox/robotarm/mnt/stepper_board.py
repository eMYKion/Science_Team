import RPi.GPIO as io
import time
import math

io.setmode(io.BCM)

STEPPER_MOVE = 23
io.setup(STEPPER_MOVE, io.OUT)

microstep_res = 1
max_speed = 3400*microstep_res
min_speed = 10
accel = max_speed/1.5
delay = 1.0/min_speed  #initializes the delay
acceleration_period = int(round(max_speed**2/accel/2.0025, 0))
loop_rate_adjust = 0.00017

current_speed = min_speed

try:
    while 1:
        delay = 1.0/min_speed  #initializes the delay
        steps = input("Step number")
        pos = 0
        ramp_end = 0
        current_speed = min_speed
        delaylist = []
        repslist = range(abs(steps))
        for x in range (0, abs(steps)/2):
            current_speed = current_speed + accel*delay/math.log(current_speed)
            pos=pos+1
            #print(current_speed)
            if current_speed > max_speed:
                current_speed = max_speed
                delay = 1.0/current_speed
                delaylist.append(delay)
                ramp_end = x
                print('MAX SPEED')
                break
            delay = 1.0/current_speed
            delaylist.append(delay)
        print(current_speed)    
        if ramp_end > 0:
            for x in range (0, abs(steps) - 2*ramp_end-1):
                delaylist.append(delay)
                pos=pos+1
            for x in range (0, ramp_end):
                #print(1.0/delaylist[ramp_end-x])
                delaylist.append(delaylist[ramp_end-x])
                pos=pos+1
        else:
            for x in range (0, abs(steps)/2):
                #print(1.0/delaylist[abs(steps)/2 - x-1])
                delaylist.append(delaylist[abs(steps)/2 - x-1])
                pos=pos+1

        #print(pos)
        
        for i in repslist:
            time.sleep(delaylist[i]-loop_rate_adjust)
            io.output(STEPPER_MOVE, 1)
            io.output(STEPPER_MOVE, 0)
            
            
except KeyboardInterrupt:
    io.cleanup()
