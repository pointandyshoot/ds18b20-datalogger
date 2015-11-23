/*
Setup routine for Data logger for multiple DS18B20 temperature probes.
When running Arduino, add sensors to your string one at a time. SD card will record a text file with the string addresses in order.
*/
#define Rev 1
// Set interval (in seconds) for looking for new sensors
#define TIME_DELAY 3
// Uncomment active pins for one wire network (valid pins 2, 3, 4, 5) (about 120 sensors max)
#define ONE_WIRE_BUS_2 2 
#define ONE_WIRE_BUS_3 3
#define ONE_WIRE_BUS_4 4
#define ONE_WIRE_BUS_5 5

//const int ONE_WIRE_BUSs[]={2,3}; // 
// Select temperature precision
#define TEMPERATURE_PRECISION 9 // 9, 10, 11, 12
/*
Changelog:

28/10/2015 Initial build

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
DeviceAddress NewSensor;
//char strAddress[16];
String strAddress;

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
  Serial.print(F("DS18B20 setup routine Rev "));
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

String returnAddress(DeviceAddress deviceAddress) 
{
  String strAddress = "";
  String tmpAddress = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) strAddress = strAddress + ("0");
    tmpAddress = String(deviceAddress[i], HEX);
    tmpAddress.toUpperCase();
    strAddress = strAddress + tmpAddress;
  }
  return strAddress;
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
  int indexAddress = 0;
  boolean AddressFound = false;
//  DeviceAddress NewSensor;
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
//  char buf[16];
  String buf;
  DateTime now = rtc.now();

 
    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("sensors.txt"); //read mode
  
    // if the file is available, read it:
  if (dataFile) {
    
    oneWire2.reset_search(); //reset index
//    oneWire3.reset_search();
//    oneWire4.reset_search();
//    oneWire5.reset_search();
    
    DeviceCount = sensors2.getDeviceCount();
    TotalDevices = TotalDevices + DeviceCount;
    // cycle through sensors and log
//    for (int x = 0; x < DeviceCount; x++) {
    //  Serial.print("x=");
    //  Serial.println(x);
    while (oneWire2.search(TempSensor)) {

      strAddress = returnAddress(TempSensor);
      
//      printAddress(TempSensor);

      dataFile.seek(0);
      Serial.print("Matching ");
      while (dataFile.available() && AddressFound == false) {
//        dataFile.read(buf,16);
        Serial.print(".");
        buf = dataFile.readStringUntil('\n');
        buf.trim();
   //     Serial.println();
   //     Serial.print("Matching: ");
   //       Serial.print(buf);
   //     Serial.print(" with ");
        strAddress.trim();
   //     Serial.print(strAddress);        
        

        if(buf == strAddress) {
     //     Serial.println(" -- Matched!");
      //    printAddress(TempSensor);
          AddressFound = true;
      //    indexAddress = x;
        } else {
      //    Serial.println(" -- not matched");
        }
  //      delay(100);

      }
      
      Serial.println();
      
      if (AddressFound == false) {
//          dispBacklight(157);
          Serial.print("Writing new sensor: ");
          printAddress(TempSensor);
          Serial.println();
          
          dataFile.close();
          
          dataFile = SD.open("sensors.txt", FILE_WRITE);
          writeAddress(TempSensor, dataFile);
          dataFile.println("");
          dataFile.close();
//          delay(50);
//          dispBacklight(128);
          dataFile = SD.open("sensors.txt");
    
          
       }
       
       AddressFound = false;
      
    }
    
    /*
 
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

    } */
    
    mySerial.write(254);  //clear SD card bad indicator on LCD display
    mySerial.write(207);
    mySerial.write(" ");

  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println(F("error opening file"));
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
  
//  Serial.print("\t");
  
  timenow = millis();
  Serial.println("/");
//  Serial.print((TIME_DELAY*1000)-(timenow-timeold));
//  if (timenow-timeold < TIME_DELAY*1000) delay((TIME_DELAY*1000)-(timenow-timeold));

}









