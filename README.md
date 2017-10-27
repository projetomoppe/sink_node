# sink_node

Assim como o *sensor_node*, este nó faz a leitura dos dados dos sensores.
O que os difere, é que este nó também recebe dados dos outros nós através do módulo **NRF24L01** (até num máximo de 6 nós)
e envia este dados para um servidor na Internet que irá emitir as notificações (em caso de nível crítico) para o aplicativo.

*Para fim demonstrativos, o dispositivo sink_node aqui apresentado apenas recebe dados de um nó e envia estes dados ao servidor.*

## Componentes
- Arduino MEGA 2560;
- Módulo NRF24L01;
- Módulo Wi-Fi ESP2866;
- Módulo Display LCD 16x2;
- Potenciômetro;
- Resistores para divisor de tensão;
- Jumpers.

## Princípio de Funcionamento

Os dados recebidos pelo módulo NRF24L01 são armazenados em uma *struct*.
Estes dados são então utilizados para a criação de uma requisição **HTTP GET** que será realizada pelo
módulo ESP8266. A cada novo registro de dados recebido, a requisição é enviada e os dados
são salvos no servidor do projeto. Além disse, o estado do rio e o valor da leitura do nível fornecido pelo sensor ultrassônico
são exibidos no display LCD.

## Pinagem dos dispositivos

- Módulo NRF24L01:
```
VCC do módulo -> 5V
GND do módulo -> GND
MISO do módulo -> MISO SPI Arduino MEGA 2560 (D50)
MOSI do módulo -> MOSI SPI Arduino MEGA 2560 (D51)
SCK do módulo -> SCK SPI Arduino MEGA 2560 (D52)
CE do módulo -> D7 Arduino MEGA 2560
CSN do módulo -> D53 Arduino MEGA 2560
```

- Módulo Wi-Fi ESP2866:
```
VCC do módulo -> 3.3V
GND do módulo -> GND
TX do módulo -> RX1 (D19) Arduino MEGA 2560
RX do módulo -> divisor de tensão (3.3V) -> TX1 (D18) Arduino MEGA 2560
RST e CH_PD do módulo -> resistor de 1K Ohm -> 3.3V
```

- Módulo Display LCD 16x2:
```
Vdd -> 5V
Vss -> GND
V0 -> Pino central do potenciômetro (controle de back light)
RS -> Pino D12 Arduino MEGA 2560
RW -> GND
E -> Pino D11 Arduino MEGA 2560
D4 -> Pino D5 Arduino MEGA 2560
D5 -> Pino D4 Arduino MEGA 2560
D6 -> Pino D3 Arduino MEGA 2560
D7 -> Pino D2 Arduino MEGA 2560
A -> 5V
K -> GND
D0 a D3 -> Não conectados
```

## Construído com

- https://github.com/maniacbug/RF24/ - Arduino driver para nRF24L01;
- https://github.com/itead/ITEADLIB_Arduino_WeeESP8266 - WeeESP8266 utilizada no módulo Wi-Fi;
- Biblioteca LiquidCrystal - Para o Display LCD -> Vem instalado com a IDE do Arduino.

## Observações

- Necessário a inclusão da biblioteca nativa da IDE do Arduino SPI.h.
Serial Peripheral Interface (SPI) (Interface Periférica Serial na tradução livre) é um
protcolo síncrono de dados serial utilizado por microcontroladores
para comunicação com um ou mais dispositivos periféricos de forma rápida em pequenas distâncias.

- Um capacitor de 100uF foi soldado entre os pinos VCC e GND do módulo NRF24L01.
Esta foi a maneira encontrada para estabilizar o funcionamento do módulo, de modo a obter leituras consistentes.
Capacitores possuem a capacidade de amenizar ou eliminar ruídos.

## Biblioteca WeeESP8266 (Funções e Construtores)

- #define SSID  "sua-rede-wireless" : Define o nome do Ponto de Acesso (Wi-Fi) que será utilizado.
- #define PASSWORD  "senha-da-rede" : Senha do Ponto de Acesso (Wi-Fi).
- #define HOST_NAME "servidor"      : Servidor (Host)
- #define HOST_PORT 80              : Porta de conexão (80 para conexão HTTP)

- ESP8266 wifi(Serial1, 115200);
  - Construtor que inicializa o módulo ESP8266. Recebe como parâmetro a porta de comunicação Serial (Serial1 no Arduino MEGA ou
  portas definidas através da biblioteca SoftwareSerial.h para o Arduino UNO) e a velocidade de comunicação (baud rate).

- wifi.setOprToStationSoftAP();
  - bool    setOprToStationSoftAP (void) : Seta o modo de operação para estação + softap.

- wifi.joinAP(SSID, PASSWORD);
  - bool    joinAP (String ssid, String pwd) : Entra em um Ponto de Acesso.

- wifi.getLocalIP();
  - String    getLocalIP (void) : obtém o endereço IP local para o módulo ESP8266.

- wifi.disableMUX();
  - bool    disableMUX (void) : Desabilita IP MUX (modo de conexão única).

- wifi.createTCP(HOST_NAME, HOST_PORT);
  - bool    createTCP (String addr, uint32_t port) : Cria conexão TCP em modo de conexão única. 

- wifi.send((const uint8_t*)httpGET, strlen(httpGET));
  - bool    send (const uint8_t *buffer, uint32_t len) : Envia dados para a conexão TCP ou UDP criada em modo de conexão única.

## Biblioteca RF24 (Funções e Construtores)

- RF24 radio(CEpin, CSpin);
  - RF24::RF24 (uint8_t _cepin, uint8_t _cspin). Construtor -> Cria uma nova instância do driver.
  
- radio.begin();
  - void RF24::begin ( void ). Inicia a operção do chip.
  
- radio.openReadingPipe(1,pipe);
  - void RF24::openReadingPipe(uint8_t number, uint64_t address). Abre um túnel para leitura. Podem ser abertos até 6 túneis simultaneamente.
  
- radio.startListening();
  - void RF24::startListening ( void ). Começa e "escutar" os túneis abertos para leitura.
  
- radio.available();
  - bool RF24::available ( void ). Testa se existem bytes disponíveis para leitura.
  
- radio.read(&dadosSensores, sizeof(dadosSensores));
  - bool RF24::read ( void * buf, uint8_t len ). Lê o pacote. Retorna o último pacore lido. O tamanho dos dados lidos
  é o tamanho fixo do pacote.
  
## LiquidCrystal - Display LCD (Funções e Construtores)

- LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
  - Cria a instância "lcd" e informa os pinos onde o módulo foi instalado no Arduino.
  
- lcd.begin(16, 2);
  - Inicializa o display LCD e define o número de colunas e linhas.
  
- lcd.clear();
  - Limpa a tela do display.
  
- lcd.setCursor(2, 0);
  - Posiciona o cursor na coluna e linha informados. Nesse caso, posicionou-se o cursor na coluna 2 e na linha 0.
  
- lcd.print("PICJr MOPPE");
  - Imprime algum texto no display LCD. Nesse caso, "PICJr MOPPE".

## Outras Funções

 - String floatToString(float x, byte precision = 2)
   - Converte dados *float* em *string*. Recebe como parâmetros o valor a ser convertido e a precisão (casas decimais).
 
 - temp_request.toCharArray(httpGET, 300);
   - Converte a requisição HTTP de String para charArray.
