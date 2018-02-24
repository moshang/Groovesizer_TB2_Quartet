void setup() {
  // DEBUG
  //Serial.begin(9600);

  // *** LCD ***
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);


  // create custom characters
  lcd.createChar(2, Tee1);
  lcd.createChar(3, Tee2);
  lcd.createChar(4, Bee1);
  lcd.createChar(5, Bee2);
  lcd.createChar(6, Two1);
  lcd.createChar(7, Two2);

  lcd.setCursor(4, 0);
  lcd.print("Groovesizer");
  lcd.setCursor(4, 1);
  lcd.print("Ver.");
  lcd.setCursor(8, 1);
  lcd.print(versionNumber);
  showTB2(0);

  // *** BUTTONS ***
  // Make input & enable pull-up resistors on switch pins
  for (byte i = 0; i < NUMBUTTONS; i ++)
    pinMode(buttons[i], INPUT_PULLUP);

  assignIncrementButtons(&menuChoice, 0, 6, 1);

  // *** POTS ***
  getPots();
  lockPot(5);

  // *** LED ***
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); // turn off the LED

  // *** SYNTH ***
  ulInput[0] = 512;

  createNoteTable(SAMPLE_RATE);
  createSineTable();
  createSquareTable(pulseWidth);
  createSawTable();
  createTriangleTable();
  clearUserTables();

  for (byte i = 0; i < 4; i++)
    wavePointer[i] = &nTriangleTable[0];
  for (byte i = 0; i < 4; i++)
    wavePointer[i + 4] = &nTriangleTable[0];

  // this is a cheat - enable the DAC
  analogWrite(DAC0, 0);
  analogWrite(DAC1, 0);

  // *** TIMERS ***
  Timer3.attachInterrupt(audioHandler).setFrequency(44100).start(); // start the audio interrupt at 44.1kHz
  Timer4.attachInterrupt(lfoHandler).setFrequency(22000).start(); // start the LFO handler
  Timer5.attachInterrupt(clockHandler).setPeriod(60000000 / bpm / 96).start(); // start the 96ppq internal clock (in microseconds)

  // *** FILTER ***
  setFilterCutoff(255);
  setFilterResonance(210);
  setFilterType(filterType); // 0 = LP, 1 = BP, 2 = HP

  // *** MIDI ***
  midiA.setHandleNoteOn(HandleNoteOn);  // these callback functions are defined in MIDI
  midiA.setHandleNoteOff(HandleNoteOff);
  //midiA.setHandleControlChange(HandleCC);
  midiA.setHandleClock(HandleClock);
  midiA.setHandleStart(HandleStart);
  midiA.setHandleStop(HandleStop);
  midiA.begin(MIDI_CHANNEL_OMNI);

  // *** WAVESHAPER ***
  //createWaveShaper(waveShapeAmount);
  createWaveShaper();
  
  // *** OSC 1 & 2 VOLUME ***
  createOsc1Volume();
  createOsc2Volume();
  
  // *** GAIN ***
  createGainTable();

  // *** SD CARD ***
  if (!sd.begin(chipSelect, SPI_FULL_SPEED))
  {
    lcd.setCursor(0, 1);
    lcd.print("*SD Unavailable*");
  }
  else
  {
    loadSettings();
    getFirstFile();
  }

  // clear the patch buffer
  for (uint16_t i = 0; i < 1900; i++)
    patchBuffer[i] = 0;

  // clear the sequencer bank buffer
  for (uint16_t i = 0; i < 1600; i++)
    seqBankBuffer[i] = 0;
}
