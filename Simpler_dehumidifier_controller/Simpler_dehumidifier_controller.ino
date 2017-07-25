/* Oto's Simpler Dehumidifier controller
   Copyright A.G.Doswell May 2017
   License: The MIT License (See full license at the bottom of this file)
   Visit Andydoz.blogspot.com for description and circuit.

*/


#include <dht.h>
dht DHT;
#define DHT22_PIN 7 // DHT data connected to pin 7
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// define constants & global variables
const int rlyPin = 8; //defines pin the relay is connected to. relay is active low
boolean confidence = 0; //confidence default
boolean mode = true; // True if dew point selected, false if humidity selected
const int TimerLength = 1800;// number of seconds in an hour / 2 - this sets the minimum time the dehumidifier will remain on for.
int Timer = 0; //timer is off to start
const int modeSwPin = 6;
const int upPin = 10;
const int downPin = 9;
int setPointHumidity; // humidity % set point
int setPointTimer = 0;
int setPointEEPROM;

void setup()
{

  lcd.begin(16, 2); // defines the LCD as a 16 x 2
  pinMode (rlyPin, OUTPUT); // sets our relay pin
  digitalWrite (rlyPin, LOW); // sets the relay off for default condition.
  pinMode (modeSwPin, INPUT_PULLUP); // sets mode switch pin
  pinMode (upPin, INPUT_PULLUP); // sets up control pin
  pinMode (downPin, INPUT_PULLUP); // sets down control pin
  setPointHumidity = EEPROM.read(0);

}

void loop()
{
  int chk = DHT.read22(DHT22_PIN); // these 4 lines get data from the sensor
  int dew = dewPointFast(DHT.temperature, DHT.humidity); // calculate dew point in degrees C
  int temp = (DHT.temperature);
  int humidity = (DHT.humidity);
  int dewTrip = dew + 5; // dew point trip 5 degrees before dew point
  readModeSwitch ();
  setPointAdjust ();

  // writes information about the system to the LCD
  lcd.clear ();
  lcd.print("Vlhkost:"); // Czech for humidity!
  lcd.print (humidity);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.print((char)0xDF);
  lcd.print("C");
  lcd.setCursor(5, 1);
  lcd.print("Rosa:"); // Czech for dew!
  lcd.print(dew);
  lcd.print((char)0xDF);
  lcd.print("C");


  // dew detect loop. If the dewTrip point is reached, start the timer running and set the dewFlag flag
  if (mode == true) {
    if ( temp <= dewTrip)  {
      Timer = 1;
    }

    if (Timer >= TimerLength) { // If the timer has expired switch the dehumidifier off.
      Timer = 0;
      digitalWrite (rlyPin, LOW);

    }
  }

  if (mode == false) {
    if (humidity >= setPointHumidity) {
      Timer = 1;
    }
  }


  if (Timer != 0) {               // If the timer is running, switch the dehumidifier on , and write On to the lcd.
    digitalWrite (rlyPin, HIGH);
    lcd.setCursor (12, 0);
    lcd.print ("Na");
    Timer++;
  }

  else {
    lcd.setCursor (12, 0);
    lcd.print ("off");
  }
  
    if (mode == true) {
      lcd.setCursor(15, 1);
      lcd.print ("R");
  
    }
   else {
      lcd.setCursor(15, 1);
      lcd.print ("%");
    }
  
  delay(2000); // we can only get data from the sensor once every 2 seconds.
}

double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}

void readModeSwitch () {

  if (digitalRead (modeSwPin) == true) {
    mode = true;
  }
  else {
    mode = false;
  }

}

void setPointAdjust () {

  if (digitalRead (upPin) == false) {
    setPointHumidity++;
    if (setPointHumidity >= 101) {
      setPointHumidity = 100;
    }
    lcd.setCursor (0, 0);
    lcd.print ("Nastavte vlhkost  ");
    lcd.setCursor (0, 1);
    lcd.print ("      ");
    lcd.print (setPointHumidity);
    lcd.print ("%               ");
    delay (200);
    setPointTimer = 100;
  }
  if (digitalRead (downPin) == false) {
    setPointHumidity--;
    if (setPointHumidity <= 0 ) {
      setPointHumidity = 1;
    }
    lcd.setCursor (0, 0);
    lcd.print ("Nastavte vlhkost"); // Czech for "Set Humidity"
    lcd.setCursor (0, 1);
    lcd.print ("      ");
    lcd.print (setPointHumidity);
    lcd.print ("%               ");
    delay (200);
    setPointTimer = 100;
  }

  if (setPointTimer == 0) { // delay return to loop for 5000 operations
    setPointEEPROM = EEPROM.read(0);
    if (setPointEEPROM != setPointHumidity) { // if the value changed, write new value to the EEPROM
      EEPROM.write(0, setPointHumidity);

    }
    return;
  }

  setPointTimer --;
  delay (50);
  setPointAdjust ();
}

/*
   Copyright (c) 2017 Andrew Doswell

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute and sublicense
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   Any commercial use is prohibited without prior arrangement.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHOR(S) OR COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/
