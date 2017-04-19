//Analog Read code for testing Ozone Sensor

//Define analog input and values for for Mics 03 sensor:
const int OzoneMPin=A1;
int OzoneMSensorValue=0;

//Define input for Aeroqual Sensor
const int OzoneAPin=A2;
int OzoneASensorValue=0;

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup() {
 Serial.begin(9600);
  Serial.println("Ozone Sensor Raw Readings");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print('             ');

}

void loop() {
  //Read Ozone from sensor
  OzoneMSensorValue=analogRead(OzoneMPin); //Read value from ozone pin
  Serial.print("Mics Sensor = ");
  Serial.println(OzoneMSensorValue);
    lcd.setCursor(0, 0);
  lcd.print("Mics: ");
   lcd.print(OzoneMSensorValue);
  OzoneASensorValue=analogRead(OzoneAPin); //Read value from ozone pin
  Serial.print("Aeroqual Sensor = ");
  Serial.println(OzoneASensorValue);
  lcd.setCursor(0, 1);
  lcd.print("Aero: ");
   lcd.print(OzoneASensorValue);
 
  Serial.println();
  delay(2000);

}
