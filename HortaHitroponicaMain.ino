#include <WiFi.h>//biblioteca para iniciar o esp-webServer
#include <HCAM2315.h>//biblioteca do leitor de umidade e temperatura do ar
#include "SSD1306.h"//biblioteca do Oled

#define I2CADD 0x5C//serve apenas para a leitura do ar dar certo

//pinos usados no esp
#define pin_relay 5
#define pin_solo 36
#define scl_oled 22
#define sda_oled 21

const char* ssid  = "Horta IOT";//nome da rede do esp
const char* password = "Iotehmtbom";//senha

int temperatura,umidade,umidade_solo_porcentagem,erro;
/*significados de cada erro
erro = 1 = temperatura muito alta para a planta
erro = 2 = umidade do ar muito baixa para a planta
erro = 3 = umidade do solo mt baixa
erro = 4 = erro na maquina*/

SSD1306 display(0x3c, sda_oled, scl_oled);

HCAM2315 HCAM2315(I2CADD);

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(pin_solo,INPUT);
  pinMode(pin_relay, OUTPUT);
  WiFi.softAP(ssid, password);
  //IPAddress IP = WiFi.softAPIP();
  display.init();//inicia o display do oled
  //display.setFont(ArialMT_Plain_16);//define a fonte e o tamanho
  display.clear();//limpa a tela
  server.begin();//inicia o server do esp
  HCAM2315.init();//inicia o objeto p leitura do ar
}

void medeUmidadeDoSoloSalvaEmPorcentagem (int *solo) {
  //funcao que faz a medicao da umidade do solo retorna em porcentagem 
  *solo = map (analogRead (pino_sensor_solo),4095,1000,0,100);
  //funcao map tranforma em um valor de 0-100
}

int medeTemperaturaEUmidadeDoAr (int *temperatura,int *umidade) {
  // funcao que faz a leitura da umidade e temperatura do ar
  if(HCAM2315.CheckCRC()){//se a medicao der certo
    *temperatura = HCAM2315.Temp();//salva a temperatura 
    *umidade =  HCAM2315.Hum();//salva a umidade do ar
    return 1;
  }else {
    return 0;//medicao nao deu certo
  }
}
//funcao que retorna o codigo do erro caso encontrado
//se nao tiver erros return 0
int verificaErros (int *temp,int *um_ar,int *um_solo) {
  medeUmidadeDoSoloSalvaEmPorcentagem(um_solo);
  if (*um_solo <= 20) {
    return 3;
  }
  if (medeTemperaturaEUmidadeDoAr(temp,um_ar)) {
    if (*temp > valor de temperatura que a planta acha ruim) {
       return 1;
    }
    if (*um_ar < valor de umidade que a planta acha ruim) {
      return 2;
    }
  }else {
    return 4;
  }

  return 0;
}

void printandoErroNoOled(int erro) {//faz exatamente oq o nome fala =)
  if (erro == 1) {
    display.setFont(ArialMT_Plain_16);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,0,"Temperatura alta\n");
    display.drawString(0,16,"Coloque a\n");
    display.drawString(0,32,"horta num local\n");
    display.drawString(0,48,"Adequado\n");
    display.display();//faz todos os comandos aparecerem no oled
  }
  if (erro == 2) {
    display.setFont(ArialMT_Plain_16);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,0,"Umid. ar baixa\n");
    display.drawString(0,16,"Coloque a\n");
    display.drawString(0,32,"horta num local\n");
    display.drawString(0,48,"Adequado\n");
    display.display();
  }
  if (erro == 3) {
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
    display.setFont(ArialMT_Plain_16);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,0,"Erro no sistema\n");
    display.drawString(0,16,"Alguma coisa esta\n");
    display.drawString(0,32,"Muito errada ae\n");
    display.drawString(0,48,";-;\n");
    display.display();
  }
  if (!erro) {
    display.setFont(ArialMT_Plain_24);
    display.drawRect(0, 0, 128, 64);
    display.drawString(0,16,"Horta saudavel\n");
    //deixar esse print main bonito, de preferencia fazer um desenho sla!
  }
}  

void loop() {
  erro = verificaErros(&temperatura,&umidade,&umidade_solo_porcentagem);
  //colocar aki uma funcao q vai mandar esses 3 dados para o app do celular caso o usuario se conecte
  if (erro == 3) {
    digitalWrite(pin_relay, HIGH);
  }
  if (umidade_solo_porcentagem > valor maximo da umidade) {
    digitalWrite(pin_relay, LOW);
  }
  printandoErroNoOled(erro);
  delay(1000);
  
}
