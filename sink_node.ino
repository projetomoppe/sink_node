#include <SPI.h>              // SPI
#include <nRF24L01.h>         // RF24
#include <RF24.h>
#include <ESP8266.h> // biblioteca do modulo Wi-Fi

// constantes de conexao do modulo RF24
const int CEpin = 7;
const int CSpin = 53;

// constantes de conexao
#define SSID        "GUTTER"
#define PASSWORD    "gutter12"
#define HOST_NAME   "edsonboldrini.000webhostapp.com"
#define HOST_PORT   (80)

// inicialização do modulo RF24
RF24 radio(CEpin,CSpin);
const uint64_t pipe = 0xF0F0F0F0E1LL; // tunel de conexao

// inicializacao do modulo Wi-Fi
ESP8266 wifi(Serial1, 115200);

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
  Serial.begin(57600); // comunicacao Serial com o computador
  Serial.println(F("\r\n----------- PICJr - MOPPE -----------"));
  Serial.println(F("\r\nIniciando Setup..."));
  
  // inicializa modulo RF24
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();

  // seta modo de operacao como estacao
  if (wifi.setOprToStationSoftAP()) {
    Serial.println(F("para estacao + softap OK"));
  } else {
    Serial.println(F("para estacao + softap ERROR"));
  }
  
  // conecta a rede Wi-Fi especificada
  if(wifi.joinAP(SSID, PASSWORD)) {
    Serial.println(F("conexao OK"));
    Serial.print(F("IP:"));
    Serial.println(wifi.getLocalIP().c_str());       
  } else {
    Serial.println(F("conexao ERROR"));
  }
  
  // modo de conexao unica
  if (wifi.disableMUX()) {
    Serial.println(F("conexao unica OK"));
  } else {
    Serial.println(F("conexao unica ERROR"));
  }
  
  Serial.println(F("Setup Finalizado!"));
} // fecha void setup()

// envia dados GET
void loop()
{
  if(radio.available()){
    bool ok = false;
    
    while(!ok){
      ok = radio.read(&dadosSensores, sizeof(dadosSensores));

      if(ok) {
        enviaDados();
        
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
      }
    }
  }
  else {
    // caso nao existam mensagens
    // Serial.println(F("NAO HA MENSAGENS DISPOSINIVEIS"));
  }
}

String floatToString(float x, byte precision = 2) {
  char tmp[50];
  dtostrf(x, 0, precision, tmp);
  return String(tmp);
}

void enviaDados()
{
  // cria conexao TCP com servidor
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.println(F("servidor tcp OK"));
  } else {
    Serial.println(F("servidor tcp ERROR"));
  }
  
  // monta o path do GET
  String elevacao_str = floatToString(10.0);
  String nivel_str    = floatToString(23.0);

  // monsta requisicao Http GET
  String temp_request = "GET /moppe-ws/public/index.php/get_insere/";
  temp_request.concat(1);
  temp_request.concat("/");
  temp_request.concat(0);
  temp_request.concat("/");
  temp_request.concat(1);
  temp_request.concat("/");
  temp_request.concat(nivel_str);
  temp_request.concat("/");
  temp_request.concat(1);
  temp_request.concat("/");
  temp_request.concat(20);
  temp_request.concat("/");
  temp_request.concat(1234);
  temp_request.concat("/");
  temp_request.concat(1);
  temp_request.concat("/");
  temp_request.concat(40);
  temp_request.concat("/");
  temp_request.concat(5678);
  temp_request.concat("/");
  temp_request.concat(elevacao_str);
  temp_request.concat("/");
  temp_request.concat(2017);
  temp_request.concat("/");
  temp_request.concat(10);
  temp_request.concat("/");
  temp_request.concat(23);
  temp_request.concat("/");
  temp_request.concat(11);
  temp_request.concat("/");
  temp_request.concat(28);
  temp_request.concat("/");
  temp_request.concat(30);
  temp_request.concat(" HTTP/1.1\r\nHost: ");
  temp_request.concat(HOST_NAME);
  temp_request.concat("\r\nUser-Agent: Arduino\r\nConnection: close\r\n\r\n");

  // converte a String para char array
  char httpGET[300];
  temp_request.toCharArray(httpGET, 300);
  
  // realiza a conexao com o servidor e envia os dados 
  if (wifi.send((const uint8_t*)httpGET, strlen(httpGET))) {
    Serial.println(F("Dados enviados ao servidor!"));
  } else {
    Serial.println(F("FALHA AO ENVIAR AO SERVIDOR"));
  }
  
  // libera a conexao TCP criada
//  if (wifi.releaseTCP()) {
//    Serial.println(F("libera tcp OK"));
//  } else {
//    Serial.println(F("libera tcp ERROR"));
//  }
}
