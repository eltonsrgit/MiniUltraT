#ifndef RCDualShock_H
#define RCDualShock_H


bool modoAutonomo = true;
bool RC = false;

void DualShock(){

if (!PS4.isConnected()) {
    motor.stop();
    Serial.println("Aguardando conexão... Pressione PS...");
    ledBlink(150, 0, 0, 100);
    pixels.clear();
    
  } else if (PS4.isConnected()) {
    modoAutonomo = false;
    Serial.println("Controle Conectado. ");
    pixels.clear();
    PS4.setLed(0, 255, 0);
    PS4.sendToController();
    ledLight(0, 150, 0); // Efeito visual indicando que está no modo de controle via rádio

    /*int x = map(PS4.RStickX(), -127, 127, -1024, 1024);
    int y = map(PS4.RStickY(), -127, 127, -1024, 1024);
    int vel_esq = constrain((-x + y), -1024, 1024);
    int vel_dir = constrain((x + y), -1024, 1024);
    motor.move(vel_esq, vel_dir);
    Serial.print("L: ");
    Serial.print(vel_esq);
    Serial.print(" R: ");
    Serial.println(vel_dir);*/
    int RS = map(PS4.RStickX(), -127, 127, -1024, 1024);

    int R2 = map(PS4.R2Value(), 0, 255, 0, 1024);
    int L2 = map(PS4.L2Value(), 0, 255, 0, 1024);

    int move = constrain((R2 + L2), -1024, 1024);

    int vel_esq = constrain(-RS + (R2 - L2), -1024, 1024);
    int vel_dir = constrain(RS + (R2 - L2), -1024, 1024);

    while (PS4.Square()){
      motor.move(-1023, -1023);
    }

    motor.move(vel_esq, vel_dir);
    
    Serial.print("L: ");
    Serial.print(vel_esq);
    Serial.print(" R: ");
    Serial.println(vel_dir);

    
    /*if (PS4.RStickX() < 20 && PS4.RStickY() < 20 && PS4.R2() && !PS4.L2()) {
      int acelerador = map(PS4.R2Value(), 0, 255, 0, 1024);
      int vel_esq = constrain(acelerador, 0, 1024);
      int vel_dir = constrain(acelerador, 0, 1024);
      motor.move(vel_esq, vel_dir);
      break;
    } else if (PS4.RStickX() < 20 && PS4.RStickY() < 20 && PS4.L2() && !PS4.R2()) {
      int reverse = map(PS4.L2Value(), 0, 255, 0, 1024);
      int vel_esq = constrain(reverse, 0, 1024);
      int vel_dir = constrain(reverse, 0, 1024);
      motor.move(-vel_esq, -vel_dir);
      break;
    }
    break;
  }*/
}
}
#endif