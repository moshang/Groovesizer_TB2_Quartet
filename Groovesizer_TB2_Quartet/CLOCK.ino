// *** 96PPQ INTERNAL CLOCK ***

void clockHandler()
{
  if (pulseCounter == 0 && lfoSync)
  {
    lfo8thSyncCounter++;
    if (lfo8thSync == lfo8thSyncCounter)
    {
      lfoSyncCounter = 0;
      lfoIndex = 0;
      lfo8thSyncCounter == 0;
    }
  }

  if (midiClockOut && clockOutCounter == 0)
    midiA.sendRealTime(midi::Clock); // send a midi clock pulse

  if (clockOutCounter < 3)
    clockOutCounter++;
  else
    clockOutCounter = 0;

  LEDon = false; // make sure it's turned off (unless it gets turn on)

  if (arp)
  {
    soundKeys = false; // we don't want to hear notes directly played from the front panel keyboard
    if (pulseCounter < 12)
      LEDon = true;

    currentDivision = arpDivision[arpDivSelection];

    if ((pulseCounter % currentDivision) == 0)
      arpNextStep();
    else if (pulseCounter == arpReleasePulse && arpReleased == false)
    {
      noteRelease();
      arpReleased = true;
      arpSendMidiNoteOff = true;
    }
  }

  else if (seqRunning)
  {
    soundKeys = false; // we don't want to hear notes directly played from the front panel keyboard
    if (pulseCounter < 12 && (eighthCounter % 2) == 0)
      LEDon = true;

    currentDivision = seqDivision[seq[currentSeq].divSelection];

    if (pulseCounter < currentDivision) // we only want to change the swing setting on the first (longer) 16th of the swing pair
      swingFactor = map(seq[currentSeq].swing, 0, 1023, 0, currentDivision / 2);

    if (pulseCounter == 0 || pulseCounter == currentDivision + swingFactor)
      seqNextStep();
    else if (seqUpdateDisplay)
    {
      seqUpdateDisplay = false;
      valueChange = true;
    }

    if (pulseCounter == seqReleasePulse && seqReleasePulse != 255)
    {
      seqSendMidiNoteOffs = true;
      if (!seqReleased)
      {
        noteRelease();
        seqReleased = true;
      }
      seqReleasePulse = 255;
    }
  }

  else if (!soundKeys)
  {
    soundKeys = true; // play sounds from the front panel keyboard
    noteRelease();
  }

  // advance the pulse counter
  if (!receivingClock)
  {
    if (pulseCounter < (currentDivision * 2) - 1)
      pulseCounter++;
    else
    {
      pulseCounter = 0;
      eighthCounter++;
    }
  }
  else // wait for sync
  {
    if (pulseCounter < (currentDivision * 2) - 1)
      pulseCounter++;
  }
}

void setBpm()
{
  bpmPeriod = 60000000 / bpm / 96;
  Timer5.setPeriod(bpmPeriod).start(); // set the timer to the new bpm
  if (lfoSync)
    updateLfoSyncTarget();
}
