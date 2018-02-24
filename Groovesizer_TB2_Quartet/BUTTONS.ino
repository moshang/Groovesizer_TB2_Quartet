void checkSwitches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  if (millis() < lasttime)
  {
    // we wrapped around, lets just try again
    lasttime = millis();
  }

  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return;
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();

  for (index = 0; index < NUMBUTTONS; index++) // when we start, we clear out the "just" indicators
  {
    justreleased[index] = 0;
    justpressed[index] = 0;


    currentstate[index] = digitalRead(buttons[index]);   // read the button

    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just released
        justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }

    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}

void clearJust()
{
  for (byte index = 0; index < NUMBUTTONS; index++) // when we start, we clear out the "just" indicators
  {
    justreleased[index] = 0;
    justpressed[index] = 0;
  }
}

void handlePresses()
{
  unSplash(); // check if we're on the splash page and deal with it

  if (pressed[13] && justpressed[14]) // fine adjust increment
  {
    if ((*adjustValue + increment) <= upperLimit)
      *adjustValue += increment;
    else
      *adjustValue = lowerLimit;

    incDecSpecials();
    shiftL = true;
    clearJust();
  }

  if (pressed[14] && justpressed[13]) // fine adjust decrement
  {
    if ((*adjustValue - increment) >= lowerLimit)
      *adjustValue -= increment;
    else
      *adjustValue = upperLimit;

    incDecSpecials();
    shiftR = true;
    clearJust();
  }

  if (justreleased[14]) // enter key
  {
    if (!shiftL && !shiftR)
    {
      switch (menu)
      {
        case 0: // SPLASH/MAIN
          switch (mainMenu)
          {
            case 0: // SYNTH
              if (synPatchLoadSave == 0) // LOAD
              {
                menu = 70;
                gotoRootDir();
                getDirCount();
                assignIncrementButtons(&dirChoice, 1, dirCount, 1);
              }
              else // SAVE
              {
                gotoRootDir();
                menu = 80;
                getDirCount();
                assignIncrementButtons(&dirChoice, 1, dirCount, 1);
              }
              valueChange = true;
              break;

            case 2: // SEQUENCER
              if (seqBankLoadSave == 0) // LOAD
              {
                gotoRootDir();
                menu = 250;
                getDirCount();
                assignIncrementButtons(&dirChoice, 1, dirCount, 1);
              }
              else // SAVE
              {
                gotoRootDir();
                menu = 260;
                getDirCount();
                assignIncrementButtons(&dirChoice, 1, dirCount, 1);
              }
              valueChange = true;
              break;

            case 3: // SETTINGS
              if (settingsConfirm)
              {
                saveSettings();
                settingsConfirm = false;
              }
              else
              {
                settingsConfirm = true;
                updateMenu();
                lcd.setCursor(0, 1);
                lcd.print("    Overwrite?  ");
              }
              break;
          }
          break;

        case 10: // OSC1
          if (osc1WaveType == 4)
          {
            menu = 11;
            gotoRootDir();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            valueChange = true;
          }
          else if (osc1WaveType == 3)
          {
            menu = 12;
            lockPot(5);
            assignIncrementButtons(&uiPulseWidth, -285, 285, 2);
            valueChange = true;
            clearLCD();
          }
          break;

        case 11: // OSC1 - choose user waveshape
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          break;

        case 20: // OSC2
          if (osc2WaveType == 5)
          {
            menu = 21;
            gotoRootDir();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            valueChange = true;
          }
          else if (osc2WaveType == 3)
          {
            menu = 22;
            lockPot(5);
            assignIncrementButtons(&uiPulseWidth, -285, 285, 2);
            valueChange = true;
            clearLCD();
          }
          break;

        case 21: // OSC2 - choose user waveshape
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          break;

        case 50:
          if (lfoShape == 6)
          {
            menu = 51;
            gotoRootDir();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            valueChange = true;
          }
          break;

        case 51: // LFO - choose user waveshape
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          break;

        case 70: // LOAD PATCH
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          lockPot(5);
          break;

        case 80: // SAVE PATCH
          if (!inFolder)
          {
            setFolder();
            justEnteredFolder = true;
          }
          else if (saveConfirm)
          {
            saveConfirm = false;
            savePatch();
          }
          else
          {
            if (strcmp(fileName, saveName) == 0)
              savePatch();
            else
            {
              saveConfirm = true;
              lcd.setCursor(4, 1);
              lcd.print("Overwrite?  ");
            }
          }
          lockPot(5);
          break;

        case 200: // SEQUENCE TRIGGER
          menu = 210;
          valueChange = true;
          break;


        case 210: // SEQUENCE EDIT
          {
            if (!midiMode)
            {
              boolean keyNotes = false;
              for (byte i = 0; i < 13; i++)
              {
                if (pressed[i])
                  keyNotes = true;
              }
              if (keyNotes)
              {
                updateSeqNotes();
                seqEditStep = (seqEditStep + 1) % 16; // increment the edit step
              }
              else // no front pane keyboard keys are held
              {
                menu = 211;
                assignIncrementButtons(&seq[currentSeq].voice[0][seqEditStep], -39, 48, 1);
                clearLCD();
                valueChange = true;
              }
            }
            else // MIDI mode
            {
              if (updateSeqNotes()) // will return true if there were notes held and added to sequencer
                seqEditStep = (seqEditStep + 1) % 16; // increment the edit step
              else
              {
                menu = 211;
                assignIncrementButtons(&seq[currentSeq].voice[0][seqEditStep], -39, 48, 1);
                clearLCD();
                valueChange = true;
              }
            }
          }
          break;

        case 250: // LOAD SEQ BANK
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          else if (seqBankLoaded)
          {
            unpackSeqBankBuffer();
            lcd.setCursor(4, 1);
            lcd.print("Loaded!     ");
          }
          lockPot(5);
          break;

        case 260: // SAVE SEQ BANK
          if (!inFolder)
          {
            setFolder();
            justEnteredFolder = true;
          }
          else if (saveConfirm)
          {
            saveConfirm = false;
            saveBank();
          }
          else
          {
            if (strcmp(fileName, saveName) == 0)
              saveBank();
            else
            {
              saveConfirm = true;
              lcd.setCursor(4, 1);
              lcd.print("Overwrite?  ");
            }
          }
          lockPot(5);
          break;

      }
    }
    else if (shiftR)
      shiftR = false;
    clearJust();
  }

  if (justreleased[13]) // back key
  {
    if (!shiftL && !shiftR)
    {
      switch (menu)
      {
        case 11: // OSC1 - choose user waveshape
          if (inFolder)
          {
            setFolder();
            getDirCount();
          }
          else
          {
            menu = 10;
            assignIncrementButtons(&osc1WaveType, 0, 7, 1);
            valueChange = true;
            clearLCD();
          }
          break;

        case 12: // Squ Pulse Width
          menu = 10;
          assignIncrementButtons(&osc1WaveType, 0, 7, 1);
          valueChange = true;
          lockPot(5);
          clearLCD();
          break;

        case 21: // OSC2 - choose user waveshape
          if (inFolder)
          {
            setFolder();
            getDirCount();
          }
          else
          {
            menu = 20;
            assignIncrementButtons(&osc2WaveType, 0, 7, 1);
            valueChange = true;
            clearLCD();
          }
          break;

        case 22: // Squ Pulse Width
          menu = 20;
          assignIncrementButtons(&osc2WaveType, 0, 7, 1);
          valueChange = true;
          lockPot(5);
          clearLCD();
          break;

        case 51: // LFO - choose user waveshape
          if (inFolder)
          {
            setFolder();
            getDirCount();
          }
          else
          {
            menu = 50;
            assignIncrementButtons(&lfoShape, 0, 6, 1);
            valueChange = true;
            clearLCD();
          }
          break;

        case 70: // LOAD PATCH
          if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            clearLCD();
            updateMenu();
          }
          lockPot(5);
          break;

        case 80: // SAVE PATCH
          if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            clearLCD();
            updateMenu();
          }
          lockPot(5);
          break;

        case 200: // SEQUENCE TRIGGER
          seqPlayStop();
          valueChange = true;
          break;

        case 210: // SEQUENCE EDIT
          seqPlayStop();
          valueChange = true;
          break;

        case 211: // SEQUENCE EDIT NOTES
          menu = 210;
          assignIncrementButtons(&seqEditStep, 0, 15, 1);
          noteRelease();
          clearLCD();
          valueChange = true;
          break;

        case 250: // LOAD SEQ BANK
          if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            clearLCD();
            updateMenu();
          }
          lockPot(5);
          break;

        case 260: // SAVE SEQ BANK
          if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            clearLCD();
            updateMenu();
          }
          lockPot(5);
          break;

      }
    }
    else if (shiftL)
      shiftL = false;
    clearJust();
  }

  switch (menu)
  {
    case 200: // SEQ TRIGGER
      for (byte i = 0; i < 8; i++)
      {
        if (justpressed[whiteButtons[i]])
        {
          if (!seqRunning)
          {
            sourceSeq = currentSeq;
            currentSeq = i;
            selectedSeq = i;
            destinationSeq = selectedSeq;
            seqUpdateDisplay =  true;
            valueChange = true;
          }
          else
          {
            selectedSeq = i;
            sourceSeq = currentSeq;
            destinationSeq = selectedSeq;
            valueChange = true;
          }
          if (midiTriggerOut)
            midiA.sendNoteOn(midiTrigger[i], 127, midiTriggerChannel);
          clearJust();
          longPress = millis();
          copied = false;
        }
        if (pressed[whiteButtons[i]])
        {
          if ((millis() - longPress) > 500 && !copied)
            copySeq();
        }
      }
      for (byte i = 0; i < 5; i++)
      {
        if (pressed[14])
        {
          if (justpressed[1])
          {
            clearSeq();
            clearJust();
            valueChange = true;
            shiftR = true;
          }
        }
        else if (justpressed[blackButtons[i]])
        {
          switch (i)
          {
            case 0:
              seq[currentSeq].transpose--;
              if (seqRunning)
                seqUpdateDisplay =  true;
              else
                valueChange = true;
              break;
            case 1:
              seq[currentSeq].transpose++;
              if (seqRunning)
                seqUpdateDisplay =  true;
              else
                valueChange = true;
              break;
              clearJust();
          }
        }
      }
      break;

    case 210: // SEQ EDIT
      break;

    case 211: // SEQ NOTE ENTRY
      break;
  }
}

void checkKeyboard()
{
  for (byte i = 0; i < 13; i++)
  {
    if (justpressed[i])
    {
      if (midiMode) // switch to listening to notes from front-panel keyboard
      {
        midiMode = false;
        if (lastFilterCutoff != 0)
          filterCutoff = lastFilterCutoff;
        clearHeld();
      }
      //clearJust();
    }
  }
  if (!midiMode)
  {
    if (soundKeys && menu != 200)
    {
      if (!monoMode) // ie. monoMode = 0
      {
        voiceCounter = 4; // how mnany voices are used?
        for (byte i = 0; i < 4; i++) // check if a previously held note has been released
        {
          if (voice[i] != 255)
          {
            if (!pressed[voice[i]])
            {
              keyAssigned[voice[i]] = false;
              voice[i] = 255;
              voiceCounter--;
              if (midiOut && keysOut)
                midiA.sendNoteOff(keyboardOut[i], outVelocity, midiChannel);
            }
          }
          else //
            voiceCounter--;
        }

        if (voiceCounter == 0 && lastVoiceCount != 0)
          noteRelease();

        for (byte i = 0; i < 13; i++) // assign note to empty voice
        {
          if (!keyAssigned[i]) // if this key isn't already assigned, look for an empty voice for it
          {
            if (pressed[i])
            {
              byte j = 0;
              while (voice[j] != 255 && j < 4) // go to the next voice if this voice isn't free
                j++;
              if (j < 4)
              {
                if (midiOut && keysOut)
                {
                  keyboardOut[j] = i + 60;
                  midiA.sendNoteOn(keyboardOut[j], keyVelocity, midiChannel);
                }
                voice[j] = i;
                keyAssigned[i] = true;
                noteTrigger();
                setVeloModulation(keyVelocity);
                if (voiceCounter == 0)
                {
                  voiceCounter++;
                  for (byte k = 0; k < 4; k++)
                  {
                    if (k != j)
                      muteVoice[k] = true;
                  }
                }
              }
            }
          }
        }
        lastVoiceCount = voiceCounter;
      }
      else // monoMode
      {
        byte highestNote = 255;
        byte lowestNote = 255;
        static byte previousNote = 255;
        switch (monoMode)
        {
          case 1: // highest note priority
            for (byte i = 0; i < 13; i++)
            {
              if (pressed[i])
                highestNote = i;
            }
            if (voice[0] != highestNote)
            {
              voice[0] = highestNote;
              if (unison)
              {
                for (byte j = 0; j < unison; j++)
                  voice[1 + j] = highestNote;
              }
              if (voice[0] == 255)
              {
                if (unison)
                {
                  for (byte j = 0; j < unison; j++)
                    voice[1 + j] = 255;
                }
                noteRelease();
                if (midiOut && keysOut)
                  midiA.sendNoteOff(keyboardOut[0], outVelocity, midiChannel);
              }
              else
              {
                if (midiOut && keysOut)
                  midiA.sendNoteOff(keyboardOut[0], outVelocity, midiChannel);
                noteTrigger();
                setVeloModulation(keyVelocity);
                if (midiOut && keysOut)
                {
                  keyboardOut[0] = highestNote + 60;
                  midiA.sendNoteOn(keyboardOut[0], keyVelocity, midiChannel);
                }
              }
            }
            break;
          case 2: // lowest note priority
            for (byte i = 0; i < 13; i++)
            {
              if (pressed[12 - i])
                lowestNote = 12 - i;
            }
            if (voice[0] != lowestNote)
            {
              voice[0] = lowestNote;
              if (unison)
              {
                for (byte j = 0; j < unison; j++)
                  voice[1 + j] = lowestNote;
              }
              if (voice[0] == 255)
              {
                if (unison)
                {
                  for (byte j = 0; j < unison; j++)
                    voice[1 + j] = 255;
                }
                noteRelease();
                if (midiOut && keysOut)
                  midiA.sendNoteOff(keyboardOut[0], outVelocity, midiChannel);
              }
              else
              {
                if (midiOut && keysOut)
                  midiA.sendNoteOff(keyboardOut[0], outVelocity, midiChannel);
                noteTrigger();
                setVeloModulation(keyVelocity);
                if (midiOut && keysOut)
                {
                  keyboardOut[0] = lowestNote + 60;
                  midiA.sendNoteOn(keyboardOut[0], keyVelocity, midiChannel);
                }
              }
            }
            break;
          case 3: // last note priority
            for (byte i = 0; i < 13; i++)
            {
              if (justpressed[i])
              {
                if (voice[0] != 255)
                {
                  if (midiOut && keysOut)
                    midiA.sendNoteOff(keyboardOut[0], outVelocity, midiChannel);
                }
                voice[0] = i;
                if (unison)
                {
                  for (byte j = 0; j < unison; j++)
                    voice[1 + j] = i;
                }
                previousNote = voice[0];
                noteTrigger();
                setVeloModulation(keyVelocity);
                if (midiOut && keysOut)
                {
                  keyboardOut[0] = i + 60;
                  midiA.sendNoteOn(keyboardOut[0], keyVelocity, midiChannel);
                }
                justpressed[i] = 0;
              }
            }
            for (byte i = 0; i < 13; i++)
            {
              if (justreleased[i])
              {
                if (voice[0] == i)
                {
                  voice[0] = 255;
                  if (unison)
                  {
                    for (byte j = 0; j < unison; j++)
                      voice[1 + j] = 255;
                  }
                  noteRelease();
                  if (midiOut && keysOut)
                    midiA.sendNoteOff(keyboardOut[0], outVelocity, midiChannel);
                }
              }
            }
            if (voice[0] == 255)
            {
              for (byte i = 0; i < 13; i++)
              {
                if (pressed[i])
                {
                  highestNote = i;
                  if (highestNote > previousNote)
                    break;
                }
              }
              if (highestNote != 255)
              {
                voice[0] = highestNote;
                if (unison)
                {
                  for (byte j = 0; j < unison; j++)
                    voice[1 + j] = highestNote;
                }
                noteTrigger();
                setVeloModulation(keyVelocity);
                if (midiOut && keysOut)
                {
                  keyboardOut[0] = highestNote + 60;
                  midiA.sendNoteOn(keyboardOut[0], keyVelocity, midiChannel);
                }
              }
            }
            break;
        }
      }
    }
    else if (arp)// arp mode is active
    {
      arpLength = 0;
      for (int i = 0; i < 13; i++)
      {
        if (pressed[i] && arpLength < 10)
        {
          rawArpList[arpLength] = i;
          arpLength++;
        }
      }
      if (arpLength == 0)
      {
        for (int i = 0; i < 4; i++)
          voice[i] = 255;
      }
      else
        sortArp();
    }
  }
}

void assignIncrementButtons(int *adjVar, int low, int up, int inc) // variable address, lower limit, upper limit, increment
{
  adjustValue = adjVar;
  lowerLimit = low;
  upperLimit = up;
  increment = inc;
}

void incDecSpecials()
{
  // special cases
  if (adjustValue == &dirChoice && dirCount != 0)
    prepNextChoice();

  else if (adjustValue == &osc1WaveType)
    setOsc1WaveType(*adjustValue);

  else if (adjustValue == &osc2WaveType)
    setOsc2WaveType(*adjustValue);

  else if (adjustValue == &filterResonance)
    setFilterResonance(filterResonance);

  else if (adjustValue == &filterType)
    setFilterType(filterType);

  else if (adjustValue == &lfoShape)
    setLfoShape(lfoShape);

  else if (adjustValue == &tmpLfoRate)
  {
    if (lfoLowRange)
    {
      assignIncrementButtons(&tmpLfoRate, 0, 63, 1);
      lfoRate = 64 - tmpLfoRate;
      userLfoRate = lfoRate;
    }
    else // high range
    {
      assignIncrementButtons(&tmpLfoRate, 0, 1023, 4);
      lfoRate = 1024 - tmpLfoRate; // 1024 here because we don't want the rate to ever be 0;
      userLfoRate = lfoRate;
    }
    velLfoRate = lfoRate;
  }

  else if (adjustValue == &mainMenu)
    updateMenu();

  else if (adjustValue == &synPatchLoadSave)
    updateMenu();

  else if (adjustValue == &seqBankLoadSave)
    updateMenu();

  else if (adjustValue == &arp)
    clearHeld();

  else if (adjustValue == &bpm)
  {
    setBpm();
    if (menu == 220)
      seq[currentSeq].bpm = bpm;
  }

  else if (adjustValue == &arpForward)
    sortArp();

  else if (adjustValue == &osc1Volume)
    createOsc1Volume();

  else if (adjustValue == &osc2Volume)
    createOsc2Volume();

  else if (adjustValue == &shaperType)
    createWaveShaper();

  else if (adjustValue == &shaperType1PotVal)
  {
    float tmp = (float)shaperType1PotVal / 1024;
    waveShapeAmount = tmp;
    createWaveShaper();
  }

  else if (adjustValue == &waveShapeAmount2)
    createWaveShaper();

  else if (adjustValue == &gainAmountPotVal)
  {
    float tmp = (float)gainAmountPotVal / 1024;
    tmp += 1.0;
    gainAmount = tmp;
    createGainTable();
  }

  else if (adjustValue == &midiSync)
    setSyncType();

  // evaluate seperately for the sake of the else
  if (adjustValue == &menuChoice)
  {
    lockTimer = millis();
    switch (mainMenu)
    {
      case 0: // synth
        menu = synthMenu[*adjustValue];
        break;
      case 1: // arp
        menu = arpMenu[*adjustValue];
        break;
      case 2: // sequencer
        menu = seqMenu[*adjustValue];
        break;
    }
    updateMenu();
  }
  else
  {
    valueChange = true;
    lockPot(5);
  }
}

void unSplash()
{
  if (splash)
  {
    if (justpressed[13] || justpressed[14])
    {
      if (justpressed[13])
        shiftL = true;
      else if (justpressed[14])
        shiftR = true;
      customCharacters();
      splash = false;
      updateMenu();
      clearJust();
    }
  }
}

void clearHeld() // makes sure no keys or voices are assigned
{
  envelopeProgress = 255;
  for (byte j = 0; j < 4; j++)
    voice[j] = 255;
  for (byte k = 0; k < 13; k++)
    keyAssigned[k] = false;
}

