
Adafruit_7segment disp = Adafruit_7segment();

void display_letter(char letter, boolean letter_after) {
//  ___   bit positions for 7 segment display
// |   |  |-0-|
// |___|  5   1
// |   |  |-6-|
// |___|  4   2
//        |-3-|
//
  byte letter_pos = 0;
  disp.setBrightness(2);
  if (letter_after) {
    letter_pos = 4;
  }
  
  if (letter == 'A') {
    disp.writeDigitRaw(letter_pos, 0x77); // A
  }
  else if (letter == 's') {
    disp.writeDigitRaw(letter_pos, 0b1100100); // s 1100100
  }
  else if (letter == 'F') {
    disp.writeDigitRaw(letter_pos, 0b1110001); // F 1110001
  }
  else if (letter == 'o') {
    disp.writeDigitRaw(letter_pos, 0b1011100); // o 1011100
  }
  else if (letter == 'O') {
    disp.writeDigitRaw(letter_pos, 0b0111111); // o 0111111
  }
  else if (letter == '_') {
    disp.writeDigitRaw(letter_pos, 0b1001001); // _= 1001001
  }
  disp.writeDisplay();
}

void display_float(float dispNumber, char letter, boolean letter_after) {
    int decimal = (int)dispNumber;
    int d1 = decimal/10;
    int d2 = decimal%10;
  int m = round((dispNumber - (float)decimal)*10);
  
  disp.writeDigitRaw(1, 0b0000000); // black 0000000
  if (d1) {
    disp.writeDigitNum(1, d1, false);
  }
  disp.writeDigitNum(3, d2, (m!=0));
  disp.writeDigitRaw(4, 0b0000000); // black 0000000
  if (m!=0) {
    disp.writeDigitNum(4, m, false);
  }
  display_letter(letter, letter_after);
}

void display(int dispNumber, char letter, boolean letter_after) {
  if (letter_after) {
    dispNumber *= 10;
  }
  disp.print(dispNumber);
  display_letter(letter, letter_after);
}

void setup_view() {
  disp.begin(0x70);
  //display(94, 'A', false);
}

