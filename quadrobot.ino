#include <Servo.h>
#include <SoftwareSerial.h>

#define INPUT_SIZE 10

SoftwareSerial BTSerial(1, 0); // RX | TX

Servo lu1;
Servo lu2;
Servo lb1;
Servo lb2;
Servo rb1;
Servo rb2;
Servo ru1;
Servo ru2;
Servo legs[8] = {lu1, lu2, lb1, lb2, rb1, rb2, ru1, ru2};
int reverse[] = {2, 3, 6, 7};
int positions[8];


void servo_attach() {
    for (int i = 0; i < 8; i++) {
        legs[i].attach(i+2);
        delay(10);
    } 
}

int invert(int pos) {
    int temp = 0;
    if(pos == 90) {
        return 90;
    }
    if(pos > 90) {
        temp = pos - 90;
    	return (90 - temp);
    } else {
        temp = 90 - pos;
        return (90 + temp);
    }
}

int get_pos(int leg, int pos) { 
    int size = sizeof(reverse) / sizeof(int);
    for (int i = 0; i < size; i++) {
        if (reverse[i] == leg) {
            return invert(pos);
        }
    }
    return pos;
}

void rotate_servo(int leg, int pos) {
    legs[leg].write(pos);
    positions[leg] = pos;
}

void write_to_all(int pos) {
    for (int i = 0; i < 8; i++) {
        rotate_servo(i, get_pos(i, pos));
        delay(5);
    }
}

void write_to_bottom(int pos) {
    for (int i = 1, j = 0; j < 4; i+=2, j++) {
        rotate_servo(i, get_pos(i, pos));
    }
}

void initialize() {
    Serial.print("Initialize");
    delay(200);
    servo_attach();
    write_to_all(90);
    delay(500);
    Serial.print("Done");
}

void setup() {

    Serial.begin(9600);
    BTSerial.begin(9600);
    
    initialize();

}

void move_forward(int leg, int pos, int speed) {
    for (int i = positions[leg]; i < pos; i++) {
        rotate_servo(leg, i);
        delay(speed);
    }
}

void move_backward(int leg, int pos, int speed) {
    for (int i = positions[leg]; i > pos; i--) {
        rotate_servo(leg, i);
        delay(speed);
    }
}

void move(int leg, int p, int speed) {
    int pos = get_pos(leg, p);
    if (speed > 0) {
        if (positions[leg] < pos && pos >= 0) {
            move_forward(leg, pos, speed);
        }
        if (positions[leg] > pos && pos <= 180) {
            move_backward(leg, pos, speed);
        }
    }
    Serial.print(pos);
    Serial.print('\n');
}

// movement
// if movement forward, use default positions
// if not, invert positions for the same movement back

// center give the best balance after one leg movement

void f_leg_center(int l, int s, bool i) {
    int pos = 50;
    if (i) 
        pos = invert(pos);
    
    move(l, pos, s);
}

void b_leg_center(int l, int s, bool i) {
    int pos = 70;
    if (i) 
        pos = invert(pos);
    
    move(l, pos, s);
}

void f_leg_step(int l, int s, bool i) {
    int pos = 135;
    if (i) 
        pos = invert(pos);

    move(l+1, 135, s);
    move(l, pos, s);
    move(l+1, 90, s);
}

void b_leg_step(int l, int s, bool i) {
    int pos = 135;
    if (i) 
        pos = invert(pos);
    
    move(l+1, 135, s);
    move(l, pos, s);
    move(l+1, 90, s);
}

void step_forward(int s) {
    b_leg_center(2, s, false);
    b_leg_step(0, s, false);
    f_leg_center(4, s, false);
    f_leg_step(6, s, false);

    b_leg_center(0, s, false);
    b_leg_step(2, s, false);
    f_leg_center(6, s, false);
    f_leg_step(4, s, false);
}

void step_backward(int s) {
    f_leg_center(2, s, true);
    f_leg_step(0, s, true);
    b_leg_center(4, s, true);
    b_leg_step(6, s, true);

    f_leg_center(0, s, true);
    f_leg_step(2, s, true);
    b_leg_center(6, s, true);
    b_leg_step(4, s, true);
}

// rotation

void rotate(int s, bool right) {
    b_leg_center(2, s, right);
    b_leg_step(0, s, right);
    f_leg_center(4, s, right);
    f_leg_step(6, s, !right);

    b_leg_center(0, s, right);
    b_leg_step(2, s, !right);
    f_leg_center(6, s, right);
    f_leg_step(4, s, right);
}

//

void fall_down() {
    write_to_bottom(135);
}

void stand_up() {
    write_to_bottom(80);
}

void command (int x, int y) {    
    switch (x) {
    case 1:
        step_forward(5);
        break;
    case 2:
        step_forward(3);
        break;
    case 3:
        step_forward(1);
        break;
    case -1:
        step_backward(5);
        break;
    case -2:
        step_backward(3);
        break;
    case -3:
        step_backward(1);
        break;
    }
    switch (y) {
    case 1:
        rotate(5, true);
        break;
    case 2:
        rotate(3, true);
        break;
    case 3:
        rotate(1, true);
        break;
    case -1:
        rotate(5, false);
        break;
    case -2:
        rotate(3, false);
        break;
    case -3:
        rotate(1, false);
        break;
    }
}

void loop() {

   // Keep reading from HC-06 and send to Arduino Serial Monitor
  if (BTSerial.available())
    Serial.write(BTSerial.read());
 
  // Keep reading from Arduino Serial Monitor and send to HC-06
  if (Serial.available())
  BTSerial.write(Serial.read());

    /* if(Serial.available()) { */
    /*     char c = 0; */
    /*     int s = 0; */
    /*     int i = 0; */
    /*     char input[INPUT_SIZE + 1]; */
    /*     byte size = Serial.readBytes(input, INPUT_SIZE); */
    /*     input[size] = 0; */

    /*     char* pch = strtok(input, " "); */
    /*     while (pch != 0) { */
    /*         c = pch[0]; */
    /*         pch = strtok(NULL, " "); */
    /*         s = atoi(pch); */
    /*         pch = strtok(NULL, " "); */
    /*         i = atoi(pch);                         */
    /*         pch = strtok(NULL, " "); */
    /*     } */

    /*     for (int j = 0; j < i; j++) { */
    /*         switch ( c ) { */
    /*         case 'f': */
    /*             step_forward(s); */
    /*             break; */
    /*         case 'b': */
    /*             step_backward(s); */
    /*             break; */
    /*         case 'r': */
    /*             rotate(s, true); */
    /*             break; */
    /*         case 'l': */
    /*             rotate(s, false); */
    /*             break; */
    /*         } */
    /*     } */
    /* }  */
}
