/*
 * INF351 Example Code 
 * Timesharing computer 
 * Fall 2019
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

//config for bluetooth
#define BLUEFRUIT_SPI_CS               8
#define BLUEFRUIT_SPI_IRQ              7
#define BLUEFRUIT_SPI_RST              4    // Optional but recommended, set to -1 if unused
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
#define BUFSIZE                        128   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   true  // If set to 'true' enables debug output

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


#include <Adafruit_FeatherOLED.h>
Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();

#include "inf351.h" 

// integer variable to hold current counter value
int count = 0;
int state = 0; 

#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
bool lastButtonStateA; 
bool lastButtonStateB; 
bool buttonStateA; 
bool buttonStateB; 
bool buttonStateC; 

int userNum = 3; 
String users[] = {"Matt", "Tim", "Nick"}; 
String tasks1[] = {"Wash dishes", "Vacuum floor", "Tidy living room"}; 
String tasks2[] = {"take out the garbage", "mop", "mow the lawn"}; 
int userTask1[3];
int userTask2[3];

void setup()
{
  Serial.begin(115200);

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    Serial.println(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      Serial.println("Couldn't factory reset");
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!
  ble.sendCommandCheckOK("AT+GAPDEVNAME=mydevice");


  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);


oled.init();

pinMode(BUTTON_A, INPUT_PULLUP); 
pinMode(BUTTON_B, INPUT_PULLUP);
pinMode(BUTTON_C, INPUT_PULLUP);

}


void loop()
{

 // Echo received data
  while ( ble.available() )
  {
    int c = ble.read();
    Serial.print((char)c);
  }


switch (state) {
  case 0:
    for (int i=0; i<userNum; i++) {
    oled.clearDisplay();
    userTask1[i] = assignUsers(i, users, tasks1); 
    userTask2[i] = assignUsers(i, users, tasks2); 
    }
    state = 1;
    break;
  case 1:
    for (int i=0; i<userNum; i++) {
    oled.clearDisplay();
    oled.setCursor(0,0); 
    oled.print(users[i]);
    oled.print(" is assigned to task: ");
    oled.print(tasks1[userTask1[i]]); 
    oled.print(tasks2[userTask2[i]]); 
    oled.display(); 
    while (digitalRead(BUTTON_A) == HIGH) {delay(10);}
    while (digitalRead(BUTTON_A) == LOW) {delay(10);}
    }
    state =2; 
    break;
  case 2:
   char n;
   char inputs[BUFSIZE+1];  
   buttonStateA = digitalRead(BUTTON_A);
    if (buttonStateA != lastButtonStateA) {
      if (buttonStateA == LOW) { 
      ble.print(tasks1[userTask1[0]]);
      Serial.println("verifying that reaching case 2");   
      ble.print("msg sent");
      delay(1000); 
      }
      }
    lastButtonStateA = buttonStateA; 
    
  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.println(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }
    break;
}
}
