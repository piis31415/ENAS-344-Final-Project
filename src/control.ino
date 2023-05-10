#include "USBHost_t36.h"

USBHost myusb;
MIDIDevice midi1(myusb);

/*
  SparkFun Inventor’s Kit
  Circuit 5A - Motor Basics
  Learn how to control one motor with the motor driver.
  This sketch was written by SparkFun Electronics, with lots of help from the 
Arduino community.
  This code is completely free for any use.mi
  View circuit diagram and instructions at: 
https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/
//PIN VARIABLES
//the motor will be controlled by the motor A pins on the motor driver
const int INS1[4] = {0,2,4,6};
const int INS2[4] = {1,3,5,7};
const int PWMS[4] = {22,23,24,25};
const int pots[4] = {14,15,16,17};

const int highnotes[13] = {30, 140, 240, 360, 450, 540, 630, 700, 770, 830, 890, 950, 1000};
const int lownotes[9] = {10, 120, 260, 370, 490, 630, 720, 810, 900};
// lownotes[5] = highnotes[0] = A

typedef struct note {
  int loc;
  int dur;
} Note;

int curr = 0;
int led = 13;
char play = 0;
//VARIABLES
int motorSpeed = 255; 
int tune[4] = {0,0,0,0};

int activepipes[4] = {0,0,0,0};

int pipes[4] = {20,20,20,20};

int curr_pipe = 0;
int counter = 2;

int curr_note = -1;
int curr_off = -1;

      //starting speed for the motor
void setup() {

  Serial.begin(115200);
  
  myusb.begin();
  midi1.setHandleNoteOff(OnNoteOff);
  midi1.setHandleNoteOn(OnNoteOn);
  midi1.setHandleControlChange(OnControlChange);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn_ch);
  usbMIDI.setHandleControlChange(OnControlChange);

  pinMode(led, OUTPUT);
  //set the motor control pins as outputs
  for (int i = 0; i < 4; i++) {
    pinMode(INS1[i], OUTPUT);
    pinMode(INS2[i], OUTPUT);
    pinMode(PWMS[i], OUTPUT);
  }
  pinMode(led, OUTPUT);

  analogWrite(led, HIGH);

}
void loop() {
  myusb.Task();
  midi1.read();
  usbMIDI.read();
  for (int i = 0; i < 4; i++) {
    moveTo(i, pipes[i] + tune[i]);
  }
}
/********************************************************************************/

int readPot(int p) {
  if (p == 0 || p == 1) {
    return analogRead(pots[p]);
  } else if (p == 2 || p == 3) {
    return (1023 - analogRead(pots[p]));
  } else {
    return -1;
  }
}
void spinMotor(int p, int motorSpeed){                       //function for driving the right motor
  digitalWrite (INS1[p], (motorSpeed > 0) ? HIGH : LOW);
  digitalWrite (INS2[p], (motorSpeed < 0) ? HIGH : LOW);
  analogWrite(PWMS[p], abs(motorSpeed));                 //now that the motor direction is set, drive it at the entered speed
}
char moveTo(int p, int loc) {
  int currpos = readPot(p);
  if (abs(loc - currpos) <= 15) {
    spinMotor(p, 0);
    return 1;
  }
  if (currpos < loc) {
    spinMotor(p, 255);
    return 0;
  } else {
    spinMotor(p, -255);
    return 0;
  }
}

void OnNoteOn(byte channel, byte note, byte velocity)
{
  if (note >= 44 && note <= 47) {
    curr_pipe = note - 44;
  } else if (note >= 63 && note <= 71 && (curr_pipe == 1 || curr_pipe == 2)) {
    pipes[curr_pipe] = lownotes[note - 63];
  } else if (note >= 69 && note <= 81 && (curr_pipe == 0 || curr_pipe == 3)) {
    pipes[curr_pipe] = highnotes[note - 69];
  }
}

void OnNoteOff(byte channel, byte note, byte velocity)
{

}

void OnControlChange(byte channel, byte control, byte value)
{
  if (1 <= control && control <= 4) {
   tune[control-1] = map(value, 0, 127, 0, 255);
  }
}



void OnNoteOn_ch(byte channel, byte note, byte velocity)
{
  switch (channel) {
    case 1:
      if (note >= 69 && note <= 81) pipes[0] = highnotes[note - 69];
    break;
    case 2:
      if (note >= 63 && note <= 71) pipes[1] = lownotes[note - 63];
    break;
    case 3:
      if (note >= 63 && note <= 71) pipes[2] = lownotes[note - 63];
    break;
    case 4:
      if (note >= 69 && note <= 81) pipes[3] = highnotes[note - 69];
    break;
  }
}
