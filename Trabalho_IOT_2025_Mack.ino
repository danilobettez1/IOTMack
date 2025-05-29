#include <WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

/* ---------- CONFIG WIFI E MQTT ---------- */
const char* ssid     = "Jabobilunga 2.0";         
const char* password = "4r3e2w1q";                    
const char* mqttHost = "broker.hivemq.com";   
const int   mqttPort = 1883;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

/* ---------- PINOS ---------- */
#define PIN_TRIG   5
#define PIN_ECHO   18
#define PIN_TURB   34
#define PIN_RELAY  23

Ultrasonic sensor(PIN_TRIG, PIN_ECHO);

/* ---------- LIMITES ---------- */
const float alturaMaxReservatorio = 20.0; // cm
const float nivelMaximoPermitido = 15.0;  // cm
const int turbidezMinSegura = 900;

/* ---------- TÓPICOS MQTT ---------- */
const char* topNivel      = "iot/nivel_cm";
const char* topTurbidez   = "iot/turbidez_raw";
const char* topValvula    = "iot/valvula_estado";

/* ---------- CONTROLE DE RECONEXÃO MQTT ---------- */
unsigned long ultimaTentativaMQTT = 0;
bool bloqueadoMQTT = false;

void conectaWiFi() {
  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado. IP: " + WiFi.localIP().toString());
}

void conectaMQTT() {
  int tentativas = 0;
  const int maxTentativas = 2;

  while (!mqtt.connected() && tentativas < maxTentativas) {
    Serial.print("Tentando conectar ao MQTT (");
    Serial.print(tentativas + 1);
    Serial.print("/");
    Serial.print(maxTentativas);
    Serial.print(")... ");

    if (mqtt.connect("esp32-iot-monitor")) {
      Serial.println("conectado!");
      bloqueadoMQTT = false;
      return;
    } else {
      Serial.print("falhou, rc=");
      Serial.print(mqtt.state());
      Serial.println(" tentando novamente em 2s");
      tentativas++;
      delay(2000);
    }
  }

  if (!mqtt.connected()) {
    Serial.println("Não foi possível conectar ao broker MQTT após 2 tentativas.");
    ultimaTentativaMQTT = millis();
    bloqueadoMQTT = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);  

  conectaWiFi();
  mqtt.setServer(mqttHost, mqttPort);
  conectaMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) conectaWiFi();

  if (!mqtt.connected()) {
    if (!bloqueadoMQTT) {
      conectaMQTT();
    } else {
      unsigned long tempoAtual = millis();
      if (tempoAtual - ultimaTentativaMQTT > 300000) { // 5 minutos
        Serial.println("Tentando novamente conexão MQTT após 5 minutos...");
        conectaMQTT();
      }
    }
  }

  mqtt.loop();

  int turbidez = analogRead(PIN_TURB);
  float distancia = sensor.distanceRead(CM);
  if (distancia == 0) distancia = alturaMaxReservatorio;
  float nivel = alturaMaxReservatorio - distancia;

  String estadoValvula = "";

  // 
  bool condTurbidezBaixa = turbidez < 900;
  bool condNivelAlto = nivel > nivelMaximoPermitido;

  if (condTurbidezBaixa || condNivelAlto) {
    digitalWrite(PIN_RELAY, LOW);
    estadoValvula = "FECHADA (SEGURANÇA)";
  } else {
    digitalWrite(PIN_RELAY, HIGH);
    estadoValvula = "ABERTA";
  }

  // Serial Monitor
  Serial.print("Turbidez: ");
  Serial.print(turbidez);
  Serial.print(" | Nível: ");
  Serial.print(nivel, 1);
  Serial.print(" cm | Válvula: ");
  Serial.print(estadoValvula);
  Serial.print(" | MQTT: ");
  Serial.println(mqtt.connected() ? "CONECTADO" : "DESCONECTADO");

  // MQTT
  if (mqtt.connected()) {
    mqtt.publish(topTurbidez, String(turbidez).c_str());
    mqtt.publish(topNivel, String(nivel, 1).c_str());
    mqtt.publish(topValvula, estadoValvula.c_str());
  }

  delay(3000);
}