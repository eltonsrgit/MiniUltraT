#ifndef Linha_H
#define Linha_H

#define SensorE 33
#define SensorD 32
// 1 é cor preta
// 0 é cor branca

//!Direita por exemplo vale zero, então é branco
// Direita vale um, então é preto

void BackToDestroy(){
  bool Esquerda = digitalRead(SensorE);
  bool Direita = digitalRead(SensorD);

  if (Esquerda && Direita){
    motor.move(1023, 1023);
  }

  else if (!Esquerda && !Direita){
    motor.move(-1023, -1023);
    delay(350);
    SeekAndDestroy();
  }

  else if (Esquerda && !Direita){
    motor.move(1023, -1023);
    delay(500);
    SeekAndDestroy();
  }

  else if (!Esquerda && Direita){
    motor.move(-1023, 1023);
    delay(500);
    SeekAndDestroy();
  }
}



#endif