import RPi.GPIO as GPIO #initializing GPIO pins
import time
import sys
import select
sys.path.insert(0, '/home/pi/raspberryPi_code')
from motor_direction import turn_left, turn_right, move_forward, move_backward, clear_pin
#import os

#----------------------------------------------------------------
# Interrupt Flags
global int_flag, int_fw_L, int_fw_R, int_lf, int_rg 
count = 0
int_flag = 0
int_fw_L = 0
int_fw_R = 0
int_lf = 0
int_rg = 0

#----------------------------------------------------------------
# Motor Pins
global MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW
MOTOR_1_FW = 29
MOTOR_1_BW = 31
MOTOR_2_FW = 33
MOTOR_2_BW = 35
MOTOR_ENABLE = 16

#----------------------------------------------------------------
# IR Sensor Pins
global IR_FW_L, IR_FW_R, IR_LF, IR_RG
IR_FW_L = 3
IR_FW_R = 13
IR_LF = 11
IR_RG = 7

ANGLE = 102
#----------------------------------------------------------------
# def clear_pin():
#     # global MOTOR_1_FW
#     # global MOTOR_1_BW 
#     # global MOTOR_2_FW 
#     # global MOTOR_2_BW 
#     GPIO.output(MOTOR_1_FW,False)
#     GPIO.output(MOTOR_1_BW,False)
#     GPIO.output(MOTOR_2_FW,False)
#     GPIO.output(MOTOR_2_BW,False)

#----------------------------------------------------------------
# Define a callback function that will be executed when the interrupt is triggered
def interrupt_IR_farward_left(channel):
    global int_fw_L
    print("Interrupt detected on channel ", channel)
    int_fw_L = 1

#----------------------------------------------------------------
def interrupt_IR_farward_right(channel):
    global int_fw_R
    print("Interrupt detected on channel ", channel)
    int_fw_R = 1

# #----------------------------------------------------------------
# def interrupt_IR_left(channel):
#     # global int_lf
#     #print("Interrupt detected on channel ", channel)
#     int_lf = 1

# #----------------------------------------------------------------
# def interrupt_IR_right(channel):
#     # global int_rg
#     #print("Interrupt detected on channel ", channel)
#     int_rg = 1
    
    
#----------------------------------------------------------------
# Setting the PinMode

# Motor Pins
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)
GPIO.setup(MOTOR_1_FW, GPIO.OUT)
GPIO.setup(MOTOR_1_BW, GPIO.OUT)
GPIO.setup(MOTOR_2_FW, GPIO.OUT)
GPIO.setup(MOTOR_2_BW, GPIO.OUT)
GPIO.setup(MOTOR_ENABLE, GPIO.OUT)

GPIO.output(MOTOR_1_FW,False)
GPIO.output(MOTOR_1_BW,False)
GPIO.output(MOTOR_2_FW,False)
GPIO.output(MOTOR_2_BW,False)
GPIO.output(MOTOR_ENABLE,True)

# IR Sensor Pins
GPIO.setup(IR_FW_L, GPIO.IN)
GPIO.setup(IR_FW_R, GPIO.IN)
GPIO.setup(IR_LF, GPIO.IN)
GPIO.setup(IR_RG, GPIO.IN)

#----------------------------------------------------------------
# Interrupts
GPIO.add_event_detect(IR_FW_L, GPIO.FALLING, callback=interrupt_IR_farward_left, bouncetime=300)
# GPIO.add_event_detect(IR_FW_R, GPIO.FALLING, callback=interrupt_IR_farward_right, bouncetime=300)
# GPIO.add_event_detect(IR_LF, GPIO.FALLING, callback=interrupt_IR_left, bouncetime=300)
# GPIO.add_event_detect(IR_RG, GPIO.FALLING, callback=interrupt_IR_right, bouncetime=300)

print("STARTING CODE:-")
clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW)
while True:
    if(int_fw_L or int_fw_R):
        if(GPIO.input(IR_LF) and GPIO.input(IR_RG)):
            print('Both and empty, turn right')
            # move_backward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, 1)
            turn_right(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, ANGLE)

        elif((not GPIO.input(IR_LF)) and GPIO.input(IR_RG)):
            print('Left is not empty but right is empty, so turn right')
            # move_backward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, 0.5)
            turn_right(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, ANGLE)

        elif((not GPIO.input(IR_RG)) and GPIO.input(IR_LF)):
            print('Right is not empty but left is empty, so turn Left')
            # move_backward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, 0.5)
            turn_left(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, ANGLE)

        elif((not GPIO.input(IR_LF)) and (not GPIO.input(IR_RG))):
            print('Both side are not empty, so move backward until u reach empty sides')

        int_fw = 0
        move_forward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, 0.3)
        clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW)
        int_fw_L = 0
        int_fw_R = 0


    if select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
        a =  sys.stdin.readline().strip()
        if('w' == a):
            print("MOVING FW:-")
            move_forward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, 0.2)
            # clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW)

        elif('d' == a):
            print("MOVING TO RIGHT:-")
            turn_right(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, ANGLE)
            clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW)

        elif('s' == a):
            print("MOVING BW:-")
            move_backward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, 1)
            
        elif('a' == a):
            print("MOVING TO RIGHT:-")
            turn_left(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, ANGLE)
            clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW)
        
        elif('f' == a):
            print("STOP")
            #clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW)
            GPIO.output(MOTOR_ENABLE,False)
            break

        elif('q' == a):
            print("PASS")
            #clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW)
            GPIO.output(MOTOR_ENABLE,True)
            # break


    