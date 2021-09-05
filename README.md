# WLEDAuraSync
Controlling Aura Sync devices through WLED

&nbsp;

## Back Story

I recently 3D printed the wall lights shown in the video and wanted to sync it to my desktop RGB since they are close to each other on my desk.
I tried diyHUE with the Armoury Crate app since it can sync with a hue bridge, but it only supports 'static' and 'color cycle' modes and it didnt even work since philips changed their api and the good folks at diyHUE are still working on it (great project btw)
I then tried OpenRGB which would have made it possible with some kind of MQTT proxy but unfortunetly it didn't support my specific Crucial Ballistix DRAM
I then moved to installing WLED on the wall lights and started this project to control my desktop lights with WLED as well so I can take advantage of te integrations with home asssitant, the amazing WLED effects, realtime sync and so on...

&nbsp;

## Description & Guide
1. The project uses the Aura Sync API [Asus Aura SDK V3.1](https://www.asus.com/microsite/aurareadydevportal/index.html)
and needs the "lighting service" install, I couldnt really find a standalone installer for it even thought the guide in the sdk link mentions it, it however automatically installs with Armoury Crate so maybe install it then uninstall it keepingt he lighting service"

2. The client app communicates with an esp8266 (Wemos D1 mini) through serial to get led data from WLED and requires a custom WLED version that adds this serial output capabilities available at [WLED with Serial Out](https://github.com/ShadyNawara/WLED) (Pull Request Pending)
which can be compiled and flashed using the WLED guide [Compile Guide](https://github.com/Aircoookie/WLED/wiki/Compiling-WLED) or if you are using the Wemos D1 mini, I have my compiled binary in releases

3. when running the app you should see a window similar to this (window hidden when starting from startup folder or with nowindow arg)

&nbsp;

<img width="675" alt="mainwindow" src="https://user-images.githubusercontent.com/981568/132116570-ef91a008-8963-4d43-8d87-9d9a19757111.png">

&nbsp;

4. the devices list are in order as far as I can tell, the way the SDK works is that it returns the total count of lights associated with a certain device but doesnt meen that all of them are used. in my case I had only 21 leds in the addressableStrip 1 and nothing connected to the addressableStrip 2, you can find that out either by counting or following the next step and keep trying till all the lights are working

5. To configure WLED to control your lights you must set WLED to the total amount of lights you want to control in order so for examble if I want to control the first 5 LEDs in the AddressableStrip 1, I must have WLED configured with atleast 29 leds. You can also use WLED segments and match the led numbers for better control. Try not to set the LEDs in WLED to more than you need because it will slow things down

6. After WLED is configured you can move the .exe file to your startup folder to run on desktop start (win + r then "shell:startup"), p.s. windows defender marked the exe I compiled as trojan which seems like a false positive. feel free to compile your own version

7. The exe runs with 3 optional command line arguments like this ".\WLEDAuraSync.exe COM3 115200 nowindow"

8. The default baud rate is 115200 which gave me about 47 fps or pixel updates per second. you can compile your own firmware (or use the release for Wemos D1 mini) by changing the baud rate to 921600 in [Wled.cpp #L263 Serial.begin](https://github.com/Aircoookie/WLED/blob/1d4487b6cd7c9a69bbce45c14ae1fb1c622e1d0e/wled00/wled.cpp#L263). the 921600 baud rate gave me constant 70 fps and thats what I am running myself but I will not be adding this to my pull request for WLED

9. There is both a c++ and a python version available in the folders cpp and python respectively. I created the python version first but wanted to see if the Aura SDK was faster in c++, both languages gave the exact same fps. (I will probably maintain the c++ version more)

&nbsp;

## Build
### Python
in a venv
```
pip install -r requirements.txt
python main.py
```

### C++
1. open in visual studio 2019 and build
2. version of serial library added is for Debug_x86 to make building easier, if you want to change configurations you will have to build the serial library as well, it uses visual studio too so its very easy to build [Serial | Github](https://github.com/wjwwood/serial)

## Final Result

The PC connected WLED is running in sync mode with the wall WLED
&nbsp;



https://user-images.githubusercontent.com/981568/132116960-584661ce-45da-4f1b-b2f8-d15433d751a7.mp4



