#include <Servo.h>
#define INPUT_SIZE 10

int LED = 13;

Servo lu1;
Servo lu2;
Servo lb1;
Servo lb2;
Servo rb1;
Servo rb2;
Servo ru1;
Servo ru2;
Servo legs[] = {lu1, lu2, lb1, lb2, rb1, rb2, ru1, ru2};
int values[8];

void setup() {

  Serial.begin(9600);
  delay(500);
  Serial.print("Start");

  for (int i = 0; i < 8; i++) {
    legs[i].attach(i+1);
    legs[i].write(90);
    values[i] = 90;
    delay(25);
  }

}

void move_forward(int leg, int deg, int speed) {
  for (int i = values[leg]; i < deg; i++) {
    legs[leg].write(i);
    delay(speed);
  }
  values[leg] = deg;
}

void move_backward(int leg, int deg, int speed) {
  for (int i = values[leg]; i > deg; i--) {
    legs[leg].write(i);
    delay(speed);
  }
  values[leg] = deg;
}

void move(int leg, int deg, int speed) {
  if (speed > 0) {
    if (values[leg] < deg && deg >= 0) {
      move_forward(leg, deg, speed);
    }
    if (values[leg] > deg && deg <= 180) {
      move_backward(leg, deg, speed);
    }
  }
}

void loop() {

  if(Serial.available()) {
      int l = 0;
      int d = 0;
      int s = 0;
      char input[INPUT_SIZE + 1];
      byte size = Serial.readBytes(input, INPUT_SIZE);
      input[size] = 0;

      char* pch = strtok(input, " ");
      while (pch != 0) {
      l = atoi(pch);
      pch = strtok(NULL, " ");
      d = atoi(pch);
      pch = strtok(NULL, " ");
      s = atoi(pch);
      pch = strtok(NULL, " ");
      }
 
      move(l, d, s);
  }
  
}
