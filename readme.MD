# ESP8266/WS2812 Light Animation
## How to start
1. Connect your ESP, LED strip and power source as shown below (Wiring)
1. Set up Arduino IDE to work with ESP as described [here](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
1. Install [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) library (typically you search for "NeoPixelBus by Makuna" in the library manager and install it from there)
1. Clone the repository (or download and unpack ZIP)
1. Open Liana.ino in the Arduino IDE
1. Look for wifi.ino file. Change YOUR_SSID and YOUR_PASS there to match name and password of your WiFi network
1. Upload the sketch

## Wiring
![enter image description here](https://raw.githubusercontent.com/Vasil-Pahomov/Liana/master/Diagram.png)
Here example of NodeMCU is shown, but you can use any ESP8266 board. You power the board, power the WS2812 chain, and connect GPIO2 output of ESP to the data input of the firsh WS2812 in chain, that's all.
Be sure you use appropriate power source; most of ESP8266 board have voltage stabilizer but "raw" ESP8266 require 3.3V while WS2812 strips require 5V or 12V. You can use different power sources for ESP and the strip, you connect grounds then. Also, be sure that there's enough power for your LEDs and ESP; too weak power source can cause unintended side effects like random rebooting, wrong coloring etc.

## Controlling the unit
Currently, when unit is powered on, it starts to play animations in random order, changing every 30 seconds. With web browser, you can control animations and palettes manually. Also you can switch the strip on and off.
The control interface is accessible at "http://liana.local" on Apple devces. For Android and Windows devices, you use IP address of your strip, which can be found out through serial port output or in the DHCP status page of your router.
There's special animation at the end of the list (called 'Магия'), designed for New Year trees mainly. When it's selected, you can control the lights with your device accelerometer (as shown in this video: https://youtu.be/ZXqNeq31DUs). You tilt the device to alter "target point", roll it to change color and lunge to highlight.
Note: this certainly works for mobile devices, not for laptops!

## Как запустить
1. Подключите модуль ESP, ленту/гирлянду WS2812 и источник питания как показано на рисунке и описано ниже
1. Настройте Arduino IDE для работы с ESP2866 как описано, например [здесь](http://geekmatic.in.ua/Arduino_IDE_with_WiFi_ESP8266)
1. Установите библиотеку [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) (обычно достаточно найти "NeoPixelBus by Makuna" в менеджере библиотек Arduino IDE и установить обычным образом)
1. Склонируйте репозиторий или распакуйте скачанный ZIP-архив
1. Откройте Liana.ino в Arduino IDE
1. Найдите файл wifi.ino. Поменяйте там YOUR_SSID и YOUR_PASS на имя и пароль вашей сети WiFi
1. Загрузите скетч

## Подключение
Картинка есть вверху. Там показано подключение модуля NodeMCU, но можно использовать любой другой модуль с ESP8266. Подключаете питание к модулю, ленте светодиодов WS2812, и соединяете выход GPIO2 ESP со входом, данных первого светодиода в ленте, всё.
Смотрите, чтобы источник питания был подходящим; большинство плат ESP8266 имеют стабилизатор напряжения, но сама ESP8266 питается от 3.3В, а ленты WS2812 обычно требуют 5В или 12В. Можно использовать разные источники для ESP и ленты, тогда нужно объединить их земли. 
Источник должен быть достаточно мощным для того, чтобы запитать и ленту, и ESP, иначе могут возникать спонтанные перегрузки, неправильные цвета и т.п.

## Управление
Когда плата включается, она начинает проигрывать анимации в случайном порядке, меняя их каждые 30 секунд. Управлять анимациями и палитрами, а также включать и выключать ленту можно через web-браузер.
Интерфейс управления доступен по адресу "http://liana.local" на устройствах Apple. Для Android и Windows devices надо использовать IP-адрес платы, который выводится на последовательный порт платы, либо же находится на странице статуса DHCP в роутере.
В конце списка есть специальная анимация ('Магия'), подходящая больше для гирлянд на новогодней ёлке. Когда она выбрана, управление производится при помощи акселерометра мобильного устройства, на котором открыт интерфейс. На видео показано как это работает: https://youtu.be/ZXqNeq31DUs. Наклоняйте устройство для подсветки разных частей гирлянды, вращайте для смены цвета, и делайте выпады для подсвечивания.
Естественно, это не работает на ноутбуках и стационарных компьютерах.