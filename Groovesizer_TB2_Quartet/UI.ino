// *** LED ***
void updateLED()
{
  if (lfoLED)
    analogWrite(LED, tmpLFO >> 4);
  else if (LEDon)
    analogWrite(LED, 255);
  else
    analogWrite(LED, 0);
}

// *** LCD ***
void showValue(byte h, byte v, int number) // a function to help us deal with displaying numbers of varying lengths
{
  if (number < 10)
  {
    lcd.setCursor(h, v);
    lcd.print(number);
    lcd.print("  ");
  }
  else if (number < 100)
  {
    lcd.setCursor(h, v);
    lcd.print(number);
    lcd.print(" ");
  }
  else
  {
    lcd.setCursor(h, v);
    lcd.print(number);
  }
}

// *** UI ***
void updateMenu()
{
  arrowAnimation = false;
  switch (menu)
  {
    case 0: // SPLASH/MAIN
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("MAIN            ");
      switch (mainMenu)
      {
        case 0:
          lcd.setCursor(0, 1);
          lcd.print("Synth           ");
          lcd.setCursor(11, 0);
          lcd.print("Patch");
          if (synPatchLoadSave == 0) // load
          {
            lcd.setCursor(11, 1);
            lcd.print("Load      ");
          }
          else // save
          {
            lcd.setCursor(11, 1);
            lcd.print("Save      ");
          }
          arrow(15, 1);
          break;
        case 1:
          lcd.setCursor(0, 1);
          lcd.print("Arpeggiator     ");
          break;
        case 2:
          lcd.setCursor(0, 1);
          lcd.print("Sequencer       ");
          lcd.setCursor(11, 0);
          lcd.print("Bank ");
          if (seqBankLoadSave == 0) // load
          {
            lcd.setCursor(11, 1);
            lcd.print("Load      ");
          }
          else // save
          {
            lcd.setCursor(11, 1);
            lcd.print("Save      ");
          }
          arrow(15, 1);
          break;
        case 3: // SETTINGS
          lcd.setCursor(0, 1);
          lcd.print("Settings   Save ");
          if (!settingsConfirm)
            arrow(15, 1);
          break;
      }
      break;
    case 10:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("OSCILLATORS     ");
      lcd.setCursor(0, 1);
      lcd.print("OSC1            ");
      break;
    case 20:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("OSCILLATORS     ");
      lcd.setCursor(0, 1);
      lcd.print("OSC2            ");
      break;
    case 30:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("FILTER          ");
      break;
    case 40:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("ENVELOPE (ADSR) ");
      break;
    case 50:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("LFO             ");
      break;
    case 60:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("MODULATION      ");
      break;
    case 65:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SHAPER & GAIN   ");
      break;
    case 68:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("MONO & PORTA    ");
      break;
    case 100:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("ARPEGGIATOR     ");
      lcd.setCursor(0, 1);
      lcd.print("Page 1          ");
      break;
    case 110:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("ARPEGGIATOR     ");
      lcd.setCursor(0, 1);
      lcd.print("Page 2          ");
      break;
    case 200:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SEQUENCER       ");
      lcd.setCursor(0, 1);
      lcd.print("Trigger         ");
      break;
    case 210:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SEQUENCER       ");
      lcd.setCursor(0, 1);
      lcd.print("Edit            ");
      break;
    case 220:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SEQUENCER       ");
      lcd.setCursor(0, 1);
      lcd.print("Settings        ");
      break;
    case 230:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SEQUENCER       ");
      lcd.setCursor(0, 1);
      lcd.print("Bank Settings   ");
      break;
    case 300:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("MIDI            ");
      break;
    case 310:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("Keyboard        ");
      break;
    case 320:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("Trigger MIDI    ");
      break;
    case 330:
      clearLCD();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("General         ");
      break;
  }
}

void updateValues()
{
  if (uiRefresh && valueChange && menu != 0)
  {
    arrowAnimation = false;
    uiRefresh = false;
    valueChange = false;
    switch (menu)
    {
      case 0: // SPLASH/MAIN
        lcd.setCursor(0, 0);
        lcd.print("Main            ");
        break;

      case 10: // OSC1
        lcd.setCursor(0, 0);
        lcd.print("Os1 Oct Lvl Det ");

        switch (osc1WaveType)
        {
          case 0:
            lcd.setCursor(0, 1);
            lcd.print("Sin ");
            break;
          case 1:
            lcd.setCursor(0, 1);
            lcd.print("Tri ");
            break;
          case 2:
            lcd.setCursor(0, 1);
            lcd.print("Saw ");
            break;
          case 3:
            lcd.setCursor(0, 1);
            lcd.print("Squ ");
            arrow(3, 1);
            break;
          case 4:
            lcd.setCursor(0, 1);
            lcd.print("Us1 ");
            arrow(3, 1);
            break;
          case 5:
            lcd.setCursor(0, 1);
            lcd.print("Us2 ");
            break;
          case 6:
            lcd.setCursor(0, 1);
            lcd.print("Us3 ");
            break;
          case 7:
            lcd.setCursor(0, 1);
            lcd.print("Nse ");
            break;
        }

        showValue(4, 1, osc1Octave); // octave

        showValue(8, 1, osc1Volume >> 2); // level

        showValue(12, 1, osc1Detune); // detune
        break;

      case 11: // OSCILLATORS - change user waveshape
        lcd.setCursor(0, 0);
        lcd.print("Us1: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Load Wave [P4] ");
          }
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getWaveform();
        break;

      case 12: // SQUARE - adjust PWM
        lcd.setCursor(0, 0);
        lcd.print("Squ: Pulse Width");
        showValue(11, 1, (map(uiPulseWidth, ((WAVE_SAMPLES / 2) - 15) * -1, ((WAVE_SAMPLES / 2) - 15), 0, 255)));
        break;

      case 20: // OSC2
        lcd.setCursor(0, 0);
        lcd.print("Os2 Oct Lvl Det ");

        switch (osc2WaveType)
        {
          case 0:
            lcd.setCursor(0, 1);
            lcd.print("Sin ");
            break;
          case 1:
            lcd.setCursor(0, 1);
            lcd.print("Tri ");
            break;
          case 2:
            lcd.setCursor(0, 1);
            lcd.print("Saw ");
            break;
          case 3:
            lcd.setCursor(0, 1);
            lcd.print("Squ ");
            arrow(3, 1);
            break;
          case 4:
            lcd.setCursor(0, 1);
            lcd.print("Us1 ");
            break;
          case 5:
            lcd.setCursor(0, 1);
            lcd.print("Us2 ");
            arrow(3, 1);
            break;
          case 6:
            lcd.setCursor(0, 1);
            lcd.print("Us3 ");
            break;
          case 7:
            lcd.setCursor(0, 1);
            lcd.print("Nse ");
            break;
        }
        showValue(4, 1, osc2Octave); // octave

        showValue(8, 1, osc2Volume >> 2); // level

        showValue(12, 1, osc2Detune >> 14); // detune
        break;

      case 21: // OSC2 - change user waveshape
        lcd.setCursor(0, 0);
        lcd.print("Us2: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Load Wave [P4] ");
          }
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getWaveform();
        break;

      case 22: // SQUARE - adjust PWM
        lcd.setCursor(0, 0);
        lcd.print("Squ: Pulse Width");

        showValue(11, 1, (map(uiPulseWidth, ((WAVE_SAMPLES / 2) - 15) * -1, ((WAVE_SAMPLES / 2) - 15), 0, 255)));
        break;

      case 30: // FILTER
        lcd.setCursor(0, 0);
        lcd.print("Cut Res Typ Byp");
        showValue(0, 1, filterCutoff);
        showValue(4, 1, filterResonance);
        lcd.setCursor(12, 1);
        switch (fType)
        {
          case 0:
            lcd.setCursor(8, 1);
            lcd.print("LP  ");
            break;
          case 1:
            lcd.setCursor(8, 1);
            lcd.print("BP  ");
            break;
          case 2:
            lcd.setCursor(8, 1);
            lcd.print("HP  ");
            break;
        }
        if (filterBypass)
        {
          lcd.print("On ");
        }
        else
          lcd.print("Off");
        break;

      case 40: // ENVELOPE
        lcd.setCursor(0, 0);
        lcd.print("Att Dec Sus Rel ");
        showValue(0, 1, attackTime >> 2);
        showValue(4, 1, decayTime >> 2);
        showValue(8, 1, sustainLevel >> 2);
        showValue(12, 1, releaseTime >> 2);
        break;

      case 50: // LFO
        lcd.setCursor(0, 0);
        if (!lfoSync)
          lcd.print("Shp Rt  Rng Trg");
        else
          lcd.print("Shp RtS Rng Trg");
        switch (lfoShape)
        {
          case 0:
            lcd.setCursor(0, 1);
            lcd.print("Sin ");
            break;
          case 1:
            lcd.setCursor(0, 1);
            lcd.print("Tri ");
            break;
          case 2:
            lcd.setCursor(0, 1);
            lcd.print("Saw ");
            break;
          case 3:
            lcd.setCursor(0, 1);
            lcd.print("Squ ");
            break;
          case 4:
            lcd.setCursor(0, 1);
            lcd.print("Us1 ");
            break;
          case 5:
            lcd.setCursor(0, 1);
            lcd.print("Us2 ");
            break;
          case 6:
            lcd.setCursor(0, 1);
            lcd.print("Us3 ");
            arrow(3, 1);
            break;
        }

        if (!lfoSync)
        {
          if (lfoLowRange)
          {
            showValue(4, 1, constrain(map(userLfoRate, 1, 64, 255, 1), 1, 255));
            lcd.setCursor(8, 1);
            lcd.print("Low ");
          }
          else // high range
          {
            showValue(4, 1, constrain(map(userLfoRate, 1, 1024, 255, 1), 1, 255));
            lcd.setCursor(8, 1);
            lcd.print("Hi  ");
          }
        }
        else
        {
          String syncName[5] = {"1/8 ", "1/4 ", "1/2 ", "1b  ", "2b  "};
          lcd.setCursor(4, 1);
          lcd.print(syncName[syncSelector]);
          lcd.setCursor(8, 1);
          lcd.print("n/a ");
        }

        if (retrigger)
        {
          lcd.setCursor(12, 1);
          lcd.print("On  ");
        }
        else // retrigger is false
        {
          lcd.setCursor(12, 1);
          lcd.print("Off ");
        }
        break;

      case 51: // LFO - change user waveshape
        lcd.setCursor(0, 0);
        lcd.print("Us3: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Load Wave [P4] ");
          }
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getWaveform();
        break;

      case 60: // MODULATION
        static byte lastSource = 255;
        lcd.setCursor(0, 0);
        lcd.print("Src Dst Amt     ");
        switch (source)
        {
          case 0: // SOURCE LFO
            if (lastSource != source)
            {
              destination = 0;
              lcd.setCursor(0, 1);
              lcd.print("                ");
              lastSource = source;
            }
            lcd.setCursor(0, 1);
            lcd.print("LFO ");

            switch (destination)
            {
              case 0:
                lcd.setCursor(4, 1);
                lcd.print("Os1 ");
                showValue(8, 1, lfoOsc1DetuneFactor >> 2);
                break;
              case 1:
                lcd.setCursor(4, 1);
                lcd.print("Os2 ");
                showValue(8, 1, lfoOsc2DetuneFactor >> 2);
                break;
              case 2:
                lcd.setCursor(4, 1);
                lcd.print("Oc1 ");
                showValue(8, 1, osc1OctaveMod);
                break;
              case 3:
                lcd.setCursor(4, 1);
                lcd.print("Oc2 ");
                showValue(8, 1, osc2OctaveMod);
                break;
              case 4:
                lcd.setCursor(4, 1);
                lcd.print("Cut ");
                showValue(8, 1, lfoAmount >> 2);
                break;
              case 5:
                lcd.setCursor(4, 1);
                lcd.print("Amp ");
                showValue(8, 1, lfoAmpFactor >> 2);
                break;
              case 6:
                lcd.setCursor(4, 1);
                lcd.print("PWi ");
                showValue(8, 1, lfoPwFactor >> 2);
                break;
            }
            break;

          case 1: // SOURCE ENVELOPE
            {
              if (lastSource != source)
              {
                destination = 7;
                lcd.setCursor(0, 1);
                lcd.print("                ");
                lastSource = source;
              }
              lcd.setCursor(0, 1);
              lcd.print("Env ");
              switch (destination)
              {
                case 7:
                  lcd.setCursor(4, 1);
                  lcd.print("Os1 ");
                  showValue(8, 1, envOsc1PitchFactor >> 4);
                  break;
                case 8:
                  lcd.setCursor(4, 1);
                  lcd.print("Os2 ");
                  showValue(8, 1, envOsc2PitchFactor >> 4);
                  break;
                case 9:
                  lcd.setCursor(4, 1);
                  lcd.print("Cut ");
                  showValue(8, 1, envFilterCutoffFactor >> 4);
                  break;
                case 10:
                  lcd.setCursor(4, 1);
                  lcd.print("LRt ");
                  showValue(8, 1, envLfoRate >> 2);
                  break;
              }
            }
            break;

          case 2: // SOURCE VELOCITY
            {
              if (lastSource != source)
              {
                destination = 11;
                lcd.setCursor(0, 1);
                lcd.print("                ");
                lastSource = source;
              }
              lcd.setCursor(0, 1);
              lcd.print("Vel ");
              switch (destination)
              {
                case 11:
                  lcd.setCursor(4, 1);
                  lcd.print("Os1 ");
                  showValue(8, 1, velOsc1DetuneFactor >> 4);
                  break;
                case 12:
                  lcd.setCursor(4, 1);
                  lcd.print("Os2 ");
                  showValue(8, 1, velOsc2DetuneFactor >> 4);
                  break;
                case 13:
                  lcd.setCursor(4, 1);
                  lcd.print("Cut ");
                  showValue(8, 1, velCutoffFactor >> 2);
                  break;
                case 14:
                  lcd.setCursor(4, 1);
                  lcd.print("Amp ");
                  showValue(8, 1, velAmpFactor >> 2);
                  break;
                case 15:
                  lcd.setCursor(4, 1);
                  lcd.print("PWi ");
                  showValue(8, 1, velPwFactor);
                  break;
                case 16:
                  lcd.setCursor(4, 1);
                  lcd.print("LRt ");
                  showValue(8, 1, velLfoRateFactor >> 2);
                  break;
              }
            }
            break;
        }
        break;

      case 65: // SHAPER & GAIN
        lcd.setCursor(0, 0);
        lcd.print("Shp Amt Bit Gain");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        if (shaperType == 0)
          lcd.print("Off ");
        else
          lcd.print(shaperType);
        lcd.setCursor(4, 1);
        switch (shaperType)
        {
          case 0:
            lcd.print("n/a ");
            break;
          case 1:
            showValue(4, 1, (int)(waveShapeAmount * 255));
            break;
          case 2:
            showValue(4, 1, waveShapeAmount2);
            break;
        }
        //lcd.setCursor(8, 1);
        showValue(8, 1, bitMuncher);
        //lcd.setCursor(12, 1);
        showValue(12, 1, (int)((gainAmount - 1) * 255));
        break;

      case 68: // MONO & PORTA
        lcd.setCursor(0, 0);
        lcd.print("Mon Uni Spr Por ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        if (!monoMode)
          lcd.print("Off n/a n/a ");
        else
        {
          lcd.setCursor(0, 1);
          switch (monoMode)
          {
            case 1:
              lcd.print(" Hi ");
              break;
            case 2:
              lcd.print("Low ");
              break;
            case 3:
              lcd.print("Lst ");
              break;
          }
          lcd.setCursor(4, 1);
          switch (unison)
          {
            case 0:
              lcd.print("Off ");
              break;
            case 1:
              lcd.print("2Vc ");
              break;
            case 2:
              lcd.print("3Vc ");
              break;
            case 3:
              lcd.print("4Vc ");
              break;
          }
          lcd.setCursor(8, 1);
          if (unison)
            lcd.print(map(uniSpread, 10000, 60000, 0, 255));
          else
            lcd.print("n/a ");
        }
        lcd.setCursor(12, 1);
        if (portamento > 0)
          lcd.print(portamento);
        else
          lcd.print("Off");
        break;

      case 70: // SYNTH LOAD
        lcd.setCursor(0, 0);
        lcd.print("Syn: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Load Patch [P4] ");
          }
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getSynthPatch();
        break;

      case 80: // SYNTH SAVE
        lcd.setCursor(0, 0);
        lcd.print("Syn: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        saveSynthPatch();
        break;

      case 100: // ARP PAGE 1
        lcd.setCursor(0, 0);
        lcd.print("Arp Div Dur BPM ");
        lcd.setCursor(0, 1);
        switch (arp)
        {
          case 0:
            lcd.print("Off ");
            break;
          case 1:
            lcd.print("On  ");
            break;
        }
        lcd.setCursor(4, 1);
        switch (arpDivSelection)
        {
          case 0:
            lcd.print(" 4  ");
            break;
          case 1:
            lcd.print(" 4t ");
            break;
          case 2:
            lcd.print(" 8  ");
            break;
          case 3:
            lcd.print(" 8t ");
            break;
          case 4:
            lcd.print("16  ");
            break;
          case 5:
            lcd.print("16t ");
            break;
          case 6:
            lcd.print("32  ");
            break;
        }
        showValue(8, 1, arpNoteDur);
        showValue(12, 1, bpm);
        break;

      case 110: // ARP PAGE 2
        lcd.setCursor(0, 0);
        lcd.print("Dir Typ Oct     ");
        lcd.setCursor(0, 1);
        switch (arpForward)
        {
          case 0:
            lcd.print("Rev ");
            break;
          case 1:
            lcd.print("Fwd  ");
            break;
        }
        lcd.setCursor(4, 1);
        switch (arpIncrement)
        {
          case 0:
            lcd.print("Rnd ");
            break;
          case 1:
            lcd.print("1St ");
            break;
          case 2:
            lcd.print("2St ");
            break;
          case 3:
            lcd.print("3St ");
            break;
          case 4:
            lcd.print("2B1 ");
            break;
        }
        showValue(8, 1, arpOctaves);
        break;

      case 200: // SEQ TRIGGER
        lcd.setCursor(0, 0);
        lcd.print("1 2 3 4 5 6 7 8 ");
        lcd.setCursor(0, 1);
        if (seqRunning)
          lcd.print("STOP ");
        else
          lcd.print("PLAY ");
        if (currentSeq != selectedSeq)
        {
          lcd.setCursor(selectedSeq * 2 + 1, 0);
          lcd.print((char)127);
        }
        arrow(currentSeq * 2 + 1, 0);
        showValue(15, 1, selectedSeq);
        lcd.setCursor(5, 1);
        switch (seqPlayMode)
        {
          case 0:
            lcd.print("Fwd ");
            break;
          case 1:
            lcd.print("Rev ");
            break;
          case 2:
            lcd.print("Pen ");
            break;
          case 3:
            lcd.print("RIn ");
            break;
          case 4:
            lcd.print("Dnk ");
            break;
          case 5:
            lcd.print("Rnd ");
            break;
        }
        lcd.setCursor(9, 1);
        lcd.print("Tr:  ");
        showValue(13, 1, seq[currentSeq].transpose);
        break;

      case 210: // SEQ EDIT
        showSequence();
        doSeqBlink = true;
        lcd.setCursor(0, 1);
        if (seqRunning)
          lcd.print("STOP ");
        else
          lcd.print("PLAY ");
        lcd.setCursor(7, 1);
        lcd.print("Step: ");
        showValue(13, 1, seqEditStep + 1);
        if (seqEditStep < 9)
          arrow(14, 1);
        else
          arrow(15, 1);
        break;

      case 211: // SEQ EDIT NOTES
        lcd.setCursor(0, 0);
        lcd.print("V1  V2  V3  V4  ");
        for (byte i = 0; i < 4; i++)
        {
          if (seq[currentSeq].voice[i][seqEditStep] < 255)
          {
            lcd.setCursor(i * 4, 1);
            lcd.print(noteName[(seq[currentSeq].voice[i][seqEditStep] + 60) % 12]);
            lcd.print((seq[currentSeq].voice[i][seqEditStep] + 60) / 12 - 1);
            lcd.print(" ");
          }
          else
          {
            lcd.setCursor(i * 4, 1);
            lcd.print("    ");
          }
        }
        break;

      case 220: // SEQ SETTINGS
        lcd.setCursor(0, 0);
        lcd.print("Len Dur Swi BPM ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        showValue(0, 1, seq[currentSeq].patternLength);
        showValue(4, 1, seq[currentSeq].noteDur >> 2);
        showValue(8, 1, seq[currentSeq].swing >> 2);
        showValue(12, 1, seq[currentSeq].bpm);
        break;

      case 230: // SEQ BANK SETTINGS
        lcd.setCursor(0, 0);
        lcd.print("Mode            ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        switch (bankMode)
        {
          case 0:
            lcd.print("Lp1  ");
            break;
          case 1:
            lcd.print("Lp2  ");
            break;
          case 2:
            lcd.print("Lp4  ");
            break;
          case 3:
            lcd.print("Lp8  ");
            break;
          case 4:
            lcd.print("Rnd  ");
            break;
        }
        lcd.setCursor(5, 0);
        lcd.print("Clock ");
        if (receivingClock)
          showValue(5, 1, bpm);
        else
        {
          lcd.setCursor(5, 1);
          lcd.print("No    ");
        }
        //showValue(4, 1, seq[currentSeq].noteDur >> 2);
        //showValue(8, 1, seq[currentSeq].swing >> 2);
        //showValue(12, 1, seq[currentSeq].bpm);
        break;

      case 250: // SEQUENCER BANK LOAD
        lcd.setCursor(0, 0);
        lcd.print("Seq: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Set Bank  [P4] ");
          }
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getSeqBank();
        break;

      case 260: // SEQUENCER BANK SAVE
        lcd.setCursor(0, 0);
        lcd.print("Seq: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        saveSeqBank();
        break;

      case 300: // SETTINGS MIDI
        lcd.setCursor(0, 0);
        lcd.print("Out Chn Thr Sync");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        if (midiOut)
        {
          lcd.setCursor(0, 1);
          lcd.print("Yes ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.print("No  ");
        }
        showValue(4, 1, midiChannel);
        lcd.setCursor(8, 1);
        switch (midiThruType)
        {
          case 0:
            lcd.print("Off ");
            break;
          case 1:
            lcd.print("All ");
            break;
          case 2:
            lcd.print("Clk ");
            break;
        }
        lcd.setCursor(12, 1);
        switch (midiSync)
        {
          case 0:
            lcd.print("None");
            break;
          case 1:
            lcd.print("Mst");
            break;
          case 2:
            lcd.print("Slv");
            break;
        }
        break;

      case 310: // SETTINGS KEYBOARD
        lcd.setCursor(0, 0);
        lcd.print("Out Vel         ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        if (keysOut)
        {
          lcd.setCursor(0, 1);
          lcd.print("Yes ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.print("No  ");
        }
        showValue(4, 1, keyVelocity);
        break;

      case 320: // SETTINGS TRIGGER MIDI
        lcd.setCursor(0, 0);
        lcd.print("Trg Chn No. Note");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        if (midiTriggerOut)
        {
          lcd.setCursor(0, 1);
          lcd.print("Yes ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.print("No  ");
        }
        showValue(4, 1, midiTriggerChannel);
        showValue(8, 1, editTrigger + 1);
        showValue(12, 1, midiTrigger[editTrigger]);
        break;

      case 330: // SETTINGS GENERAL
        lcd.setCursor(0, 0);
        lcd.print("Vol             ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        showValue(0, 1, volume >> 2);
        break;
    }
  }
}
void arrow(byte x, byte y)
{
  arrowAnimation = true;
  arrowX = x;
  arrowY = y;
}

void arrowAnim()
{
  static byte lastArrowFrame = 255; // can't initialize to 0
  byte arrowMap[10] = {0, 0, 0, 0, 0, 1, 2, 3, 2, 1}; // the sequence of frames for the arrow animation
  if (arrowAnimation && arrowFrame != lastArrowFrame)
  {
    lastArrowFrame = arrowFrame;
    lcd.setCursor(arrowX, arrowY);
    switch (arrowMap[arrowFrame])
    {
      case 0:
        lcd.write(byte(0));
        break;
      case 1:
        lcd.write((byte) 1);
        break;
      case 2:
        lcd.write(2);
        break;
      case 3:
        lcd.write(3);
        break;
    }
  }
}

void showTB2(byte x) // horizontal position
{
  lcd.setCursor(x, 0);
  lcd.write(2);
  lcd.write(4);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(5);
  lcd.write(7);
}

void showSequence() // displays the sequence currently being edited
{
  for (byte i = 0; i < 16; i++)
    showStep(i);
}

void seqBlinker()
{
  if (menu == 210 && !seqRunning && doSeqBlink)
  {
    static boolean lastSeqBlink = 0;
    if (seqBlink != lastSeqBlink)
    {
      lastSeqBlink = seqBlink;
      lcd.setCursor(seqEditStep, 0);
      if (seqBlink)
        lcd.print(" ");
      else
        showStep(seqEditStep);
    }
  }
}

void showStep(byte Step)
{
  lcd.setCursor(Step, 0);
  if (seq[currentSeq].mute[Step])
    lcd.write(6);
  else if (seq[currentSeq].tie[Step])
    lcd.write(7);
  //lcd.print((char)126); // forward arrow
  else if (seq[currentSeq].voice[0][Step] == 255)
    lcd.write(5);
  else
    lcd.write(4);
}

void clearLCD()
{
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}
