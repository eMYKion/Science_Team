
#shoulder + elbow on/off code, using 4 pots 

from full_lib import *
import time

EL_PIN_FORWARD = 20
EL_PIN_BACKWARD = 25
EL_BRAKE_PIN = 23

#replace later
SH_PIN_FORWARD = 17
SH_PIN_BACKWARD = 27
SH_BRAKE_PIN = 19


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



def setTolSlope():
  el._tolerance = 2
  el._tol_max = 42
  el._slope = 100/(el._tol_max - el._tolerance)
  el._intercept = -el._tolerance*el._slope

  sh._tolerance = 2
  sh._tol_max = 42
  sh._slope = 100/(sh._tol_max - sh._tolerance)
  sh._intercept = -sh._tolerance*sh._slope
  


def getOffset():
  for x in range (0, 10):
    sh._offset += round(sh.inavgADCMaster() - sh.inavgADCSlave(),3)
    el._offset += round(el.inavgADCMaster() - el.inavgADCSlave(),3)
  sh._offset = round(sh._offset/10, 3)
  el._offset = round(el._offset/10, 3)

def updateElShPots():
  el._potValMaster = el.rolling_readadc_Master(el._potValMaster)
  el._potValSlave = el.rolling_readadc_Slave(el._potValSlave)
  el._diff = round(el._potValMaster - el._potValSlave - el._offset, 2)
  sh._potValMaster = sh.rolling_readadc_Master(sh._potValMaster)
  sh._potValSlave = sh.rolling_readadc_Slave(sh._potValSlave)
  sh._diff = round(sh._potValMaster - sh._potValSlave - sh._offset, 2)

setTolSlope()

el.pwmFStart()
el.pwmBStart()
sh.pwmFStart()
sh.pwmBStart()


getOffset()


#==============STEPPER======================
BASE_IN_CHANNEL_MASTER = 4
base = IOUnit(BASE_IN_CHANNEL_MASTER)

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

stepperPos = 0
for e in range(0, 5):
  stepperPos += 1.4*base.inavgADCMaster()
stepperPos/=5
stepperPos = int(stepperPos)
basepot = stepperPos


speed = 0
accel = 20
max_speed = 100
delay = 0.5

def step(direction):
    global stepperPos
    if direction == 0:
        io.output(COIL_A1, 0)
        io.output(COIL_A2, 0)
        io.output(COIL_B1, 0)
        io.output(COIL_B2, 0)
        print('release')
    elif direction == -1 or 1:
        stepperPos += direction
        io.output(COIL_A1, STEP[stepperPos%4][0])
        io.output(COIL_A2, STEP[stepperPos%4][1])
        io.output(COIL_B1, STEP[stepperPos%4][2])
        io.output(COIL_B2, STEP[stepperPos%4][3])
    else:
        print('error')

def stepperspeed(old_speed):
  global basepot
  global error
  global speed
  basepot = (1.4*base.inavgADCMaster())/3.0 + basepot/3.0*2
  error = round(stepperPos - basepot, 3)
  if(error > 2):
    speed += accel
  elif(error < -2):
    speed -= accel
  else:
    speed = 0
  if(speed < -max_speed):
    speed = -max_speed
  elif(speed > max_speed):
    speed = max_speed


#print (sh._offset, el._offset)

stringFormat = "SH_M %4.2f SH_S %4.2f SH_D %4.2f || EL_M %4.2f EL_S %4.2f EL_D %4.2f"


oldTime = time.time()


def run_main():
  try:
    global oldTime
    global speed
    global basepot
    while 1:
      newTime = time.time()
      timeElapsed = newTime - oldTime
      
      

      updateElShPots()

      #=============STEPPER================
        
      if(speed!=0):
        delay = 1/abs(speed)
        if(timeElapsed >= delay):
          oldTime = newTime
          step(int(-speed/abs(speed)))
          stepperspeed(speed)
        else:
          pass
      else:
        stepperspeed(speed)
      
      
      #print(basepot, error, stepperPos, speed)
      #print(stringFormat % (sh._potValMaster,sh._potValSlave,sh._diff,el._potValMaster,el._potValSlave,el._diff))
      
      if (el._diff > el._tolerance):
        el.brakeoff()
      	#forward
        el.pwmBChangeDutyCycle(0)
        #print('EL FORWARD')

        
        if(el._diff>el._tol_max):#max
          el.pwmFChangeDutyCycle(100)
          asdf=1
        else:
          el.pwmFChangeDutyCycle(el.dutyFunction())
          asdf=1
          
      elif(el._diff < -1*el._tolerance):
      	#backward
        #print('EL BACKWARD')
        el.brakeoff()
        el.pwmFChangeDutyCycle(0)

        if(el._diff<-1*el._tol_max):
          el.pwmBChangeDutyCycle(100)
          asdf=1
        else:
          el.pwmBChangeDutyCycle(el.dutyFunction())
          asdf=1
      else:
      	#still
        #print('EL STILL')
        el.brakeon()
        pass
        
      if(sh._diff < -1*sh._tolerance):
      	#forward
        #print('SH FORWARD')
        sh.brakeoff()
        sh.pwmBChangeDutyCycle(0)

        if(sh._diff<-1*sh._tol_max):
          sh.pwmFChangeDutyCycle(100)
          asdf=1
        else:
          sh.pwmFChangeDutyCycle(sh.dutyFunction())
          asdf=1
      	
      elif(sh._diff > sh._tolerance):
      	#backward
        #print('SH BACKWARD')
        sh.brakeoff()
        sh.pwmFChangeDutyCycle(0)

        if(sh._diff>sh._tol_max):
          sh.pwmBChangeDutyCycle(100)
          asdf=1
        else:
          sh.pwmBChangeDutyCycle(sh.dutyFunction())
          asdf=1
      	
      else:
      	#still
        #print('SH STILL')
        sh.brakeon()
        pass
      


  except KeyboardInterrupt:
    el.pwmFStop()
    el.pwmBStop()
    sh.pwmFStop()
    sh.pwmBStop()
    IOquit()

run_main()

