// --- WIFI ---
#include <ESP8266WiFi.h>
const char* ssid = "caelum_sala3";
const char* password = "5987cb800f";
WiFiClient nodemcuClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_Broker = "iot.eclipse.org";
const char* mqtt_ClientID = "PauloScalercio";
PubSubClient client(nodemcuClient);
const char* topicoTemperatura = "lab-pscalercio/temperatura";
const char* topicoUmidade = "lab-pscalercio/umidade";

// --- DHT ---
#include <DHT.h>
#define DHTPIN D3
#define DHTTYPE DHT22 // selecionar o tipo de sensor DHT11, 22...
DHT dht(DHTPIN,DHTTYPE);
float umidade;
float temperatura;

// --- DISPLAY ---
#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

// --- SETUP ---
void setup() {
  Serial.begin(9600);
  configurarDisplay();
  conectarWifi();
  client.setServer(mqtt_Broker, 1883);

}

// --- LOOP ---
void loop() {
  if(!client.connected()){
    reconectarMQTT();
  }
  medirTemperaturaUmidade();
  publicarTemperaturaUmidadeNoTopico();
  mostrarTemperaturaUmidade();
}

// --- FUNCOES AUXILIARES ---

// --- CONFIGURACAO E CONEXAO WIFI ---
void conectarWifi(){
  delay(10);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Conectando");
  display.display(); 
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
}

// --- RECONECTAR O CLIENTE MQTT ---
void reconectarMQTT(){
  while(!client.connected()){
    client.connect(mqtt_ClientID);
  }
}

// --- CONFIGURA O DISPLAY ---
void configurarDisplay(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();  
}

// --- PUBLICA (MQTT) TEMPERATURA E UMIDADE ---
void publicarTemperaturaUmidadeNoTopico(){
  client.publish(topicoTemperatura, String(temperatura).c_str(), true); 
  client.publish(topicoUmidade, String(umidade).c_str(), true);  
}

// --- MEDICAO DE TEMPERATURA E UMIDADE ---
void medirTemperaturaUmidade() {
 umidade = dht.readHumidity();
 temperatura = dht.readTemperature(false);

 Serial.print("temperatura: " );
 Serial.print(temperatura);
 Serial.print("   ");
 Serial.print("Umidade: ");
 Serial.println(umidade);
 delay(5000);
}

// --- MOSTRA TEMPERATURA E UMIDADE ---
void mostrarTemperaturaUmidade(){
  mostrarMensagemNoDisplay("Temperatura", (temperatura), " C");
  mostrarMensagemNoDisplay("Umidade", (umidade), " %");
}

// --- EXIBE TEMPERATURA E UMIDADE NO DISPLAY ---

void mostrarMensagemNoDisplay (const char* texto1, float medicao, const char* texto2){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(texto1);
  display.setTextSize(5);
  display.setCursor(20,20);
  display.print(medicao);
  display.setTextSize(2);
  display.print(texto2);
  display.display();
  delay(2000);
}

