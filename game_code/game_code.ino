/*
 * Game_code  
 *
 * Fall 2019
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

String RECEIVE; 
String SENDING; 
char recArray[4]; 
String BUTTONPRESSES; 


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


// integer variable to hold current counter value
int count = 0;
int state = 0; 
int counter = 0; 
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define BUTTON_D  0 //change this and above to correct ones 
bool lastButtonStateA; 
bool lastButtonStateB;
bool lastButtonStateC;
bool lastButtonStateD;   
bool buttonStateA; 
bool buttonStateB; 
bool buttonStateC; 
bool buttonStateD; 

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

pinMode(BUTTON_A, INPUT_PULLUP); 
pinMode(BUTTON_B, INPUT_PULLUP);
pinMode(BUTTON_C, INPUT_PULLUP);
pinMode(BUTTON_D, INPUT_PULLUP);

}


void loop()
{

switch (state) {
  case 0: 
  {
   //check for button presses 
   //advance state to 1 if any button pressed 
   //maybe play a pattern here on the leds, or chase them until button press? 
   Serial.println("waiting for button press to start"); 
   buttonStateB = digitalRead(BUTTON_A);
    if (buttonStateA != lastButtonStateA) {
      if (buttonStateB == LOW) { 
      state = 1;
      Serial.print("waiting on char sequence from bluetooth");
      ble.print("send 4 chars combo of RYGB");   
      }
      }
    lastButtonStateA = buttonStateA; 
   }
  break; 
  case 1:
    { 
       // wait for char sequence from bluetooth 
   while ( ble.available() )
  {
    int c = ble.read();
    Serial.print((char)c);
    RECEIVE+=char(c);
    counter++;  
  }
   if (counter>4) {
    RECEIVE.toCharArray(recArray,5); 
    for (int i=0; i<5; i++) {
        Serial.println(recArray[i]);
        delay(500); 
    }
    state = 2; 
   }
    }
    break;
  case 2:
    {
      //play sequence 
      for (int i=0; i<4; i++) {
        if (recArray[i]=='R') {
        //light up correct LED and pause
        Serial.println("RED");
        delay(500);    
        }else if (recArray[i]=='Y'){
        //light up correct LED and pause
        Serial.println("YELLOW");
        delay(500);  
        }else if (recArray[i]=='G'){
        //light up correct LED and pause 
        Serial.println("GREEN");
        delay(500); 
        }else if (recArray[i]=='B'){
         //light up correct LED and pause
         Serial.println("BLUE");
         delay(500);  
        }
      }
    }
    state =3; 
    break;
  case 3:
   {
   //wait for four button presses, check for match and send Y or N over bluetooth 
   int NumButtonPress = 0; 
   while (NumButtonPress <5) {
    //RED BUTTON 
    buttonStateA = digitalRead(BUTTON_A);
    if (buttonStateA != lastButtonStateA) {
      if (buttonStateA == LOW) { 
      //light up RED LED 
      NumButtonPress++;
      BUTTONPRESSES += "R";   
      }
      }
    // YELLOW BUTTON 
    buttonStateB = digitalRead(BUTTON_B);
    if (buttonStateB != lastButtonStateB) {
      if (buttonStateB == LOW) { 
        //light up YELLOW LED 
      NumButtonPress++;
      BUTTONPRESSES += "Y";   
      }
      }
    // GREEN BUTTON 
    buttonStateC = digitalRead(BUTTON_C);
    if (buttonStateC != lastButtonStateC) {
      if (buttonStateC == LOW) {
      //light up GREEN LED  
      NumButtonPress++;
      BUTTONPRESSES += "G";   
      }
      }
    // BLUE BUTTON 
    buttonStateD = digitalRead(BUTTON_D);
    if (buttonStateD != lastButtonStateD) {
      if (buttonStateD == LOW) { 
      //light up BLUE LED 
      NumButtonPress++;
      BUTTONPRESSES += "B";   
      }
      }
    lastButtonStateA = buttonStateA; 
    lastButtonStateB = buttonStateB; 
    lastButtonStateC = buttonStateC; 
    lastButtonStateD = buttonStateD; 
   }
    // now do comparison between received string and button press string
    if (BUTTONPRESSES ==RECEIVE) { //they match!!!
        Serial.print("Sending: ");
        Serial.println("Success");
        //maybe play crazy LED sequence here?
        // Send input data to host via Bluefruit
        ble.print("Success!");
    }else {
       Serial.print("Sending: ");
        Serial.println("fail");
        //maybe play difference crazy LED sequence here?
        // Send input data to host via Bluefruit
        ble.print("fail");
    }
    state =0; //return to state 0 to try again  
    RECEIVE = ""; 
    BUTTONPRESSES = ""; 
    break;
}
}
}
