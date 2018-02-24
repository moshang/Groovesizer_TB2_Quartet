void loop() {
  midiA.read();
  checkForClock(); // are we receiving MIDI clock?
  sendMidi(); // we have to take the serial messages out of the interrupt callbacks - will definitely impact MIDI timing though ;^/
  checkSwitches(); // gets the current state of the buttons - defined in BUTTONS
  handlePresses(); // what to do with button presses - defined in BUTTONS
  checkKeyboard(); // checks the front-panel keyboard
  currentEnvelope(); // defined in ENVELOPE
  getPots(); // update the pot values - defined in POTS
  getMenu(); // defined in UI
  adjustValues(); // defined in POTS
  updateValues(); // defined in UI - only executes if the variable valueChange is set to true
  createSquareTable(constrain((pulseWidth + velPw), ((WAVE_SAMPLES / 2) - 10) * -1, (WAVE_SAMPLES / 2) - 10)); // have to call this in the loop for modulation - can't call it at lfo frequency
  arrowAnim(); // animate the arrow
  seqBlinker(); // blink the selected step in the sequencer
  updateLED(); // turn the LED on or off
}
