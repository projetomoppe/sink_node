#include <SPI.h>              // SPI
#include <nRF24L01.h>         // RF24
#include <RF24.h>

// definindo as constantes do programa

// MODULO RF24
const int CEpin = 7;
const int CSpin = 53;
const int LED = LED_BUILTIN;

// inicialização dos objetos
RF24 radio(CEpin,CSpin);
const uint64_t pipe = 0xF0F0F0F0E1LL;

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
  pinMode(LED, OUTPUT);

  Serial.println(F("\r\n----------- PICJr - MOPPE -----------"));
  Serial.println(F("------- Programa inicializado -------\r\n"));

  Serial.println(F("Iniciando Setup..."));
  
  delay(2000);

  // inicializa modulo RF24
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  
  Serial.println(F("Setup Finalizado!"));
  delay(5000);
} // fecha void setup()

void loop()
{
  unsigned long inicio_espera = millis();
  bool timeout = false;
  
  while(!radio.available() && !timeout) {
    if(millis() - inicio_espera > 250)
      timeout = true;
      
    if(timeout) {
      Serial.println(F("Timeout RF24"));
      digitalWrite(LED, HIGH);
    }
    else {
      radio.read(&dadosSensores, sizeof(dadosSensores));
      digitalWrite(LED, LOW);
    }
  }
  
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
  Serial.println("\r\n");
}
