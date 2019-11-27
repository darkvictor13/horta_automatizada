#include <WiFi.h>//biblioteca para iniciar o esp-webServer
#include <Wire.h>
#include <Adafruit_AM2315.h>//biblioteca do leitor de umidade e temperatura do ar
#include "SSD1306.h"//biblioteca do Oled

Adafruit_AM2315 am2315;

//pinos usados no esp
#define pin_relay 13//5
#define pin_solo 15//36
#define scl 5//22
#define sda 18//21

const char* ssid  = "Horta IOT";//nome da rede do esp
const char* password = "Iotehmtbom";//senha

float temperatura,umidade,umidade_solo_porcentagem,umidade_solo_minima = 20,temp_maxima = 30,humidade_minima = 60;
int erro;
/*significados de cada erro
erro = 1 = temperatura muito alta para a planta
erro = 2 = umidade do ar muito baixa para a planta
erro = 3 = umidade do solo mt baixa
erro = 4 = erro na maquina*/

SSD1306 display(0x3c, sda, scl);

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(pin_solo,INPUT);
  pinMode(pin_relay, OUTPUT);
  WiFi.softAP(ssid, password);
  //IPAddress IP = WiFi.softAPIP();
  display.init();//inicia o display do oled
  //display.setFont(ArialMT_Plain_16);//define a fonte e o tamanho
  display.clear();//limpa a tela
  server.begin();//inicia o server do esp
  if (! am2315.begin()) {
     Serial.println("Sensor not found, check wiring & pullups!");
     while (1);
  }
  //am2315.begin();
}

void medeUmidadeDoSoloSalvaEmPorcentagem () {
  //funcao que faz a medicao da umidade do solo retorna em porcentagem 
  umidade_solo_porcentagem = map (analogRead (pin_solo),4095,1000,0,100);
  //funcao map tranforma em um valor de 0-100
}

//funcao que retorna o codigo do erro caso encontrado
//se nao tiver erros return 0
void verificaErros () {
  medeUmidadeDoSoloSalvaEmPorcentagem();
  if (umidade_solo_porcentagem <= umidade_solo_minima) {//valor de humidade do solo que a planta acha ruim
    erro = 3;
  }
  if (am2315.readTemperatureAndHumidity(&temperatura,&umidade)) {
    if (temperatura > temp_maxima) {//valor de temperatura que a planta acha ruim
       erro = 1;
    }
    if (umidade < humidade_minima) {//valor de umidade que a planta acha ruim
      erro = 2;
    }
  }else {
    erro = 4;
  }
  erro = 0;
}

void printandoErroNoOled(int erro) {//faz exatamente oq o nome fala =)
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
}  

void loop() {
  /*verificaErros();
  //colocar aki uma funcao q vai mandar esses 3 dados para o app do celular caso o usuario se conecte
  if (erro == 3) {
    digitalWrite(pin_relay, HIGH);
  }else {
    digitalWrite(pin_relay, LOW);
  }
  printandoErroNoOled(erro);
  delay(1000);*/
  if (am2315.readTemperatureAndHumidity(&temperatura,&umidade)) {
    Serial.print("Temp : ");
    Serial.println (temperatura);
    Serial.print("Hum : ");
    Serial.println (umidade);
  }else {
    Serial.println("Nao foi possivel ler");
  }
}
