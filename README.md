# Watering station
Watering station project based on Arduino microcontroller.
System is turn on payload at configured watering time (twice, before and after the noon) and with configured duration. Also it's possible to configure all values by keypad and to see current system status on display.

## Components:
1. [Arduino UNO R3 (CH340/ ATmega328P)](http://www.kosmodrom.com.ua/el.php?name=ARDUINO-UNO-R3-MEGA328P);
2. [Power supply for Arduino UNO (OV-A002 9V/1A)](https://www.rcscomponents.kiev.ua/product/ov-a002-9v-1a_67292.html);
3. [5V Rellay for Arduino](http://www.kosmodrom.com.ua/el.php?name=1CH5VRMA);
4. [Real Time Clock](http://www.kosmodrom.com.ua/el.php?name=DS1302-MOD);
5. [Rain sensor](http://www.kosmodrom.com.ua/el.php?name=T1592-MODUL);
6. [LCD Keypad Shield](http://www.kosmodrom.com.ua/el.php?name=LCD1602KEYPAD);
7. [Jumpers FM](http://www.kosmodrom.com.ua/el.php?name=JUMPERS-FM-100MM);
8. [Device body](https://www.rcscomponents.kiev.ua/product/21-12-47-sanhe-korpus-plastik-140x82x23mm_36265.html).


## Soft:
- [Arduino IDE](https://www.arduino.cc/en/Main/Software);
- [Driver for USB on chipset CH340](http://www.arduined.eu/ch340-windows-8-driver-download/);
- [Library 'iarduino_RTC' (for RTC DS1302, DS1307, DS3231)](http://iarduino.ru/lib/ef47f92e639da4d4e009cc6f16dedd12.zip).

## Circiut diagram
![Circiut diagram](/Docs/Circuit_diagram.PNG)

## Pinout diagram
![Pinout diagram](/Docs/Pinout.PNG)

## User guide
1. **Get started**

For device set up, plug the outdoor sensor and high voltage cable according to the 'Pinout diagram'. Then plug power supply. After the boot you will see the current date on a top line. The device is ready to use.

The default settings are:

- First watering time is 06:00 with 30 minutes duration;
- Second watering time is 18:00 with 30 minutes duration.

Each parameters can be configured by settings.

2. **Screen indication**

   "15.12.2017 11:09" - Normal mode. Current date and time indication;
   
   "W18:00 -> -03:28" - Normal mode. Next watering will be at 18:00 in 3 hours and 28 minutes;
   
   "W18:00 'LL SKIP!" - Normal mode. Watering at 18:00 will be skip;
   
   "WATERING: -00:11" - Normal mode. It is watering right now. And will continue for 11 more minutes;
   
   "*FORSE WATERING*" - Normal mode. It is forse watering active;
   
   "< W.DURATION 1 >" - Configuration mode. Changing the duration of the first watering.
   

3. **Keys and parts**

- **Mode key:** Changes device mode (normal or configuration)
- **Left key:** 

  Normal mode: Skip next watering (one time). For regular cancellation set duration to zero
  
  Configuration mode: Navigates between parameters (Watering Hours 1, Watering Minutes 1, Duration 1, Watering Hours 2, Watering Minutes 2, Duration 2)
  
- **Right key:** 
  
  Normal mode: Turn on/off forse watering
  
  Configuration mode: Navigates between parameters like the "Left key"
  
- **Up key:** 

  Normal mode: Nothing
  
  Configuration mode: Increase current parameter

- **Down key:**
  
  Normal mode: Nothing
  
  Configuration mode: Decrease current parameter
  
- **Reset key:** Reboots device and sets settings to default.
