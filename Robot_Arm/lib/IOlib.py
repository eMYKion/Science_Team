#IO lib for Raspberry Pi in Robot Arm, Science Olympiad, December 2015

#Authors: Mayank Mali and Peter Wilson
#Description: Uses harware-fixed spi interface pins, so no GPIO imports are required

import spidev
import RPi.GPIO as io

#remember to setup SPI PINS
spi = spidev.SpiDev()   #make spi object
spi.open(0,0)           #open spi port 0 on device 0

#io modes
IO_BCM = 0b101
IO_BOARD = 0b011

def IOinit(mode):       #IO_BCM or IO_BOARD
    if(mode==IO_BCM):
        io.setmode(io.BCM)
    elif(mode==IO_BOARD):
        io.setmode(io.BOARD)
def IOquit():
    io.cleanup()

# read SPI data from MCP3008 chip, 8 possible adc's (0 thru 7)
def readadc(adcnum):
    if ((adcnum > 7) or (adcnum < 0)):
            return -1
    r = spi.xfer2([1,(8+adcnum)<<4,0])
    adcout = ((r[1]&3) << 8) + r[2]
    return adcout

def close_readadc(adcnum, val, times=3):

    final = val
    for e in range(0,times):
        temp = readadc(adcnum)
        if(abs(temp-val)<abs(final-val)):
            final = temp
        #now final is closest of the data to original val

    return final

class IOUnit:

    _inChannel = None
    _outF = None
    _outB = None
    _pwmF=None
    _dutyF = None
    _pwmB=None
    _dutyB = None

    _potVal = 0

    def __init__(self, inChannel=None, outF=None, outB=None, freq=None, dutyF=None, dutyB=None):
        if(inChannel!=None):
            self._inChannel = inChannel
            #spi pins already setup
        if(outF!=None):
            self._outF = outF
            io.setup(self._outF, io.OUT)
        if(outB!=None):
            self._outB = outB
            io.setup(self._outB, io.OUT)
        if((freq!=None) & (dutyF!=None)& (dutyB!=None)):
            self._pwmF = io.PWM(self._outF, freq)
            self._dutyF = dutyF

            self._pwmB = io.PWM(self._outB, freq)
            self._dutyB = dutyB



    #for out
    def outF(self, state):
        io.output(self._outF, state)
    def outB(self, state):
        io.output(self._outB, state)

    #for in
    def inADC(self):
        return readadc(self._inChannel)

    def inCloseADC(self, times=3):
        return close_readadc(self._inChannel, self._potVal, times)

    def pwmFStart(self):
        self._pwmF.start(self._dutyF)
    def pwmBStart(self):
        self._pwmB.start(self._dutyB)

    def pwmFChangeDutyCycle(self, duty):
        self._dutyF = duty
        self._pwmF.changeDutyCycle(duty)
    def pwmBChangeDutyCycle(self, duty):
        self._dutyB = duty
        self._pwmB.changeDutyCycle(duty)

    def pwmFStop(self):
        self._pwmF.stop()
    def pwmBStop(self):
        self._pwmB.stop()
#end        
