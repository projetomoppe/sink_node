#include <SPI.h>              // SPI
#include <NewPing.h>         // Sensor ultrassonico
#include <TinyGPS++.h>       // Modulo GPS
#include <ESP8266.h>         // Modulo WeeWiFiESP8266
#include <SdFat.h>           // Modulo Cartão SD

// definindo as constantes do programa
// ID DO DISPOSITIVO
const char ID_dispositivo[2] = "01";  // ID do pacote enviado

// SENSORES ICOS
const int S1 = 4; // pinos sensores ICOS
const int S2 = 3;

// SENSOR ULTRASSONICO
const int ECHO = 12; // pinos sensor ultrassonico
const int TRIG = 13;
const float REF = 35.0; // referencia de altura de instalacao do sensor ultrassonico

// MODULO GPS
static const uint32_t GPSB = 9600; // definindo a velocidade de comunicação do módulo GPS

// MODULO ESP8266
#define SSID     "moppe_wireless"
#define PASSWORD "Moppe123JAC12"

// MODULO SD CARD
SdFat sdCard;
const int pinSD = 6;

// nome dos arquivos
#define LOG_FILE_PREFIX "log"
#define MAX_LOG_FILES 100
#define LOG_FILE_SUFIX "csv"
char logNomeArquivo[10];

// colunas do arquivo
#define LOG_COLUMN_COUNT 8
char* logColunas[LOG_COLUMN_COUNT] = {
  "ID", "ICOS Inferior", "ICOS Superior", "Nivel Ultrassonico", "Latitude", "Longitude", "Data", "Hora"
};

// controle de rate de log
const unsigned long LOG_RATE = 3000;
unsigned long ultimoLog = 0;

// definindo variaveis globais do programa
float nivel = 0.0;
int icos_inf;
int icos_sup;

// inicialização dos objetos
TinyGPSPlus gps;
NewPing us(TRIG, ECHO);
ESP8266 wifi(Serial1, 115200);

void setup()
{ 
  Serial.begin(115200); // comunicacao Serial com o computador
  Serial3.begin(GPSB);  // modulo GPS

  // define os sensores ICOS como input
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);

  Serial.println(F("\r\n----------- PICJr - MOPPE -----------"));
  Serial.println(F("------- Programa inicializado -------\r\n"));

  Serial.println(F("Iniciando Setup"));
  
  if(wifi.kick())
    Serial.println(F("Modulo ESP8266 iniciado..."));
  else{
    Serial.println(F("falha em wifi.kick..."));
  }
  
  if(wifi.restart())
    Serial.println(F("Modulo ESP8266 reiniciado..."));
  else{
    Serial.println(F("falha em wifi.restart..."));
    Serial.println(F("tentando novamente..."));
    wifi.restart();
  }
    
  // especifica modo de operacao
  if (wifi.setOprToStation()) {
      Serial.println(F("Modo Conecta a WiFi: OK"));
  } else {
      Serial.println(F("Modo Conecta a WiFi: ERRO"));
  }

  // conecta ao Ponto de Acesso informado
  if (wifi.joinAP(SSID, PASSWORD)) {
      Serial.println(F("Conectado ao Ponto de Acesso: OK\r\n"));
      Serial.print(F("IP: "));
      Serial.println(wifi.getLocalIP().c_str());    
  } else {
      Serial.println(F("Conectado ao Ponto de Acesso: ERRO"));
  }

  // habilita multiplas conexoes
  if (wifi.enableMUX()) {
      Serial.println(F("Multiplas conexoes: OK"));
  } else {
      Serial.println(F("Multiplas conexoes: ERRO"));
  }

  // inicializa servidor TCP na porta 8090
  if (wifi.startTCPServer(8090)) {
      Serial.println(F("Inicializa servicor TCP: OK"));
  } else {
      Serial.println(F("Inicializa servicor TCP: ERRO"));
  }

  // define timeout do servidor
  if (wifi.setTCPServerTimeout(10)) { 
      Serial.println(F("TimeOut do servidro TCP: 10 segundos"));
  } else {
      Serial.println(F("TimeOut do servidro TCP: ERRO"));
  }

  // Inicializa o modulo SD
  if(!sdCard.begin(pinSD, SPI_HALF_SPEED))
    sdCard.initErrorHalt();

  // cria novo arquivo a cada inicializacao
  updateFileName(); // cria um novo arquivo a cada inicializacao
  printHeader(); // imprime o cabecalho do arquivo
  
  Serial.println(F("Setup Finalizado"));
} // fecha void setup()

void loop()
{ 
  // obtencao dos dados
  icos_inf     = digitalRead(S1); // leitura do sensor ICOS inferior
  icos_sup     = digitalRead(S2); // leitura do sensor ICOS superior
  nivel        = dados_su();      // obtencao do nivel dado pelo sensor ultrassonico
  dados_gps();                    // alimenta o objeto gps

  if((ultimoLog + LOG_RATE) <= millis()) // Se  LOG_RATE em  milissegundos desde o último registro
  {
    if (logData(ID_dispositivo, icos_inf, icos_sup, nivel)) // Registrar os dados do GPS
    {
      Serial.println("Dados Logados!"); //mostratr essa mensagem
      ultimoLog = millis(); // atualizar a variavel
    }
    else // se nao atualizou
    {
      Serial.println("Falha no log de dados."); // sera mostrado uma mensagem de erro no GPS
    }
  }

  // envia dados
  if ((gps.location.isValid()) && (gps.date.isValid()) && (gps.time.isValid()))
  {    
    String webpage = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n";
      webpage += "<!DOCTYPE html lang=\"pt-br\">\r\n<meta charset=\"utf-8\">\r\n<meta http-equiv=\"refresh\" content=\"3\">\r\n";
      webpage += "<html><head><meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">";
      webpage += "<title>MOPPE - Demonstração do Protótipo</title>";
      webpage += "<style>body{font-size:16px;background-color:#F2F2F2;font-family:Arial, Helvetica, sans-serif;color:#FFF;} .centraliza{width:90%;heigth:auto;margin:20px auto 0 auto;background-color:#666;padding:15px;text-align:center;} h1{font-size:112.5%;margin:0 0 20px} p{font-size:100%;margin:5px 0;} #map{width:90%;height:300px;margin:30px auto 0;}</style>";
      webpage += "</head><body><div class=\"centraliza\">";
      webpage += "<h1>MOPPE - Demonstração do Protótipo</h1>";
      webpage += "<p>ID do Dispositivo: ";
      webpage += ID_dispositivo;
      webpage += "</p><p>Sensor ICOS Inferior - Leitura: ";
      webpage += icos_inf;
      webpage += "</p><p>Sensor ICOS Superior - Leitura: ";
      webpage += icos_sup;
      webpage += "</p><p>Estado do Rio Jacaraípe: ";
      if((icos_inf == 1) && (icos_sup == 0))
        webpage += "<span style=\"color:#3CB371\">NÍVEL NORMAL</span>";
      if((icos_inf == 0) && (icos_sup == 0))
        webpage += "<span style=\"color:#FFCC33\">NÍVEL DE ALERTA</span>";
      if((icos_inf == 0) && (icos_sup == 1))
        webpage += "<span style=\"color:#FA8072\">NÍVEL DE EMERGÊNCIA</span>";
      webpage += "</p><p>Sensor Ultrassônico - Leitura: ";
      webpage += nivel;
      webpage += " cm</p><p>GPS Latitude: ";
      webpage += gps.location.rawLat().negative ? "-" : "+";
      webpage += gps.location.rawLat().deg;
      webpage += ".";
      webpage += gps.location.rawLat().billionths;
      webpage += "</p><p>GPS Longitude: ";
      webpage += gps.location.rawLng().negative ? "-" : "+";
      webpage += gps.location.rawLng().deg;
      webpage += ".";
      webpage += gps.location.rawLng().billionths;
      webpage += "</p><p>GPS Data: ";
      webpage += gps.date.day();
      webpage += "/";
      webpage += gps.date.month();
      webpage += "/";
      webpage += gps.date.year();
      webpage += "</p><p>GPS Hora: ";
      webpage += gps.time.hour();
      webpage += ":";
      webpage += gps.time.minute();
      webpage += ":";
      webpage += gps.time.second();
      webpage += "</p>";
      webpage += "<div id=\"map\"></div><script>";
      webpage += "function initMap(){ var uluru = {lat: ";
      webpage += gps.location.rawLat().negative ? "-" : "+";
      webpage += gps.location.rawLat().deg;
      webpage += ".";
      webpage += gps.location.rawLat().billionths;
      webpage += ", lng: ";
      webpage += gps.location.rawLng().negative ? "-" : "+";
      webpage += gps.location.rawLng().deg;
      webpage += ".";
      webpage += gps.location.rawLng().billionths;
      webpage += "};";
      webpage += "var map = new google.maps.Map(document.getElementById('map'), {zoom: 16,center: uluru });";
      if((icos_inf == 1) && (icos_sup == 0)){
        webpage += "var image = 'http://maps.google.com/intl/en_us/mapfiles/ms/micons/green-dot.png';";
        webpage += "var marker = new google.maps.Marker({position: uluru,map: map,icon: image});}";
      }
      if((icos_inf == 0) && (icos_sup == 0)){
        webpage += "var image = 'http://maps.google.com/intl/en_us/mapfiles/ms/micons/yellow-dot.png';";
        webpage += "var marker = new google.maps.Marker({position: uluru,map: map,icon: image});}";
      }
      if((icos_inf == 0) && (icos_sup == 1)){
       webpage += "var marker = new google.maps.Marker({position: uluru,map: map});}"; 
      }
      webpage += "</script></div><script async defer src=\"https://maps.googleapis.com/maps/api/js?key=AIzaSyCY2LEZ6b0SDMwiuqE8vO789ws5TVdonkc&callback=initMap\"></script>";
      webpage += "</body></html>";

    // converte string para array char
    char web[webpage.length()+1];
    webpage.toCharArray(web,webpage.length()+1);

    // obtem o tamanho do array
    uint32_t len = sizeof(web);
    
    // para armazenar id em multiplas conexoes e verificar se houve requisicao
    uint8_t mux_id;
    uint8_t buffer[128] = {0};
    uint32_t len_req = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);

    // se houve requisicao, responde
    if (len_req > 0) {
        // envia dados
        if(wifi.send(mux_id, web, len)) {
            Serial.println(F("Dados enviados: OK"));
        } else {
            Serial.println(F("Dados enviados: ERRO"));
        }

        // libera conexao TCP
        if (wifi.releaseTCP(mux_id)) {
            Serial.print(F("Liberando TCP: "));
            Serial.print(mux_id);
            Serial.println(F(" OK"));
        } else {
            Serial.print(F("Liberando TCP: "));
            Serial.print(mux_id);
            Serial.println(F(" ERRO"));
        }
        
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
    }
  }
  else{
    Serial.print(F("Sem dados validos do GPS. Satelites: "));
    Serial.println(gps.satellites.value());
  }
} // fecha void loop()

// obtendo dados do sensor ultrassonico
float dados_su(){
  float dist = us.ping_cm();
  float nivel = REF - dist; 
  return nivel;
}

// alimentando o objeto gps com dados do módulo GPS
void dados_gps(){
  while(Serial3.available())
    gps.encode(Serial3.read());
}

void updateFileName(){

  for(int i=0; i < MAX_LOG_FILES; i++){
    memset(logNomeArquivo, 0, strlen(logNomeArquivo));
    sprintf(logNomeArquivo,"%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFIX);
    
    if (!sdCard.exists(logNomeArquivo))
      break;
    else{
      Serial.print(logNomeArquivo);
      Serial.println(" existe!");
    }
  }

  Serial.print("Nome do Arquivo: ");
  Serial.println(logNomeArquivo);
}

void printHeader(){
  
  SdFile logFile;
  bool sd_open = logFile.open(logNomeArquivo, O_RDWR | O_CREAT | O_AT_END);
  
  if(sd_open){
    for(int i=0; i < LOG_COLUMN_COUNT; i++){
      logFile.print(logColunas[i]);
      if(i < (LOG_COLUMN_COUNT -1))
         logFile.print(',');
      else
         logFile.println();
    }
    logFile.close();
  }
  else{
    sdCard.errorHalt("\r\nErro na abertura do arquivo!\r\n");
  }
}

byte logData(char ID[2], int inf, int sup, float us)
{
  SdFile logFile;
  bool sd_open = logFile.open(logNomeArquivo, O_RDWR | O_CREAT | O_AT_END);
  
  if(sd_open){
    logFile.print(ID);
    logFile.print(',');
    logFile.print(inf);
    logFile.print(',');
    logFile.print(sup);
    logFile.print(',');
    logFile.print(us);
    logFile.print(',');
    logFile.print(gps.location.lat(), 6);
    logFile.print(',');
    logFile.print(gps.location.lng(), 6);
    logFile.print(',');
    logFile.print(gps.date.value());
    logFile.print(',');
    logFile.print(gps.time.value());
    logFile.println();
    logFile.close();
    return 1;
  }
  else{
    sdCard.errorHalt("\r\nErro na abertura do arquivo!\r\n");
    return 0;
  }
}
