import win32com.client
import serial
import json
import sys


###########
## User Configurable Section or through command line
###########
wled_com_port = "COM3"
wled_baud_rate = 115200

if len(sys.argv) > 1:
    wled_com_port = str(sys.argv[1])
    if len(sys.argv) > 2:
        wled_baud_rate = int(sys.argv[2])
#
## End of User Configurable Section
###########


wled_serial = serial.Serial(port=wled_com_port, baudrate=wled_baud_rate)

auraSdk = win32com.client.Dispatch("aura.sdk.1")
auraSdk.SwitchMode()
devices = auraSdk.Enumerate(0)

while True:
    wled_serial.write(b'l')
    led_values = json.loads(wled_serial.readline())
    if "leds" in led_values:
        led_index = 0
        led_response_count = len(led_values["leds"])
        for dev in devices:
            for i in range(dev.Lights.Count):
                if led_index >= led_response_count:
                    break
                rgb = led_values["leds"][led_index]
                dev.Lights(i).Color = int("0x00"+rgb[-2:] + rgb[2:-2] + rgb[0:2], 16)
                led_index += 1
            dev.Apply()

