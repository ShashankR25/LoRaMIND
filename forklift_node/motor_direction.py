import RPi.GPIO as GPIO
import time

class movement:
    def turn_right(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, angle):
        GPIO.output(MOTOR_1_FW,False)
        GPIO.output(MOTOR_2_BW,False)
        GPIO.output(MOTOR_1_BW,True)
        GPIO.output(MOTOR_2_FW,True)
        angle = angle/90
        time.sleep(angle - 0.45)

    def turn_left(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, angle):
        GPIO.output(MOTOR_1_BW,False)
        GPIO.output(MOTOR_2_FW,False)
        GPIO.output(MOTOR_1_FW,True)
        GPIO.output(MOTOR_2_BW,True)
        angle = angle/90
        time.sleep(angle - 0.45)

    def move_backward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, seconds):
        GPIO.output(MOTOR_1_BW,False)
        GPIO.output(MOTOR_2_BW,False)
        GPIO.output(MOTOR_1_FW,True)
        GPIO.output(MOTOR_2_FW,True)
        time.sleep(seconds)

    def move_forward(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW, seconds):
        GPIO.output(MOTOR_1_FW,False)
        GPIO.output(MOTOR_2_FW,False)
        GPIO.output(MOTOR_1_BW,True)
        GPIO.output(MOTOR_2_BW,True)
        time.sleep(seconds)


    def clear_pin(MOTOR_1_FW, MOTOR_1_BW, MOTOR_2_FW, MOTOR_2_BW):
        GPIO.output(MOTOR_1_FW,False)
        GPIO.output(MOTOR_1_BW,False)
        GPIO.output(MOTOR_2_FW,False)
        GPIO.output(MOTOR_2_BW,False)
    

if __name__ == "__main__":
    pass



