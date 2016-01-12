import time
import RPi.GPIO as io

io.setmode(io.BCM)

COIL_A1 = 5
COIL_A2 = 6
COIL_B1 = 26
COIL_B2 = 13

io.setup(COIL_A1, io.OUT)
io.setup(COIL_A2, io.OUT)
io.setup(COIL_B1, io.OUT)
io.setup(COIL_B2, io.OUT)

STEP = [[1, 0, 1, 0],
        [0, 1, 1, 0],
        [0, 1, 0, 1],
        [1, 0, 0, 1]]

pos = 0
delay_max = 0.02
delay_min = 0.005
k = 1.2

def step(direction):
    global pos
    if direction == 0:
        io.output(COIL_A1, 0)
        io.output(COIL_A2, 0)
        io.output(COIL_B1, 0)
        io.output(COIL_B2, 0)
        print('release')
    elif direction == -1 or 1:
        pos += direction
        io.output(COIL_A1, STEP[pos%4][0])
        io.output(COIL_A2, STEP[pos%4][1])
        io.output(COIL_B1, STEP[pos%4][2])
        io.output(COIL_B2, STEP[pos%4][3])
    else:
        print('error')
try:
    while True:
        steps = input("Step number")
        print(steps/abs(steps))
        for x in range (0, abs(steps)):
            step(steps/abs(steps))
            sleep = delay_min + (delay_max-delay_min)*(k**-x+k**(x+1-abs(steps)))
            time.sleep(sleep)
            print(sleep)
except KeyboardInterrupt:
    step(0)
    io.cleanup()
