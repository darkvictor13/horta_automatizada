#include <SSD1306Wire.h> 
#include "DHT.h"

#define RELAY 12
#define DHT_PIN 5    
#define DHT_TYPE DHT22 
#define SCL 22
#define SDA 21
#define PIN_SOLO 15

#define HUM_SOLO_MIN 30
#define HUM_AR_MIN 60
#define TEMP_AR_MAX 30

SSD1306Wire display(0x3c, SDA, SCL);
DHT dht(DHT_PIN, DHT_TYPE);

float humidade, temperatura, umidade_do_solo;
String texto_temperatura = "";
String texto_umidade = "";
String texto_umidade_solo = "";

void ligarBomba(){
  digitalWrite(RELAY, HIGH);
}

void desligarBomba(){
  digitalWrite(RELAY, LOW);
}

void medeUmidadeDoSolo() {
  umidade_do_solo = map (analogRead (PIN_SOLO), 4095, 1000, 0, 100);
  texto_umidade_solo = String(umidade_do_solo) + " % ";
  delay(2000);
}

void iniciaSensorSolo(){
  pinMode(PIN_SOLO, INPUT);
}

void inicializaSensorHumETempAr(){
  Serial.println(F("Sensor Test"));
  dht.begin();
}

void leituraSensorHumidadeEtemperaturaDoAr(){
  delay(2000);

  humidade = dht.readHumidity();
  temperatura = dht.readTemperature();  

  if (isnan(humidade) || isnan(temperatura)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    
  }
  texto_temperatura = String(temperatura) + " C ";
  texto_umidade = String(humidade) + " % ";
}

void imprimeDadosNoDisplay(){
  display.clear();
  Serial.println("Imprimindo no display");
  Serial.println(texto_temperatura);
  Serial.println(texto_umidade);
  Serial.println(texto_umidade_solo);
  
  display.setFont(ArialMT_Plain_16);
  display.drawRect(0, 0, 128, 64);
  //display.setTextAlignment(TEXT_ALIGN_CENTER);
  //display.drawString(50, 0, texto_temperatura + texto_umidade + textoUmidadeSolo);
  display.drawString(10, 2, "Temp : " + texto_temperatura + "\n");
  display.drawString(10, 20, "Umid : " + texto_umidade + "\n");
  display.drawString(10, 40, "SoLo : " + texto_umidade_solo + "\n");
  display.display();
}

void lerTodosSensores(){
  medeUmidadeDoSolo();
  leituraSensorHumidadeEtemperaturaDoAr();  
}

void mensagemTempAlta() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawRect(0, 0, 128, 22);
  display.drawString(2, 12, "Temperatura alta");//16

  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 27, "  Mude a horta\n");
  display.drawString(35, 45,"de lugar\n");
  delay(1000);
}

void mensagemHumBaixa() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawRect(0, 0, 128, 22);
  display.drawString(4, 2, "Umid do ar baixa");//16

  display.setFont(ArialMT_Plain_16);
  display.drawString(10, 27, "  Mude a horta\n");
  display.drawString(35, 45,"de lugar\n");
  display.display();
  delay(1000);
}

void tomaAcoes () {
  if (umidade_do_solo < HUM_SOLO_MIN) {
    ligarBomba();
  }else {
    desligarBomba();
  }

  if (temperatura > TEMP_AR_MAX) mensagemTempAlta();
  if (humidade < HUM_AR_MIN) mensagemHumBaixa();
}

void setup() {
  //Inicia Monitor Serial
  Serial.begin(9600);

  //Incia sensor de temperatura e humidade do ar
  inicializaSensorHumETempAr();

  //Inicia Display oled
  display.init();

  //Inicia Sensor de umidade do solo
  iniciaSensorSolo();

  //Inicia o relay
  pinMode(RELAY, OUTPUT);
}

void loop() {  
  lerTodosSensores();
  tomaAcoes();   
  imprimeDadosNoDisplay();
}
