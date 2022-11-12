#include <ArduinoJson.h>

#include <SoftwareSerial.h>

#include <FastLED.h>
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];
uint8_t r = 0;
uint8_t g = 0;
uint8_t b = 0;


SoftwareSerial Serial1(4, 5); // RX, TX

void setup() {
  // Initialize "debug" serial port
  // The data rate must be much higher than the "link" serial port
  Serial.begin(115200);
  while (!Serial) continue;

  // Initialize the "link" serial port
  // Use a low data rate to reduce the error ratio
  Serial1.begin(9600);

  FastLED.addLeds<WS2812B, 8, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
}

void loop() {
  // Check if the other Arduino is transmitting
  if (Serial1.available())
  {
    // Allocate the JSON document
    // This one must be bigger than the sender's because it must store the strings
    StaticJsonDocument<300> doc;

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, Serial1);

    if (err == DeserializationError::Ok)
    {
      // Print the values
      // (we must use as<T>() to resolve the ambiguity)
      Serial.print("r = ");
      Serial.println(doc["r"].as<uint8_t>());
      Serial.print("g = ");
      Serial.println(doc["g"].as<uint8_t>());
      Serial.print("b = ");
      Serial.println(doc["b"].as<uint8_t>());
      r = doc["r"].as<uint8_t>();
      g = doc["g"].as<uint8_t>();
      b = doc["b"].as<uint8_t>();
      fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
      FastLED.show();
      Serial.println("-------------");

    }
    else
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());

      // Flush all bytes in the "link" serial port buffer
      while (Serial1.available() > 0)
        Serial1.read();
    }
  }
}
