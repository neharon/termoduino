#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>                  // Библиотека для разбора JSON


const char* ssid = "Xiaomi_49C2";  // SSID 
const char* password = "Killme21";  //пароль
ESP8266WebServer server(80); // объект библиотеки ESP8266WebServer,80 - порт прослушивания стандартный для HTTP чтобы не прописывать его в URL

const size_t capacity = 48;             // Эта константа определяет размер буфера под содержимое JSON (https://arduinojson.org/v5/assistant/)
StaticJsonDocument<capacity> doc;        // Инициализируем буфер под JSON
//StaticJsonDocument<48> jsonBuffer;




void handleRoot() {
  
  server.send(200, "text/plain", "handleRoot()");
  
}


String getStatus() {

  return("{ \"methodName\": \"getStatus\", \"response\": { \"data\": [{ \"time\": \"12345\" }, { \"currentT\": \"22.22\" }, { \"hightAlarm\": \"33.33\" }, { \"lowAlarm\": \"-33.33\" } ] } }");
  //server.send(200, \"text/plain\", handleRoot()");
  
}


void handleWS() {
  
  Serial.print("hostHeader: ");
  Serial.println(server.hostHeader());

  Serial.print("method: ");
  Serial.println(server.method()); // post or get or ...

  Serial.print("headers: ");
  Serial.println(server.headers()); 

  Serial.print("hostheaders: ");
  Serial.println(server.hostHeader()); 
  
 
  Serial.print("args: ");
  Serial.println(server.args());
  //Serial.println(server.header());
  Serial.print("json body: ");
  Serial.println(server.arg("plain")); //arg - get request argument value, use arg("plain") to get POST body
  //JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
  //String StringMethodName = root["methodName"].as<String>();
  //DeserializationError error = deserializeJson(doc, server.arg("plain"));
 

 // const char* methodName = doc["methodName"]; // "getStatus"
 // Serial.println(StringMethodName);
  
  //Serial.println(server.argName(0));

  for (int i = 0; i < server.headers(); i++) 
  {
    Serial.print("header nº" + (String)i + " –>");
    Serial.print(server.headerName(i) + ": ");
    Serial.print(server.hostHeader() + " --- ");
    Serial.println(server.header(i));
    
  }

 for (int i = 0; i < server.args(); i++) 
  {
    Serial.print("arg nº" + (String)i + " –>");
    Serial.print(server.argName(i) + ": ");
    Serial.println(server.arg(i));
  }


  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
   Serial.print(F("deserializeJson() failed: "));
   Serial.println(error.f_str());
   return;
  }

  const char* methodName = doc["methodName"]; // "getStatus"
  Serial.print("json methodName: ");
  Serial.println(methodName); 
  //if(methodName == "getStatus"){
    
  server.send(200, "application/json", getStatus());
    //Serial.println(getStatus());
    //}
  
 
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  delay(100);


 
  Serial.println("Connecting to ");
  Serial.println(ssid);
  
    // подключаемся к локальной Wi-Fi сети
  WiFi.begin(ssid, password);
  // проверка подключения Wi-Fi к сети
  while (WiFi.status() != WL_CONNECTED) 
  {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  //JsonObject& root = jsonBuffer.parseObject(client);
  server.on("/", handleRoot);
  server.on("/ws/", handleWS);

  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
