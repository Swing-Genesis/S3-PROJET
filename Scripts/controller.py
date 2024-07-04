import pygame
import serial, time
import json

X_BUTTON = 3
Y_BUTTON = 4
LB_BUTTON = 6
RB_BUTTON = 7

RT_AXIS = 4
LT_AXIS = 5

class XBOXController(object):
    
    controller = None
    axis_data = None
    button_data = None
    hat_data = None
    isRTAxisInitialized = False
    isLTAxisInitialized = False

    def __init__(self, serial_port):
        pygame.init()
        pygame.joystick.init()
        self.controller = pygame.joystick.Joystick(0)
        self.controller.init()
        self.serial_port = serial.Serial(serial_port, 9600)
        self.axis_data = {}
        self.button_data = {}
        self.hat_data = {}

    def send_command(self, command):
        json_command = json.dumps(command)
        self.serial_port.write(json_command.encode('utf-8'))
        self.serial_port.write(b'\n')

    def listen(self):

        time.sleep(0.1)

        if not self.axis_data:
            self.axis_data = {}

        if not self.button_data:
            self.button_data = {}
            for i in range(self.controller.get_numbuttons()):
                self.button_data[i] = False

        if not self.hat_data:
            self.hat_data = {}
            for i in range(self.controller.get_numhats()):
                self.hat_data[i] = (0, 0)

        while True:
            for event in pygame.event.get():

                if event.type == pygame.JOYAXISMOTION:

                    self.axis_data[event.axis] = round(event.value, 2)
                    
                    if event.axis in [LT_AXIS, RT_AXIS]:
                        lt_mapped = -self.map(self.axis_data.get(LT_AXIS,0), -1.0, 1.0, 0.0, 1.0)
                        rt_mapped = self.map(self.axis_data.get(RT_AXIS,0), -1.0, 1.0, 0.0, 1.0)

                        if(event.axis == LT_AXIS and not self.isRTAxisInitialized):
                            rt_mapped = 0
                            self.isLTAxisInitialized = True

                        if(event.axis == RT_AXIS and not self.isLTAxisInitialized):
                            lt_mapped = 0
                            self.isRTAxisInitialized = True

                        real_value = lt_mapped + rt_mapped
                        print(round(real_value,2))

                        self.send_command({"pulsePWM": real_value})

                if event.type == pygame.JOYBUTTONDOWN:
                    self.button_data[event.button] = True
                    if event.button == X_BUTTON:
                        self.send_command({"magnet": 1})
                        print("Aimant : true")

                if event.type == pygame.JOYBUTTONUP:
                    self.button_data[event.button] = False
                    if event.button == X_BUTTON:
                        self.send_command({"magnet": 0})
                        print("Aimant : false")

    def map(self, value, leftMin, leftMax, rightMin, rightMax):
        leftSpan = leftMax - leftMin
        rightSpan = rightMax - rightMin

        valueScaled = float(value - leftMin) / float(leftSpan)

        return rightMin + (valueScaled * rightSpan)

def controller_main():
    serial_port = '/dev/ttyACM0'  # Remplacez par le port série correct
    xbox = XBOXController(serial_port)
    xbox.listen()

controller_main()