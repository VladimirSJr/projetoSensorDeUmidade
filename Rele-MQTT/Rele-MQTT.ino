#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define pinRele 4

//Wifi
const char* SSID = "********";
const char* PASSWORD = "*********";
WiFiClient wifiClient;

//MQTT Server
const char* BROKER_MQTT = "iot.eclipse.org";
int BROKER_PORT = 1883;

#define ID_MQTT "BCI02"
#define TOPIC_PUBLISH "BCISolo"
PubSubClient MQTT(wifiClient);

//Declaração das funções
void mantemConexao();
void conectaWiFi();
void conectaMQTT();
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(pinRele, OUTPUT);

  Serial.begin(9600);
  
  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(recebePacote);
}

void loop() {
  mantemConexao();
  MQTT.loop();
}

void mantemConexao(){
  if(!MQTT.connected()){
    conectaMQTT();
  }

  conectaWiFi();
}

void conectaWiFi(){
  if(WiFi.status() == WL_CONNECTED){
    return;
  }

  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println(" Aguarde!");

  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
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

void recebePacote(char* topic, byte* payload, unsigned int length){
  String msg;

  //obtem a string do payload recebido
  for(int i = 0; i < length; i++){
    char c = (char)payload[i];
    msg += c;
  }

  if(msg == "0"){
    digitalWrite(pinRele, LOW);
  }
  if(msg == "1"){
    digitalWrite(pinRele, HIGH);
  }
}