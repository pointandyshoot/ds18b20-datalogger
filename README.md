# ds18b20-datalogger
This project uses an Arduino to record temperature from a string of DS18B20 sensors to SD card.

It uses the following shields and equipment:

1 x Arduino UNO
1 x Adafruit SD card logger with real time clock
1 x Sparkfun serial interface 16x2 LCD display
1 x 4xAA battery pack 
4 x Catalex Digital Temperature Sensor v1.0
    This is used for its built in resistor, one for each active arduino pin
    
The unit is able to use up to 4 Arduino pins at a time, each one containing a long string of DS18B20 temperature sensors. In practise I have good reliability with up to ~30 sensors per string, each one about 30cm from the last. Beyond that they start dropping out all the sensors on that string.

The display tends to be dim when running lots of sensors at once (~80-100) while running off battery power.

I have encountered a range of issues getting the program to run with many sensors (such as data corruption etc). Most of these are resolved however the current version seems to have issues compiling and uploading on my PC (regardless of the Arduino IDE I use). It compiles quite happily on my Raspberry Pi using Arduino IDE 1.0.5. The error I encounter is that the program will not read any sensors correctly in loop().

Hardware:
Plug the SD card shield and a pin breakout shield into the arduino.

Pin 2, 3, 4 and 5 are the 4 sensor strings
Pin 6 drives the LCD display

The sensor string needs to be wired GND, DATA, VCC (left to right when looking at the flat side). It needs at least one pullup resistor. More don't seem to help or hinder. Placement of the resistor doesn't seem to matter either, I have them on my first sensor. I pkay with this a bit trying to get longer strings to work.

Software:
There are two programs.

Setup - use this to record your sensor string addresses [SENSOR.TXT]. Start with no sensors connected and add them one at a time to pin 2. You will get a file that has the address of each sensor in your string, in the order that you added it. This is useful for interpreting your data if you care about where each sensor is located.

Datalogger - this is the main program, it will record from all 4 pins to the SD card [DATALOG.TXT, tab seperated]. Temperature data is recorded all at once and then collected from each sensor during the delay time. If the collection process takes longer than the delay time you may find it starts misbehaving. 15 seconds is sufficient for over 80 sensors. Distribution across strings won't change the collection speed (all done in series).
