void getPots() // read the current pot values
{
  for (byte i = 0; i < 5; i++)
  {
    pot[i] = analogRead(i);
  }

  // see if it's time to lock the pots
  if (lockTimer != 0 && (millis() - lockTimer) > 500) // the pots lock after 500ms
  {
    lockPot(5);
    if (menu != 0)
      valueChange = true;
  }
}

void lockPot(byte Pot) // values of 0 - 4 for each pot and 5 for all
{
  if (Pot == 5)
  {
    for (byte i = 0; i < 5; i++)
      potLock[i] = pot[i];
    lockTimer = 0;
  }
  else
    potLock[Pot] = pot[Pot];
}

boolean unlockedPot(byte Pot) // check if a pot is locked or not
{
  if (potLock[Pot] == 9999)
  {
    if (abs(pot[Pot] - lastPotValue[Pot]) > 10) // the threshold value is 10
    {
      lastPotValue[Pot] = pot[Pot];
      lockTimer = millis();
      if (Pot < 4)
        valueChange = true;
    }
    return true;
  }
  else if (abs(potLock[Pot] - pot[Pot]) > 10) // the threshold value is 10
  {
    potLock[Pot] = 9999;
    lockTimer = millis();
    lastPotValue[Pot] = pot[Pot];
    return true;
  }
  else
    return false;
}

void adjustValues()
{
  switch (menu)
  {
    case 0: // main
      if (unlockedPot(0))
      {
        if (splash)
        {
          customCharacters();
          splash = false;
          updateMenu();
        }
        else
          assignIncrementButtons(&mainMenu, 0, 3, 1);

        if (pot[0] < 1023 / 4) // there are 4 mainMenu items
        {
          if (mainMenu != 0)
          {
            mainMenu = 0;
            updateMenu();
          }
        }
        else if (pot[0] < (1023 / 4) * 2)
        {
          if (mainMenu != 1)
          {
            mainMenu = 1;
            updateMenu();
          }
        }
        else if (pot[0] < (1023 / 4) * 3)
        {
          if (mainMenu != 2)
          {
            mainMenu = 2;
            updateMenu();
          }
        }
        else
        {
          if (mainMenu != 3)
          {
            mainMenu = 3;
            updateMenu();
            settingsConfirm = false;
          }
        }
      }
      else if (unlockedPot(3))
      {
        if (splash)
        {
          customCharacters();
          splash = false;
          mainMenu = 0;
          updateMenu();
        }
        else
          assignIncrementButtons(&synPatchLoadSave, 0, 1, 1);
      }
      switch (mainMenu)
      {
        case 0: // Synth
          if (unlockedPot(3))
          {
            if (pot[3] < 512)
            {
              if (synPatchLoadSave != 0)
              {
                synPatchLoadSave = 0;
                updateMenu();
              }
            }
            else
            {
              if (synPatchLoadSave != 1)
              {
                synPatchLoadSave = 1;
                updateMenu();
              }
            }
          }
          break;
        case 2: // Sequencer
          if (unlockedPot(3))
          {
            assignIncrementButtons(&seqBankLoadSave, 0, 1, 1);

            if (pot[3] < 512)
            {
              if (seqBankLoadSave != 0)
              {
                seqBankLoadSave = 0;
                updateMenu();
              }
            }
            else
            {
              if (seqBankLoadSave != 1)
              {
                seqBankLoadSave = 1;
                updateMenu();
              }
            }
          }
          break;
      }
      break;
    case 10: // OSC1
      if (unlockedPot(0))
      {
        assignIncrementButtons(&osc1WaveType, 0, 7, 1);
        if (pot[0] < (1023 / waveshapes))
          setOsc1WaveType(0); // sine
        else if (pot[0] < (1023 / waveshapes) * 2)
          setOsc1WaveType(1); // triangle
        else if (pot[0] < (1023 / waveshapes) * 3)
          setOsc1WaveType(2); // saw
        else if (pot[0] < (1023 / waveshapes) * 4)
          setOsc1WaveType(3); // square
        else if (pot[0] < (1023 / waveshapes) * 5)
          setOsc1WaveType(4); // user1
        else if (pot[0] < (1023 / waveshapes) * 6)
          setOsc1WaveType(5); // user2
        else if (pot[0] < (1023 / waveshapes) * 7)
          setOsc1WaveType(6); // user3
        else
          setOsc1WaveType(7); // noise
      }

      if (unlockedPot(1))
      {
        assignIncrementButtons(&osc1Octave, 1, 9, 1);
        osc1Octave = map(pot[1], 0, 1023, 1, 9);
        if (osc1Octave != lastOsc1Octave)
        {
          lastOsc1Octave = osc1Octave;
          assignVoices();
        }
      }

      if (unlockedPot(2))
      {
        if (osc1Volume != pot[2])
        {
          osc1Volume = pot[2];
          createOsc1Volume();
        }
        assignIncrementButtons(&osc1Volume, 0, 1023, 4);
      }

      if (unlockedPot(3))
      {
        assignIncrementButtons(&osc1Detune, -24, +24, 1);
        osc1Detune = map(pot[3], 0, 1023, -24, 24);
        if (osc1Detune != lastOsc1Detune)
        {
          lastOsc1Detune = osc1Detune;
          assignVoices();
        }
      }
      break;

    case 11: // OSCILLATORS - set user wave shapes
      if (unlockedPot(3) && dirCount != 0)
      {
        justEnteredFolder = false;
        assignIncrementButtons(&dirChoice, 1, dirCount, 1);
        dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          prepNextChoice();
        }
      }
      break;

    case 12: // Squ: Pulse Width
      if (unlockedPot(3))
      {
        //valueChange = true;
        uiPulseWidth = map(pot[3], 0, 1023, ((WAVE_SAMPLES / 2) - 15) * -1, ((WAVE_SAMPLES / 2) - 15));
      }
      break;

    case 20: // OSC2
      if (unlockedPot(0))
      {
        assignIncrementButtons(&osc2WaveType, 0, 7, 1);
        if (pot[0] < (1023 / waveshapes))
          setOsc2WaveType(0); // sine
        else if (pot[0] < (1023 / waveshapes) * 2)
          setOsc2WaveType(1); // triangle
        else if (pot[0] < (1023 / waveshapes) * 3)
          setOsc2WaveType(2); // saw
        else if (pot[0] < (1023 / waveshapes) * 4)
          setOsc2WaveType(3); // square
        else if (pot[0] < (1023 / waveshapes) * 5)
          setOsc2WaveType(4); // user1
        else if (pot[0] < (1023 / waveshapes) * 6)
          setOsc2WaveType(5); // user2
        else if (pot[0] < (1023 / waveshapes) * 7)
          setOsc2WaveType(6); // user3
        else
          setOsc2WaveType(7); // noise
      }

      if (unlockedPot(1))
      {
        assignIncrementButtons(&osc2Octave, 1, 9, 1);
        osc2Octave = map(pot[1], 0, 1023, 1, 9);
        if (osc2Octave != lastOsc2Octave)
        {
          lastOsc2Octave = osc2Octave;
          assignVoices();
        }
      }

      if (unlockedPot(2))
      {
        if (osc2Volume != pot[2])
        {
          osc2Volume = pot[2];
          createOsc2Volume();
        }
        assignIncrementButtons(&osc2Volume, 0, 1023, 4);
      }

      if (unlockedPot(3))
      {
        assignIncrementButtons(&osc2Detune, -512 << 11, 512 << 11, 2 << 11);
        if (pot[3] > 502 && pot[3] < 522)
          osc2Detune = 0;
        else
          osc2Detune = int(pot[3] - 512) << 11;
        if (osc2Detune != lastOsc2Detune)
        {
          lastOsc2Detune = osc2Detune;
          assignVoices();
        }
      }
      break;

    case 21: // OSCILLATORS - set user wave shapes
      if (unlockedPot(3) && dirCount != 0)
      {
        justEnteredFolder = false;
        dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
          waveshapeLoaded = false;
        }
      }
      break;

    case 22: // Squ: Pulse Width
      if (unlockedPot(3))
      {
        //valueChange = true;
        uiPulseWidth = map(pot[3], 0, 1023, ((WAVE_SAMPLES / 2) - 15) * -1, ((WAVE_SAMPLES / 2) - 15));
      }
      break;

    case 30: // FILTER
      if (unlockedPot(0))
      {
        assignIncrementButtons(&filterCutoff, 0, 255, 1);
        //valueChange = true;
        float sensVar = (pot[0] >> 2) * 1.0;
        filterCutoff = pow(sensVar / 10, 1.71); //(base, exponent)
      }

      if (unlockedPot(1))
      {
        assignIncrementButtons(&filterResonance, 0, 255, 1);
        filterResonance = pot[1] >> 2;
        setFilterResonance(filterResonance);
      }

      if (unlockedPot(2))
      {
        assignIncrementButtons(&filterType, 0, 2, 1);
        if (pot[2] < 341)
          setFilterType(0);
        else if (pot[2] < 682)
          setFilterType(1);
        else
          setFilterType(2);
      }

      if (unlockedPot(3))
      {
        assignIncrementButtons(&filterBypass, 0, 1, 1); // a toggle value
        if (pot[3] < 512)
          filterBypass = true;
        else
          filterBypass = false;
      }
      break;

    case 40: // ENVELOPE
      if (unlockedPot(0))
      {
        attackTime = constrain(pot[0], 1, 1023); // we don't want to be able to set 0
        assignIncrementButtons(&attackTime, 1, 1023, 4);
      }
      if (unlockedPot(1))
      {
        decayTime = constrain(pot[1], 1, 1023); // we don't want to be able to set 0
        assignIncrementButtons(&decayTime, 1, 1023, 4);
      }
      if (unlockedPot(2))
      {
        sustainLevel = pot[2];
        assignIncrementButtons(&sustainLevel, 0, 1023, 4);
      }
      if (unlockedPot(3))
      {
        releaseTime = constrain(pot[3], 1, 1023); // we don't want to be able to set 0
        assignIncrementButtons(&releaseTime, 1, 1023, 4);
      }
      break;

    case 50: // LFO
      if (unlockedPot(0))
      {
        assignIncrementButtons(&lfoShape, 0, 6, 1);
        if (pot[0] < (1023 / 7))
        {
          if (lfoShape != 0)
          {
            lfoShape = 0; // sine
            setLfoShape(lfoShape);
          }
        }
        else if (pot[0] < (1023 / 7) * 2)
        {
          if (lfoShape != 1)
          {
            lfoShape = 1; // triangle
            setLfoShape(lfoShape);
          }
        }
        else if (pot[0] < (1023 / 7) * 3)
        {
          if (lfoShape != 2)
          {
            lfoShape = 2; // saw
            setLfoShape(lfoShape);
          }
        }
        else if (pot[0] < (1023 / 7) * 4)
        {
          if (lfoShape != 3)
          {
            lfoShape = 3; // square
            setLfoShape(lfoShape);
          }
        }
        else if (pot[0] < (1023 / 7) * 5)
        {
          if (lfoShape != 4)
          {
            lfoShape = 4; // user1
            setLfoShape(lfoShape);
          }
        }
        else if (pot[0] < (1023 / 7) * 6)
        {
          if (lfoShape != 5)
          {
            lfoShape = 5; // user2
            setLfoShape(lfoShape);
          }
        }
        else
        {
          if (lfoShape != 0)
          {
            {
              lfoShape = 6; // user3
              setLfoShape(lfoShape);
            }
          }
        }
      }
      if (unlockedPot(1))
      {
        if (pressed[14])
        {
          shiftR = true;
          lfoSync = true;
          getSyncSelector();
          updateLfoSyncTarget();
        }
        else
        {
          lfoSync = false;
          if (lfoLowRange)
          {
            tmpLfoRate = constrain(pot[1] >> 4, 0, 63);
            assignIncrementButtons(&tmpLfoRate, 0, 63, 1);
            lfoRate = 64 - tmpLfoRate;
            if (userLfoRate != lfoRate)
              userLfoRate = lfoRate;
          }
          else
          {
            tmpLfoRate = constrain(pot[1], 0, 1023); // 1024 here because we don't want the rate to ever be 0;
            assignIncrementButtons(&tmpLfoRate, 0, 1023, 4);
            lfoRate = 1024 - tmpLfoRate;
            if (userLfoRate != lfoRate)
              userLfoRate = lfoRate;
          }
          velLfoRate = lfoRate;
        }
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&lfoLowRange, 0, 1, 1);
        if (pot[2] < 512)
        {
          if (lfoRate > 31)
            lfoRate = 31;
          lfoLowRange = true;
        }
        else
          lfoLowRange = false;
      }
      if (unlockedPot(3))
      {
        if (pot[3] < 512)
          retrigger = true;
        else
          retrigger = false;
      }
      break;

    case 51: // LFO - set user wave shapes
      if (unlockedPot(3) && dirCount != 0)
      {
        justEnteredFolder = false;
        dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
          waveshapeLoaded = false;
        }
      }
      break;

    case 60: // MODULATION
      if (unlockedPot(0))
      {
        assignIncrementButtons(&source, 0, 2, 1);
        source = map(pot[0], 0, 1023, 0, 2);
      }

      if (unlockedPot(1))
      {
        switch (source)
        {
          case 0: // SOURCE LFO
            assignIncrementButtons(&destination, 0, 6, 1);
            dest = 1023 / 7; // how many destinations are there
            if (pot[1] < dest)
              destination = 0;
            else if (pot[1] < dest * 2)
              destination = 1;
            else if (pot[1] < dest * 3)
              destination = 2;
            else if (pot[1] < dest * 4)
              destination = 3;
            else if (pot[1] < dest * 5)
              destination = 4;
            else if (pot[1] < dest * 6)
              destination = 5;
            else
              destination = 6;
            break;

          case 1: // SOURCE ENVELOPE
            assignIncrementButtons(&destination, 7, 10, 1);
            dest = 1023 / 4; // how many destinations are there
            if (pot[1] < dest)
              destination = 7;
            else if (pot[1] < dest * 2)
              destination = 8;
            else if (pot[1] < dest * 3)
              destination = 9;
            else
              destination = 10;
            break;

          case 2: // SOURCE VELOCITY
            assignIncrementButtons(&destination, 11, 16, 1);
            dest = 1023 / 6; // how many destinations are there
            if (pot[1] < dest)
              destination = 11;
            else if (pot[1] < dest * 2)
              destination = 12;
            else if (pot[1] < dest * 3)
              destination = 13;
            else if (pot[1] < dest * 4)
              destination = 14;
            else if (pot[1] < dest * 5)
              destination = 15;
            else
              destination = 16;
            break;
        }
      }

      if (unlockedPot(2))
      {
        switch (source)
        {
          case 0: // SOURCE LFO
            switch (destination)
            {
              case 0:
                assignIncrementButtons(&lfoOsc1DetuneFactor, 0, 1023, 4);
                lfoOsc1DetuneFactor = pot[2];
                break;
              case 1:
                assignIncrementButtons(&lfoOsc2DetuneFactor, 0, 1023, 4);
                lfoOsc2DetuneFactor = pot[2];
                break;
              case 2:
                assignIncrementButtons(&osc1OctaveMod, 0, 3, 1);
                osc1OctaveMod = pot[2] >> 8;
                break;
              case 3:
                assignIncrementButtons(&osc2OctaveMod, 0, 3, 1);
                osc2OctaveMod = pot[2] >> 8;
                break;
              case 4:
                assignIncrementButtons(&lfoAmount, 0, 1023, 4);
                lfoAmount = pot[2];
                break;
              case 5:
                assignIncrementButtons(&lfoAmpFactor, 0, 1023, 4);
                lfoAmpFactor = pot[2];
                break;
              case 6:
                assignIncrementButtons(&lfoPwFactor, 0, 1023, 4);
                lfoPwFactor = pot[2];
                break;
            }
            break;

          case 1: // SOURCE ENVELOPE
            switch (destination)
            {
              case 7:
                assignIncrementButtons(&envOsc1PitchFactor, -1023, 1023, 4);
                if (pot[2] > 502 && pot[2] < 522) // snap to center / 0
                  envOsc1PitchFactor = 0;
                else
                  envOsc1PitchFactor = (pot[2] - 512) << 1; // - 1023 to 1023
                break;
              case 8:
                assignIncrementButtons(&envOsc2PitchFactor, -1023, 1023, 4);
                if (pot[2] > 502 && pot[2] < 522) // snap to center / 0
                  envOsc2PitchFactor = 0;
                else
                  envOsc2PitchFactor = (pot[2] - 512) << 1; // - 1023 to 1023
              case 9: // FILTER CUTOFF
                assignIncrementButtons(&envFilterCutoffFactor, -1023, 1023, 4);
                if (pot[2] > 502 && pot[2] < 522) // snap to center / 0
                  envFilterCutoffFactor = 0;
                else
                  envFilterCutoffFactor = (pot[2] - 512) << 1; // - 1023 to 1023
                break;
              case 10: // LFO RATE
                assignIncrementButtons(&envLfoRate, 0, 1023, 4);
                envLfoRate = pot[2];
                break;
            }
            break;

          case 2: // SOURCE VELOCITY
            switch (destination)
            {
              case 11:
                assignIncrementButtons(&velOsc1DetuneFactor, -1023, 1023, 8);
                velOsc1DetuneFactor = map(pot[2], 0, 1023, -1023, 1023);
                break;
              case 12:
                assignIncrementButtons(&velOsc2DetuneFactor, -1023, 1023, 8);
                velOsc2DetuneFactor = map(pot[2], 0, 1023, -1023, 1023);
                break;
              case 13:
                assignIncrementButtons(&velCutoffFactor, 0, 1023, 4);
                velCutoffFactor = pot[2];
                break;
              case 14:
                assignIncrementButtons(&velAmpFactor, 0, 1023, 4);
                velAmpFactor = pot[2];
                break;
              case 15:
                assignIncrementButtons(&velPwFactor, 0, (WAVE_SAMPLES / 2) - 10, 1);
                velPwFactor = map(pot[2], 0, 1023, 0, (WAVE_SAMPLES / 2) - 10);
                break;
              case 16:
                assignIncrementButtons(&velLfoRateFactor, 0, 1023, 4);
                velLfoRateFactor = pot[2];
                break;
            }
            break;
        }
      }
      break;

    case 65: // SHAPER & GAIN
      if (unlockedPot(0))
      {
        assignIncrementButtons(&shaperType, 0, 2, 1);
        int items = 3;
        if (shaperType != (pot[0] / ((1023 / items) + 1)))
        {
          shaperType = pot[0] / ((1023 / items) + 1);
          createWaveShaper();
        }
      }
      if (unlockedPot(1))
      {
        if (shaperType == 1)
        {
          assignIncrementButtons(&shaperType1PotVal, 0, 1023, 4);
          if (pot[1] != shaperType1PotVal)
          {
            shaperType1PotVal = pot[1];
            float tmp = (float)shaperType1PotVal / 1024;
            waveShapeAmount = tmp;
            createWaveShaper();
          }
        }
        else if (shaperType == 2)
        {
          assignIncrementButtons(&waveShapeAmount2, 1, 10, 1);
          int tmp = map(pot[1], 0, 1023, 1, 10);
          if (tmp != waveShapeAmount2)
          {
            waveShapeAmount2 = tmp;
            createWaveShaper();

          }
        }
      }

      if (unlockedPot(2))
      {
        assignIncrementButtons(&bitMuncher, 0, 11, 1);
        int values = 11;
        if (bitMuncher != (pot[2] / ((1023 / values) + 1)))
          bitMuncher = pot[2] / ((1023 / values) + 1);
      }

      if (unlockedPot(3))
      {
        assignIncrementButtons(&gainAmountPotVal, 0, 1023, 4);
        if (pot[3] != gainAmountPotVal)
        {
          gainAmountPotVal = pot[3];
          float tmp = (float)gainAmountPotVal / 1024;
          tmp += 1.0;
          gainAmount = tmp;
          createGainTable();
        }
      }
      break;

    case 68: // MONO & PORTA
      if (unlockedPot(0))
      {
        assignIncrementButtons(&monoMode, 0, 3, 1);
        int values = 4;
        byte tmp = constrain(pot[0] / ((1023 / values) + 1), 0, 3);
        if (monoMode != tmp)
        {
          monoMode = tmp;
          clearCurrentNotes(); // clears the currentNotes table and resets variables that keep track of incoming MIDI notes 
        }
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&unison, 0, 3, 1);
        int values = 4;
        byte tmp = constrain(pot[1] / ((1023 / values) + 1), 0, 3);
        if (unison != tmp)
          unison = tmp;
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&uniSpread, 10000, 60000, 200);
        int tmp = map(pot[2], 0, 1023, 10000, 60000);
        if (uniSpread != tmp)
          uniSpread = tmp;
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&portamento, 0, 255, 1);
        if (portamento != constrain(pot[3] >> 2, 0, 255))
          portamento = constrain(pot[3] >> 2, 0, 255);
      }
      break;

    case 70: // LOAD SYNTH PATCH
      if (unlockedPot(3) && dirCount != 0)
      {
        int potSteps = 1023 / dirCount;
        dirChoice = constrain((pot[3] / potSteps) + 1, 1, dirCount);
        //dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          synthPatchLoaded = false;
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
          justEnteredFolder = false;
        }
      }
      break;

    case 80: // SAVE SYNTH PATCH
      if (unlockedPot(3) && dirCount != 0)
      {
        int potSteps = 1023 / dirCount;
        dirChoice = constrain((pot[3] / potSteps) + 1, 1, dirCount);
        //dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
        }
      }
      break;

    case 100: // ARP SETTINGS 1
      if (unlockedPot(0)) // arp on/off
      {
        assignIncrementButtons(&arp, 0, 1, 1);
        if (pot[0] < 512)
        {
          if (arp == true)
          {
            arp = false;
            clearHeld();
          }
        }
        else
          arp = true;
      }
      if (unlockedPot(1)) // time division
      {
        assignIncrementButtons(&arpDivSelection, 0, 6, 1);
        arpDivSelection = map(pot[1], 0, 1023, 0, 6);
      }
      if (unlockedPot(2)) // step duration in pulses
      {
        assignIncrementButtons(&arpNoteDur, 2, 95, 1);
        arpNoteDur = map(pot[2], 0, 1023, 2, 95);
      }
      if (unlockedPot(3)) // set the BPM
      {
        assignIncrementButtons(&bpm, 20, 320, 1);
        bpm = map(pot[3], 0, 1023, 20, 320);
        if (bpm != lastBpm)
        {
          lastBpm = bpm;
          setBpm();
        }
      }
      break;

    case 110: // ARP SETTINGS 2
      if (unlockedPot(0)) // direction
      {
        assignIncrementButtons(&arpForward, 0, 1, 1);
        if (pot[0] < 512)
        {
          if (arpForward != 0)
          {
            arpForward = 0;
            sortArp();
          }
        }
        else
        {
          if (arpForward != 1)
          {
            arpForward = 1;
            sortArp();
          }
        }
      }
      if (unlockedPot(1)) // type
      {
        assignIncrementButtons(&arpIncrement, 0, 4, 1);
        arpIncrement = map(pot[1], 0, 1023, 0, 4);
      }
      if (unlockedPot(2)) // octaves
      {
        assignIncrementButtons(&arpOctaves, 1, 4, 1);
        arpOctaves = map(pot[2], 0, 1023, 1, 4);
      }
      break;

    case 200: // SEQUENCER TRIGGER
      if (unlockedPot(0))
      {
        if (pot[0] < 512)
        {
          if (seqRunning != 0)
            seqPlayStop();
        }
        else
        {
          if (!seqRunning)
            seqPlayStop();
        }
      }

      if (unlockedPot(1))
      {
        assignIncrementButtons(&seqPlayMode, 0, 5, 1);
        seqPlayMode = map(pot[1], 0, 1023, 0, 5);
      }

      if (unlockedPot(2))
      {
        assignIncrementButtons(&seq[currentSeq].transpose, -36, 36, 1);
        seq[currentSeq].transpose = map(pot[2], 0, 1023, -36, 36);
      }
      break;

    case 210: // SEQUENCER EDIT
      if (unlockedPot(0))
      {
        if (pot[0] < 512)
        {
          if (seqRunning != 0)
            seqPlayStop();
        }
        else
        {
          if (!seqRunning)
            seqPlayStop();
        }
      }
      if (unlockedPot(1))
      {
        clearStep();
        lockPot(1);
        valueChange = true;
      }
      if (unlockedPot(2))
      {
        if (pot[2] < 341)
        {
          seq[currentSeq].tie[seqEditStep] = 0;
          seq[currentSeq].mute[seqEditStep] = 0;
        }
        else if (pot[2] < 682)
        {
          seq[currentSeq].tie[seqEditStep] = 1;
          seq[currentSeq].mute[seqEditStep] = 0;
        }
        else if (pot[2] < 1023)
        {
          seq[currentSeq].tie[seqEditStep] = 0;
          seq[currentSeq].mute[seqEditStep] = 1;
        }
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&seqEditStep, 0, 15, 1);
        int tmp = constrain(pot[3] / (1023 / 16), 0, 15);
        if (seqEditStep != tmp)
        {
          seqEditStep = tmp;
          seqBlink = 0; // so the just selected step is switched off
          seqBlinkCounter = 7000; // so it stays off for just a little bit before it's switched back on
          valueChange = true;
        }
      }
      break;

    case 211: // SEQUENCER EDIT STEP
      if (unlockedPot(0))
      {
        assignIncrementButtons(&seq[currentSeq].voice[0][seqEditStep], -39, 48, 1);
        if (pot[0] < 100)
        {
          seq[currentSeq].voice[0][seqEditStep] = 255;
          valueChange = true;
        }
        else
        {
          int tmpNote = map(pot[0], 100, 1023, 21, 108) - 60;
          if (tmpNote != seq[currentSeq].voice[0][seqEditStep])
          {
            seq[currentSeq].voice[0][seqEditStep] = tmpNote;
            valueChange = true;
          }
        }
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&seq[currentSeq].voice[1][seqEditStep], -39, 48, 1);
        if (pot[1] < 100)
        {
          seq[currentSeq].voice[1][seqEditStep] = 255;
          valueChange = true;
        }
        else
        {
          int tmpNote = map(pot[1], 100, 1023, 21, 108) - 60;
          if (tmpNote != seq[currentSeq].voice[1][seqEditStep])
          {
            seq[currentSeq].voice[1][seqEditStep] = tmpNote;
            valueChange = true;
          }
        }
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&seq[currentSeq].voice[2][seqEditStep], -39, 48, 1);
        if (pot[2] < 100)
        {
          seq[currentSeq].voice[2][seqEditStep] = 255;
          valueChange = true;
        }
        else
        {
          int tmpNote = map(pot[2], 100, 1023, 21, 108) - 60;
          if (tmpNote != seq[currentSeq].voice[2][seqEditStep])
          {
            seq[currentSeq].voice[2][seqEditStep] = tmpNote;
            valueChange = true;
          }
        }
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&seq[currentSeq].voice[3][seqEditStep], -39, 48, 1);
        if (pot[3] < 100)
        {
          seq[currentSeq].voice[3][seqEditStep] = 255;
          valueChange = true;
        }
        else
        {
          int tmpNote = map(pot[3], 100, 1023, 21, 108) - 60;
          if (tmpNote != seq[currentSeq].voice[3][seqEditStep])
          {
            seq[currentSeq].voice[3][seqEditStep] = tmpNote;
            valueChange = true;
          }
        }
      }
      break;

    case 220: // SEQUENCER EDIT STEP
      if (unlockedPot(0))
      {
        assignIncrementButtons(&seq[currentSeq].patternLength, 1, 16, 1);
        seq[currentSeq].patternLength = map(pot[0], 0, 1023, 1, 16);
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&seq[currentSeq].noteDur, 0, 1023, 4);
        seq[currentSeq].noteDur = pot[1];
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&seq[currentSeq].swing, 0, 1023, 4);
        seq[currentSeq].swing = pot[2];
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&bpm, 20, 320, 1);
        bpm = map(pot[3], 0, 1023, 20, 320);
        if (bpm != lastBpm)
        {
          lastBpm = bpm;
          setBpm();
          seq[currentSeq].bpm = bpm;
        }
      }
      break;

    case 230: // SEQUENCER BANK SETTINGS
      if (unlockedPot(0))
      {
        int tmpMode = map(pot[0], 0, 1023, 0, 4);
        if (bankMode != tmpMode)
          bankMode = tmpMode;
        //valueChange = true;
      }
      break;

    case 250: // LOAD SEQ BANK
      if (unlockedPot(3) && dirCount != 0)
      {
        justEnteredFolder = false;
        dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          seqBankLoaded = false;
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
        }
      }
      break;

    case 260: // SAVE SEQ BANK
      if (unlockedPot(3) && dirCount != 0)
      {
        dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
        }
      }
      break;

    case 300: // SETTINGS MIDI
      if (unlockedPot(0))
      {
        assignIncrementButtons(&midiOut, 0, 1, 1);
        int tmp = (pot[0] < 512) ? 0 : 1;
        if (midiOut != tmp)
        {
          midiOut = tmp;
          valueChange;
        }
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&midiChannel, 1, 16, 1);
        int tmp = map(pot[1], 0, 1023, 1, 16);
        if (midiChannel != tmp)
        {
          midiChannel = tmp;
          valueChange;
        }
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&midiThruType, 0, 2, 1);
        byte items = 3;
        int tmp = (pot[2] / (1022 / items) < (items - 1)) ? pot[2] / (1022 / items) : items - 1;
        if (midiThruType != tmp)
        {
          midiThruType = tmp;
          checkThru();
          valueChange;
        }
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&midiSync, 0, 2, 1);
        int tmp = (pot[3] / (1027 / 3));
        if (midiSync != tmp)
        {
          midiSync = tmp;
          setSyncType();
        }
      }
      break;

    case 310: // SETTINGS KEYBOARD
      if (unlockedPot(0))
      {
        assignIncrementButtons(&keysOut, 0, 1, 1);
        if (pot[0] < 512)
          keysOut = 0;
        else
          keysOut = 1;
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&keyVelocity, 0, 127, 1);
        if (keyVelocity != pot[1] >> 3)
          keyVelocity = pot[1] >> 3;
      }
      break;
    case 320: // // SETTINGS TRIGGER MIDI
      if (unlockedPot(0))
      {
        assignIncrementButtons(&midiTriggerOut, 0, 1, 1);
        if (pot[0] < 512)
          midiTriggerOut = 0;
        else
          midiTriggerOut = 1;
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&midiTriggerChannel, 1, 16, 1);
        int tmp = (pot[1] / (1024 / 16)) + 1;
        if (tmp != midiTriggerChannel)
          midiTriggerChannel = tmp;
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&editTrigger, 0, 7, 1);
        int tmp = pot[2] / (1024 / 8);
        if (tmp != editTrigger)
          editTrigger = tmp;
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&midiTrigger[editTrigger], 0, 127, 1);
        int tmp = pot[3] >> 3;
        if (tmp != midiTrigger[editTrigger])
          midiTrigger[editTrigger] = tmp;
      }
      break;
    case 330: // // SETTINGS GENERAL
      if (unlockedPot(0))
      {
        assignIncrementButtons(&volume, 0, 1023, 4);
        if (pot[0] != volume)
          volume = pot[0];
      }
      break;
  }
}

void getMenu()
{
  switch (mainMenu)
  {
    case 0: // SYNTH
      menuPages = 9;
      if (unlockedPot(4)) // select the menu page
      {
        assignIncrementButtons(&menuChoice, 0, 8, 1);
        int tmp = 1023 / menuPages;
        menuChoice = constrain(pot[4] / tmp, 0, menuPages - 1);
        menu = synthMenu[menuChoice];
        if (menu != lastMenu)
        {
          lastMenu = menu;

          for (byte i = 0; i < 4; i++)
            lockPot(i);
          updateMenu();
          if (splash)
          {
            customCharacters();
            splash = false;
          }
        }
        else if (lastMainMenu != mainMenu)
        {
          lastMainMenu = mainMenu;
          lockPot[5];
        }
      }
      if (menu == 50) // lfo page
        lfoLED = true;
      else
        lfoLED = false;
      break;

    case 1: // ARP
      menuPages = 3;
      if (unlockedPot(4)) // select the menu page
      {
        assignIncrementButtons(&menuChoice, 0, 2, 1);
        int tmp = 1023 / menuPages;
        menuChoice = constrain(pot[4] / tmp, 0, menuPages - 1);
        menu = arpMenu[menuChoice];
        if (menu != lastMenu)
        {
          lastMenu = menu;
          for (byte i = 0; i < 4; i++)
            lockPot(i);
          updateMenu();
        }
      }
      break;

    case 2: // SEQUENCER
      menuPages = 5;
      if (unlockedPot(4)) // select the menu page
      {
        assignIncrementButtons(&menuChoice, 0, 4, 1);
        int tmp = 1023 / menuPages;
        menuChoice = constrain(pot[4] / tmp, 0, menuPages - 1);
        menu = seqMenu[menuChoice];
        if (menu != lastMenu)
        {
          lastMenu = menu;
          for (byte i = 0; i < 4; i++)
            lockPot(i);
          if (menu == 210)
            doSeqBlink = false; // so the page name doesn't blink
          updateMenu();
        }
      }
      break;

    case 3: // SETTINGS
      menuPages = 5;
      if (unlockedPot(4)) // select the menu page
      {
        assignIncrementButtons(&menuChoice, 0, 4, 1);
        int tmp = 1023 / menuPages;
        menuChoice = constrain(pot[4] / tmp, 0, menuPages - 1);
        menu = settingsMenu[menuChoice];
        if (menu != lastMenu)
        {
          lastMenu = menu;
          for (byte i = 0; i < 4; i++)
            lockPot(i);
          updateMenu();
        }
      }
      break;
  }
}

void getSyncSelector()
{
  int lastSyncSelector = syncSelector;
  syncSelector = constrain((pot[1] / 204), 0, 4);
  if (syncSelector == lastSyncSelector)
    lockPot(5);
  valueChange = true;
}
