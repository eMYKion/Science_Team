import spidev
import RPi.GPIO as io
import time

#remember to setup SPI PINS
spi = spidev.SpiDev()   #make spi object
spi.open(0,0)    #open spi port 0 on device 0
spi.max_speed_hz=(2100000) #set SPI speed

def readadc(channel):
    adc = spi.xfer2([12+((6&channel)>>1),(1&channel)<<7,0])
    data = ((adc[1]&15) << 8) + adc[2]
    return data

#oldtime = time.time()
while True:
    print(readadc(4))
    time.sleep(0.1)
    
#print(time.time()-oldtime)
