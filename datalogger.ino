/*
Data logger for multiple DS18B20 temperature probes.
*/
#define Rev 8
// Set interval (in seconds) for samples (can't guarantee it won't take longer to cycle the readings)
#define TIME_DELAY 15
// Uncomment active pins for one wire network (valid pins 2, 3, 4, 5) (about 120 sensors max)
#define ONE_WIRE_BUS_2 2 
#define ONE_WIRE_BUS_3 3
#define ONE_WIRE_BUS_4 4
#define ONE_WIRE_BUS_5 5

//const int ONE_WIRE_BUSs[]={2,3}; // 
// Select temperature precision
#define TEMPERATURE_PRECISION 12 // 9, 10, 11, 12
/*
Changelog:

16/10/15 Support for multiple pins with sensors (capable of about 30 sensors on each linear string)
15/10/15 Added number of sensors to LCD display. Fixed identifies if number of online sensors change without needing restart. Changed SD card log and serial readout to tab delimited instead of comma sperated (easier to import into Excel as thats the default)
13/10/15 LCD status display
12/10/15 Trying to fix issue with restarting - note, issue unresolved, works when compiled on raspberry pi IDE v 1.0.5+dfsg2-4, doesnt work when compiled under Windows either IDE 1.06 or 1.6 etc
09/07/15 Change method of addressing to avoid running out of memory
09/07/15 Serial output updated so it won't corrupt with 15+ sensors
11/6/15 Change file writing format to avoid string corruption. Writes as it gets the data now.

Pins Used:

Analog

Digital
10 - SD card
11 - SD card
12 - SD card
13 - SD card
4 - SD card
*/

/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
 	
 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */
#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <SoftwareSerial.h>


RTC_DS1307 rtc;

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (1c0 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.

# define chipSelect 10



// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire2(ONE_WIRE_BUS_2);
OneWire oneWire3(ONE_WIRE_BUS_3);
OneWire oneWire4(ONE_WIRE_BUS_4);
OneWire oneWire5(ONE_WIRE_BUS_5);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors2(&oneWire2);
DallasTemperature sensors3(&oneWire3);
DallasTemperature sensors4(&oneWire4);
DallasTemperature sensors5(&oneWire5);

// Variable to temporarily hold device address (cycled through due to memory constraints)
DeviceAddress TempSensor;

// Attach the serial display's RX line to digital pin 3
SoftwareSerial mySerial(7,6); // pin 6 = TX, pin 7 = RX (unused)

int cyclecount = 0;
int maxTemp = 0;

void setup()
{
  
 // Open serial communications and wait for port to open:
  Serial.begin(57600);
  mySerial.begin(9600); // set up serial port for 9600 baud display
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
   }
  Serial.println(F("--------------------------------------------------------"));
  Serial.print(F("DS18B20 datalogger Rev "));
  Serial.println(Rev);
  Serial.print(F("Timing interval: "));
  Serial.println(TIME_DELAY);
  Serial.print(F("\tPrecision: "));
  Serial.print(TEMPERATURE_PRECISION);
  Serial.println(F("bit"));
  Serial.println(F("--------------------------------------------------------"));
  Serial.println(F(""));


//----------------------------
  Serial.print(F("Initializing SD card..."));
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    return;
  }
  Serial.println(F("card initialized."));
  //---------------------------------
  
  Serial.print(F("Initializing RTC..."));  
  #ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif


  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println(F("RTC is NOT running!"));

    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  } else {
    Serial.println(F("clock initialized"));
  }
  //-----------------------------------------
    // following line sets the RTC to the date & time this sketch was compiled
 //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Start up the library
  

  // locate devices on the bus
  int DeviceCount = 0;
  int TotalDevices = 0;
  
  Serial.println(F("Initializing temperature probes:"));
  mySerial.write(0xFE);
  mySerial.write(0x01); //clear the display
  
  mySerial.write(254); // cursor to beginning of first line
  mySerial.write(128);
  mySerial.write("P2  3  4  5  T  ");
  
  mySerial.write(254); // cursor to beginning of first line
  mySerial.write(192);
  mySerial.write("#               ");
  
  
    sensors2.begin();
    Serial.print(F("Pin "));
    Serial.print(ONE_WIRE_BUS_2, DEC);
  //  Serial.println(F("..."));
    Serial.print(F("\t"));
    DeviceCount = sensors2.getDeviceCount();
    Serial.print(DeviceCount, DEC);
    Serial.println(F(" devices"));
    
    char dispnum[2];
    sprintf(dispnum, "%2d", DeviceCount);
    mySerial.write(254);
    mySerial.write(193);
    mySerial.write(dispnum);    
    
    TotalDevices = TotalDevices + DeviceCount;
    DeviceCount = 0;
    
    sensors3.begin();
    Serial.print(F("Pin "));
    Serial.print(ONE_WIRE_BUS_3, DEC);
 //   Serial.println(F("..."));
    Serial.print(F("\t"));
    DeviceCount = sensors3.getDeviceCount();
    Serial.print(DeviceCount, DEC);
    Serial.println(F(" devices"));
    
    sprintf(dispnum, "%2d", DeviceCount);
    mySerial.write(254);
    mySerial.write(196);
    mySerial.write(dispnum);  
    
    TotalDevices = TotalDevices + DeviceCount;
    DeviceCount = 0;
    
    sensors4.begin();
    Serial.print(F("Pin "));
    Serial.print(ONE_WIRE_BUS_4, DEC);
//    Serial.println(F("..."));
    Serial.print(F("\t"));
    DeviceCount = sensors4.getDeviceCount();
    Serial.print(DeviceCount, DEC);
    Serial.println(F(" devices"));
    
    sprintf(dispnum, "%2d", DeviceCount);
    mySerial.write(254);
    mySerial.write(199);
    mySerial.write(dispnum);      
 
    TotalDevices = TotalDevices + DeviceCount;
    DeviceCount = 0;
        
    sensors5.begin();
    Serial.print(F("Pin "));
    Serial.print(ONE_WIRE_BUS_5, DEC);
//    Serial.println(F("..."));
    Serial.print(F("\t"));
    DeviceCount = sensors5.getDeviceCount();
    Serial.print(DeviceCount, DEC);
    Serial.println(F(" devices"));
    
    sprintf(dispnum, "%2d", DeviceCount);
    mySerial.write(254);
    mySerial.write(202);
    mySerial.write(dispnum);  
    
    TotalDevices = TotalDevices + DeviceCount;
    DeviceCount = 0;
    Serial.print(F("Total Devices: "));
    Serial.println(TotalDevices);
    
    char dispnum3[3];
    sprintf(dispnum3, "%3d", TotalDevices);
    mySerial.write(254);
    mySerial.write(205);
    mySerial.write(dispnum3); 

  // report parasite power requirements
//  Serial.print(F("\tParasite power is: ")); 
//  if (sensors.isParasitePowerMode()) Serial.println(F("ON"));
//  else Serial.println(F("OFF"));  
  
  // iterate through the sensors, setting precision and displaying the address
  oneWire2.reset_search();
  while (oneWire2.search(TempSensor)) {
    sensors2.setResolution(TempSensor, TEMPERATURE_PRECISION);
    Serial.print(F("Pin 2\tDevice: "));
    printAddress(TempSensor);
    Serial.print(F("\t Resolution: "));
    Serial.println(sensors2.getResolution(TempSensor), DEC);
  }
  
  oneWire3.reset_search();
  while (oneWire3.search(TempSensor)) {
    sensors3.setResolution(TempSensor, TEMPERATURE_PRECISION);
    Serial.print(F("Pin 3\tDevice: "));
    printAddress(TempSensor);
    Serial.print(F("\t Resolution: "));
    Serial.println(sensors3.getResolution(TempSensor), DEC);
  }

  oneWire4.reset_search();
  while (oneWire4.search(TempSensor)) {
    sensors4.setResolution(TempSensor, TEMPERATURE_PRECISION);
    Serial.print(F("Pin 4\tDevice: "));
    printAddress(TempSensor);
    Serial.print(F("\t Resolution: "));
    Serial.println(sensors4.getResolution(TempSensor), DEC);
  }

  oneWire5.reset_search();
  while (oneWire5.search(TempSensor)) {
    sensors5.setResolution(TempSensor, TEMPERATURE_PRECISION);
    Serial.print(F("Pin 5\tDevice: "));
    printAddress(TempSensor);
    Serial.print(F("\t Resolution: "));
    Serial.println(sensors5.getResolution(TempSensor), DEC);
  }  
  Serial.println(F("...sensors initialized"));
  
  //clear display
  Serial.println(F("Initialising LCD display"));
  
  mySerial.write(0xFE);
  mySerial.write(0x01); //clear the display
  
  mySerial.write(254); // cursor to beginning of first line
  mySerial.write(128);
  mySerial.write("T Max:      #   ");

  mySerial.write(254); // cursor to beginning of second line
  mySerial.write(192);
  mySerial.write("Cycle:          ");  
  
  mySerial.write(0x7C);
  mySerial.write(0x80); // turn off backlight to increase battery life
  
  Serial.println(F("..display initialized"));
  
  Serial.println(F("Logging..."));
}


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print(F("0"));
    Serial.print(deviceAddress[i], HEX);
  }
}

//function to write device address to SD card
void writeAddress(DeviceAddress deviceAddress, File dataFile)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) dataFile.print(F("0"));
    dataFile.print(deviceAddress[i], HEX);
  }
}

/* function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(F("Temp C: "));
  Serial.print(tempC);
  Serial.print(F(" Temp F: "));
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}  */

/* function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print(F("Resolution: "));
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
} */

/* main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print(F("Device Address: "));
  printAddress(deviceAddress);
  Serial.print(F(" "));
  printTemperature(deviceAddress);
  Serial.println();
} */

void dispBacklight(int brightness) //128 = OFF, 157 = 100% ON
{
  mySerial.write(0x7C);
  mySerial.write(brightness);
}

void loop()
{
  unsigned long timeold = millis();
  unsigned long timenow = 0;
  int DeviceCount = 0;
  int TotalDevices = 0;
  sensors2.begin();
  sensors3.begin();
  sensors4.begin();
  sensors5.begin();

  // delay(100);

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors2.requestTemperatures();
  sensors3.requestTemperatures();
  sensors4.requestTemperatures();
  sensors5.requestTemperatures();
  
  DateTime now = rtc.now();

 
    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
    // if the file is available, write to it:
  if (dataFile) {
  
    // make a string for assembling the data to log:
    String dataString = String("");
    String smldataString = String("");
    char outBuffr[6];
    smldataString = String(now.year());
    dataString = String(dataString + smldataString);
    dataString = String(dataString + "/");
    if (now.month() < 10) {
      dataString = String(dataString + "0");
    }
    smldataString = String(now.month());
    dataString = String(dataString + smldataString);
    dataString = String(dataString + "/");
    if (now.day() < 10) {
      dataString = String(dataString + "0");
    }
    smldataString = String(now.day());
    dataString = String(dataString + smldataString);
    dataString = String(dataString + " ");
    if (now.hour() < 10) {
      dataString = String(dataString + "0");
    }
    smldataString = String(now.hour());
    dataString = String(dataString + smldataString);
    dataString = String(dataString + ":");
    if (now.minute() < 10) {
      dataString = String(dataString + "0");
    }
    smldataString = String(now.minute());
    dataString = String(dataString + smldataString);
    dataString = String(dataString + ":");
    if (now.second() < 10) {
      dataString = String(dataString + "0");
    }
    smldataString = String(now.second());
    dataString = String(dataString + smldataString);

    Serial.println(F(""));
    dataFile.println(F(""));
    
    dataFile.print(dataString);
    Serial.print(dataString);
    dataString = "";
    smldataString = "";

    oneWire2.reset_search(); //reset index
    oneWire3.reset_search();
    oneWire4.reset_search();
    oneWire5.reset_search();
    
    DeviceCount = sensors2.getDeviceCount();
    TotalDevices = TotalDevices + DeviceCount;
    // cycle through sensors and log
    for (int x = 0; x < DeviceCount; x++) {
  
      oneWire2.search(TempSensor);
      float tempTemp = sensors2.getTempC(TempSensor);
      if (tempTemp > maxTemp) maxTemp = tempTemp;
      dtostrf(tempTemp, 6, 2, outBuffr);
      
      dataFile.print(F("\t"));
      writeAddress(TempSensor, dataFile);
      dataFile.print(F("\t"));
      dataFile.print(outBuffr);
      
      Serial.print(F("\t"));
      printAddress(TempSensor);
      Serial.print(F("\t"));
      Serial.print(outBuffr);

    } 
 
     DeviceCount = sensors3.getDeviceCount();
     TotalDevices = TotalDevices + DeviceCount;
    // cycle through sensors and log
    for (int x = 0; x < DeviceCount; x++) {
  
      oneWire3.search(TempSensor);
      float tempTemp = sensors3.getTempC(TempSensor);
      if (tempTemp > maxTemp) maxTemp = tempTemp;
      dtostrf(tempTemp, 6, 2, outBuffr);
      
      dataFile.print(F("\t"));
      writeAddress(TempSensor, dataFile);
      dataFile.print(F("\t"));
      dataFile.print(outBuffr);
      
      Serial.print(F("\t"));
      printAddress(TempSensor);
      Serial.print(F("\t"));
      Serial.print(outBuffr);

    }    

    DeviceCount = sensors4.getDeviceCount();
    TotalDevices = TotalDevices + DeviceCount;
    // cycle through sensors and log
    for (int x = 0; x < DeviceCount; x++) {
  
      oneWire4.search(TempSensor);
      float tempTemp = sensors4.getTempC(TempSensor);
      if (tempTemp > maxTemp) maxTemp = tempTemp;
      dtostrf(tempTemp, 6, 2, outBuffr);
      
      dataFile.print(F("\t"));
      writeAddress(TempSensor, dataFile);
      dataFile.print(F("\t"));
      dataFile.print(outBuffr);
      
      Serial.print(F("\t"));
      printAddress(TempSensor);
      Serial.print(F("\t"));
      Serial.print(outBuffr);

    } 

    DeviceCount = sensors5.getDeviceCount();
    TotalDevices = TotalDevices + DeviceCount;
    // cycle through sensors and log
    for (int x = 0; x < DeviceCount; x++) {
  
      oneWire5.search(TempSensor);
      float tempTemp = sensors5.getTempC(TempSensor);
      if (tempTemp > maxTemp) maxTemp = tempTemp;
      dtostrf(tempTemp, 6, 2, outBuffr);
      
      dataFile.print(F("\t"));
      writeAddress(TempSensor, dataFile);
      dataFile.print(F("\t"));
      dataFile.print(outBuffr);
      
      Serial.print(F("\t"));
      printAddress(TempSensor);
      Serial.print(F("\t"));
      Serial.print(outBuffr);

    }
    
    mySerial.write(254);  //clear SD card bad indicator on LCD display
    mySerial.write(207);
    mySerial.write(" ");

  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println(F("error opening datalog.txt"));
    mySerial.write(254);
    mySerial.write(207);
    mySerial.write("*");  //signal LCD card bad on bottom right corner of LCD Display
    
    pinMode(chipSelect, OUTPUT);
    SD.begin(chipSelect);
  }
  
  dataFile.close();

  
  char displaystring[10];
  sprintf(displaystring,"%4d",maxTemp);
  mySerial.write(254); // cursor to 7th position on first line
  mySerial.write(135);
  mySerial.write(displaystring);
  
  char dispnum[3];
  sprintf(dispnum, "%3d", TotalDevices);
  mySerial.write(254);
  mySerial.write(141);
  mySerial.write(dispnum);
  
  sprintf(displaystring,"%4d",cyclecount);
  mySerial.write(254); // cursor to 7th position on first line
  mySerial.write(199);
  mySerial.write(displaystring); 
  
  cyclecount++;
  maxTemp = 0;
  
  Serial.print("\t");
  
  timenow = millis();
  
  Serial.print((TIME_DELAY*1000)-(timenow-timeold));
  if (timenow-timeold < TIME_DELAY*1000) delay((TIME_DELAY*1000)-(timenow-timeold));

}









