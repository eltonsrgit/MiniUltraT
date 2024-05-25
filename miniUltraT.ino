#include <PS4Controller.h>  
#include <WiFi.h> 
#include "SumoIR.h"
#include "DRV8833.h"
#include "sensoresIR.h"
#include "ledFX.h"  


#define sensorReflex 14
#define boot 0

const int motor_esq_1 = 18;   //4 vespa
const int motor_esq_2 = 19;  //27 vespa
const int motor_dir_1 = 4;  //13 vespa
const int motor_dir_2 = 23;  //14 vespa


DRV8833 motor(motor_esq_1, motor_esq_2, motor_dir_1, motor_dir_2);


#include "SeekAndDestroy.h"  // Função de busca e destruição >:DDDDDD
#include "RCDualShock.h"

SumoIR IR;

unsigned long tempoPressionado = 0;  // armazena o tempo que o botão foi pressionado
bool botaoPressionado = false;       // indica se o botão foi pressionado


void setup(){
  pinMode(boot, INPUT_PULLUP);
  Serial.begin(115200);
  IR.begin(15);
  PS4.begin("60:5b:b4:7e:74:a4");  // mac do meu ps4 "60:5b:b4:7e:74:a4"
  motor.begin();
  motor.bip(3, 200, 2280);
  pinMode(sensorReflex, INPUT);
  pinMode(leftIRpin, INPUT);
  pinMode(rightIRpin, INPUT);
  pixels.begin();
  pixels.setBrightness(100);
  pixels.clear();
  ledLight(0, 0, 0);
  motor.stop();
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
