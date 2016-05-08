#!/usr/bin/python
#--------------------------------------   
# This script reads data from a 
# MCP3008 ADC device using the SPI bus.
#
# Author : Matt Hawkins
# Date   : 13/10/2013
#
# http://www.raspberrypi-spy.co.uk/
#
#--------------------------------------

import spidev
import time
import os
import RPi.GPIO as io

# Open SPI bus
spi = spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz=(3000000)

io.setmode(io.BCM)
io.setup(18, io.OUT)
io.PWM(18,100).start(0)


# Function to read SPI data from MCP3008 chip
# Channel must be an integer 0-7
def ReadChannel(channel):
  adc = spi.xfer2([1,(8+channel)<<4,0])
  data = ((adc[1]&3) << 8) + adc[2]
  if data == 0:
    print(SPI ERROR)
  return data

def ReadChannelRound(channel):
  sum = 0
  for X in range (0, 1):
    sum += ReadChannel(channel)
  return sum/50.0

# Function to convert data to voltage level,
# rounded to specified number of decimal places. 
def ConvertVolts(data,places):
  volts = (data * 5.0) / float(1023)
  volts = round(volts,places)  
  return volts
  
# Function to calculate temperature from
# TMP36 data, rounded to specified
# number of decimal places.
def ConvertTemp(data,places):

  # ADC Value
  # (approx)  Temp  Volts
  #    0      -50    0.00
  #   78      -25    0.25
  #  155        0    0.50
  #  233       25    0.75
  #  310       50    1.00
  #  388       75    1.25
  #  465      100    1.50
  #  543      125    1.75
  #  620      150    2.00
  #  698      175    2.25
  #  775      200    2.50
  #  853      225    2.75
  #  930      250    3.00
  # 1008      275    3.25
  # 1023      280    3.30

  temp = ((data * 330)/float(1023))-50
  temp = round(temp,places)
  return temp
  
# Define sensor channels
light_channel = 0
temp_channel  = 1

# Define delay between readings
counter = 0
delay = 0.05
try:
  while True:

    # Read the light sensor data
    light_level = ReadChannelRound(light_channel)
    light_volts = ConvertVolts(light_level,3)
    
    # Read the temperature sensor data
    temp_level = ReadChannelRound(temp_channel)
    temp_volts = ConvertVolts(temp_level,3)
    temp       = ConvertTemp(temp_level,2)
    
    # Print out results
    print "--------------------------------------------"  
    print("Light : {} ({}V)".format(light_level,light_volts))  
    print("Temp  : {} ({}V)".format(temp_level,temp_volts))
    print(ReadChannelRound(2))
    print(ReadChannelRound(3))
    print(ReadChannelRound(4))

    # Wait before repeating loop
    counter = counter+10
    io.PWM(18,100).ChangeDutyCycle(counter%100)
    time.sleep(delay)
except KeyboardInterrupt:
  x=0
 
