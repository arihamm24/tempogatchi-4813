#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include "image.h"
#include <WiFiS3.h>

//which Arduino pins are connected to the display
#define TFT_CS   10
#define TFT_DC    7
#define TFT_RST   8

//Bytes of non-image information before the pixels start!
#define IMG_HEADER_BYTES 8

//Zoom magnification. 1 pixel on the LCD = one square with IMG_SCALE as length
#define IMG_SCALE 9

//Offsets to position the zoomed image
#define IMG_X_OFFSET -10 //Neg = left, Pos = right
#define IMG_Y_OFFSET 0//Neg = up, Pos = down
#define IMG_ROTATION 1   // 3 = -90 degrees / 270 degrees

#define DHT11_PIN 3
#define MIC_PIN A0
#define NEO_PIN 4
#define NUM_PIXELS 16

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); //initializing the screen for easy reference
DHT dht11(DHT11_PIN, DHT11); //initializing the temp sensor for easy reference
Adafruit_NeoPixel pixels(NUM_PIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

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

//HELPER FUNCTIONS
void drawImageRGB(const unsigned char* img, int width, int height) {
  for (int sourceY = 0; sourceY < height; sourceY++) { //each row of image
    for (int sourceX = 0; sourceX < width; sourceX++) { //each column of the image
    //this loop colors 1 pixel at a time!

      int srcIndex = IMG_HEADER_BYTES + ((sourceY * width + sourceX) * 3); 

      //read the RGB values from the image buffer(array) 
      uint8_t r = pgm_read_byte(&img[srcIndex + 0]) * 4;
      uint8_t g = pgm_read_byte(&img[srcIndex + 1]) * 4;
      uint8_t b = pgm_read_byte(&img[srcIndex + 2]) * 4;

      uint16_t color = tft.color565(r, g, b); //turn RGB values into the 16-bit color

      //-90 degree rotation
      int dx = sourceY; //rotation makes X -> Y
      int dy = width - 1 - sourceX; //new Y

      tft.fillRect(
        IMG_X_OFFSET + dx * IMG_SCALE, //starting x
        IMG_Y_OFFSET + dy * IMG_SCALE, //starting y
        IMG_SCALE, //width of scaled up image
        IMG_SCALE, //height of scaled up image
        color //color (duh)
      );
    }
  }
}

void updateLEDs(uint32_t color) {
  pixels.fill(color);
  pixels.show();
}


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


void setup() {
  //LCD SETUP
  tft.init(240, 320); //dimension of the screen
  tft.setRotation(3); //-90 degree rotation (just how we had to arrange the screen in the igloo)
  tft.fillScreen(ST77XX_BLACK);
  drawImageRGB(gImage_cold3, 28, 38);

  //NEOPIXEL SETUP
  pixels.begin();
  pixels.setBrightness(50);

  //DHT11 SETUP
  /* Serial monitors are how we send data to and from the Arduino. We need it to use any sensors. */
  Serial.begin(115200);
  Serial.println("Waiting for data...");
  dht11.begin(); //sensing data

}

void loop() {
  //sensing data - temperature, humidity, and sound
  temperature = dht11.readTemperature();
  humidity = dht11.readHumidity();
  sound = readSoundLevel();

  // SENSOR ERROR HANDLING
  if (isnan(humidity) || isnan(temperature)) {
      Serial.println("ERROR: Failed to read from DHT11");
      delay(3000);
      return;
  }
  
  // PRINT TEMP SOUND AND HUMIDITY
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity);
  Serial.print(" | Sound Level: ");;
  Serial.println(sound);

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
  if (temperature <= 21 || humidity >= 70) {  //21C = 70F, lower range for most public schools
    //BLUE NEOPIXELS
    updateLEDs(pixels.Color(0,0,255));

    //COLD ANIMATION
    drawImageRGB(gImage_cold1, 28,38);

  } else if (temperature >= 24) { //24C = 75F, a little higher than usual upper limit but offers wider range for our purposes
    //RED NEOPIXELS
    updateLEDs(pixels.Color(255,0,0));

    //HOT ANIMATION
    
  } else { //Anything with in this range is fairly ambient
    //GREEN NEOPIXELS
    updateLEDs(pixels.Color(0,255,0));

    //HELLO ANIMATION

  }

  delay(2500);
}
  
