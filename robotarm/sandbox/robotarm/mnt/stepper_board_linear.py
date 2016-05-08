import RPi.GPIO as io
import time

io.setmode(io.BCM)

STEPPER_MOVE = 23
io.setup(STEPPER_MOVE, io.OUT)

microstep_res = 1
max_speed = 3000*microstep_res
min_speed = 10
accel = max_speed/1.5
delay = 1.0/min_speed  #initializes the delay
acceleration_period = int(round(max_speed**2/accel/2.0025, 0))
print(acceleration_period)
current_speed = min_speed

try:
    while 1:
        delay = 1.0/min_speed  #initializes the delay
        steps = input("Step number")
        pos = 0
        delaylist = []
        repslist = range(abs(steps))
        if abs(steps) > 2*acceleration_period:
            for step in range (0, acceleration_period):
                current_speed = current_speed + delay*accel
                #print(current_speed)
                
                delay = 1.0/current_speed
                delaylist.append(delay)
                '''
                io.output(STEPPER_MOVE, 1)
                time.sleep(delay/2.0)
                io.output(STEPPER_MOVE, 0)
                time.sleep(delay/2.0)
                '''
            for step in range (0, abs(steps)-2*acceleration_period):
                #print(current_speed)
                delay = 1.0/current_speed
                delaylist.append(delay)
                
                '''
                io.output(STEPPER_MOVE, 1)
                time.sleep(delay/2.0)
                io.output(STEPPER_MOVE, 0)
                time.sleep(delay/2.0)
                '''
        
            for step in range (0, acceleration_period):
                current_speed = current_speed - accel*delay
                #print(current_speed)
                delay = 1.0/current_speed
                delaylist.append(delay)
                
                '''
                io.output(STEPPER_MOVE, 1)
                time.sleep(delay/2.0)
                io.output(STEPPER_MOVE, 0)
                time.sleep(delay/2.0)
                '''
        else:
            if abs(steps)%2 == 0:
                ramp_up = abs(steps/2)
                ramp_down = abs(steps/2)
            else:
                ramp_up = (abs(steps)+1)/2
                ramp_down = (abs(steps)-1)/2
                
            for step in range (0, ramp_up):
                current_speed = current_speed + delay*accel
                #print(current_speed)
                delay = 1.0/current_speed
                delaylist.append(delay)
                
                '''
                io.output(STEPPER_MOVE, 1)
                time.sleep(delay/2.0)
                io.output(STEPPER_MOVE, 0)
                time.sleep(delay/2.0)
                '''
            for step in range (0, ramp_down):
                current_speed = current_speed - accel*delay
                #print(current_speed)
                delay = 1.0/current_speed
                delaylist.append(delay)
                
                '''
                io.output(STEPPER_MOVE, 1)
                time.sleep(delay/2.0)
                io.output(STEPPER_MOVE, 0)
                time.sleep(delay/2.0)
                '''
        for i in repslist:
            time.sleep(delaylist[i])
            io.output(STEPPER_MOVE, 1)
            io.output(STEPPER_MOVE, 0)
            pos=pos+1
        print(pos)

        current_speed = min_speed

            
except KeyboardInterrupt:
    io.cleanup()
