//SENAI Antônio Ermírio de Moraes
// Vagalumens - Controle de Luminosidade

//Grupo
//Matheus Reis
//André Felipe
//Yasmin Bejarano
//Pedro Henrique
//Murilo Silva

// Biblioteca WIFI para ESP32
#include <WiFi.h>
String onn = "ON";
String barra = "/";
// Bibliotecas para contato com o Firebase no ESP32
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"  // Provide the token generation process info.
#include "addons/RTDBHelper.h"   // Provide the RTDB payload printing info and other helper functions.

// Biblioteca bluetooth serial para ESP32
#include "BluetoothSerial.h"

// Biblioteca para armazenar dados na memória do ESP32
#include <Preferences.h>

// Bibliotecas para display LCD 16x2 I2C
#include <Wire.h>               //Gerencia comunicação I2C
#include <LiquidCrystal_I2C.h>  //LCD 16x2 I2C

// Parâmetros display LCD
#define col 16     // Define o número de colunas do display utilizado
#define lin 2      // Define o número de linhas do display utilizado
#define ende 0x27  // Define o endereço do display

// Set dos pinos botões
#define btmais 12
#define btmenos 13
#define btenter 14
#define btreset 27
#define mabt 33  // Interruptor Offline/Online

// Constantes para leitura LDR
#define LDR_PIN 34
#define MAX_ADC_READING 4095
#define ADC_REF_VOLTAGE 5.0
#define REF_RESISTANCE 3300  // Resistor do divisor de tensão
#define LUX_CALC_SCALAR 12518931
#define LUX_CALC_EXPONENT -1.405

// Variáveis para cálculo LUX
int ldrRawData;
float resistorVoltage, ldrVoltage;
float ldrResistance;
float Lux;

// Definição Bibliotecas
LiquidCrystal_I2C lcd(ende, 16, 2);
BluetoothSerial ESP_BT;
Preferences preferences;

//Composição Nomes formação váriavel endereços Firebase
String nomeambiente = "/Ambiente D";
String nomemodo = "/Modo D";
String nomeluminosidade = "/Luminosidade D";
String lampada1 = " L1";
String lampada2 = " L2";
String lampada3 = " L3";
String lampada4 = " L4";
String endeon1 = "";

// Endereços dados Firebase
String endeambiente1 = "";  // Endereços Ambiente
String endeambiente2 = "";
String endeambiente3 = "";
String endeambiente4 = "";
String endemodo1 = "";  // Endereços Modo
String endemodo2 = "";
String endemodo3 = "";
String endemodo4 = "";
String endeluminosidade1 = "";  // Endereços Luminosidade
String endeluminosidade2 = "";
String endeluminosidade3 = "";
String endeluminosidade4 = "";
String endeluminosidade5 = "";
String endeon = "";

//Váriaveis salvar valor em número do tipo do ambiente para cada lâmpada individualmente
int ambiente1 = 0;
int ambiente2 = 0;
int ambiente3 = 0;
int ambiente4 = 0;

int travageral1 = 0;
// Variáveis WIFI
String receba = "";
String wifi = "";
String senha = "";
String ndispositivo = "";
String ndispositivo1 = "";

// Set das saídas controladoras lâmpadas
const int pin1 = 23;      // Saída lâmpada 1
const int pin2 = 19;      // Saída lâmpada 2
const int pin3 = 18;      // Saída lâmpada 3
const int pin4 = 5;       // Saída lâmpada 4
const int canal1 = 0;     // Canal PWM lâmpada 1
const int canal2 = 1;     // Canal PWM lâmpada 2
const int canal3 = 2;     // Canal PWM lâmpada 3
const int canal4 = 3;     // Canal PWM lâmpada 4
const int freq = 490;     // Frequência de trabalho PWM
const int escalabit = 8;  // Escala 0-255 8bits PWM

//Váriaveis dutyCycle das saídas PWM
int saida1 = 0;
int saida2 = 0;
int saida3 = 0;
int saida4 = 0;

int travabot = 0;
int posion = 1;
int on = 0;
String ons = "";
int travamemoria = 0;
int ndp = 1;
int resetmode = 0;

String lcdambiente1 = "Basic";
String lcdambiente2 = "Basic";
String lcdambiente3 = "Basic";
String lcdambiente4 = "Basic";
String lcdmodo1 = "MS";
String lcdmodo2 = "MS";
String lcdmodo3 = "MS";
String lcdmodo4 = "MS";
int travamillis4 = 0;
int travamillis2 = 0;
int travamillis3 = 0;
int travamillis5 = 0;
// Sistema de Menus modo Offline
int posi = 1;    //Posição do menu
int ntrava = 0;  // Travas do menu
int ntrava2 = 0;
int ntrava3 = 0;
int ntrava4 = 0;
int ntrava5 = 0;
int ntrava6 = 0;
int ntrava7 = 0;
int ntrava8 = 0;
int ntrava9 = 0;
int ntrava10 = 0;
int ntrava11 = 0;
int ntrava12 = 0;
int ntrava13 = 0;
int ntravax = 0;

// Váriaveis Botões
int enter = 0;
int botaomenos;
int botaomais;
int botaoenter;
int travabt;

//Variável do interruptor Online/Offline
int vmabt;

// Pixelart WIFI
byte esq[] = {
  B00100,
  B01001,
  B01011,
  B01011,
  B01001,
  B00101,
  B00001,
  B00000
};
byte dir[] = {
  B00100,
  B10010,
  B11010,
  B11010,
  B10010,
  B10100,
  B10000,
  B00000
};
// Pixelart Seta para a direita
byte seta[] = {
  B00000,
  B01100,
  B00110,
  B11111,
  B11111,
  B00110,
  B01100,
  B00000
};
// Pixelart Seta para a esquerda
byte setainv[] = {
  B00000,
  B00110,
  B01100,
  B11111,
  B11111,
  B01100,
  B00110,
  B00000
};
// Pixelart Bloco Cheio
byte cheio[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
//Pixelart parentêses esquerdo
byte parenesq[] = {
  B00001,
  B00010,
  B00100,
  B00100,
  B00100,
  B00100,
  B00010,
  B00001
};
//Pixelart parentêses direito
byte parendir[] = {
  B10000,
  B01000,
  B00100,
  B00100,
  B00100,
  B00100,
  B01000,
  B10000
};

// Travas modo Online
int travadp = 0;
int travamillis = 0;
int trava1 = 0;
int trava2 = 0;
int trava3 = 0;
int trava4 = 0;
int trava5 = 0;
int trava6 = 0;
int trava7 = 0;
int trava8 = 0;
int trava9 = 0;
int trava10 = 0;
int trava11 = 0;
int trava12 = 0;
int trava13 = 0;
int trava14 = 0;
int trava15 = 0;
int trava16 = 0;
int trava17 = 0;
int trava18 = 0;
int trava19 = 0;
int trava20 = 0;
int trava21 = 0;

// Travas do display
int travad1 = 0;
int travad2 = 0;
int travad3 = 0;
int travad4 = 0;
int travad5 = 0;

// Váriaveis Millis de hora do sistema
unsigned long millis1 = millis();
unsigned long millis2 = millis();
unsigned long millis3 = millis();
unsigned long millis4 = millis();
unsigned long millis7 = millis();


//Login do database do Firebase
#define API_KEY "AIzaSyAJllva87Ll0jVLsd30yjaJw8q4kPXrlPY"
#define USER_EMAIL "vagalumens.proj@gmail.com"
#define USER_PASSWORD "esp32123"
#define DATABASE_URL "https://vagalumens-senai-default-rtdb.firebaseio.com/"

//Funções Firebase
FirebaseData fbdo;
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config;

//Variáveis Vagalumens modo Online e algumas outras
String listenerPath = "";
String mod = "";
String ambient = "";
String luminosidad1 = "";
String luminosidad2 = "";
String luminosidad3 = "";
String luminosidad4 = "";
String luminosidad5 = "";
String wifii = "";
String senhai = "";
String listenerPathi = "";
String deletes = "";
int DeleteD = 0;  // Trava reset geral do sistema
int lumens = 0;
int luminosidade1 = 50;
int luminosidade2 = 50;
int luminosidade3 = 50;
int luminosidade4 = 50;
int luminosidade5 = 50;
int modo = 2;
int ambiente = 1;
int wifis;
int nn = 1;

void setup() {

  // Set e texto inicial comunicação serial (Sempre usar 115200 padrão ESP)
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Vagalumens - Controle de luminosidade - Projeto SENAI");
  Serial.println("");

  // Configuração botões em Entrada PullUP
  pinMode(mabt, INPUT_PULLUP);
  pinMode(btmenos, INPUT_PULLUP);
  pinMode(btmais, INPUT_PULLUP);
  pinMode(btenter, INPUT_PULLUP);
  pinMode(btreset, INPUT_PULLUP);

  // Inicialização do display LCD
  lcd.begin(16, 2);
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Pixelarts set
  lcd.createChar(0, dir);
  lcd.createChar(1, esq);
  lcd.createChar(2, seta);
  lcd.createChar(3, cheio);
  lcd.createChar(4, setainv);
  lcd.createChar(5, parenesq);
  lcd.createChar(6, parendir);

  // Tela de ínico VagaLumens
  lcd.setCursor(3, 0);
  lcd.print("VagaLumens");
  lcd.setCursor(0, 1);
  lcd.print("Inicializando...");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(1, 0);
  lcd.print("Projeto SENAI");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("Build 1.1 - 2023");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  // Set pinos de saída comando PWM
  ledcSetup(canal1, freq, escalabit);
  ledcSetup(canal2, freq, escalabit);
  ledcSetup(canal3, freq, escalabit);
  ledcSetup(canal4, freq, escalabit);
  ledcAttachPin(pin1, canal1);
  ledcAttachPin(pin2, canal2);
  ledcAttachPin(pin3, canal3);
  ledcAttachPin(pin4, canal4);

  // Nome visível do bluetooth do dispositivo
  ESP_BT.begin("Vagalumens Central");

  // Pasta de armazenamendo de dados
  preferences.begin("WF", false);
}

// Função Firebase que executa quando há alteração na pasta do servidor selecionada
void streamCallback(FirebaseStream data) {
  // Altera as variáveis para os valores do servidor

  if (data.dataPath().equals(endeambiente1)) {
    ambient = data.stringData();
    ambiente = ambient.toInt();
    ndp = 1;
    processoambi();
  }
  if (data.dataPath().equals(endeambiente2)) {
    ambient = data.stringData();
    ambiente = ambient.toInt();
    ndp = 2;
    processoambi();
  }
  if (data.dataPath().equals(endeambiente3)) {
    ambient = data.stringData();
    ambiente = ambient.toInt();
    ndp = 3;
    processoambi();
  }
  if (data.dataPath().equals(endeambiente4)) {
    ambient = data.stringData();
    ambiente = ambient.toInt();
    ndp = 4;
    processoambi();
  }

  if (data.dataPath().equals(endemodo1)) {
    mod = data.stringData();
    modo = mod.toInt();
    ndp = 1;
    processomodo();
  }
  if (data.dataPath().equals(endemodo2)) {
    mod = data.stringData();
    modo = mod.toInt();
    ndp = 2;
    processomodo();
  }
  if (data.dataPath().equals(endemodo3)) {
    mod = data.stringData();
    modo = mod.toInt();
    ndp = 3;
    processomodo();
  }
  if (data.dataPath().equals(endemodo4)) {
    mod = data.stringData();
    modo = mod.toInt();
    ndp = 4;
    processomodo();
  }

  if (data.dataPath().equals(endeluminosidade1)) {
    luminosidad1 = data.stringData();
    luminosidade1 = luminosidad1.toInt();
    Serial.print("Luminosidade L1: ");
    Serial.println(luminosidade1);
    ddtrava();
  }

  if (data.dataPath().equals(endeluminosidade2)) {
    luminosidad2 = data.stringData();
    luminosidade2 = luminosidad2.toInt();
    Serial.print("Luminosidade L2: ");
    Serial.println(luminosidade2);
    ddtrava();
  }

  if (data.dataPath().equals(endeluminosidade3)) {
    luminosidad3 = data.stringData();
    luminosidade3 = luminosidad3.toInt();
    Serial.print("Luminosidade L3: ");
    Serial.println(luminosidade3);
    ddtrava();
  }

  if (data.dataPath().equals(endeluminosidade4)) {
    luminosidad4 = data.stringData();
    luminosidade4 = luminosidad4.toInt();
    Serial.print("Luminosidade L4: ");
    Serial.println(luminosidade4);
    ddtrava();
  }
  if (data.dataPath().equals(endeluminosidade5)) {
    luminosidad5 = data.stringData();
    luminosidade5 = luminosidad4.toInt();
    Serial.print("Luminosidade Todas: ");
    Serial.println(luminosidade5);
    ddtrava();
  }













  if (data.dataPath().equals("/DeleteD" + ndispositivo)) {
    deletes = data.stringData();
    DeleteD = deletes.toInt();
  }
  delay (300);
}



void ddtrava() {
  travad2 = 0;
}
//Processo que salva os dados das configurações do ambiente no sua determinada lâmpada
void processoambi() {

  if (ambiente == 1) {
    Serial.print("Ambiente: ");
    Serial.println("padrão");
    if (ndp == 1) {
      lcdambiente1 = "Basic";
    }
    if (ndp == 2) {
      lcdambiente2 = "Basic";
    }
    if (ndp == 3) {
      lcdambiente3 = "Basic";
    }
    if (ndp == 4) {
      lcdambiente4 = "Basic";
    }
  }

  if (ambiente == 2) {
    Serial.print("Ambiente: ");
    Serial.println("computador");
    if (ndp == 1) {
      lcdambiente1 = "PC";
    }
    if (ndp == 2) {
      lcdambiente2 = "PC";
    }
    if (ndp == 3) {
      lcdambiente3 = "PC";
    }
    if (ndp == 4) {
      lcdambiente4 = "PC";
    }
  }

  if (ambiente == 3) {
    Serial.print("Ambiente: ");
    Serial.println("projetor");
    if (ndp == 1) {
      lcdambiente1 = "Proj";
    }
    if (ndp == 2) {
      lcdambiente2 = "Proj";
    }
    if (ndp == 3) {
      lcdambiente3 = "Proj";
    }
    if (ndp == 4) {
      lcdambiente4 = "Proj";
    }
  }

  if (ambiente == 4) {
    Serial.print("Ambiente: ");
    Serial.println("solda");
    if (ndp == 1) {
      lcdambiente1 = "Solda";
    }
    if (ndp == 2) {
      lcdambiente2 = "Solda";
    }
    if (ndp == 3) {
      lcdambiente3 = "Solda";
    }
    if (ndp == 4) {
      lcdambiente4 = "Solda";
    }
  }
  travad2 = 0;
}

void processomodo() {

  if (modo == 2) {
    if (ndp == 1) {
      lcdmodo1 = "MS";
    }
    if (ndp == 2) {
      lcdmodo2 = "MS";
    }
    if (ndp == 3) {
      lcdmodo3 = "MS";
    }
    if (ndp == 4) {
      lcdmodo4 = "MS";
    }

    Serial.print("Modo:");
    Serial.println("Manual L" + ndp);
  }

  if (modo == 3) {
    if (ndp == 1) {
      lcdmodo1 = "AS";
    }
    if (ndp == 2) {
      lcdmodo2 = "AS";
    }
    if (ndp == 3) {
      lcdmodo3 = "AS";
    }
    if (ndp == 4) {
      lcdmodo4 = "AS";
    }
    Serial.print("Modo:");
    Serial.println("Auto L" + ndp);
  }
  travad2 = 0;
}


void pathing() {
  //Composição endereço Ambiente da Lâmpada 1
  endeambiente1 = nomeambiente + ndispositivo;
  endeambiente1 = endeambiente1 + lampada1;
  //Composição endereço Ambiente da Lâmpada 2
  endeambiente2 = nomeambiente + ndispositivo;
  endeambiente2 = endeambiente2 + lampada2;
  //Composição endereço Ambiente da Lâmpada 3
  endeambiente3 = nomeambiente + ndispositivo;
  endeambiente3 = endeambiente3 + lampada3;
  //Composição endereço Ambiente da Lâmpada 4
  endeambiente4 = nomeambiente + ndispositivo;
  endeambiente4 = endeambiente4 + lampada4;
  //Composição endereço Modo da Lâmpada 1
  endemodo1 = nomemodo + ndispositivo;
  endemodo1 = endemodo1 + lampada1;
  //Composição endereço Modo da Lâmpada 2
  endemodo2 = nomemodo + ndispositivo;
  endemodo2 = endemodo2 + lampada2;
  //Composição endereço Modo da Lâmpada 3
  endemodo3 = nomemodo + ndispositivo;
  endemodo3 = endemodo3 + lampada3;
  //Composição endereço Modo da Lâmpada 4
  endemodo4 = nomemodo + ndispositivo;
  endemodo4 = endemodo4 + lampada4;
  //Composição endereço Luminosidade da Lâmpada 1
  endeluminosidade1 = nomeluminosidade + ndispositivo;
  endeluminosidade1 = endeluminosidade1 + lampada1;
  //Composição endereço Luminosidade da Lâmpada 2
  endeluminosidade2 = nomeluminosidade + ndispositivo;
  endeluminosidade2 = endeluminosidade2 + lampada2;
  //Composição endereço Luminosidade da Lâmpada 3
  endeluminosidade3 = nomeluminosidade + ndispositivo;
  endeluminosidade3 = endeluminosidade3 + lampada3;
  //Composição endereço Luminosidade da Lâmpada 4
  endeluminosidade4 = nomeluminosidade + ndispositivo;
  endeluminosidade4 = endeluminosidade4 + lampada4;
  endeon = onn + ndispositivo;
  endeon1 = barra + onn;
  endeon1 = endeon1 + ndispositivo;
}







//Reconecta ao Firebase servidor caso tenha caído a conexão
void streamTimeoutCallback(bool timeout) {
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void loop() {

  // Ler valores dos Botões
  botaomenos = !digitalRead(btmenos);
  botaomais = !digitalRead(btmais);
  botaoenter = !digitalRead(btenter);

  //Ler interruptor Manual/Automático
  vmabt = !digitalRead(mabt);

  //Ler botão de HARDRESET
  resetmode = !digitalRead(btreset);

  // Função de HARD RESET ESP Padrão Fábrica
  if (DeleteD == 2 || resetmode == 1) {
    preferences.remove("wifis");
    preferences.remove("senhas");
    preferences.remove("path");
    preferences.remove("dispon");
    ESP.restart();
  }

  //Tempo de Leitura para atualização do valor Sensor Luminosidade Interno
  if (travamillis == 0) {
    millis3 = millis();
    travamillis = 1;
  }
  if ((millis() - millis3) > 500) {
    //Cálculos Matemáticos para converter o valor de leitura do pino do LDR na quantidade de LUX efetiva (precisa +- 20%)
    ldrRawData = analogRead(LDR_PIN);                                                          // Lê o valor entre 0-1023 do pino do ESP do LDR INTERNO
    resistorVoltage = (float)ldrRawData / MAX_ADC_READING * ADC_REF_VOLTAGE;                   // Calcula a tensão em cima do resistor
    ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;                                            // Calcula a tensão em cima do LDR
    ldrResistance = ldrVoltage / resistorVoltage * REF_RESISTANCE;                             // Calcula a resistência do LDR de acordo com a voltagem
    Lux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);                             // Converte os calculos no valor em LUX
    Firebase.RTDB.setInt(&fbdo, String(listenerPath + "/LUX Central D" + ndispositivo), Lux);  // Envia a informação para o servidor
    travamillis = 0;
  }

  //Modo Online do dispositivo
  if (vmabt == 0) {
    posi = 1;
    ntrava = 0;
    ntrava2 = 0;
    ntrava3 = 0;
    ntrava4 = 0;
    ntrava5 = 0;
    ntrava6 = 0;
    ntrava7 = 0;
    ntrava8 = 0;
    ntrava9 = 0;
    ntrava10 = 0;
    ntrava11 = 0;
    if (travamemoria == 0) {
      // Ler dados do Wifi salvos na memória (caso não exista retorna "")
      wifii = preferences.getString("wifis", "");
      senhai = preferences.getString("senhas", "");
      listenerPathi = preferences.getString("path", "");
      ndispositivo1 = preferences.getString("dispon", "");
      //Verificar se os dados da memória existem ou não, caso existam salvar nas determinadas váriaveis
      if (wifii == "" || senhai == "" || listenerPathi == "" || ndispositivo1 == "") {

      } else {
        //Número do dispositivo de 1-4
        ndispositivo = ndispositivo1;

        //Chama a função de endereçamento
        pathing();

        //SSID Wifi
        wifi = wifii;
        //Senha Wifi
        senha = senhai;
        //Pasta do Usuário Vagalumens no Firebase
        listenerPath = listenerPathi;
        //Autorizando travas que fazem parte da configuração inicial desnecessária
        travamemoria = 1;
      }
    }
    if (botaomenos == 1 && botaomais == 0 && botaoenter == 0) {
      if (travabot == 0) {
        if (posion == 1) {
          posion = 4;
        } else {
          posion = posion - 1;
        }
        travad2 = 0;
        travabot = 1;
      }
    }
    if (botaomenos == 0 && botaomais == 1 && botaoenter == 0) {
      if (travabot == 0) {
        if (posion == 4) {
          posion = 1;
        } else {
          posion = posion + 1;
        }
        travad2 = 0;
        travabot = 1;
      }
    }
    if (botaomenos == 0 && botaomais == 0 && botaoenter == 0) {
      travabot = 0;
    }





    //Tela LCD Inicial com WIFI conectado
    if (trava6 == 1) {
      if (travad3 == 0) {

        lcd.clear();
        delay(100);
        lcd.setCursor(4, 1);  // Símbolo Esquerdo WIFI
        lcd.write(1);
        lcd.setCursor(5, 1);  // Símbolo Direito WIFI
        lcd.write(0);
        lcd.setCursor(15, 1);  // Seta direita
        lcd.write(2);
        lcd.setCursor(13, 0);  // Parênteses esquerdo
        lcd.write(5);
        lcd.setCursor(15, 0);  // Parênteses direito
        lcd.write(6);
        lcd.setCursor(0, 1);  // Seta esquerda
        lcd.write(4);
        lcd.setCursor(2, 1);
        lcd.print("On");
        travad3 = 1;
      }
      if (travad2 == 0) {
        if (posion == 1) {

          lcd.setCursor(14, 0);  // Número da lâmpada das informações em exibição
          lcd.print("1");
          //Informações Modo
          lcd.setCursor(0, 0);
          lcd.print("M:");
          lcd.setCursor(2, 0);
          lcd.print(lcdmodo1);
          //Informações Ambiente
          lcd.setCursor(7, 0);
          lcd.print("      ");
          lcd.setCursor(5, 0);
          lcd.print("A:");
          lcd.setCursor(7, 0);
          lcd.print(lcdambiente1);
          //Informações Luminosidade
          lcd.setCursor(10, 1);
          lcd.print("   ");
          lcd.setCursor(8, 1);
          lcd.print("L:");
          lcd.setCursor(10, 1);
          lcd.print(luminosidade1);
          lcd.setCursor(13, 1);
          lcd.print("%");
        }
        if (posion == 2) {
          lcd.setCursor(14, 0);  // Número da lâmpada das informações em exibição
          lcd.print("2");
          //Informações Modo
          lcd.setCursor(0, 0);
          lcd.print("M:");
          lcd.setCursor(2, 0);
          lcd.print(lcdmodo2);
          //Informações Ambiente
          lcd.setCursor(7, 0);
          lcd.print("     ");
          lcd.setCursor(5, 0);
          lcd.print("A:");
          lcd.setCursor(7, 0);
          lcd.print(lcdambiente2);
          //Informações Luminosidade
          lcd.setCursor(10, 1);
          lcd.print("    ");
          lcd.setCursor(8, 1);
          lcd.print("L:");
          lcd.setCursor(10, 1);
          lcd.print(luminosidade2);
          lcd.setCursor(13, 1);
          lcd.print("%");
        }

        if (posion == 3) {
          lcd.setCursor(14, 0);  // Número da lâmpada das informações em exibição
          lcd.print("3");
          //Informações Modo
          lcd.setCursor(0, 0);
          lcd.print("M:");
          lcd.setCursor(2, 0);
          lcd.print(lcdmodo3);
          //Informações Ambiente
          lcd.setCursor(7, 0);
          lcd.print("     ");
          lcd.setCursor(5, 0);
          lcd.print("A:");
          lcd.setCursor(7, 0);
          lcd.print(lcdambiente3);
          //Informações Luminosidade
          lcd.setCursor(10, 1);
          lcd.print("    ");
          lcd.setCursor(8, 1);
          lcd.print("L:");
          lcd.setCursor(10, 1);
          lcd.print(luminosidade3);
          lcd.setCursor(13, 1);
          lcd.print("%");
        }

        if (posion == 4) {
          lcd.setCursor(14, 0);  // Número da lâmpada das informações em exibição
          lcd.print("4");
          //Informações Modo
          lcd.setCursor(0, 0);
          lcd.print("M:");
          lcd.setCursor(2, 0);
          lcd.print(lcdmodo4);
          //Informações Ambiente
          lcd.setCursor(7, 0);
          lcd.print("     ");
          lcd.setCursor(5, 0);
          lcd.print("A:");
          lcd.setCursor(7, 0);
          lcd.print(lcdambiente4);
          //Informações Luminosidade
          lcd.setCursor(10, 1);
          lcd.print("    ");
          lcd.setCursor(8, 1);
          lcd.print("L:");
          lcd.setCursor(10, 1);
          lcd.print(luminosidade4);
          lcd.setCursor(13, 1);
          lcd.print("%");
        }
        travad2 = 1;
        travad1 = 0;
      }
    }

    // Tela LCD aparelho sem wifi na memória
    if (trava1 == 0 && travamemoria == 0) {
      if (travad1 == 0) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Desconectado");
        lcd.setCursor(0, 1);
        lcd.print("Link WIFI c/ APP");
        travad1 = 1;
        travad2 = 0;
      }
    }

    //Revalída token Firebase server se ocorrer erros
    if (trava6 == 1) {
      if (Firebase.isTokenExpired()) {
        Firebase.refreshToken(&config);
        Serial.println("Refresh token");
      }
    }

    //Inicializar Firebase
    if (trava5 == 1 && trava6 == 0) {
      auth.user.email = USER_EMAIL;
      auth.user.password = USER_PASSWORD;
      config.api_key = API_KEY;
      config.database_url = DATABASE_URL;
      config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
      config.max_token_generation_retry = 5;
      Firebase.reconnectWiFi(true);
      Firebase.begin(&config, &auth);
      if (!Firebase.RTDB.beginStream(&stream, listenerPath.c_str()))
        Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());
      Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
      //Salvar dados do WIFI na memória do ESP
      preferences.putString("wifis", wifi);
      preferences.putString("senhas", senha);
      preferences.putString("path", listenerPath);
      preferences.putString("dispon", ndispositivo);
      pathing();
      trava6 = 1;
    }

    //Inicializar WIFI
    if (trava4 == 1 && trava5 == 0 || travamemoria == 1 && trava5 == 0) {
      WiFi.begin(wifi, senha);  //Conecta WIFI com SSID e Senhas salvos em váriaveis
      Serial.println("");
      Serial.print("-> Conectando no Wifi..");  //Informação em processo de conectar no WIFI no Serial
      lcd.clear();                              //Informação em processo de conectar WIFI no LCD
      delay(100);
      lcd.setCursor(0, 0);
      lcd.write(2);
      lcd.setCursor(1, 0);
      lcd.print("Conectando WIFI");
      lcd.setCursor(0, 1);
      lcd.print("|              |");

      while (WiFi.status() != WL_CONNECTED) {

        Serial.print(".");  //Pontos carregamento Serial

        lcd.setCursor(nn, 1);  //Sistema de barras carregamento LCD
        lcd.print("/");
        nn = nn + 1;
        if (nn == 15) {
          lcd.setCursor(0, 1);
          lcd.print("|              |");
          nn = 1;
        }
        delay(500);  //Uma barra a mais a cada meio segundo, quanto chegar ao limite da tela reseta
      }
      Serial.println("");
      trava5 = 1;
    }




    //Receber dados do bluetooth para configuração inicial WIFI
    if (travamemoria == 0) {
      if (ESP_BT.available()) {
        //Salva a informação recebida no bluetooth como uma váriavel
        receba = ESP_BT.readStringUntil('\n');
        //Salva os dados lidos no bluetooth como Usuário Firebase
        if (trava3 == 1 && trava4 == 0) {
          listenerPath = receba;
          Serial.print("Usuário: ");  //Informação Usuário Firebase no Serial
          Serial.println(listenerPath);
          lcd.clear();
          delay(100);
          lcd.setCursor(0, 0);
          lcd.print("User:");
          lcd.setCursor(6, 0);
          lcd.print(listenerPath);
          delay(3000);
          trava4 = 1;
        }
        //Salva os dado lidos no bluetooth como Senha WIFI
        if (trava2 == 1 && trava3 == 0) {
          senha = receba;
          lcd.clear();  //Tela WIFI Senha no LCD (Com informação SSID e Senha)
          delay(100);
          lcd.setCursor(1, 0);
          lcd.print("WIFI:");
          lcd.setCursor(7, 0);
          lcd.print(wifi);
          lcd.setCursor(0, 1);
          lcd.print("Senha:");
          lcd.setCursor(7, 1);
          lcd.print(senha);
          Serial.print("Senha: ");  //Informação Senha WIFI no Serial
          Serial.println(senha);
          trava3 = 1;
        }
        //Salvar os dados lidos no bluetooth como SSID WIFI
        if (trava1 == 1 && trava2 == 0) {
          wifi = receba;
          lcd.clear();  //Tela WIFI Senha no LCD (Com informação SSID)
          delay(100);
          lcd.setCursor(1, 0);
          lcd.print("WIFI:");
          lcd.setCursor(7, 0);
          lcd.print(wifi);
          lcd.setCursor(0, 1);
          lcd.print("Senha:");
          Serial.print("WIFI: ");  //Informação SSID WIFI no Serial
          Serial.println(wifi);
          trava2 = 1;
        }
        //Salva os dados lidos no bluetooth como número do dispositivo
        if (trava1 == 0) {
          ndispositivo = receba;
          trava1 = 1;
          Serial.print("-> Sincronizando login via Bluetooth");  //Informação número do dispositivo no Serial
          Serial.println("");
          Serial.print("Number Device: ");
          Serial.println(ndispositivo);
        }
      }





      if (trava1 == 1 && trava4 == 0) {
        if (trava12 == 0) {
          millis1 = millis();
          trava12 = 1;
        }
        if ((millis() - millis1) > 600) {
          lcd.setCursor(16, 1);
          lcd.scrollDisplayLeft();
          trava12 = 0;
        }
      }
    }
  }








































  //Modo OFFLINE de operação
  if (vmabt == 1) {

    // Caso botão menos pressionado
    if (botaomenos == 1 && botaomais == 0 && botaoenter == 0) {
      if (posi >= 7 && posi <= 10) {
        if (travabt == 0) {
          if (posi == 7) {
            posi = 10;
            ntravax = 0;
            travabt = 1;
          }
        }
        if (travabt == 0) {
          if (posi == 8 || posi == 9 || posi == 10) {
            posi = posi - 1;
            ntravax = 0;
            travabt = 1;
          }
        }
      }


      if (posi == 5) {
        if (travabt == 0) {
          posi = 6;
          travabt = 1;
        }
      }
      if (posi == 6) {
        if (travabt == 0) {
          posi = 5;
          travabt = 1;
        }
      }

      if (posi == 4) {
        if (ntrava3 == 0) {
          millis4 = millis();
          ntrava3 = 1;
        }
        if ((millis() - millis4) > 100) {
          if (luminosidade1 > 0) {
            luminosidade1 = luminosidade1 - 1;
          }
          ntrava3 = 0;
        }
      }
      if (posi < 4) {
        if (travabt == 0) {
          if (posi == 1) {
            posi = 3;
            travabt = 1;
          }
        }
        if (travabt == 0) {
          if (posi == 2 || posi == 3) {
            posi = posi - 1;
            travabt = 1;
          }
        }
      }
    }

    if (botaomenos == 0 && botaomais == 1 && botaoenter == 0) {
      if (posi >= 7 && posi <= 10) {
        if (travabt == 0) {
          if (posi == 10) {
            posi = 7;
            ntravax = 0;
            travabt = 1;
          }
        }
        if (travabt == 0) {
          if (posi == 7 || posi == 8 || posi == 9) {
            posi = posi + 1;
            ntravax = 0;
            travabt = 1;
          }
        }
      }








      if (posi == 5) {
        if (travabt == 0) {
          posi = 6;
          travabt = 1;
        }
      }
      if (posi == 6) {
        if (travabt == 0) {
          posi = 5;
          travabt = 1;
        }
      }

      if (posi == 4) {
        if (ntrava3 == 0) {
          millis4 = millis();
          ntrava3 = 1;
        }
        if ((millis() - millis4) > 100) {
          if (luminosidade1 < 100) {
            luminosidade1 = luminosidade1 + 1;
          }
          ntrava3 = 0;
        }
      }

      if (posi < 4) {
        if (travabt == 0) {
          if (posi == 3) {
            posi = 1;
            travabt = 1;
          }
        }
        if (travabt == 0) {
          if (posi == 1 || posi == 2) {
            posi = posi + 1;
            travabt = 1;
          }
        }
      }
    }
    if (botaomenos == 0 && botaomais == 0 && botaoenter == 0) {
      travabt = 0;
      enter = 0;
    }

    if (botaomenos == 0 && botaomais == 0 && botaoenter == 1) {
      enter = 1;
      ntravax = 0;
    }
    travad1 = 0;
    trava1 = 0;
    trava2 = 0;
    trava3 = 0;
    trava4 = 0;
    trava5 = 0;
    trava6 = 0;
    trava7 = 0;
    trava8 = 0;
    trava9 = 0;
    trava10 = 0;
    trava11 = 0;
    trava12 = 0;
    trava13 = 0;
    trava14 = 0;
    trava20 = 0;
    trava21 = 0;
    if (ntrava == 0) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("M:");
      if (ambiente == 1) {
        lcd.setCursor(12, 0);
        lcd.print("BS");
      }
      if (ambiente == 2) {
        lcd.setCursor(12, 0);
        lcd.print("PC");
      }
      if (ambiente == 3) {
        lcd.setCursor(12, 0);
        lcd.print("Proj");
      }
      if (ambiente == 5) {
        lcd.setCursor(12, 0);
        lcd.print("Ler");
      }
      if (modo == 2) {
        lcd.setCursor(3, 0);
        lcd.print("Manual");
      }
      if (modo == 3) {
        lcd.setCursor(3, 0);
        lcd.print("Auto");
        lcd.setCursor(7, 0);
        lcd.print("   ");
      }
      lcd.setCursor(10, 0);
      lcd.print("A:");
      lcd.setCursor(10, 1);
      lcd.print("L:");
      lcd.setCursor(1, 1);
      lcd.print("Offline");
      lcd.setCursor(0, 0);
      lcd.write(2);
      lcd.setCursor(14, 1);
      lcd.print("%");
      posi = 1;
      ntrava = 1;
    }
    if (posi < 4) {
      lcd.setCursor(12, 1);
      lcd.print(luminosidade1);
      if (luminosidade1 < 10) {
        lcd.setCursor(13, 1);
        lcd.print(" ");
      }
    }


    if (posi == 1) {
      ntravax = 0;
      if (enter == 1 && travabt == 0) {
        posi = 5;
        travabt = 1;
      }
      lcd.setCursor(0, 0);
      lcd.write(2);
      lcd.setCursor(9, 0);
      lcd.print(" ");
      lcd.setCursor(9, 1);
      lcd.print(" ");
    }
    if (posi == 2) {
      if (enter == 1 && travabt == 0) {
        posi = 7;
        travabt = 1;
      }
      lcd.setCursor(9, 0);
      lcd.write(2);
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.setCursor(9, 1);
      lcd.print(" ");
    }
    if (posi == 3) {
      if (enter == 1 && travabt == 0) {
        posi = 4;
        travabt = 1;
      }
      lcd.setCursor(9, 1);
      lcd.write(2);
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.setCursor(9, 0);
      lcd.print(" ");
    }
    if (posi == 4) {
      if (ntrava2 == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Luminosidade");
        lcd.setCursor(13, 0);
        lcd.print(luminosidade1);
        lcd.setCursor(15, 0);
        lcd.print("%");
        lcd.setCursor(2, 1);
        lcd.print("-");
        lcd.setCursor(13, 1);
        lcd.print("+");
        ntrava2 = 1;
      }
      lcd.setCursor(13, 0);
      lcd.print(luminosidade1);
      if (luminosidade1 < 10) {
        lcd.setCursor(14, 0);
        lcd.print(" ");
      }
      if (luminosidade1 > 99) {
        lcd.setCursor(16, 0);
        lcd.print("%");
      }
      if (luminosidade1 < 99) {
        lcd.setCursor(16, 0);
        lcd.print(" ");
        lcd.setCursor(15, 0);
        lcd.print("%");
      }

      if (enter == 1 && travabt == 0) {
        ntrava2 = 0;
        ntrava = 0;
        travabt = 1;
        posi = 1;
      }


      if (luminosidade1 > 0) {
        lcd.setCursor(3, 1);
        lcd.write(3);

        if (luminosidade1 > 19) {
          lcd.setCursor(4, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(4, 1);
          lcd.print(" ");
        }

        if (luminosidade1 > 29) {
          lcd.setCursor(5, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(5, 1);
          lcd.print(" ");
        }

        if (luminosidade1 > 39) {
          lcd.setCursor(6, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(6, 1);
          lcd.print(" ");
        }

        if (luminosidade1 > 49) {
          lcd.setCursor(7, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(7, 1);
          lcd.print(" ");
        }

        if (luminosidade1 > 59) {
          lcd.setCursor(8, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(8, 1);
          lcd.print(" ");
        }

        if (luminosidade1 > 69) {
          lcd.setCursor(9, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(9, 1);
          lcd.print(" ");
        }

        if (luminosidade1 > 79) {
          lcd.setCursor(10, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(10, 1);
          lcd.print(" ");
        }

        if (luminosidade1 > 89) {
          lcd.setCursor(11, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(11, 1);
          lcd.print(" ");
        }
        if (luminosidade1 > 99) {
          lcd.setCursor(12, 1);
          lcd.write(3);
        } else {
          lcd.setCursor(12, 1);
          lcd.print(" ");
        }

      } else {
        lcd.setCursor(3, 1);
        lcd.print("          ");
      }
    }

    if (posi == 5) {
      if (ntrava4 == 0) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Manual");
        lcd.setCursor(1, 1);
        lcd.print("Auto");
        ntrava4 = 1;
      }

      lcd.setCursor(0, 0);
      lcd.write(2);
      lcd.setCursor(0, 1);
      lcd.print(" ");
      if (enter == 1 && travabt == 0) {
        ntrava4 = 0;
        ntrava = 0;
        travabt = 1;
        posi = 1;
        modo = 2;
      }
    }

    if (posi == 6) {

      lcd.setCursor(0, 1);
      lcd.write(2);
      lcd.setCursor(0, 0);
      lcd.print(" ");
      if (enter == 1 && travabt == 0) {
        ntrava4 = 0;
        ntrava = 0;
        travabt = 1;
        posi = 1;
        modo = 3;
      }
    }




    if (posi == 7) {
      if (ntravax == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(1, 0);
        lcd.print("Basic");
        lcd.setCursor(1, 1);
        lcd.print("Computador");
        ntravax = 1;
      }
      if (enter == 1 && travabt == 0) {
        ambiente = 1;
        posi = 1;
        travabt = 1;
        ntrava = 0;
      }
    }




    if (posi == 8) {
      if (ntravax == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.write(2);
        lcd.setCursor(1, 0);
        lcd.print("Basic");
        lcd.setCursor(1, 1);
        lcd.print("Computador");
        ntravax = 1;
      }
      if (enter == 1 && travabt == 0) {
        ambiente = 2;
        posi = 1;
        travabt = 1;
        ntrava = 0;
      }
    }





    if (posi == 9) {
      if (ntravax == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.setCursor(1, 0);
        lcd.print("Projetor");
        lcd.setCursor(1, 1);
        lcd.print("Leitura");
        ntravax = 1;
      }
      if (enter == 1 && travabt == 0) {
        ambiente = 3;
        posi = 1;
        travabt = 1;
        ntrava = 0;
      }
    }





    if (posi == 10) {
      if (ntravax == 0) {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.write(2);
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(1, 0);
        lcd.print("Projetor");
        lcd.setCursor(1, 1);
        lcd.print("Leitura");
        ntravax = 1;
      }
      if (enter == 1 && travabt == 0) {
        ambiente = 5;
        posi = 1;
        travabt = 1;
        ntrava = 0;
      }
    }
  }











  if (travamillis4 == 0) {
    millis2 = millis();
    travamillis4 = 1;
  }
  if ((millis() - millis2) > 500) {
    travageral1 = 0;
    travamillis4 = 0;
  }
  if (travageral1 == 0) {
    if (lcdmodo1 == "AS") {
      if (lcdambiente1 == "Basic") {
        if (Lux < 500) {
          if (luminosidade1 < 100) {
            luminosidade1 = luminosidade1 + 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
            lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(14, 1);
            lcd.print("%");
          }
        }
        if (Lux > 750) {
          if (luminosidade1 > 0) {
            luminosidade1 = luminosidade1 - 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
            lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
      }

      if (lcdambiente1 == "Ler") {
        if (Lux < 650) {
          if (luminosidade1 < 100) {
            luminosidade1 = luminosidade1 + 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
            lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
        if (Lux > 850) {
          if (luminosidade1 > 0) {
            luminosidade1 = luminosidade1 - 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
            lcd.setCursor(12, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
      }

      if (lcdambiente1 == "Solda") {
        if (Lux < 900) {
          if (luminosidade1 < 100) {
            luminosidade1 = luminosidade1 + 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
            lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
        if (Lux > 1100) {
          if (luminosidade1 > 0) {
            luminosidade1 = luminosidade1 - 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
          lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
      }

      if (lcdambiente1 == "Proj") {
        if (Lux < 50) {
          if (luminosidade1 < 100) {
            luminosidade1 = luminosidade1 + 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
            lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
        if (Lux > 300) {
          if (luminosidade1 > 0) {
            luminosidade1 = luminosidade1 - 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
            lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
      }
      if (lcdambiente1 == "PC") {
        if (Lux < 300) {
          if (luminosidade1 < 100) {
            luminosidade1 = luminosidade1 + 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
            saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
          lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
        if (Lux > 500) {
          if (luminosidade1 > 0) {
            luminosidade1 = luminosidade1 - 1;
            saida1 = map(luminosidade1, 0, 100, 0, 255);
             saida2 = map(luminosidade1, 0, 100, 0, 255);
            ledcWrite(canal1, saida1);
          lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(10, 1);
            lcd.print(luminosidade1);
            lcd.setCursor(13, 1);
            lcd.print("%");
          }
        }
      }
    }
    travageral1 = 1;
  }









  if (trava20 == 0) {
    millis7 = millis();
    trava20 = 1;
  }
  if ((millis() - millis7) > 500) {




    //Modo Manual de set luminosidade da lâmpada
    if (modo == 2) {

      saida1 = map(luminosidade1, 0, 100, 0, 255);
      ledcWrite(canal1, saida1);

      saida2 = map(luminosidade2, 0, 100, 0, 255);
      ledcWrite(canal2, saida2);

      saida3 = map(luminosidade3, 0, 100, 0, 255);
      ledcWrite(canal3, saida3);

      saida4 = map(luminosidade4, 0, 100, 0, 255);
      ledcWrite(canal4, saida4);
    }
    trava20 = 0;
  }
}
