#include <PS4Controller.h>  // Biblioteca para controle DualShock
#include <esp_now.h>
#include <WiFi.h> 
#include "SumoIR.h"
#include "DRV8833.h"
#include "sensoresIR.h"
#include "ledFX.h"  // .h para efeitos de LED


// #include "GoBackAttack.h" // Função de retorno e ataque
/*----------------------As funções SeekAndDestroy e GoBackAttack não podem ser chamadas juntas no void loop!!!!!!------------------*/

#define sensorReflex 14
#define boot 0

const int motor_esq_1 = 18;   //4
const int motor_esq_2 = 19;  //27
const int motor_dir_1 = 4;  //13
const int motor_dir_2 = 23;  //14


DRV8833 motor(motor_esq_1, motor_esq_2, motor_dir_1, motor_dir_2);
#include "SeekAndDestroy.h"  // Função de busca e destruição
#include "RCDualShock.h"
//#include "GoBackAttack.h"
/*bool modoAutonomo = true;
bool RC = false;*/

// Declaração de objetos e variáveis globais
SumoIR IR;

unsigned long tempoPressionado = 0;  // armazena o tempo que o botão foi pressionado
bool botaoPressionado = false;       // indica se o botão foi pressionado


esp_now_peer_info_t peerInfo;
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0x4A, 0x4D, 0x60};  //34:94:54:E2:CC:44 vespa furiosa
typedef struct pacote {
    int bot;
    int len;
    int ID;
    int ch[20];
} pacote;
pacote pack_rx;

void setup(){
  pinMode(boot, INPUT_PULLUP);
  Serial.begin(115200);
  IR.begin(15);
  PS4.begin("60:5b:b4:7e:74:a4");  // mac do meu ps4 "60:5b:b4:7e:74:a4"
  motor.begin();
  motor.bip(3, 200, 2000);
  pinMode(sensorReflex, INPUT);
  pinMode(leftIRpin, INPUT);
  pinMode(rightIRpin, INPUT);
  pixels.begin();
  pixels.setBrightness(100);
  pixels.clear();
  ledLight(0, 0, 0);
  //RadioControle();
  motor.stop();

  //pinMode((pino do sensor de linha), INPUT); 
}



void loop(){

  // Verifica se o botão foi pressionado
  if (digitalRead(boot) == LOW && !botaoPressionado) {
    tempoPressionado = millis();  // Armazena o tempo atual
    botaoPressionado = true;      // Marca o botão como pressionado
  }

  // Verifica se o botão foi pressionado por mais de 1000ms (1 segundo)
  if (botaoPressionado && (millis() - tempoPressionado >= 1000) && modoAutonomo && !RC) {
    modoAutonomo = false;  // Alterna o modo
    Serial.print("Modo alternado para: ");
    Serial.println(modoAutonomo ? "Autônomo." : "RC.");

    // Reseta a variável do botão pressionado
    botaoPressionado = false;
  }
  if (botaoPressionado && (millis() - tempoPressionado >= 1000) && !modoAutonomo && RC) {
    modoAutonomo = true;  // Alterna o modo
    Serial.print("Modo alternado para: ");
    Serial.println(modoAutonomo ? "Autônomo. " : "RC. ");
    // Reseta a variável do botão pressionado
    botaoPressionado = false;
  }
  

  if (!modoAutonomo) {
    RC = true;
    // Código RC
    DualShock();
    


  } else if (modoAutonomo) {
    // Código autônomo
    RC = false;
    PS4.setLed(255, 0, 0);   // seta a cor do led do controle
    PS4.sendToController();  // necessário para enviar o comando acima para o controle

    IR.update();

    if (IR.prepare()) {

      motor.stop();
      ledBlink(150, 150, 150, 65);
      Serial.println("-> sumo prepare");

    } else if (IR.start()) {
      Serial.println("-> sumo start");
    } else if (IR.on()) {
      pixels.clear();
      ledLight(0, 150, 0);
      Serial.println("-> sumo on");
      SeekAndDestroy();
      
      

    } else if (IR.stop()) {
      pixels.clear();
      motor.stop();

      Serial.println("-> sumo stop");

      ledLight(150, 0, 0);

    } else /* if (!IR.prepare() && !IR.start() && !IR.on() && !IR.stop()) */ {
      /* Código quando o robô está desligado */
      pixels.clear();
      motor.stop();
      ledLight(150, 0, 0);
    }
  }
}

void RadioControle() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  motor.begin();
  motor.stop();
  if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      motor.stop();
      return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
  }

  // Register callback for received data
  esp_now_register_recv_cb(OnDataReceived);

  pixels.clear();
  ledLight(150, 150, 150);

}

void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // Check if the received data matches the size of the pacote struct
  if (data_len == sizeof(pacote)) {
    // Copy received data to the pack_rx structure
    memcpy(&pack_rx, data, sizeof(pacote));

    int x = map(pack_rx.ch[0], 1000, 2000, -1024, 1024);
    int y = map(pack_rx.ch[1], 1000, 2000, -1024, 1024);

    int vel_esq = constrain( (-x + y ), -1024, 1024);
    int vel_dir = constrain( ( x + y ), -1024, 1024);

    motor.move(vel_esq, vel_dir);

  


  } else {
    Serial.println("Received data with incorrect size");
  }
}

