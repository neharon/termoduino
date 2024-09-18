// адрес датчика для D2 Sensor 1 : 0x28, 0xFF, 0x64, 0x1E, 0x83, 0x61, 0xCB, 0x99
#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Шина данных подключена к выводу D2 на ESP8266
#define ONE_WIRE_BUS D2

// Настройка объекта oneWire для связи с любыми устройствами OneWire
OneWire oneWire(ONE_WIRE_BUS);

// Передаем ссылку на объект oneWire объекту Dallas Temperature.
DallasTemperature sensors(&oneWire);

float tempSensor1, tempSensor2, tempSensor3;

#define TEMPERATURE_PRECISION 12  // точность измерений (9 ... 12)
uint8_t sensor1[8] = { 0x28, 0xFF, 0x64, 0x1E, 0x83, 0x61, 0xCB, 0x99 };


/* Введите SSID и пароль от вашей сети*/
//const char* ssid = "Xiaomi_49C2";  // SSID
//const char* password = "Killme21";  //пароль

const char* ssid = "RT-GPON-96C8";         // SSID
const char* password = "718281828459045";  //пароль

//const char* ssid = "termoduino";           // SSID
//const char* password = "718281828459045";  //пароль

const WiFiMode wiFiMode = WIFI_STA;  // WIFI_AP - access point   WIFI_STA - station

DeviceAddress Thermometer;

ESP8266WebServer server(80);

void setup() {
  Serial.begin(9600);
  delay(100);

  sensors.begin();                                            // инициализация DS18B20
  sensors.getAddress(Thermometer, 0);                         // адрес DS18B20 (поиск по индексу)
  sensors.setResolution(Thermometer, TEMPERATURE_PRECISION);  // установка точности измерения 9...12 разрядов
  Serial.print("Sensor addr: ");
  Serial.println((int)&Thermometer);


  if (wiFiMode == WIFI_AP) {
    WiFi.mode(WIFI_AP);
    // Точка доступа
    if (!WiFi.softAP(ssid, password)) {
      Serial.println("Soft AP creation failed.");
      while (1)
        ;
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  } else {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    // подключиться к вашей локальной wi-fi сети
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // проверить, выполнено ли подключение wi-fi сети
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected..!");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
  }
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  sensors.requestTemperatures();
  tempSensor1 = sensors.getTempC(Thermometer);  // Получить значение температуры
  Serial.print("tempSensor1 = ");
  Serial.println(tempSensor1);
  server.send(200, "text/html", SendHTML(tempSensor1, tempSensor2, tempSensor3));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float tempSensor1, float tempSensor2, float tempSensor3) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 Temperature Monitor</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ESP8266 Temperature Monitor</h1>\n";
  ptr += "<p>Living Room: ";
  ptr += tempSensor1;
  ptr += "&deg;C</p>";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
