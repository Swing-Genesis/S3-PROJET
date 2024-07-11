import pygame
import serial
import time
import json

# Button and axis mappings
X_BUTTON = 3
Y_BUTTON = 4
LB_BUTTON = 6
RB_BUTTON = 7

RT_AXIS = 4
LT_AXIS = 5

class XBOXController:
    
    controller = None
    axis_data = None
    button_data = None
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

    def send_command(self, command):
        json_command = json.dumps(command)
        self.serial_port.write(json_command.encode('utf-8'))
        self.serial_port.write(b'\n')

    def listen(self):
        time.sleep(0.1)

        # Initialize button and axis data if not already done
        if not self.axis_data:
            self.axis_data = {}

        if not self.button_data:
            self.button_data = {}
            for i in range(self.controller.get_numbuttons()):
                self.button_data[i] = False

        while True:
            axis_updated = False
            button_updated = False
            
            for event in pygame.event.get():
                if event.type == pygame.JOYAXISMOTION:
                    self.axis_data[event.axis] = round(event.value, 2)
                    axis_updated = True

                if event.type == pygame.JOYBUTTONDOWN:
                    self.button_data[event.button] = True
                    button_updated = True

                if event.type == pygame.JOYBUTTONUP:
                    self.button_data[event.button] = False
                    button_updated = True

            if axis_updated or button_updated:
                command_data = self.get_combined_data()
                self.send_command(command_data)
                print(command_data)

    def get_combined_data(self):
        command_data = {"pulsePWM": 0, "magnet": 0}
        
        # Process axis data
        lt_mapped = -self.map(self.axis_data.get(LT_AXIS, 0), -1.0, 1.0, 0.0, 1.0)
        rt_mapped = self.map(self.axis_data.get(RT_AXIS, 0), -1.0, 1.0, 0.0, 1.0)

        if not self.isLTAxisInitialized and lt_mapped != 0:
            self.isLTAxisInitialized = True
        if not self.isRTAxisInitialized and rt_mapped != 0:
            self.isRTAxisInitialized = True

        if not self.isLTAxisInitialized:
            lt_mapped = 0
        if not self.isRTAxisInitialized:
            rt_mapped = 0

        real_value = lt_mapped + rt_mapped
        command_data["pulsePWM"] = round(real_value, 2)
        
        # Process button data
        if self.button_data.get(X_BUTTON, False):
            command_data["magnet"] = 1
        else:
            command_data["magnet"] = 0
        
        return command_data

    def map(self, value, leftMin, leftMax, rightMin, rightMax):
        leftSpan = leftMax - leftMin
        rightSpan = rightMax - rightMin

        valueScaled = float(value - leftMin) / float(leftSpan)

        return rightMin + (valueScaled * rightSpan)

def controller_main():
    serial_port = '/dev/ttyACM0'  # Replace with the correct serial port
    xbox = XBOXController(serial_port)
    xbox.listen()

controller_main()