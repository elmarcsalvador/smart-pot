
// Smart Pot
// A simple way to manage indoor plants using arduino
// Made with love by @elmarcsalvador

#include <math.h> //math library to calculate complex equations described in this code (Steinhart-Hart Equation)

#include <SPI.h>                //     Used to
#include <Wire.h>               //     Control
#include <Adafruit_GFX.h>       //       the
#include <Adafruit_SSD1306.h>   //   OLED DISPLAY

#define SCREEN_WIDTH 128 //defining width of the oled screen (i.e. 128 pixels)
#define SCREEN_HEIGHT 64 //defining height of the oled screen (i.e. 64 pixels)

#define OLED_RESET     -1 //used to handle display with the reset function of the microcontroller board (Arduino UNO)
#define SCREEN_ADDRESS 0x3C //defining the I2C address of the OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //configuring the display settings by using Adafruit's library for OLED displays (SSD1306)

const int soilMoisturePin = A0; //set AVOT of moisture sensor to A0 pin of the microcontroller
const int sunlightPin = A2; //set AO of LDR to A2 pin of the microcontroller
const int thermistor_output = A1; //set AO of thermistor to A1 pin of the microcontroller
const int relayPin = 2; //set IN of relay to D2 pin of the microcontroller


void setup() {
  //for additional debugging (not required necessarily)
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  pinMode(relayPin, OUTPUT);//setting D2 as output
  //initializing the display
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  int moisture = analogRead(soilMoisturePin);//function to read moisture data
  int moist = map(moisture, 1012, 500, 0, 100);//function to map moisture data to percentage
  // conditions for calibration of moisture sensor ([!] may vary on different soil, be sure to alter before upload)
  if (moisture<80){
    moist = 100;
    }
  else if (moisture>1012){
    moist = 0;
    }
  int sunlight = map(analogRead(sunlightPin), 0, 1023, 0, 100);//function to calculate sunlight intensity
  
  //function to find temperature from thermistor by using Steinhart-Hart equation
  int thermistor_adc_val;
  double output_voltage, thermistor_resistance, therm_res_ln, temperature;
  thermistor_adc_val = analogRead(thermistor_output);
  output_voltage = ( (thermistor_adc_val * 5.0) / 1023.0 );
  thermistor_resistance = ( ( 5 * ( 10.0 / output_voltage ) ) - 10 ); // resistance in kilo ohms
  thermistor_resistance = thermistor_resistance * 1000 ; // resistance in ohms
  therm_res_ln = log(thermistor_resistance);
  //  Steinhart-Hart Thermistor Equation:
  //  temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
  //  where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8
  temperature = ( 1 / ( 0.001129148 + ( 0.000234125 * therm_res_ln ) + ( 0.0000000876741 * therm_res_ln * therm_res_ln * therm_res_ln ) ) ); //temperature in Kelvin
  temperature = temperature - 273.15; //temperature in degree Celsius

  //function to show the realtime data on the OLED display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print(F("Sunlight: "));
  display.print(sunlight);
  display.print(F("%"));

  display.setCursor(0, 20);
  display.print(F("Soil Moisture: "));
  display.print(moist);
  display.print(F("%"));

  display.setCursor(0, 40);
  display.print(F("Temperature: "));
  display.print(temperature);
  display.print(F("C"));

  display.display();
  //for additional debugging (not required necessarily)
  Serial.print(sunlight);
  Serial.print(",");
  Serial.print(moist);
  Serial.print(",");
  Serial.print(moisture);
  Serial.print(",");
  Serial.println(temperature);

  //conditions for watering ([!]may vary depending upon the plant, be sure to alter before upload)
   if (moist < 54) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }

  if (sunlight > 98 && moist < 64) {
    digitalWrite(relayPin, HIGH);
  }
  else {
    digitalWrite(relayPin, LOW);
  }
  if (temperature > 45 && moist < 60) {
    digitalWrite(relayPin, HIGH);
  }
  else {
    digitalWrite(relayPin, LOW);
  }
  
  delay(200);//each realtime data is updated with a delay of 200ms
}
