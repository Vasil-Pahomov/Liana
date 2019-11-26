# ESP8266/WS2812 Light Animation
## How to start
1. Connect your ESP, LED strip and power source as shown below (Wiring)
1. Upload binary files: [program file](https://github.com/Vasil-Pahomov/Liana/raw/master/bin/Liana.ino.bin) at 0x000000, and [SPIFFS file](https://github.com/Vasil-Pahomov/Liana/raw/master/bin/Liana.spiffs.bin) at 0x100000. You should adjust settings for your board, particuarly COM port number, flash size, and SPI mode. I used official Flash download tools (can be found [here](https://www.espressif.com/en/support/download/other-tools)), and my working settings for NodeMCU board looked like this:

![Flash download tools settings](https://raw.githubusercontent.com/Vasil-Pahomov/Liana/master/pics/Download_tool_settings.png)

1. Connect to "Liana" wireless network and tap on pop-up authorization panel. You'll be brought directly to the Liana web interface (if not, go to http://192.168.4.1/ in the browser). You can configure connection to your WiFi network there.
1. Once you have connected Liana to your WiFi, open "http://liana.local" in web browser of any iOS, MacOS or Windows device on the same network. This does not work for Android however, you should find IP address of the Liana (e.g. in your router admin console, or using https://play.google.com/store/apps/details?id=com.dokoden.dotlocalfinder). Then type "http://<IP_ADDRESS>" in Android browser.

## How to compile (advanced)
1. Set up Arduino IDE to work with ESP as described [here](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/) Note to install 2.5.2 version of ESP8266 board (2.6.0 does not work, not sure why)
1. Install libraries (all can be found in the library manager)

	[NeoPixelBus by Makuna](https://github.com/Makuna/NeoPixelBus)

	[WebSockets library by Markus Sattler](https://github.com/Links2004/arduinoWebSockets)

	[SPIFFSReadServer by Ryan Downing](https://github.com/r-downing/SPIFFSReadServer)

	[PerfWiFiManager by Ryan Downing](https://github.com/r-downing/PersWiFiManager)
1. Clone the repository (or download and unpack ZIP)
1. Upload data files to SPIFFS with [the plugin](https://github.com/esp8266/arduino-esp8266fs-plugin)
1. Set number of LEDs in your strip in the LEDS define at the beginning of anim.h
1. Compile and run the sketch

## Wiring
![Wiring diagram](https://raw.githubusercontent.com/Vasil-Pahomov/Liana/master/pics/Diagram.png)

Here example of NodeMCU is shown, but you can use any ESP8266 board. You power the board, power the WS2812 chain, and connect GPIO2 output of ESP to the data input of the firsh WS2812 in chain, that's all.
Be sure you use appropriate power source; most of ESP8266 board have voltage stabilizer but "raw" ESP8266 require 3.3V while WS2812 strips require 5V or 12V. You can use different power sources for ESP and the strip, you connect grounds then. Also, be sure that there's enough power for your LEDs and ESP; too weak power source can cause unintended side effects like random rebooting, wrong coloring etc.

## Controlling the unit
Currently, when unit is powered on, it starts to play animations in random order, changing every 30 seconds. With web browser, you can control animations and palettes manually. Also you can switch the strip on and off.
The control interface is accessible at "http://liana.local" on Apple devces. For Android and Windows devices, you use IP address of your strip, which can be found out through serial port output or in the DHCP status page of your router.
There's special animation at the end of the list (called 'Magic'), designed for New Year trees mainly. When it's selected, you can control the lights with your device accelerometer (as shown in this video: https://youtu.be/ZXqNeq31DUs). You tilt the device to alter "target point", roll it to change color and lunge to highlight.
Note: this certainly works for mobile devices, not for laptops! Also it doesn't work in Chrome (thanks Google, they disabled access to device sensors in non-https sites).
