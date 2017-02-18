
// light sensor on Analog pin
int sensorPin = A2;
int sensorValue = 0;
int light_reading = 0;

byte BTN_SH = 5;
byte BTN_SP = 3;
byte BTN_RW = 4;

byte button_sh = 1; // 1 is up, 0 is down
byte button_sp = 1; // ""
byte button_rw = 1; // ""

void setup_inputs() {
  pinMode(BTN_SH, INPUT_PULLUP);
  pinMode(BTN_SP, INPUT_PULLUP);
  pinMode(BTN_RW, INPUT_PULLUP);
}

// detect a button change and enqueue a message.
void listen_inputs() {
  if (!digitalRead(BTN_SH)) {
    digitalWrite(13, HIGH);
    // Shutter button is down
    if (button_sh) {
      enqueue(shutter_dn, -1, false, false);
      button_sh = 0;
    }
  }
  else {
    digitalWrite(13, LOW);
    // shutter button is up
    if (!button_sh) {
      //enqueue(shutter_up, -1, false, false);
      button_sh = 1;
    }
  }
  
  if (!digitalRead(BTN_SP)) {
    // shutter Speed button is down
    if (button_sp) {
      enqueue(speed_dn, -1, false, false);
      button_sp = 0;
    }
  }
  else {
    // shutter Speed button is up
    if (!button_sp) {
      //enqueue(speed_up, -1, false, false);
      button_sp = 1;
    }
  }
  
  if (!digitalRead(BTN_RW)) {
    // ReWind button is down
    if (button_rw) {
      enqueue(rewind_dn, -1, false, false);
      button_rw = 0;
    }
  }
  else {
    // rewind button is up
    if (!button_rw) {
      //enqueue(rewind_up, -1, false, false);
      button_rw = 1;
    }
  }
  
}


