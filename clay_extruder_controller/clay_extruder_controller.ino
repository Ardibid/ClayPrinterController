/*********************************************************************
 This is a code to control a 3d printer extruder based on
 Mechanical Paste Extruder: https://www.thingiverse.com/thing:3142561

 This code is developed by Ardavan Bidgoli(https://github.com/Ardibid)

 The Arduino board interfaces a DM542 Microstep driver to controls a
 step motor (tested on NEMA 23).

 The circuit is designed to work with Digital Output signals from an
 ABB robot. The signal from the robot is 24v, it must be regualted
 down to 5-3.3v to prevent frying the Arduino baord.
 Use a realy or 24V to 5V 8 Signal Converter.
*********************************************************************/
// LCD Screen settings
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10  // Number of snowflakes in the animation example
#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

// Arduino Settings
// Pins Setup
int loadButtonPin = 2;  // When pushed, the plunge moves up and let you load clay
int printButtonPin = 5; // If not in load mode, pushing this will push plunge down
int dirPin = 3;         // Sends the **direction** signal to the microstepper
int stepPin = 4;        // Sends the **step** or **pull** signal to the microstepper
int potentiometerPin = A0;

// Note: The pins for I2C are defined by the Wire-library.
// On an arduino UNO & Nano:  A4(SDA), A5(SCL)

// setting default variables
float stepDelay;            // defines the delay time between two steps, shorter is faster
float rps;                  // revolution per second
int loadButtonState = 0;  // stores loadButtonPin state
int printButtonState = 0; // stores printButtonPin state
int sensorValue;          // reads the potentiometer pin values
int prevSensorVal = 10000;// stores the previous reading of potentiometer, used for damping fluctoations
int noiseTresh = 3;       // minimum acceptable diffrence between two readings
int currentSensorVal = 0; // stores the most recent valid reading of potentiometer
int systemState = 2;      // State of the system: 0 loading, 1 printing, 2 idle

long int timer = millis();  // Timer to control the UI updates
int messageInterval = 1000; // time delay between UI update in ms

void setup() {
  Serial.begin(9600);
  // step motor main signals
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // input signal setup
  pinMode(printButtonPin, INPUT);
  pinMode(loadButtonPin, INPUT);

  // setting the initial direction for the motor
  digitalWrite(dirPin, HIGH);

  // checking the LCD screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  init_display();
}

void loop() {
  check_potentiometer_value();
  command_motor();
  update_UI();
}

void update_UI() {
  // in intervals of .1 second, shows message in Serial
  // or in the LCD display. Any faster update will mess
  // with the stepper controller
   
  if (millis()-timer > messageInterval) {
    show_message();
    timer = millis();
  }
}

void check_potentiometer_value() {
  // reads the potentiometer value through pin A0
  sensorValue = analogRead(potentiometerPin);

  // this makes sure that we are damping flactuations smaller than noiseTresh
  if (abs(sensorValue - prevSensorVal) > noiseTresh) {
    currentSensorVal = sensorValue;
    prevSensorVal = currentSensorVal;
    // Convert the analog reading (which goes from 0 - 1023) 
    // to a stepdelay time between 3 and 2000 milli second
    stepDelay = map(currentSensorVal, 0, 1023, 100.0, 2);
    rps = 1000.0/(stepDelay*200.0);
  }
}

void command_motor() {
  // Commands the motor to move
  // 0 means button pressed, 1 means released
  loadButtonState = digitalRead(loadButtonPin);
  printButtonState = digitalRead(printButtonPin);

  // loadButtonState HIGH regardless of printButtonPin --> Load material in
  if (loadButtonState == 0) {
    digitalWrite(dirPin, HIGH);
    systemState = 0;  // loading
    motor_control();
  }
  
  // loadButtonState LOW and printButtonPin HIGH      --> Push material out
  else if (printButtonState == 1) {
    digitalWrite(dirPin, LOW);
    systemState = 1;  // print
    motor_control();
  } else {
    systemState = 2;  // idle
  }
}

void motor_control() {
  // Moves the motor by sendign the Pul/Step signal to MicroStepper
  // changing the stepDelay changes the speed
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepDelay);
}

void show_message() {
  // show message on the small LCD panel
  int secondRow = 10;
  int thirdRow = 20;
  int secondCol = 70;

  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text

  // Rev Per Second Message
  display.setCursor(0, 0);
  display.println(F("Rev/Sec:"));
  display.setTextSize(1);
  display.setCursor(secondCol, 0);
  display.println(rps);
  
  // Delay Time Message
  display.setCursor(0, secondRow);
  display.println(F("Step Delay:"));
  display.setTextSize(1);
  display.setCursor(secondCol, secondRow);
  display.println(stepDelay);

  // Direction Message
  display.setCursor(0, thirdRow);
  display.println(F("Direction:"));
  display.setTextSize(1);
  display.setCursor(secondCol, thirdRow);

  if (systemState == 0) {
    display.println(F("L"));
  }

  else if (systemState == 1) {
    display.println(F("P"));
  } else {
    display.println(F("Idle"));
  }
  display.display();
  Serial.print("Rev/Sec: ");
  Serial.print(rps);
  Serial.print(" | Step Delay: ");
  Serial.print(stepDelay);
  Serial.print(" | Load: ");
  Serial.print(loadButtonState);
  Serial.print(" | Print: ");
  Serial.print(printButtonState);
  Serial.print(" | State: ");
  Serial.print(systemState);
  Serial.println();
  delay(100);
}

void init_display() {
  /**************************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x32 pixel display using I2C to communicate
  3 pins are required to interface (two I2C and one reset).

  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source
  hardware by purchasing products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries,
  with contributions from the open source community.
  BSD license, check license.txt for more information
  All text above, and the splash screen below must be
  included in any redistribution.
 **************************************************************************/
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(100);
  // Clear the buffer
  display.clearDisplay();
  display.invertDisplay(true);
  delay(100);
  display.invertDisplay(false);
  delay(100);
}
