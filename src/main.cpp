/*
Características:
Una entrada AD.
Entradas Micro USB.
Un LED programable (D0).
Sistema de carga y descarga de batería 18650 integrado.
Un interruptor controla si la batería 18650 está encendida o no.
Los SDA y SCL de OLED se conectan al pin D1 y al pin D2 respectivamente.
Los cinco botones se controlan por flashh, RSET, D5, D6 y d7.
Los 5 pines digitales pueden configurar la escritura/lectura/interrupción/pwm/I2C/un cable compatible por separado.
Operación y NodeMCU consistente, agregando un LED programable, puede usar GPIO16 para controlar, mostrar el estado de funcionamiento 8266 y otras funciones. OLED integrado y cinco botones, más conveniente para el desarrollo.
El concepto de diseño se origina en el proyecto de código abierto NodeMCU, y la placa de desarrollo integra sistemas de carga y descarga 18650 con protección de carga y descarga. Al mismo tiempo, un OLED y cinco botones direccionales están integrados para facilitar el desarrollo.

Especificaciones:
Modelo: ESP-8266 ESP-12F
Color: Negro
Protección de carga: Sí
Protección de descarga: Sí
Amplio rango de entrada de voltaje: DC 5V -12V
Corriente de carga: 500 mA
Tamaño: 10,3x3cm/4,06x1,18"
Tamaño de la pantalla: 0,96 pulgadas

https://www.studiopieters.nl/esp8266-pinout/
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SSD1306.h"
// Update these with values suitable for your network.

const char *ssid = "***";
const char *password = "***";
const char *mqtt_server = "192.168.*.**";
const int mqttPort = 1883;
const char *mqttUser = "***";
const char *mqttPassword = "**";

char *Nombre = "Mando";

long lastMsg = 0; // Registre la última vez para enviar la información la última vez

const int ledPIN_azul = D4;
const int ledPIN_verde = D0;

const int BOTON1 = 13; // D7 BAJAR
const int BOTON2 = 12; // D6 SUBIR
const int BOTON3 = 14; // D5 CENTRO

int bajar = 0;  // val se emplea para almacenar el estado del boton
int subir = 0;  // val se emplea para almacenar el estado del boton
int centro = 0; // val se emplea para almacenar el estado del boton

int state = 0;

WiFiClient espClient;
PubSubClient client(espClient);
SSD1306 display(0x3c, 5, 4); // D1 D2

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // WiFi.hostname(Nombre);
  wifi_station_set_hostname(Nombre);
  WiFi.begin(ssid, password);

  uint8_t t = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    
    display.setFont(ArialMT_Plain_24);
    display.drawString(0 + (t * 15), 10, "O");
    t++;
    display.display();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Nombre de Esp: ");
  Serial.println(WiFi.hostname());
  display.clear();
  // display.drawString(0,10,"WIFI CONECTADO la IP ES");
  // display.drawString(0,20, WiFi.localIP());

  display.display();
  display.clear();
  display.display();

}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String content = "";
  for (size_t i = 0; i < length; i++)
  {
    content.concat((char)payload[i]);
  }
  Serial.print(content);
  Serial.println();

  String myString = String((char *)payload);

  String consumo_diario = "Con :";
  String potencia_usada = "Pot :";

  String primer_corte = myString.substring(myString.indexOf('\'')+1);
  String segundo_corte = primer_corte.substring(primer_corte.indexOf('\'')+4); 
  int index1 = (primer_corte.indexOf('\''));  
  int index2 = (segundo_corte.indexOf('\''));  
  consumo_diario = consumo_diario + primer_corte.substring(0, index1);
  potencia_usada = potencia_usada + segundo_corte.substring(0, index2);

  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 10, potencia_usada);
  display.drawString(0, 30, consumo_diario);
  
  display.display();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Intentando conexion MQTT...");
    // Attempt to connect
    if (client.connect(Nombre, mqttUser, mqttPassword))
    {
      Serial.println("CoNeCtAdo");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe

      //client.subscribe("shellies/shellyem-C45BBE793C1C/emeter/0/power");
      client.subscribe("homeassistant/energia/state");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{

  pinMode(ledPIN_azul, OUTPUT);
  pinMode(ledPIN_verde, OUTPUT);

  digitalWrite(ledPIN_azul, HIGH);
  digitalWrite(ledPIN_verde, HIGH);

  display.init();
  display.invertDisplay();
  display.flipScreenVertically();

  Serial.begin(9600);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}