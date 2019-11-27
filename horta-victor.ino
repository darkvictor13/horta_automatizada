#include <WiFi.h>//biblioteca para iniciar o esp-webServer
#include <Wire.h>
#include <Adafruit_AM2315.h>//biblioteca do leitor de umidade e temperatura do ar
//#include "SSD1306.h"//biblioteca do Oled

Adafruit_AM2315 am2315;

//pinos usados no esp
#define pin_relay 13//5
#define pin_solo 15//36
#define scl 5//22
#define sda 18//21

const char* ssid  = "Horta IOT";//nome da rede do esp
const char* password = "Iotehmtbom";//senha

float temperatura,umidade,umidade_solo_porcentagem,
umidade_solo_minima = 20,temp_maxima = 30,humidade_minima = 60,
aux_umidade, aux_temperatura, aux_solo;

int erro;
/*significados de cada erro
erro = 1 = temperatura muito alta para a planta
erro = 2 = umidade do ar muito baixa para a planta
erro = 3 = umidade do solo mt baixa
erro = 4 = erro na maquina*/

//SSD1306 display(0x3c, sda, scl);

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(pin_solo,INPUT);
  pinMode(pin_relay, OUTPUT);
  WiFi.softAP(ssid, password);
  //IPAddress IP = WiFi.softAPIP();
  //display.init();//inicia o display do oled
  //display.setFont(ArialMT_Plain_16);//define a fonte e o tamanho
  //display.clear();//limpa a tela
  server.begin();//inicia o server do esp
  //delay(1000);
  //if (! am2315.begin()) {
  //   Serial.println("Sensor not found, check wiring & pullups!");
  //   while (1);
  //}
  delay(2000);
  while (! am2315.begin()) {
    Serial.println("Sensor not found, check wiring & pullups!");
    delay(10);
  }
  Serial.println("Sensor achado!");
  //am2315.begin();
}

void medeUmidadeDoSoloSalvaEmPorcentagem () {
  //funcao que faz a medicao da umidade do solo retorna em porcentagem
  aux_solo = 0;
  while (! aux_solo) {
    aux_solo = map (analogRead (pin_solo),4095,1000,0,100);
  }
  umidade_solo_porcentagem = aux_solo;
  //funcao map tranforma em um valor de 0-100
}


void leitura () {
  medeUmidadeDoSoloSalvaEmPorcentagem();
  while (! am2315.readTemperatureAndHumidity(&aux_temperatura, &aux_umidade)) {
    Serial.print("Hum %: "); Serial.println(aux_umidade);
    Serial.print("temperatura: "); Serial.println(aux_temperatura);
    delay(100);
  }
  umidade = aux_umidade;
  temperatura = aux_temperatura;
}

void verificaErros () {
  if (umidade_solo_porcentagem <= umidade_solo_minima) {//valor de humidade do solo que a planta acha ruim
    erro = 3;
  }else if (temperatura > temp_maxima) {//valor de temperatura que a planta acha ruim
     erro = 1;
  }else if (umidade < humidade_minima) {//valor de umidade que a planta acha ruim
    erro = 2;
  }else {
    erro = 0;
  }
}

/*void MostraParaUsuario() {//faz exatamente oq o nome fala =)
  if (erro == 1) {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,0,"Temperatura alta\n");
    display.drawString(0,16,"Coloque a\n");
    display.drawString(0,32,"horta num local\n");
    display.drawString(0,48,"Adequado\n");
    display.display();//faz todos os comandos aparecerem no oled
  }
  if (erro == 2) {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,0,"Umid. ar baixa\n");
    display.drawString(0,16,"Coloque a\n");
    display.drawString(0,32,"horta num local\n");
    display.drawString(0,48,"Adequado\n");
    display.display();
  }
  if (erro == 3) {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawRect(0, 0, 128, 64);
    //desenha um retangulo nas bordas da tela ja que a resolucao eh 128x64
    display.drawString(0,0,"Umid. solo baixa\n");
    display.drawString(0,16,"Iniciando irrigacao\n");
    display.drawString(0,32,"O Bomba d`agua\n");
    display.drawString(0,48,"Deve ligar\n");
    display.display();
  }
  if (erro == 4) {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,0,"Erro no sistema\n");
    display.drawString(0,16,"Alguma coisa esta\n");
    display.drawString(0,32,"Muito errada ae\n");
    display.drawString(0,48,"(;-;)\n");
    display.display();
  }
  if (!erro) {
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,16,"Horta saudavel\n");
    //deixar esse print mais bonito, de preferencia fazer um desenho sla!
  }
}*/

void realizaAcoes () {
  if (erro == 3) {
    digitalWrite(pin_relay, HIGH);
  }else {
    digitalWrite(pin_relay, LOW);
  }
}

void loop() {
  leitura();
  //Serial.print("Temp *C: "); Serial.println(temperatura);
  //Serial.print("Hum %: "); Serial.println(umidade);
  //Serial.print("Hum solo: "); Serial.println(umidade_solo_porcentagem);
  verificaErros();
  Serial.print("Erro = "); Serial.println(erro);
  realizaAcoes();
  //MostraParaUsuario();
}
