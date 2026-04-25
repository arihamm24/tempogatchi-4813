#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include "image.h"
#include <WiFiS3.h>

#define TFT_CS   10
#define TFT_DC    7
#define TFT_RST   8

#define IMG_HEADER_BYTES 8
<<<<<<< HEAD

//Zoom magnification. 1 pixel on the LCD = one 9x9 PX square
=======
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
#define IMG_SCALE 9
#define IMG_X_OFFSET -10
#define IMG_Y_OFFSET 0
#define IMG_ROTATION 1
#define IMG_X_OFFSET -10
#define IMG_Y_OFFSET 0
#define IMG_ROTATION 1

#define DHT11_PIN 3
#define MIC_PIN A0
#define NEO_PIN 4
#define NUM_PIXELS 16

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
DHT dht11(DHT11_PIN, DHT11);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
DHT dht11(DHT11_PIN, DHT11);
Adafruit_NeoPixel pixels(NUM_PIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

float temperature;
float humidity;
int sound;

char ssid[] = ""; //enter hotspot name here
char pass[] = ""; //enter password to hotspot here

char host[] = "script.google.com";
String scriptPath = "/macros/s/AKfycbw8CTQ6rLaCNyg0ZCN2DIP3lWgGBUgFYQx8Q0NR8hZC-nH6f4odVU0cqXctE6CGDkwF/exec";

WiFiSSLClient client;

void drawImageRGB(const unsigned char* img, int width, int height) {
  for (int sourceY = 0; sourceY < height; sourceY++) {
    for (int sourceX = 0; sourceX < width; sourceX++) {
      int srcIndex = IMG_HEADER_BYTES + ((sourceY * width + sourceX) * 3);

<<<<<<< HEAD
      int srcIndex = IMG_HEADER_BYTES + ((sourceY * width + sourceX) * 3); 

      //read the RGB values from the image buffer(array) 
=======
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
      uint8_t r = pgm_read_byte(&img[srcIndex + 2]) * 4;
      uint8_t g = pgm_read_byte(&img[srcIndex + 1]) * 4;
      uint8_t b = pgm_read_byte(&img[srcIndex + 0]) * 4;

      uint16_t color = tft.color565(r, g, b);

      int dx = sourceY;
      int dy = width - 1 - sourceX;

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

void updateLEDs(uint32_t color) {
  pixels.fill(color);
  pixels.show();
}

<<<<<<< HEAD

=======
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
int readSoundLevel() {
  const int samples = 50;
  int minVal = 1023;
  int maxVal = 0;

  for (int i = 0; i < samples; i++) {
    int v = analogRead(MIC_PIN);
    if (v < minVal) minVal = v;
    if (v > maxVal) maxVal = v;
  }
<<<<<<< HEAD
  return maxVal - minVal;
}


void setup() {
  //LCD SETUP
  tft.init(240, 320); //dimension of the screen
  tft.setRotation(3); //-90 degree rotation (just how we had to arrange the screen in the igloo)
=======

  return maxVal - minVal;
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(1500);
  }

  Serial.println();
  Serial.println("Connected to WiFi");

  delay(1500);

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendToGoogleSheets(float temperature, float humidity, int sound) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
    return;
  }

  String url = scriptPath +
               "?temperature=" + String(temperature, 1) +
               "&humidity=" + String(humidity, 1) +
               "&sound=" + String(sound);

  Serial.println("Sending data...");

  if (client.connect("script.google.com", 443)) {

    client.print("GET " + url + " HTTP/1.1\r\n");
    client.print("Host: script.google.com\r\n");
    client.print("User-Agent: Arduino/1.0\r\n");   // <-- IMPORTANT
    client.print("Connection: close\r\n\r\n");

    unsigned long timeout = millis();

    while (client.connected() && millis() - timeout < 5000) {
      while (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        timeout = millis();
      }
    }

    client.stop();
    Serial.println("Done");

  } else {
    Serial.println("Connection failed");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  tft.init(240, 320);
  tft.setRotation(3);
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
  tft.fillScreen(ST77XX_BLACK);

  pixels.begin();
  pixels.setBrightness(50);
  pixels.show();

  dht11.begin();

<<<<<<< HEAD
  //DHT11 SETUP
  /* Serial monitors are how we send data to and from the Arduino. We need it to use any sensors. */
  Serial.begin(115200);
=======
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
  Serial.println("Waiting for data...");
  connectToWiFi();
}

void loop() {
  temperature = dht11.readTemperature();
  humidity = dht11.readHumidity();
  sound = readSoundLevel();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("ERROR: Failed to read from DHT11");
    delay(3000);
    return;
  }
<<<<<<< HEAD
  
  // PRINT TEMP SOUND AND HUMIDITY
=======

>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity);
<<<<<<< HEAD
  Serial.print(" | Sound Level: ");;
=======
  Serial.print(" | Sound Level: ");
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
  Serial.println(sound);

  sendToGoogleSheets(temperature, humidity, sound);

  if (temperature <= 21 || humidity >= 70) {
    updateLEDs(pixels.Color(0, 0, 255));

<<<<<<< HEAD
    //COLD ANIMATION
    drawImageRGB(gImage_cold1, 28,38);
    delay(500);
    drawImageRGB(gImage_cold2, 28,38);
    delay(500);
    drawImageRGB(gImage_cold3, 28,38);
=======
    drawImageRGB(gImage_cold1, 28, 38);
    delay(500);
    drawImageRGB(gImage_cold2, 28, 38);
    delay(500);
    drawImageRGB(gImage_cold3, 28, 38);
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
    delay(500);

  } else if (temperature >= 24) {
    updateLEDs(pixels.Color(255, 0, 0));

<<<<<<< HEAD
    //HOT ANIMATION
    drawImageRGB(gImage_hot1, 24,37);
    delay(500);
    drawImageRGB(gImage_hot2, 24,37);
    delay(500);
    
  } else { //Anything with in this range is fairly ambient
    //GREEN NEOPIXELS
    updateLEDs(pixels.Color(0, 255, 0));

    //HELLO ANIMATION
    drawImageRGB(gImage_hello1, 30,37);
    delay(500);
    drawImageRGB(gImage_hello2, 30,37);
    delay(500);

=======
    drawImageRGB(gImage_hot1, 24, 37);
    delay(500);
    drawImageRGB(gImage_hot2, 24, 37);
    delay(500);

  } else {
    updateLEDs(pixels.Color(0, 255, 0));
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)

    drawImageRGB(gImage_hello1, 30, 37);
    delay(500);
    drawImageRGB(gImage_hello2, 30, 37);
    delay(500);
  }
<<<<<<< HEAD
}
  
=======

  delay(5000);
}
>>>>>>> 752c01f (updated tempogatchi arduino code and dashboard)
