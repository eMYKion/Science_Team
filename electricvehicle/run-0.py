import RPi.GPIO as io
import time

def iobinInit():
    io.setmode(io.BCM)

def iobinReadPinArray(pinArray):
    Sum=0
    for i in range(0, len(pinArray)):
        
        io.setup(pinArray[i], io.IN)
    time.sleep(1)
        
    for i in range(0, len(pinArray)):
        bit = io.input(pinArray[i])
        Sum += bit*(2**i)
        print(bit, pinArray[i])
        time.sleep(0.1)

    return Sum

def iobinQuit():
    io.cleanup()

iobinInit()

print(iobinReadPinArray([18, 23, 24, 25, 12, 16, 20, 21, 19]))

iobinQuit()


    
    
