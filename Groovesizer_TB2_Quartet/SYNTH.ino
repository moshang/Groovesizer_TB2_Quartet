// *** WAVE SHAPES ***

// fill the note table with the phase increment values we require to generate the note
void createNoteTable(float fSampleRate)
{
  for (uint32_t unMidiNote = 0; unMidiNote < MIDI_NOTES; unMidiNote++)
  {
    float fFrequency = ((pow(2.0, (unMidiNote - 69.0) / 12.0)) * 440.0);
    nMidiPhaseIncrement[unMidiNote] = fFrequency * TICKS_PER_CYCLE;
  }
}

// create the individual samples for our sinewave table
void createSineTable()
{
  for (uint32_t nIndex = 0; nIndex < WAVE_SAMPLES; nIndex++)
  {
    // SINE
    nSineTable[nIndex] = (uint16_t)  (((1 + sin(((2.0 * PI) / WAVE_SAMPLES) * nIndex)) * 4095.0) / 2);
  }
}

void createSquareTable(int16_t pw)
{
  static int16_t lastPw = 127; // don't initialize to 0
  if (pw != lastPw)
  {
    for (uint32_t nIndex = 0; nIndex < WAVE_SAMPLES; nIndex++)
    {
      // SQUARE
      if (nIndex <= ((WAVE_SAMPLES / 2) + pw))
        nSquareTable[nIndex] = 0;
      else
        nSquareTable[nIndex] = 4095;
    }
    lastPw = pw;
  }
}

void createSawTable()
{
  for (uint32_t nIndex = 0; nIndex < WAVE_SAMPLES; nIndex++)
  {
    // SAW
    nSawTable[nIndex] = (4095 / WAVE_SAMPLES) * nIndex;
  }
}

void createTriangleTable()
{
  for (uint32_t nIndex = 0; nIndex < WAVE_SAMPLES; nIndex++)
  {
    // Triangle
    if (nIndex < WAVE_SAMPLES / 2)
      nTriangleTable[nIndex] = (4095 / (WAVE_SAMPLES / 2)) * nIndex;
    else
      nTriangleTable[nIndex] = (4095 / (WAVE_SAMPLES / 2)) * (WAVE_SAMPLES - nIndex);
  }
}

void clearUserTables()
{
  for (uint32_t nIndex = 0; nIndex < WAVE_SAMPLES; nIndex++)
  {
    nUserTable1[nIndex] = 2048; // 2048 is silence
    nUserTable2[nIndex] = 2048;
    nUserTable3[nIndex] = 2048;
  }
}

// *** SYNTH ENGINE ***

void audioHandler()
{
  for (int i = 0; i < 8; i++)
  {
    ulPhaseAccumulator[i] += ulPhaseIncrement[i];   // 32 bit phase increment, see below
  }

  // if the phase accumulator over flows - we have been through one cycle at the current pitch,
  // now we need to reset the grains ready for our next cycle
  for (int i = 0; i < 8; i++)
  {
    if (ulPhaseAccumulator[i] > SAMPLES_PER_CYCLE_FIXEDPOINT)
    {
      // DB 02/Jan/2012 - carry the remainder of the phase accumulator
      ulPhaseAccumulator[i] -= SAMPLES_PER_CYCLE_FIXEDPOINT;
    }
  }

  int32_t ulOutput[8];

  // get the current sample
  for (int i = 0; i < 8; i++)
  {
    switch (voiceSounding[i % 4])
    {
      case true:
        ulOutput[i] = *(wavePointer[i] + (ulPhaseAccumulator[i] >> 20));
        break;
      case false:
        ulOutput[i] = 2048; // 2048 is silence
        break;
    }
  }

  int32_t sampleOsc1;
  int32_t sampleOsc2;

  for (int i = 0; i < 4; i++)
  {
    sampleOsc1 += ulOutput[i];
  }
  sampleOsc1 = sampleOsc1 / 4;

  for (int i = 0; i < 4; i++)
  {
    sampleOsc2 += ulOutput[i + 4];
  }
  sampleOsc2 = sampleOsc2 / 4;

  // look up the volume for the current sample
  if (osc1WaveType == 7) // noise
    sampleOsc1 = osc1VolTable[random(0, 4096)]; //(((random(0, 4096) - 2048) * osc1Volume) >> 10) + 2048;
  else
    sampleOsc1 = osc1VolTable[sampleOsc1]; //(((sampleOsc1 - 2048) * osc1Volume) >> 10) + 2048;

  if (osc2WaveType == 7) // noise
    sampleOsc2 = osc2VolTable[random(0, 4096)]; //(((random(0, 4096) - 2048) * osc2Volume) >> 10) + 2048;
  else
    sampleOsc2 = osc2VolTable[sampleOsc2]; //(((sampleOsc2 - 2048) * osc2Volume) >> 10) + 2048;

  // get the panning for the two oscillators
  //int32_t sampleOutL = (((((sampleOsc1 - 2048) * (osc1PanL)) >> 10) + 2048) + ((((sampleOsc2 - 2048) * (osc2PanL)) >> 10) + 2048)) >> 1;
  //int32_t sampleOutR = (((((sampleOsc1 - 2048) * osc1PanR) >> 10) + 2048) + ((((sampleOsc2 - 2048) * osc2PanR) >> 10) + 2048)) >> 1;

  // mix the comboOscillators to one channel
  int32_t sampleMix = (sampleOsc1 + sampleOsc2) >> 1;

  // XOR mix
  // int32_t sampleMix = sampleOsc1 ^ sampleOsc2;


  // bitMucher
  int32_t bitMuncherOut = (((sampleMix - 2048) >> bitMuncher) << bitMuncher) + 2048;

  // get the current envelope volumes
  int32_t envelopeOut = (((bitMuncherOut - 2048) * envelopeVolume) >> 10) + 2048;
  //int32_t envelopeOutR = (((filterOutR - 2048) * envelopeVolume) >> 10) + 2048;

  // get the LFO volume
  int32_t lfoAmpOut = (((envelopeOut - 2048) * lfoAmp) >> 10) + 2048;

  // get the velocity volume
  int32_t velAmpOut = (((lfoAmpOut - 2048) * (1023 - velAmp)) >> 10) + 2048;

  // waveshaper
  int32_t waveShaperOut = waveShaper[velAmpOut];

  // get the filter
  int32_t filterOut = filterNextL(waveShaperOut);
  //int32_t filterOutL = filterNextL(sampleOutL);
  //int32_t filterOutR = filterNextR(sampleOutR);;

  // constrain the filter output
  if (filterOut > 4095) filterOut = 4095;
  //if (filterOutR > 4095) filterOutR = 4095;

  if (filterOut < 0) filterOut = 0;
  // if (filterOutR < 0) filterOutR = 0;

  // gain
  int32_t gainOut = gainTable[filterOut];

  // load mute - needed so we don't get load thunks when loading patches
  int32_t loadMuteOut = (((gainOut - 2048) * loadRampFactor) >> 10) + 2048;

  // volume
  int32_t volumeOut = (((loadMuteOut - 2048) * volume) >> 10) + 2048;

  // write to DAC0
  dacc_set_channel_selection(DACC_INTERFACE, 0);
  dacc_write_conversion_data(DACC_INTERFACE, volumeOut);
  // write to DAC1
  dacc_set_channel_selection(DACC_INTERFACE, 1);
  dacc_write_conversion_data(DACC_INTERFACE, volumeOut);
}

void assignVoices()
{
  static uint32_t incrementSource[8];
  static uint32_t incrementCurrent[8];
  static uint32_t incrementTarget[8];

  for (int i = 0; i < 8; i++)
  {
    if (voice[i % 4] != 255)
    {
      voiceSounding[i % 4] = true;
      if (i < 4)
      {
        incrementTarget[i] = nMidiPhaseIncrement[voice[i % 4] + (osc1OctaveOut * 12)  + osc1Detune];
      }
      else
      {
        incrementTarget[i] = nMidiPhaseIncrement[voice[i % 4] + (osc2OctaveOut * 12)] + osc2Detune;
      }
    }
  }

  if (portamento == 0 || millis() >= portaEndTime)
  {
    for (byte i = 0; i < 8; i++)
    {
        incrementCurrent[i] = incrementTarget[i];
        incrementSource[i] = incrementTarget[i];
    }
  }
  else
  {
    for (byte i = 0; i < 8; i++)
        incrementCurrent[i] = map(millis(), portaStartTime, portaEndTime, incrementSource[i], incrementTarget[i]);
  }

  if (monoMode && unison)
  {
    for (byte i = 0; i < 8; i++)
    {
      if (i < 4)
        incrementCurrent[i] += (uniSpread * i);
      else
        incrementCurrent[i] += (uniSpread * (i - 4));
    }
  }
  //MOD detune
  for (int i = 0; i < 8; i++)
  {
    if (i < 4)
      ulPhaseIncrement[i] = constrain(incrementCurrent[i] + lfoOsc1Detune + envOsc1Pitch + velOsc1Detune, 0, 178954880);
    else
      ulPhaseIncrement[i] = constrain(incrementCurrent[i] + lfoOsc2Detune + envOsc2Pitch + velOsc2Detune, 0, 178954880);
  }
}

void setOsc1WaveType(int shape)
{
  osc1WaveType = shape;
  for (int i = 0; i < 4; i++)
  {
    switch (shape)
    {
      case 0:  // sine
        wavePointer[i] = &nSineTable[0];
        break;
      case 1: // triangle
        wavePointer[i] = &nTriangleTable[0];
        break;
      case 2: // saw
        wavePointer[i] = &nSawTable[0];
        break;
      case 3: // square
        wavePointer[i] = &nSquareTable[0];
        break;
      case 4: // user1
        wavePointer[i] = &nUserTable1[0];
        break;
      case 5: // user2
        wavePointer[i] = &nUserTable2[0];
        break;
      case 6: // user3
        wavePointer[i] = &nUserTable3[0];
        break;
    }
  }
}

void setOsc2WaveType(int shape)
{
  osc2WaveType = shape;
  for (int i = 0; i < 4; i++)
  {
    switch (shape)
    {
      case 0:  // sine
        wavePointer[i + 4] = &nSineTable[0];
        break;
      case 1: // triangle
        wavePointer[i + 4] = &nTriangleTable[0];
        break;
      case 2: // saw
        wavePointer[i + 4] = &nSawTable[0];
        break;
      case 3: // square
        wavePointer[i + 4] = &nSquareTable[0];
        break;
      case 4: // user1
        wavePointer[i + 4] = &nUserTable1[0];
        break;
      case 5: // user2
        wavePointer[i + 4] = &nUserTable2[0];
        break;
      case 6: // user3
        wavePointer[i + 4] = &nUserTable3[0];
        break;
    }
  }
}

// lookup tables for OSC1 & OSC2 volume
void createOsc1Volume()
{
  for (int i = 0; i < 4096; i++)
    osc1VolTable[i] = (((i - 2048) * osc1Volume) >> 10) + 2048;
}

void createOsc2Volume()
{
  for (int i = 0; i < 4096; i++)
    osc2VolTable[i] = (((i - 2048) * osc2Volume) >> 10) + 2048;
}

void createGainTable()
{
  for (int i = 0; i < 4096; i++)
  {
    float tmp = (((float)i - 2048) * gainAmount) + 2048;
    // hard clip
    if (tmp > 4095)
      tmp = 4095;
    else if (tmp < 0)
      tmp = 0;
    gainTable[i] = (int)tmp;
  }
}
