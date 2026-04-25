#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include "image.h"

#define TFT_CS   10
#define TFT_DC    7
#define TFT_RST   8

#define IMG_W 50
#define IMG_H 71
#define IMG_HEADER_BYTES 8
#define IMG_SCALE 5

#define IMG_X_OFFSET -10
#define IMG_Y_OFFSET 0
#define IMG_ROTATION 1   // 3 = -90 degrees / 270 degrees

#define DHT11_PIN 2
#define MIC_PIN A0;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); //initializing the screen for easy reference
DHT dht11(DHT11_PIN, DHT11); //initializing the temp sensor for easy reference

float temperature;
float humidity;
int sound;


/* WIFI SETUP
  char ssid[] = "YOUR_WIFI_NAME";
  char pass[] = "YOUR_WIFI_PASSWORD";
  
  char host[] = "script.google.com";
  String scriptPath = "/macros/s/AKfycbw8CTQ6rLaCNyg0ZCN2DIP3lWgGBUgFYQx8Q0NR8hZC-nH6f4odVU0cqXctE6CGDkwF/exec";
  
  WiFiSSLClient client;

*/


void drawImageRGB888(const unsigned char* img) {
  for (int sy = 0; sy < IMG_H; sy++) {
    for (int sx = 0; sx < IMG_W; sx++) {

      int srcIndex = IMG_HEADER_BYTES + ((sy * IMG_W + sx) * 3);

      uint8_t r = pgm_read_byte(&img[srcIndex + 0]) * 4;
      uint8_t g = pgm_read_byte(&img[srcIndex + 1]) * 4;
      uint8_t b = pgm_read_byte(&img[srcIndex + 2]) * 4;

      uint16_t color = tft.color565(r, g, b);

      int dx, dy;

      // fixed -90 degree rotation
      dx = sy;
      dy = IMG_W - 1 - sx;

      tft.fillRect(
        IMG_X_OFFSET + dx * IMG_SCALE,
        IMG_Y_OFFSET + dy * IMG_SCALE,
        IMG_SCALE,
        IMG_SCALE,
        color
      );
    }
  }
}

/* SOUND FUNCTION (from Victor)
  int readSoundLevel() {
    const int samples = 200;
    int minVal = 1023;
    int maxVal = 0;
  
    for (int i = 0; i < samples; i++) {
      int v = analogRead(MIC_PIN);
      if (v < minVal) minVal = v;
      if (v > maxVal) maxVal = v;
      delay(1);
    }
    return maxVal - minVal;
  }
*/
void setup() {
  //LCD SETUP
  tft.init(240, 320);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  //DHT11 SETUP
  /* Serial monitors are how we send data to and from the Arduino. We need it to use any sensors.*/
  Serial.begin(115200);
  Serial.println("Waiting for data...");
  dht11.begin(); //sensing data

}

void loop() {
  //sensing data - temperature, humidity, and sound
  temperature = dht11.readTemperature();
  humidity = dht11.readHumidity();
  //sound = readSoundLevel();

  // SENSOR ERROR HANDLING
  if (isnan(humidity) || isnan(temperature)) {
      Serial.println("ERROR: Failed to read from DHT11");
      int red = tft.color565(5, 0, 0);
      tft.drawLine(80, 100 y0, 80, 110, red);
      tft.drawLine(120, 100 y0, 120, 110, red);
      
      return;
  }
  
  // PRINT TEMP AND HUMIDITY
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity);

  //send to server
  /* 
    String url = scriptPath
        + "?temperature=" + String(temperature, 1)
        + "&humidity=" + String(humidity, 1)
        + "&sound=" + String(sound);
 
    Serial.println("Sending data...");
  
    if (client.connect(host, 443)) {
      client.println("GET " + url + " HTTP/1.1");
      client.println("Host: script.google.com");
      client.println("Connection: close");
      client.println();
  
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
      }
  
      while (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
  
      client.stop();
      Serial.println("Done");
    } else {
      Serial.println("Connection failed");
    }
*/

  //amimation and neopixels
  if (temperature <= 21 || humidity ) { //21C = 70F, lower range for most public schools
    //BLUE NEOPIXELS
    //COLD ANIMATION
  } else if (temperature >= 24) { //24C = 75F, a little higher than usual upper limit but offers wider range for our purposes
    //RED NEOPIXELS
    //HOT ANIMATION
  } else { //Anything with in this range is fairly ambient
    //GREEN NEOPIXELS
    //HELLO ANIMATION
  }
  
