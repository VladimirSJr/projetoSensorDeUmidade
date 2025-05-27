#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define pinSolo A0

//WiFi
const char* SSID = "*********";
const char* PASSWORD = "*********";
WiFiClient wifiClient;

//MQTT Server
const char* BROKER_MQTT = "iot.eclipse.org";
int BROKER_PORT = 1883;

#define ID_MQTT "BCI01"
#define TOPIC_PUBLISH "BCISolo"
PubSubClient MQTT(wifiClient);

//Declaração das funções
void mantemConexao();
void conectaWifi();
void conectaMQTT();
void enviaValores();

void setup() {
  Serial.begin(9600);
  
  pinMode(pinSolo, INPUT);

  conectaWifi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void loop() {
  mantemConexao();
  enviaValores();
  MQTT.loop();
  delay(1000);
}

void mantemConexao(){
  if(!MQTT.connected()){
    conectaMQTT();
  }

  conectaWifi();
}

void conectaWifi(){
  if(WiFi.status() == WL_CONNECTED){
    return;
  }

  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println(" Aguarde!");

  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);
  Serial.print(" IP obtido: ");
  Serial.println(WiFi.localIP());
}

void conectaMQTT(){
  while(!MQTT.connected()){
    Serial.print("Conectando ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if(MQTT.connect(ID_MQTT)){
      Serial.println("Conectado ao Broker com sucesso");
    }
    else{
      Serial.println("Não foi possível se conectar ao Broker!");
      Serial.println("Nova tentativa de conexão em 10s");
      delay(10000);
    }
  }
}

void enviaValores(){
  static uint16_t estadoSolo;
  static unsigned long debounceSolo;

  estadoSolo = analogRead(pinSolo);
  
  if ( (millis() - debounceSolo) > 30 ){
    if(estadoSolo > 700){
      //Ligar bomba de água
      MQTT.publish(TOPIC_PUBLISH, "1");
      Serial.println("Bomba ligada. Payload enviado.");

      debounceSolo = millis();
    }else if(estadoSolo < 700){
      //Desligar bomba de água
      MQTT.publish(TOPIC_PUBLISH, "0");
      Serial.println("Bomba desligada. Payload enviado.");

      debounceSolo = millis();
    }
  }
}