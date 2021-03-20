#include <Servo.h>
#define INPUT_SIZE 10


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

void rotate(int leg, int pos) {
    legs[leg].write(pos);
    positions[leg] = pos;
}

void write_to_all(int pos) {
    for (int i = 0; i < 8; i++) {
        rotate(i, get_pos(i, pos));
        delay(25);
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
    initialize();

}

void move_forward(int leg, int pos, int speed) {
    for (int i = positions[leg]; i < pos; i++) {
        rotate(leg, i);
        delay(speed);
    }
}

void move_backward(int leg, int pos, int speed) {
    for (int i = positions[leg]; i > pos; i--) {
        rotate(leg, i);
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
}

void f_leg_center(int l, int s) {
    move(l, 50, s);
}

void b_leg_center(int l, int s) {
 move(l, 70, s); 
}

void f_leg_forward(int l, int s) {
    move(l+1, 135, s);
    move(l, 135, s);
    move(l+1, 90, s);
}

void b_leg_forward(int l, int s) {
    move(l+1, 135, s);
    move(l, 170, s);
    move(l+1, 90, s);
}

void step_forward(int s) {
    b_leg_center(2, s);
    b_leg_forward(0, s);
    f_leg_center(4, s);
    f_leg_forward(6, s);

    b_leg_center(0, s);
    b_leg_forward(2, s);
    f_leg_center(6, s);
    f_leg_forward(4, s);
}

void loop() {

    for (;;) {
        step_forward(2);
        delay(50);
    }

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
