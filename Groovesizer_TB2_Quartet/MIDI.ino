static int currentNotes[10]; // to keep track of pressed notes in monomode

static int highestNote = 255;
static int lowestNote = 255;
static int lastNote = 255;
static int previousNote = 255;

void HandleNoteOn(byte channel, byte note, byte velocity)
{
  if (channel == midiChannel)
  {
    midiVelocity = velocity;

    if (!midiMode)
    {
      midiMode = true; // switch to listening to notes from incoming MIDI
      envelopeProgress = 255;
      for (byte j = 0; j < 4; j++)
        voice[j] = 255;
      for (byte k = 0; k < 10; k++)
        rawArpList[k] = 255;
    }

    if (!arp)
    {
      if (!monoMode) // 4 voice paraphonic mode
      {
        voiceCounter = 0;
        for (byte i = 0; i < 4 ; i ++)
        {
          if (voice[i] != 255)
            voiceCounter++;
        }
        if (velocity == 0 && voiceCounter != 0)
        {
          for (byte i = 0; i < 4; i++)
          {
            if (voice[i] == note - 60)
            {
              voice[i] = 255;
              voiceCounter--;
            }
          }
          if (voiceCounter == 0)
          {
            envelopeProgress = 3; // we're in release phase
            releaseStartTime = millis();
          }
        }
        else
        {
          setVeloModulation(velocity);
          if (voiceCounter < 4)
          {
            byte j = 0;
            while (voice[j] != 255 && j < 4) // go to the next voice if this voice isn't free
              j++;
            if (j < 4)
            {
              voice[j] = note - 60;
              //showValue(13, 1, voice[j]);
              envelopeTrigger = true;
              if (retrigger) // should we retrigger the LFO?
                lfoIndex = 0;
              if (voiceCounter == 0)
              {
                //voiceCounter++;
                for (byte k = 0; k < 4; k++)
                {
                  if (k != j)
                    muteVoice[k] = true;
                }
              }
              voiceCounter++;
            }
          }
        }
      }
      else // monoMode
      {
        if (velocity != 0)
        {
          if (voice[0] == 255)
            clearCurrentNotes();

          switch (monoMode)
          {
            case 1: // highest note priority
              if (highestNote == 255)
              {
                highestNote = note - 60;
              }
              else if (note - 60 > highestNote)
                highestNote = note - 60;

              if (voice[0] != highestNote)
              {
                previousNote = voice[0];
                voice[0] = highestNote;
                if (unison)
                {
                  for (byte i = 0; i < unison; i++)
                    voice[1 + i] = highestNote;
                }
                setVeloModulation(velocity);
                noteTrigger();
                for (byte i = 0; i < 11; i++)
                {
                  if (i == 10)
                  {
                    currentNotes[0] = highestNote;
                    break;
                  }
                  else if (currentNotes[i] == 255)
                  {
                    LEDon = true;
                    currentNotes[i] = highestNote;
                    break;
                  }
                }
              }
              break;
            case 2: // lowest note priority
              if (lowestNote == 255)
              {
                lowestNote = note - 60;
              }
              else if (note - 60 < lowestNote)
                lowestNote = note - 60;

              if (voice[0] != lowestNote)
              {
                previousNote = voice[0];
                voice[0] = lowestNote;
                if (unison)
                {
                  for (byte i = 0; i < unison; i++)
                    voice[1 + i] = lowestNote;
                }
                setVeloModulation(velocity);
                noteTrigger();
                for (byte i = 0; i < 11; i++)
                {
                  if (i == 10)
                  {
                    currentNotes[0] = lowestNote;
                    break;
                  }
                  else if (currentNotes[i] == 255)
                  {
                    LEDon = true;
                    currentNotes[i] = lowestNote;
                    break;
                  }
                }
              }
              break;
            case 3: // last note priority
              lastNote = note - 60;
              voice[0] = lastNote;
              if (unison)
              {
                for (byte i = 0; i < unison; i++)
                  voice[1 + i] = lastNote;
              }
              setVeloModulation(velocity);
              noteTrigger();
              for (byte i = 0; i < 10; i++)
              {
                if (currentNotes[i] == 255)
                {
                  currentNotes[i] = lastNote; // assumes an ordered list;
                  break;
                }
              }
              break;
          }
        }
      }
    }
    else //we're in arp mode
    {
      byte counter = 0;
      while (rawArpList[counter] != 255 && counter < 9)
        counter++;
      if (rawArpList[counter] == 255)
      {
        rawArpList[counter] = note - 60;
        sortArp();
      }
    }
  }
  if (channel == midiTriggerChannel)
  {
    for (byte i = 0; i < 8; i++)
    {
      if (note == midiTrigger[i])
      {
        if (seqRunning)
        {
          selectedSeq = i;
        }
        else
        {
          currentSeq = i;
          selectedSeq = i;
        }
      }
    }
  }
}

void HandleNoteOff(byte channel, byte note, byte velocity)
{
  if (channel == midiChannel)
  {
    if (!arp)
    {
      if (!monoMode) // we're in 4-voice paraphonic mode
      {
        voiceCounter = 0;
        for (byte i = 0; i < 4 ; i ++)
        {
          if (voice[i] != 255)
            voiceCounter++;
        }

        for (byte i = 0; i < 4; i++)
        {
          if (voice[i] == note - 60)
          {
            voice[i] = 255;
            voiceCounter--;
          }
        }
        if (voiceCounter == 0)
        {
          envelopeProgress = 3; // we're in release phase
          releaseStartTime = millis();
        }
        //showValue(13, 1, voiceCounter);
      }
      else // we are in mono mode
      {
        int tmp = 255;
        switch (monoMode)
        {
          case 1: // highest note priority
            for (byte i = 0; i < 10; i++)
            {
              if ((note - 60) == currentNotes[i])
                currentNotes[i] = 255;
            }

            for (byte i = 0; i < 10; i++)
            {
              if (currentNotes[i] != 255 )
              {
                if (tmp == 255)
                  tmp = currentNotes[i];
                else if (currentNotes[i] > tmp) // for higest note
                  tmp = currentNotes[i];
              }
            }

            if (tmp != 255) // something is held
            {
              if (voice[0] != tmp)
              {
                voice[0] = tmp;
                if (unison)
                {
                  for (byte j = 0; j < unison; j++)
                    voice[1 + j] = tmp;
                }
                highestNote = tmp;
                noteTrigger();
              }
            }
            break;
          case 2: // lowest note priority
            for (byte i = 0; i < 10; i++)
            {
              if ((note - 60) == currentNotes[i])
                currentNotes[i] = 255;
            }

            for (byte i = 0; i < 10; i++)
            {
              if (currentNotes[i] != 255 )
              {
                if (tmp == 255)
                  tmp = currentNotes[i];
                else if (currentNotes[i] < tmp) // for lowest note
                  tmp = currentNotes[i];
              }
            }

            if (tmp != 255) // something is held
            {
              if (voice[0] != tmp)
              {
                voice[0] = tmp;
                if (unison)
                {
                  for (byte j = 0; j < unison; j++)
                    voice[1 + j] = tmp;
                }
                lowestNote = tmp;
                noteTrigger();
              }
            }
            break;
          case 3: // last note priority
            byte i = 0;
            while (currentNotes[i] != (note - 60) && i < 10) // find the released note in the list
            {
              i++;
            }
            if (i < 10)
            {
              currentNotes[i] = 255; // remove released not from the list
              while (i < 9) // reorder the list
              {
                currentNotes[i] = currentNotes[i + 1];
                currentNotes[i + 1] = 255;
                i++;
              }
            }
            i = 0;
            while (currentNotes[i] != 255 && i < 10)
            {
              tmp = currentNotes[i];
              i++;
            }
            if (tmp != 255) // something is held
            {
              if (voice[0] != tmp)
              {
                voice[0] = tmp;
                if (unison)
                {
                  for (byte j = 0; j < unison; j++)
                    voice[1 + j] = tmp;
                }
                lowestNote = tmp;
                noteTrigger();
              }
            }
            break;
        }
        if (tmp == 255) // nothing is held
        {
          for (byte j = 0; j < 4; j++)
            voice[0 + j] = 255;
          noteRelease();
          clearCurrentNotes();
        }
      }
    }
    else //we're in arp mode
    {
      byte counter = 0;
      while (rawArpList[counter] != (note - 60) && counter < 9)
        counter++;
      if (rawArpList[counter] == (note - 60))
      {
        rawArpList[counter] = 255;
        sortArp();
      }
    }
  }
}

void HandleClock(void) // what to do for 24ppq clock pulses
{
  if (syncIn)
  {
    lastClock = millis();
    receivingClock = true;
    static unsigned long lastClockInMicros = 0;
    if ((inPulseCounter % 12) == 0 && !seqJustStarted) // on each 8th except the first
    {
      pulseCounter = 0;
      eighthCounter++;
    }
    else
      seqJustStarted = false;

    clockInMicros = micros();
    if (inPulseCounter != 0)
    {
      pulseMicros = micros() - lastClockInMicros;
      tmpBpm[inPulseCounter % 24] = 60000000 / (pulseMicros * 24);
    }
    if (inPulseCounter > 4)
    {
      int totalBpm = 0;
      for (byte i = 0; i < 24; i++)
        totalBpm += tmpBpm[i];

      if (bpm != totalBpm / 24 + 1)
      {
        bpm = totalBpm / 24 + 1; // +1 here because for some reason the BPM reading is off by one
        setBpm();
        if (menu == 230)
          showValue(5, 1, bpm);
      }
    }
    inPulseCounter++;
    lastClockInMicros = clockInMicros;
  }
}

void HandleStart(void)
{
  if (syncIn)
  {
    seqRunning = true;
    seqStep = 0; // rewind the sequence
    inPulseCounter = 0;
    pulseCounter = 0;
    eighthCounter = 0;
    longStep = true;
    cueNextSeq();
    seqJustStarted = true;
  }
}

void HandleStop(void)
{
  if (syncIn)
  {
    seqRunning = false;
    seqSendMidiNoteOffs = true;
    seqStep = 0; // rewind the sequence
    pulseCounter = 0;
    eighthCounter = 0;
    longStep = true;

    noteRelease();
  }
}

void sendMidi()
{
  if (midiOut)
  {
    // ARPEGGIATOR
    if (arpSendMidiNoteOn)
    {
      if (arpMidiOut != 255)
        midiA.sendNoteOff(arpMidiOut, 127, midiChannel);
      arpMidiOut = sortedArpList[arpPosition] + (arpOctaveCounter * 12) + 60;
      midiA.sendNoteOn(arpMidiOut, 127, midiChannel);
      arpSendMidiNoteOn = false;
    }

    else if (arpSendMidiNoteOff)
    {
      midiA.sendNoteOff(arpMidiOut, 127, midiChannel);
      arpMidiOut = 255;
      arpSendMidiNoteOff = false;
    }

    // SEQUENCER
    else if (seqSendMidiNoteOns)
    {
      for (byte i = 0; i < 4; i++)
      {
        if (seqMidiOff[i] == 255 && seqMidiOn[i] != 255)
        {
          midiA.sendNoteOn(seqMidiOn[i] + 60, outVelocity, midiChannel);
          seqMidiOff[i] = seqMidiOn[i];
        }
        else if (seqMidiOn[i] != 255)
        {
          midiA.sendNoteOff(seqMidiOff[i] + 60, 127, midiChannel);
          midiA.sendNoteOn(seqMidiOn[i] + 60, outVelocity, midiChannel);
          seqMidiOff[i] = seqMidiOn[i];
        }
      }
      seqSendMidiNoteOns = false;
    }

    else if (seqSendMidiNoteOffs)
    {
      for (byte i = 0; i < 4; i++)
      {
        if (seqMidiOff[i] != 255)
        {
          midiA.sendNoteOff(seqMidiOff[i] + 60, 127, midiChannel);
          seqMidiOff[i] = 255;
        }
      }
      seqSendMidiNoteOffs = false;
    }
  }
}

void checkForClock()
{
  if (receivingClock)
  {
    if ((millis() - lastClock) > 300)
      receivingClock = false;
  }
}

void checkThru()
{
  if (midiThruType == 0)
    midiA.turnThruOff();
  else if (midiThruType == 1)
    midiA.turnThruOn(midi::Full);
  else if (midiThruType == 2)
    midiA.turnThruOn(midi::DifferentChannel); // in conjunction with MIDI_CHANNEL_OMNI, this only allows system messages though like clock, stop, start
}

void setSyncType()
{
  switch (midiSync)
  {
    case 0:
      syncIn = false;
      midiClockOut = 0;
      break;
    case 1:
      syncIn = false;
      midiClockOut = 1;
      break;
    case 2:
      syncIn = true;
      midiClockOut = 0;
      break;
  }
}

void clearCurrentNotes()
{
  for (byte i = 0; i < 10; i++)
    currentNotes[i] = 255;

  highestNote = 255;
  lowestNote = 255;
  lastNote = 255;
  previousNote = 255;
}

