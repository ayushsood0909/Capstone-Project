/*
   Controller - ESP 8266
   D1 SCL
   D2 SDA
*/

#include <Adafruit_MLX90614.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#define DEBUG true
#define ID 26
#define SSID "Mehra"
#define PASS "Mehra@7805"
#define SERVER "http://computersshiksha.info/student-projects/iot/"
#define BUZZER 12        // D6
#define SERVO_DOOR 2   // D4 

int temp_factor = 6;
float ambient_f, object_f;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Servo servo;
WiFiClient wifiClient;
HTTPClient http;
void setup() {
  Serial.begin(9600);
  mlx.begin();
  servo.attach(SERVO_DOOR);
  servo.write(90);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  initWiFi();
  Serial.println("Setup Complete");
}
bool isMask = false;
void loop() {
  getDataFromServer();
  if (isMask) {
    beep(1);
    get_temp();
    sendDataToServer("&field2=" + String(object_f));
    if (object_f > 96 && object_f < 98) {
      door(true);
    } else {
      beep(1);
      door(false);
    }
  }

}
void get_temp() {
  ambient_f = mlx.readAmbientTempF() + temp_factor;
  object_f = mlx.readObjectTempF() + temp_factor;

  Serial.println(object_f);
}
void door(bool s) {
  if (s) {
    servo.write(0);
    delay(5000);
    servo.write(90);
    delay(1000);
  } else {
    servo.write(90);
  }
}

void beep(int times) {
  while (times--) {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
  }
}

void sendDataToServer(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(SERVER) + "/set_values.php?id=" + String(ID) + msg;
    Serial.println(url);
    http.begin(wifiClient, url);     //Specify request destination
    //  http.addHeader("Content-Type", "text/plain");  //Specify content-type header
    int httpCode = http.GET(); //Send the request
    String responseData = http.getString();  //Get the response data
    if (DEBUG) {
      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(responseData);
    }
  } else {
    initWiFi();
  }
}
void getDataFromServer() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(SERVER) + "get_values.php?id=" + String(ID);
    Serial.println(url);

    http.begin(wifiClient, url);
    int code = http.GET();
    if ( code == 200) {
      String payload = http.getString();
      JSONVar json_object = JSON.parse(payload);
      if (JSON.typeof(json_object) == "undefined") {
        if (DEBUG)
          Serial.println("Parsing input failed!");
        return;
      }

      if (DEBUG)
        Serial.println(json_object);

      char marray[5];
      strcpy(marray, json_object["field1"]);
      Serial.println(marray[0]);
      if (marray[0] == 'M' && !isMask) {
        isMask = true;
        if (DEBUG)
          Serial.print("Started...");
      } else if (marray[0] == 'N') {
        isMask = false;
      }

      http.end();
    } else {
      Serial.println(code);
    }
    http.end();
  } else {
    initWiFi();
  }
}
void initWiFi() {

  if (DEBUG) {
    Serial.println("\nConnecting to WiFi ..");
    Serial.print("SSID : ");
    Serial.print(SSID);
    Serial.print(", Pass : ");
    Serial.println(PASS);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  unsigned long s_time = millis();
  while (WiFi.status() != WL_CONNECTED) {

    if (DEBUG)
      Serial.print('.');
    if (millis() - s_time > 5000)
      break;
    delay(500);
  }

  if (DEBUG)
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("\nWiFi Connected ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nWiFi not Connected ");
    }
}
