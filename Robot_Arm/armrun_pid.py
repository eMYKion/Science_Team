#shoulder + elbow on/off code, using 4 pots 

from IOlib import *
import time

EL_PIN_FORWARD = 21
EL_PIN_BACKWARD = 16
EL_BRAKE_PIN = 19

#replace later
SH_PIN_FORWARD = 17
SH_PIN_BACKWARD = 27
SH_BRAKE_PIN = 20


PWM_FREQ = 1000
DUTY_FORWARD = 100
DUTY_BACKWARD = 100


#we correct these to true values later
EL_IN_CHANNEL_MASTER = 1
EL_IN_CHANNEL_SLAVE = 3
SH_IN_CHANNEL_MASTER = 0
SH_IN_CHANNEL_SLAVE = 2

IOinit(IO_BCM) #initializes to BCM mode

el = IOUnit(EL_IN_CHANNEL_MASTER, EL_IN_CHANNEL_SLAVE, EL_PIN_FORWARD, EL_PIN_BACKWARD, EL_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
#no input, backward and forward out enabled, brake enabled, backward and forward PWM enabled

sh = IOUnit(SH_IN_CHANNEL_MASTER, SH_IN_CHANNEL_SLAVE, SH_PIN_FORWARD, SH_PIN_BACKWARD, SH_BRAKE_PIN, PWM_FREQ, DUTY_FORWARD, DUTY_BACKWARD)
#no input, backward and forward out enabled, brake enabled, backward and forward PWM enabled

el.pwmFStart()
el.pwmBStart()
sh.pwmFStart()
sh.pwmBStart()

el._k_pos = 
el._k_vel = 
el._k_accel = 
sh._k_pos = 
sh._k_vel = 
sh._k_accel = 

sh._offset = round(sh.inavgADCMaster(50) - sh.inavgADCSlave(50), 3)
el._offset = round(el.inavgADCMaster(50) - el.inavgADCSlave(50), 3)

el._positionMaster = inavgADCMaster()
el._positionSlave = inavgadjustADCSlave()
sh._positionMaster = inavgADCMaster()
sh._positionSlave = inavgadjustADCSlave()

def run_main():
  try: 
    while 1:
      time = time.clock()
      el._positionMaster = el.inavgADCMaster()
      el._velocityMaster = (el._positionMaster - el._oldpositionMaster)/time
      el._accelerationMaster = (el._velocityMaster - el._oldvelocityMaster)/time
      
      el._positionSlave = el.inavgadjustADCSlave()
      el._velocitySlave = (el._positionSlave - el._oldpositionSlave)/time
      el._accelerationSlave = (el._velocitySlave - el._oldvelocitySlave)/time

      sh._positionMaster = sh.inavgADCMaster()
      sh._velocityMaster = (sh._positionMaster - sh._oldpositionMaster)/time
      sh._accelerationMaster = (sh._velocityMaster - sh._oldvelocityMaster)/time
      
      sh._positionSlave = el.inavgadjustADCSlave()
      sh._velocitySlave = (sh._positionSlave - sh._oldpositionSlave)/time
      sh._accelerationSlave = (sh._velocitySlave - sh._oldvelocitySlave)/time

      sh.Pidval_update()
      el.Pidval_update()

      #need to fix the signs
      if sh._PIDval>0:
        sh.brakeoff()
        if sh._PIDval>100:
          sh.pwmFChangeDutyCycle(100)
        else:
          sh.pwmFChangeDutyCycle(sh._PIDval)
      elif sh._PIDval<0:
        sh.brakeoff()
        if sh._PIDval<-100:
          sh.pwmBChangeDutyCycle(100)
        else:
          sh.pwmBChangeDutyCycle(-sh._PIDval)
      else:
        if sh._positionMaster == sh._positionSlave:
          sh.brakeon()
        else:
          sh.pwmFChangeDutyCycle(0)

      if el._PIDval>0:
        el.brakeoff()
        if el._PIDval>100:
          el.pwmFChangeDutyCycle(100)
        else:
          el.pwmFChangeDutyCycle(el._PIDval)
      elif el._PIDval<0:
        el.brakeoff()
        if el._PIDval<-100:
          el.pwmBChangeDutyCycle(100)
        else:
          el.pwmBChangeDutyCycle(-el._PIDval)
      else:
        if abs(el._positionMaster - el._positionSlave)<2:
          el.brakeon()
        else:
          el.pwmFChangeDutyCycle(0)
        

      el._oldpositionMaster = el._positionMaster
      el._oldvelocityMaster = el._velocityMaster
      el._oldpositionSlave = el._positionSlave
      el._oldvelocitySlave = el._velocitySlave

      sh._oldpositionMaster = sh._positionMaster
      sh._oldvelocityMaster = sh._velocityMaster
      sh._oldpositionSlave = sh._positionSlave
      sh._oldvelocitySlave = sh._velocitySlave
  except KeyboardInterrupt:
    el.pwmFStop()
    el.pwmBStop()
    sh.pwmFStop()
    sh.pwmBStop()
    IOquit()

run_main()


