#define DEBUG

#include <SPI.h>              // SPI
#include <nRF24L01.h>         // RF24
#include <RF24.h>
#include <ESP8266.h> // biblioteca do modulo Wi-Fi
#include <LiquidCrystal.h> // biblioteca para o display LCD

// inicialização do modulo RF24
const int CEpin = 7;
const int CSpin = 53;
RF24 radio(CEpin, CSpin);
const uint64_t pipe = 0xF0F0F0F0E1LL; // tunel de conexao

// constantes de conexao
#define SSID        "rede-wifi"
#define PASSWORD    "senha-rede-wifi"
#define HOST_NAME   "servidor.com"
#define HOST_PORT   (80)

// inicializacao do modulo Wi-Fi
ESP8266 wifi(Serial1, 115200);

// inicializacao do modulo LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// DEFININDO STRUCT DE DADOS
// struct dados sensores
typedef struct{
  int ID;
  int ICOS_INF;
  int ICOS_SUP;
  float NIVEL;
  bool LAT_NEG;
  int LAT_DEG;
  uint16_t LAT_BILLIONTHS;
  bool LNG_NEG;
  int LNG_DEG;
  uint16_t LNG_BILLIONTHS;
  double ELEVACAO;
  uint16_t ANO;
  uint8_t MES;
  uint8_t DIA;
  uint8_t HORA;
  uint8_t MINUTO;
  uint8_t SEGUNDO;
}
S_t;

// declarando as structs que armazenarão os dados
S_t dadosSensores;

void setup()
{
  #ifdef DEBUG
    Serial.begin(57600); // comunicacao Serial com o computador
    Serial.println(F("\r\n----------- PICJr - MOPPE -----------"));
    Serial.println(F("\r\nIniciando Setup..."));
  #endif
  
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);

  // Imprime mensagem de inicialização
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("PICJr MOPPE");
  lcd.setCursor(1, 1);
  lcd.print("Aguardando...");
  
  // inicializa modulo RF24
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();

  // seta modo de operacao como estacao
  #ifdef DEBUG
    if (wifi.setOprToStationSoftAP()) {
      Serial.println(F("para estacao + softap OK"));
    } else {
      Serial.println(F("para estacao + softap ERROR"));
    }
  #else
    wifi.setOprToStationSoftAP();
  #endif
  
  // conecta a rede Wi-Fi especificada
  #ifdef DEBUG
    if(wifi.joinAP(SSID, PASSWORD)) {
      Serial.println(F("conexao OK"));
      Serial.print(F("IP:"));
      Serial.println(wifi.getLocalIP().c_str());       
    } else {
      Serial.println(F("conexao ERROR"));
    }
  #else
    wifi.joinAP(SSID, PASSWORD);
  #endif
  
  // modo de conexao unica
  #ifdef DEBUG
    if (wifi.disableMUX()) {
      Serial.println(F("conexao unica OK"));
    } else {
      Serial.println(F("conexao unica ERROR"));
    }
  #else
    wifi.disableMUX();
  #endif
  
  #ifdef DEBUG
    Serial.println(F("Setup Finalizado!"));
  #endif
} // fecha void setup()

// envia dados GET
void loop()
{
  if(radio.available()){
    bool ok = false;
    
    while(!ok){
      ok = radio.read(&dadosSensores, sizeof(dadosSensores));

      if(ok) {
        exibeNoLCD();
        enviaDados();
        
        #ifdef DEBUG
          Serial.print("dadosSensores.ID = ");
          Serial.println(dadosSensores.ID);
          Serial.print("dadosSensores.ICOS_INF = ");
          Serial.println(dadosSensores.ICOS_INF);
          Serial.print("dadosSensores.ICOS_SUP = ");
          Serial.println(dadosSensores.ICOS_SUP);
          Serial.print("dadosSensores.NIVEL = ");
          Serial.println(dadosSensores.NIVEL);
          Serial.print("dadosSensores.LAT_NEG = ");
          Serial.println(dadosSensores.LAT_NEG);
          Serial.print("dadosSensores.LAT_DEG = ");
          Serial.println(dadosSensores.LAT_DEG);
          Serial.print("dadosSensores.LAT_BILLIONTHS = ");
          Serial.println(dadosSensores.LAT_BILLIONTHS);
          Serial.print("dadosSensores.LNG_NEG = ");
          Serial.println(dadosSensores.LNG_NEG);
          Serial.print("dadosSensores.LNG_DEG = ");
          Serial.println(dadosSensores.LNG_DEG);
          Serial.print("dadosSensores.LNG_BILLIONTHS = ");
          Serial.println(dadosSensores.LNG_BILLIONTHS);
          Serial.print("dadosSensores.ELEVACAO = ");
          Serial.println(dadosSensores.ELEVACAO);
          Serial.print("dadosSensores.ANO = ");
          Serial.println(dadosSensores.ANO);
          Serial.print("dadosSensores.MES = ");
          Serial.println(dadosSensores.MES);
          Serial.print("dadosSensores.DIA = ");
          Serial.println(dadosSensores.DIA);
          Serial.print("dadosSensores.HORA = ");
          Serial.println(dadosSensores.HORA);
          Serial.print("dadosSensores.MINUTO = ");
          Serial.println(dadosSensores.MINUTO);
          Serial.print("dadosSensores.SEGUNDO = ");
          Serial.println(dadosSensores.SEGUNDO);
          Serial.println();
        #endif
      }
    }
  }
  else {
    // caso nao existam mensagens
    #ifdef DEBUG
      //Serial.println(F("NAO HA MENSAGENS DISPOSINIVEIS"));
    #endif
  }
}

String floatToString(float x, byte precision = 2) {
  char tmp[50];
  dtostrf(x, 0, precision, tmp);
  return String(tmp);
}

void exibeNoLCD()
{
  //Limpa a tela
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  if(dadosSensores.ICOS_INF == 1 && dadosSensores.ICOS_SUP == 0) {
    lcd.print("NIVEL NORMAL");
  } else if(dadosSensores.ICOS_INF == 0 && dadosSensores.ICOS_SUP == 0) {
    lcd.print("NIVEL ALERTA");
  } else if(dadosSensores.ICOS_INF == 0 && dadosSensores.ICOS_SUP == 1) {
    lcd.print("NIVEL CRITICO");
  }
  else {
    lcd.print("INDETERMINADO");
  }
  lcd.setCursor(0, 1);
  lcd.print("NIVEL: ");
  lcd.setCursor(8, 1);
  lcd.print(dadosSensores.NIVEL);
}

void enviaDados()
{
  // cria conexao TCP com servidor
  #ifdef DEBUG
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
      Serial.println(F("servidor tcp OK"));
    } else {
      Serial.println(F("servidor tcp ERROR"));
    }
  #else
    wifi.createTCP(HOST_NAME, HOST_PORT);
  #endif
  
  // monta o path do GET
  String elevacao_str = floatToString(dadosSensores.ELEVACAO);
  String nivel_str    = floatToString(dadosSensores.NIVEL);

  // monsta requisicao Http GET
  String temp_request = "GET /moppe-ws/public/index.php/get_insere/";
  temp_request.concat(dadosSensores.ID);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.ICOS_INF);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.ICOS_SUP);
  temp_request.concat("/");
  temp_request.concat(nivel_str);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.LAT_NEG);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.LAT_DEG);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.LAT_BILLIONTHS);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.LNG_NEG);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.LNG_DEG);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.LNG_BILLIONTHS);
  temp_request.concat("/");
  temp_request.concat(elevacao_str);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.ANO);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.MES);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.DIA);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.HORA);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.MINUTO);
  temp_request.concat("/");
  temp_request.concat(dadosSensores.SEGUNDO);
  temp_request.concat(" HTTP/1.1\r\nHost: ");
  temp_request.concat(HOST_NAME);
  temp_request.concat("\r\nUser-Agent: Arduino\r\nConnection: close\r\n\r\n");

  // converte a String para char array
  char httpGET[300];
  temp_request.toCharArray(httpGET, 300);
  
  // realiza a conexao com o servidor e envia os dados
  #ifdef DEBUG
    if (wifi.send((const uint8_t*)httpGET, strlen(httpGET))) {
      Serial.println(F("Dados enviados ao servidor!"));
    } else {
      Serial.println(F("FALHA AO ENVIAR AO SERVIDOR"));
    }
  #else
    wifi.send((const uint8_t*)httpGET, strlen(httpGET));
  #endif
}
