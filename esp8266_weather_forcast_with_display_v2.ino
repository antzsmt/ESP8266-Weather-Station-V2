#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ST7735 TFT module connections
#define TFT_RST   D4     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    D3     // TFT CS  pin is connected to NodeMCU pin D3 (GPIO0)
#define TFT_DC    D2     // TFT DC  pin is connected to NodeMCU pin D2 (GPIO4)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const char* ssid = "abcd";
const char* password = "qwertyui";
String openWeatherMapApiKey = "a754b7b33d8e1aca1b0e14ce8ff422f2";
String city = "Kanpur";
String countryCode = "IN";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

const long utcOffsetInSeconds = 19800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


const unsigned char cloud[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000011,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000011,B00000000,B00000000,B00000000,B00000000,
  B00000000,B11000011,B00000110,B00000000,B00000000,B00000000,
  B00000000,B11000011,B00001110,B00000000,B00000000,B00000000,
  B00000000,B01100011,B00001100,B00000000,B00000000,B00000000,
  B00000000,B01100011,B00011000,B00000000,B00000000,B00000000,
  B00010000,B00111111,B11111000,B00000000,B00000000,B00000000,
  B00111100,B00111111,B11110000,B01110000,B00000000,B00000000,
  B00001110,B01111111,B11111101,B11100000,B00000000,B00000000,
  B00000111,B11111000,B00111111,B10000000,B00000000,B00000000,
  B00000001,B11110000,B00011110,B00000000,B00000000,B00000000,
  B00000001,B11100000,B00001000,B01111111,B10000000,B00000000,
  B00000001,B11000000,B00000001,B11111111,B11100000,B00000000,
  B01111111,B11000000,B00000011,B11111111,B11110000,B00000000,
  B01111111,B11000000,B00000111,B11000000,B11111000,B00000000,
  B00000001,B11000000,B00001111,B00000000,B01111100,B00000000,
  B00000001,B11100001,B11111111,B00000000,B00111100,B00000000,
  B00000001,B11100111,B11111110,B00000000,B00011110,B00000000,
  B00000111,B10001111,B11111100,B00000000,B00011110,B00000000,
  B00001110,B00001111,B00000100,B00000000,B00001110,B00000000,
  B00111000,B00011110,B00000000,B00000000,B00001111,B10000000,
  B00000000,B00011100,B00000000,B00000000,B00001111,B11000000,
  B00000000,B01111100,B00000000,B00000000,B00000111,B11100000,
  B00000011,B11111100,B00000000,B00000000,B00000001,B11100000,
  B00000111,B11111100,B00000000,B00000000,B00000000,B11110000,
  B00001111,B11111100,B00000000,B00000000,B00000000,B01110000,
  B00001111,B00000000,B00000000,B00000000,B00000000,B01110000,
  B00001110,B00000000,B00000000,B00000000,B00000000,B01110000,
  B00011110,B00000000,B00000000,B00000000,B00000000,B01110000,
  B00011110,B00000000,B00000000,B00000000,B00000000,B11110000,
  B00001110,B00000000,B00000000,B00000000,B00000000,B11110000,
  B00001111,B00000000,B00000000,B00000000,B00000011,B11100000,
  B00001111,B11111111,B11111111,B11111111,B11111111,B11000000,
  B00000111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00000001,B11111111,B11111111,B11111111,B11111111,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};
const unsigned char CLOCK[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00111100,B01110000,B00000000,
  B00000000,B11000000,B00000100,B00000000,
  B00000001,B00000001,B00000011,B00000000,
  B00000010,B01000000,B00001000,B10000000,
  B00000100,B00000000,B00000000,B01000000,
  B00001000,B00000000,B00000000,B00100000,
  B00010000,B00000000,B00000000,B00100000,
  B00010100,B00000001,B00000000,B10010000,
  B00000000,B00000001,B00000000,B00010000,
  B00100000,B00000001,B00000000,B00001000,
  B00100000,B00000001,B00000000,B00001000,
  B00100000,B00000001,B00000000,B00001000,
  B00101000,B00000001,B11100000,B00101000,
  B00101000,B00000000,B00000000,B00101000,
  B00100000,B00000000,B00000000,B00001000,
  B00100000,B00000000,B00000000,B00001000,
  B00100000,B00000000,B00000000,B00000000,
  B00010000,B00000000,B00000000,B00010000,
  B00010000,B00000000,B00000000,B10010000,
  B00001000,B00000000,B00000000,B00100000,
  B00001000,B00000000,B00000000,B01100000,
  B00000100,B00000000,B00000000,B01000000,
  B00000010,B00000000,B00000000,B10000000,
  B00000001,B10000001,B00000011,B00000000,
  B00000000,B01100000,B00001100,B00000000,
  B00000000,B00011110,B11110000,B00000000,
  B00000000,B00000000,B00000000,B00000000
};
const unsigned char humid[] PROGMEM = {
  B00000000,B00100000,B00000000,
  B00000000,B00110000,B00000000,
  B00000000,B01111000,B00000000,
  B00000000,B11111100,B00000000,
  B00000001,B11111110,B00000000,
  B00000011,B11111110,B00000000,
  B00000011,B11111111,B00000000,
  B00000111,B11111111,B10000000,
  B00001111,B11111111,B10000000,
  B00001111,B11111111,B11000000,
  B00011111,B11111111,B11000000,
  B00011111,B11111111,B11100000,
  B00111111,B11111111,B11100000,
  B00111111,B11111111,B11110000,
  B01111111,B11111111,B11110000,
  B01111110,B01110111,B11111000,
  B01111110,B00110111,B11111000,
  B01111100,B10101111,B11111000,
  B01111110,B01101111,B11111000,
  B01111111,B11011011,B11111000,
  B01111111,B11010001,B11111000,
  B01111111,B10110101,B11111000,
  B01111111,B10110001,B11111000,
  B01111111,B01111011,B11111000,
  B00111111,B11111111,B11110000,
  B00111111,B11111111,B11110000,
  B00011111,B11111111,B11100000,
  B00001111,B11111111,B11000000,
  B00000011,B11111111,B00000000,
  B00000000,B01110000,B00000000
};
const unsigned char pressure[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00001111,B11000000,B00000000,
  B00000000,B00111100,B11110000,B00000000,
  B00000000,B11000000,B00011100,B00000000,
  B00000001,B10000000,B00000110,B00000000,
  B00000011,B00000000,B00000010,B00000000,
  B00000010,B00000000,B00000001,B00000000,
  B00000100,B00000000,B00010001,B10000000,
  B00000100,B00000000,B01100000,B10000000,
  B00001100,B00000000,B11000000,B10000000,
  B00001000,B00000011,B10000000,B11000000,
  B00001000,B00000111,B10000000,B11000000,
  B00001000,B00000111,B10000000,B11000000,
  B00001100,B00000011,B00000000,B11000000,
  B00001100,B00000000,B00000000,B10000000,
  B00000100,B00000000,B00000000,B10000000,
  B00000110,B00000000,B00000001,B10000000,
  B00000010,B00000000,B00000001,B00000000,
  B00000011,B00000000,B00000010,B00000000,
  B00000001,B10000000,B00000100,B00000000,
  B00000000,B01100000,B00011000,B00000000,
  B00000000,B00111111,B11110000,B00000000,
  B00000000,B00000111,B10000000,B00000000,
  B00000000,B00000101,B10000000,B00000000,
  B00000000,B00000101,B10000000,B00000000,
  B00111111,B11111101,B11111111,B11110000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00111111,B11111111,B11111111,B11110000
};
const unsigned char windS[] PROGMEM = {
  B00000000,B00000000,B00001111,B00000000,B11110000,
  B00000000,B00000000,B00011101,B11000011,B10011100,
  B00000000,B00000000,B00110000,B11000110,B00000110,
  B00000000,B00000000,B00100000,B01100100,B00000010,
  B00000000,B00000000,B00000000,B01101100,B00000011,
  B00000000,B00000000,B00000000,B01101100,B00000001,
  B00000000,B00000000,B00000000,B11000000,B00000011,
  B00000111,B11111111,B11111111,B10000000,B00000011,
  B00001111,B11111111,B11111111,B00000000,B00000010,
  B00000000,B00000000,B00000000,B00000000,B00000110,
  B11111111,B11111111,B11111111,B11111111,B11111100,
  B11111111,B11111111,B11111111,B11111111,B11110000,
  B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B01111111,B11111111,B11111110,B00000000,
  B00000000,B01111111,B11111111,B11111111,B00000000,
  B00000000,B00000000,B00000000,B00000001,B10000000,
  B00000000,B00000000,B00000000,B00010001,B10000000,
  B00000000,B00000000,B00000000,B00011011,B00000000,
  B00000000,B00000000,B00000000,B00001110,B00000000
};

void setup(void) {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
  timeClient.begin();
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  //tft.setRotation(1);
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  
  delay(500);
 
  tft.fillScreen(ST7735_BLACK);
    //layout
    tft.drawFastHLine(0, 0, tft.width(), ST7735_WHITE);
    tft.drawFastHLine(1, 159, tft.width(), ST7735_WHITE);
    tft.drawFastVLine(0, 0, tft.height(), ST7735_WHITE);
    tft.drawFastVLine(127, 0, tft.height(), ST7735_WHITE);

    tft.drawBitmap(6, 5, cloud, 45, 38,ST7735_YELLOW);
  //delay(2000);
  tft.drawBitmap(90, 47, CLOCK, 30, 28,ST7735_WHITE);
  //delay(2000);
  tft.drawBitmap(6, 75, humid, 22, 30,0x07FF);
  tft.drawBitmap(95, 100, pressure, 30, 30,ST7735_RED);
  tft.drawBitmap(6, 131, windS, 40, 19,0xAFE5);
    tft.setFont(&FreeSans9pt7b);
    
    tft.setTextColor(ST7735_YELLOW);
    tft.setCursor(50,20);
    tft.setTextSize(1);
    tft.print(city);
    //tft.setFont(&FreeMono9pt7b);
    tft.setFont();
    tft.setTextSize(1);
    tft.setCursor(55,31);
    tft.print(countryCode);
    
    tft.setCursor(100,26);
    tft.print("\370");
    tft.setCursor(106,29);
    tft.print("C");

    tft.setTextColor(0x07FF);
    tft.setFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    tft.setCursor(74,95);
    tft.print("%");
    tft.setFont();
    tft.setTextColor(0x07FF);
    tft.setCursor(95,85);
    tft.setTextSize(1);
    tft.print("Humid");
    tft.setTextColor(0xAFE5);
    tft.setTextSize(1);
    tft.setCursor(107,135);
    tft.print("m/s");
    tft.setTextColor(ST7735_RED);
    tft.setCursor(72,108);
    tft.print(" hpa");
    
     delay(1000);
    
}

int temp;
int pres;
int hum;
String wind;
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      timeClient.update();
      String jsonString = JSON.stringify(myObject["main"]["temp"]);
      temp=jsonString.toInt();
      temp=(temp-273);
      jsonString = JSON.stringify(myObject["main"]["pressure"]);
      pres=jsonString.toInt();
      jsonString = JSON.stringify(myObject["main"]["humidity"]);
      hum=jsonString.toInt();
      jsonString = JSON.stringify(myObject["wind"]["speed"]);
      wind=jsonString;
      Serial.println();
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(temp);
      Serial.print("Pressure: ");
      Serial.println(pres);
      Serial.print("Humidity: ");
      Serial.println(hum);
      Serial.print("Wind Speed: ");
      Serial.println(wind);
      
      tft.fillRect(14, 45 , 68, 28, ST7735_BLACK);
      tft.fillRect(24, 70 , 55, 10, ST7735_BLACK);
      tft.fillRect(73, 24 , 25, 21, ST7735_BLACK);
      tft.fillRect(40, 80 , 26, 21, ST7735_BLACK);
      tft.fillRect(20, 105 , 55, 18, ST7735_BLACK);
      tft.fillRect(50, 130 , 45, 19, ST7735_BLACK);
      
      tft.setTextColor(ST7735_YELLOW);
      tft.setFont(&FreeMonoBold12pt7b);
      tft.setTextSize(1);
      tft.setCursor(72,42);
      tft.print(temp);
      //tft.setFont();
      tft.setTextColor(ST7735_WHITE);
      tft.setCursor(13,62);
      tft.print(timeClient.getHours());
      if (timeClient.getMinutes() < 10) {
        tft.print(":0");
      } else {
        tft.print(":");
      }
      tft.print(timeClient.getMinutes());
      tft.setFont();
      tft.setTextSize(1);
      tft.setCursor(26,70);
      tft.print(daysOfTheWeek[timeClient.getDay()]);
      tft.setTextColor(0x07FF);
      tft.setFont(&FreeMono12pt7b);
      tft.setTextSize(1);
      tft.setCursor(40,97);
      tft.print(hum);
      tft.setTextColor(ST7735_RED);
      tft.setCursor(18,121);
      tft.print(pres);
      
      tft.setTextColor(0xAFE5);
      tft.setCursor(48,148);
      tft.print(wind);
    
    
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
    
    
  
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  
  http.begin(client, serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}
