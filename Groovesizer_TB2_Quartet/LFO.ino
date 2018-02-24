static uint16_t lfoCounter = 0;

void lfoHandler()
{
  // *** LFO ***
  if (!lfoSync)
  {
    lfoCounter++;
    if (lfoCounter >= velLfoRate)
      updateLFO();
  }
  else // lfo is synced to tempo
  {
    lfoSyncCounter++;
    if (lfoSyncCounter >= lfoSyncTarget)
      updateLFO();
  }


  // for the arrow animation
  static uint16_t arrowCounter = 0;
  arrowCounter++;
  if (arrowCounter >= 2200) // 10 times a second
  {
    arrowFrame = (arrowFrame < 9) ? arrowFrame + 1 : 0;
    arrowCounter = 0;
  }

  // to blink selected steps in sequencer edit mode
  seqBlinkCounter++;
  if (seqBlinkCounter >= 11000) // twice a second
  {
    seqBlink = !seqBlink;
    seqBlinkCounter = 0;
  }

  // *** DISPLAY REFRESH ***
  static uint16_t refreshCounter = 0;
  refreshCounter++;
  if (refreshCounter >= 2200) // 22000 / 10 = 2200, so we refresh the the display 10 times a second
  {
    uiRefresh = true;
    refreshCounter = 0;
  }
}

void updateLFO()
{
  tmpLFO = *(lfoShapePointer + lfoIndex);
  lfoIndex = (lfoIndex < 599) ? lfoIndex + 1 : 0;

  // *** OSC1 PITCH ***
  if (lfoOsc1DetuneFactor < 200) // so the pitch modulation is less dramatic at low levels
    lfoOsc1Detune = ((tmpLFO - 2048) * lfoOsc1DetuneFactor) << 1; // needs to be a positive and negative value
  else
    lfoOsc1Detune = ((tmpLFO - 2048) * lfoOsc1DetuneFactor) << 2; // needs to be a positive and negative value

  // *** OSC2 PITCH ***
  if (lfoOsc2DetuneFactor < 200)
    lfoOsc2Detune = ((tmpLFO - 2048) * lfoOsc2DetuneFactor) << 1;
  else
    lfoOsc2Detune = ((tmpLFO - 2048) * lfoOsc2DetuneFactor) << 2;

  // *** FILTER CUTOFF ***
  tmpCutoff = ((tmpLFO - 2048) * lfoAmount) >> 14; // value centered around 0 (positive and negative)
  setFilterCutoff(constrain((filterCutoff + tmpCutoff + envFilterCutoff + velCutoff), 0, 255)); // center  the LFO amount around the current filter cutoff setting

  // *** AMPLITUDE ***
  lfoAmp = 1023 - (((tmpLFO >> 2) * lfoAmpFactor) >> 10);

  // *** PULSE WIDTH ***
  if (lfoPwFactor > 10)
  {
    pulseWidth = ((tmpLFO - 2048) * lfoPwFactor) >> 13;
  }
  else
    pulseWidth = uiPulseWidth;

  if (envelopeProgress != 255) // the envelope is not idle
  {
    assignVoices(); // have to call this in the LFO so pitch modulation is updated
    // *** OSC1 OCTAVE ***
    osc1OctaveOut = osc1Octave + map(tmpLFO, 0, 4095, osc1OctaveMod * -1, osc1OctaveMod);
    constrain(osc1OctaveOut, 1, 9);

    // *** OSC2 OCTAVE ***
    osc2OctaveOut = osc2Octave + map(tmpLFO, 0, 4095, osc2OctaveMod * -1, osc2OctaveMod);
    constrain(osc2OctaveOut, 1, 9);
  }
  if (!lfoSync)
    lfoCounter = 0;
  else
    lfoSyncCounter = 0;
}

void setLfoShape(byte shape)
{
  switch (shape)
  {
    case 0: // sine
      lfoShapePointer = &nSineTable[0];
      break;
    case 1: // triangle
      lfoShapePointer = &nTriangleTable[0];
      break;
    case 2: // saw
      lfoShapePointer = &nSawTable[0];
      break;
    case 3: // square
      lfoShapePointer = &nSquareTable[0];
      break;
    case 4: // user1
      lfoShapePointer = &nUserTable1[0];
      break;
    case 5: // user2
      lfoShapePointer = &nUserTable2[0];
      break;
    case 6: // user3
      lfoShapePointer = &nUserTable3[0];
      break;
  }
}

void updateLfoSyncTarget()
{
  lfoSyncTarget = bpmPeriod * syncTicks[syncSelector] / 600 / 45; // 600 samples in the lfo lookup table, 45microseconds for each lfo tick at 22kHz
  lfo8thSync = syncTicks[syncSelector] / 48;
}
