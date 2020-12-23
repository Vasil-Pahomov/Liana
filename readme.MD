# ESP8266/WS2812 Light Animation
## How to start
1. Connect your ESP, LED strip and power source as shown below (Wiring)
1. Upload binary files: [program file](https://github.com/Vasil-Pahomov/Liana/raw/master/bin/firmware.bin) at 0x000000, and [SPIFFS file](https://github.com/Vasil-Pahomov/Liana/raw/master/bin/spiffs.bin) at 0x100000. You should adjust settings for your board, particuarly COM port number, flash size, and SPI mode. I used official Flash download tools (can be found [here](https://www.espressif.com/en/support/download/other-tools)), and my working settings for NodeMCU board looked like this:

![Flash download tools settings](https://raw.githubusercontent.com/Vasil-Pahomov/Liana/master/pics/Download_tool_settings.png)

1. Connect to "Liana" wireless network and tap on pop-up authorization panel. You'll be brought directly to the Liana web interface (if not, go to http://192.168.4.1/ in the browser). You can configure connection to your WiFi network there.
1. Once you have connected Liana to your WiFi, open "http://liana.local" in web browser of any iOS, MacOS or Windows device on the same network. This does not work for Android however, you should find IP address of the Liana (e.g. in your router admin console, or using https://play.google.com/store/apps/details?id=com.dokoden.dotlocalfinder). Then type "http://<IP_ADDRESS>" in Android browser.

## Wiring
![Wiring diagram](https://raw.githubusercontent.com/Vasil-Pahomov/Liana/master/pics/Diagram.png)

Here example of NodeMCU is shown, but you can use any ESP8266 board. But note you may have to recompile the code! 
You power the board, power the WS2812 chain, and connect GPIO2 output of ESP to the data input of the firsh WS2812 in chain, that's all.
Be sure you use appropriate power source; most of ESP8266 board have voltage stabilizer but "raw" ESP8266 require 3.3V while WS2812 strips require 5V or 12V. You can use different power sources for ESP and the strip, you connect grounds then. Also, be sure that there's enough power for your LEDs and ESP; too weak power source can cause unintended side effects like random rebooting, wrong coloring etc.

## How to compile 
1. Currently the project is built with PlatformIO (https://platformio.org/) and Visual Studio Code (https://code.visualstudio.com/)
1. Setting up PlatformIO and Visual Studio Code is described here: https://docs.platformio.org/en/latest/integration/ide/vscode.html
1. All library dependencies are enumerated in platformio.ini file and should be downloaded automatically.
1. Be sure to set up your board settings in platformio.ini file (at least COM port for downloading)
1. Don't forget to upload SPIFFS image (Run task "Upload Filesystem Image (nodemcuv2)")

## Controlling the unit
Currently, when unit is powered on, it starts to play animations in random order, changing every 30 seconds. With web browser, you can control animations and palettes manually. Also you can switch the strip on and off.
The control interface is accessible at "http://liana.local" on Apple devces. For Android and Windows devices, you use IP address of your strip, which can be found out through serial port output or in the DHCP status page of your router.
There's special animation at the end of the list (called 'Magic'), designed for New Year trees mainly. When it's selected, you can control the lights with your device accelerometer (as shown in this video: https://youtu.be/ZXqNeq31DUs). You tilt the device to alter "target point", roll it to change color and lunge to highlight.
Note: this certainly works for mobile devices, not for laptops! Also it doesn't work in Chrome (thanks Google, they disabled access to device sensors in non-https sites).

## MQTT operation
MQTT broker connection is set up in web interface. Liana works as MQTT client only.
Commands are sent in plain text. Below description of commands goes. **{XXX}** is replaced with actual value of parameter
1. **on** - turn on
1. **off** - turn off
1. **palette={palette index}** - set palette, index numbering starts from 0
1. **animation={animation index}** - set animation, index numbering starts from 0
1. **duration={seconds}** - sets current effect (palette/animation) duration; random cycling starts after specified time elapses
1. **brightness={brightness}** - sets brigthness (0..255)
1. **brightness up** - increases brightness (exponential)
1. **brightness down** - decreases brightness ((exponential)
