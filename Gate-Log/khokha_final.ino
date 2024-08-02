#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN    5
#define LED_COUNT  1

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_CS   15  // Chip select control pin (GPIO 20)
#define TFT_DC   12  // Data Command control pin (GPIO 18)
#define TFT_RST  4   // Reset pin (GPIO 19)

# define rx 16  //to green
# define tx 17  //to white
#include <WiFi.h>
#include <HTTPClient.h>
const size_t capacity = JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);
//JsonObject sendData = root["sendData"].to<JsonObject>();
const char* gate = "Khokha Gate";  
const char* ssid = "GalaxyA14";//"Spandhansrirag_EXT";//"T-Works Foundation";
const char* password = "poiuytrewq";//"Kailash@08";//"Explore@TWorks";

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600,SERIAL_8N1,rx,tx);
  strip.begin();
  strip.show();
  strip.setBrightness(100);
//  strip.setPixelColor(0,strip.Color(255,   0,   0));
//  strip.show();
  initTFT();
  clearTFT();
  //screenWelcome();
  delay(1000);
  screenWelcome();
  displayCenteredParagraph("Connecting to WiFi");
  // Connect to WiFi network
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  screenWelcome();
  displayCenteredParagraph("Connected to WiFi");
  delay(1000);
  screenWelcome();
  displayCenteredParagraph("Scan QR for Entry/Exit");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)&&(Serial2.available())) {
    String sensorData = readSensorData();
    strip.setPixelColor(0,strip.Color(255, 255, 0));
    strip.show();
    clearTFT();
    screenWelcome();
    //screenWelcome();
    DeserializationError error = deserializeJson(doc, sensorData);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    displayCenteredParagraph("Error Detected");
    //goto er;
  }

    //screenWelcome();
    
    HTTPClient http;
    int httpResponseCode;
    // Specify the destination URL and the HTTPS port (default is 443)
    if(doc["isExit"]==true)
    {
    http.begin("https://swc.iitg.ac.in/test/khokhaEntry/api/v1/newEntry"); 
    http.addHeader("khokha-security-key", "KhOkHa-DeV");
    http.addHeader("Content-Type", "application/json");
    //JsonObject rem = doc["destination"];
    doc.remove("isExit");
    doc["checkOutGate"]= gate;
    serializeJsonPretty(doc,sensorData);

    httpResponseCode = http.POST(sensorData);
    }
    else if(doc["isExit"]==false)
    {
      String id = doc["entryId"];
      //Serial.println("https://swc.iitg.ac.in/test/khokhaEntry/api/v1/closeEntry/"+id);
      http.begin("https://swc.iitg.ac.in/test/khokhaEntry/api/v1/closeEntry/"+id);
      http.addHeader("khokha-security-key", "KhOkHa-DeV");
      http.addHeader("Content-Type", "application/json");
      doc.remove("isExit");
      doc["checkInGate"]= gate;
      serializeJsonPretty(doc,sensorData);

      httpResponseCode = http.PATCH(sensorData);
    }


    Serial.println(sensorData);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String response = http.getString();
      
      DeserializationError error = deserializeJson(doc,response);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

      if(doc["success"]==true)
      {
        strip.setPixelColor(0,strip.Color(255, 0, 0));
        strip.show();
      }
      else
      {
        strip.setPixelColor(0,strip.Color(0, 255, 0));
        strip.show();
        Serial.println("bachao");
      }
      displayCenteredParagraph(doc["message"]);
      //screenOverlayID(doc["message"]);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    // Close connection
    http.end();

    //er:
    delay(1000);
    screenWelcome();
    displayCenteredParagraph("Scan QR for Entry/Exit");
    strip.setPixelColor(0,strip.Color(0, 0, 0));
    strip.show();
  }
  
  //delay(1000); // Wait before sending next request
}



String readSensorData() {
  String dat = "";

  // Read data from sensor until newline character is received
  while (Serial2.available()) {
    dat = Serial2.readString();
    
  }
  return dat;
}
