// *** CUSTOM CHARACTERS ***
boolean arrowAnimation = false;
byte arrowX = 0;
byte arrowY = 0;
byte arrowFrame = 0;

byte arrow1[8] = {
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b01100,
  0b00100,
  0b00000,
  0b00000
};

byte arrow2[8] = {
  0b00000,
  0b00000,
  0b01100,
  0b11111,
  0b01100,
  0b00000,
  0b00000,
  0b00000
};

byte arrow3[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b11111,
  0b00100,
  0b00000,
  0b00000,
  0b00000
};

byte arrow4[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte Tee1[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b10101,
  0b10101,
  0b10101,
  0b00100
};

byte Tee2[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b01110,
  0b01110,
  0b01110,
  0b01110
};

byte Bee1[8] = {
  0b11110,
  0b11110,
  0b11111,
  0b11111,
  0b01011,
  0b01001,
  0b01011,
  0b01110
};

byte Bee2[8] = {
  0b01110,
  0b01011,
  0b01001,
  0b01011,
  0b11111,
  0b11111,
  0b11110,
  0b11110
};

byte Two1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b11011,
  0b11011
};

byte Two2[8] = {
  0b10011,
  0b00111,
  0b01110,
  0b11100,
  0b11001,
  0b11111,
  0b11111,
  0b11111
};

byte Note[8] = {
  0b00100,
  0b00110,
  0b00101,
  0b00100,
  0b00100,
  0b11100,
  0b11100,
  0b00000
};

byte Rest[8] = {
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b11111,
  0b00000
};

byte Mute[8] = {
  0b10001,
  0b11011,
  0b10101,
  0b10001,
  0b10001,
  0b00000,
  0b11111,
  0b00000,
};

byte Tie[8] = {
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
  0b11111,
  0b00000
};

void customCharacters()
{
  lcd.createChar(0, arrow1);
  lcd.createChar(1, arrow2);
  lcd.createChar(2, arrow3);
  lcd.createChar(3, arrow4);
  lcd.createChar(4, Note);
  lcd.createChar(5, Rest);
  lcd.createChar(6, Mute);
  lcd.createChar(7, Tie);

}
