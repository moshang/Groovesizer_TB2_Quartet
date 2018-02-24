void seqNextStep()
{
  if (seqStep == 0 && currentSeq != selectedSeq) // to switch to the next selected sequence
  {
    currentSeq = selectedSeq;
    seqUpdateDisplay = true;
    cueNextSeq();
  }

  switch (seqPlayMode) // sets the seqIncrement variable (how many steps the sequence is advance by) according to the play mode
  {
    case 0: // forward
      seqIncrement = 1;
      break;

    case 1: // reverse
      seqIncrement = -1;
      break;

    case 2: // pendulum
      if (seqStep == 0)
        seqIncrement = 1;
      else if (seqStep == seq[currentSeq].patternLength - 1)
        seqIncrement = -1;
      break;

    case 3: // random interval
      if (seqStep == 0) // choose another interval when the step reaches 0
        seqIncrement = random(1, seq[currentSeq].patternLength);
      break;

    case 4: // drunk
      {
        byte decider = random(0, 2);
        if (decider == 0 )
          seqIncrement = 1;
        else
          seqIncrement = -1;
      }
      break;

    case 5: // random
      seqIncrement = random(1, seq[currentSeq].patternLength);
      break;
  }

  if (!seq[currentSeq].mute[seqStep] && !seq[currentSeq].tie[seqStep]) // if this step isn't muted or a tie step
  {
    if (!monoMode)
    {
      for (int i = 0; i < 4; i++)
      {
        voice[i] = (seq[currentSeq].voice[i][seqStep] != 255) ?  seq[currentSeq].voice[i][seqStep] + seq[currentSeq].transpose : 255;
        seqMidiOn[i] = voice[i];
        if (voice[i] == 255)
          muteVoice[i] = true;
      }
    }
    else // mono mode
    {
      voice[0] = (seq[currentSeq].voice[0][seqStep] != 255) ?  seq[currentSeq].voice[0][seqStep] + seq[currentSeq].transpose : 255;
      seqMidiOn[0] = voice[0];
      if (voice[0] == 255)
        muteVoice[0] = true;
      else
      {
        for (byte j = 1; j < 4; j++)
        {
          if (j - 1 < unison)
            voice[j] = voice[0];
          else
          {
            voice[j] = 255;
            muteVoice[j] = true;
          }
        }
      }
    }
    if (!seq[currentSeq].tie[nextStep()])
      seqReleasePulse = (pulseCounter + map(seq[currentSeq].noteDur, 0, 1023, 4, (seqDivision[seq[currentSeq].divSelection]) - 1)) % 96;

    if (seqMidiOn[0] != 255)
    {
      noteTrigger();
      setVeloModulation(seq[currentSeq].velocity[seqStep]);
      outVelocity = seq[currentSeq].velocity[seqStep];
      seqReleased = false;
    }
    seqSendMidiNoteOns = true; // trigger the sending of notes in the loop (can't do it here in the interrupt)
  }

  if (seq[currentSeq].tie[seqStep] && !seq[currentSeq].tie[nextStep()])
    seqReleasePulse = (pulseCounter + map(seq[currentSeq].noteDur, 0, 1023, 4, (seqDivision[seq[currentSeq].divSelection]) - 1)) % 96;

  if (seq[currentSeq].mute[seqStep])
  {
    if (seqReleasePulse != 255)
      seqReleasePulse = (pulseCounter + 1) % 96;
  }

  seqStep = nextStep();
  longStep = !longStep;
}

int nextStep()
{
  // deal with seqIncrement that's out of range
  if (seqIncrement == 0) // we don't want to deal with a zero
    seqIncrement = 1;

  int followingStep = seqStep + seqIncrement;
  if (followingStep >= seq[currentSeq].patternLength)
    followingStep %= seq[currentSeq].patternLength;
  else if (followingStep < 0)
    followingStep += seq[currentSeq].patternLength;
  return followingStep;
}

void copySeq() // copy the current sequence to another location
{
  for (byte j = 0; j < 4; j++) // for each of the 4 voices
  {
    for (byte k = 0; k < 16; k++) // NOTE VALUES for each of the 16 steps
      seq[destinationSeq].voice[j][k] = seq[sourceSeq].voice[j][k];
  }
  for (byte m = 0; m < 16; m++) // TIES for each of the 16 steps
    seq[destinationSeq].tie[m] = seq[sourceSeq].tie[m];
  for (byte n = 0; n < 16; n++) // MUTES for each of the 16 steps
    seq[destinationSeq].mute[n] = seq[sourceSeq].mute[n];
  for (byte o = 0; o < 16; o++) // VELOCITY for each of the 16 steps
    seq[destinationSeq].velocity[o] = seq[sourceSeq].velocity[o];
  for (byte p = 0; p < 4; p++) // CC NUMBER for each of the 4 controllers
    seq[destinationSeq].controlNum[p] = seq[sourceSeq].controlNum[p];
  for (byte q = 0; q < 4; q++) // for each of the 4 controllers
  {
    for (byte r = 0; r < 16; r++) // CC VALUE for each of the 16 steps
      seq[destinationSeq].controlValue[q][r] = seq[sourceSeq].controlValue[q][r];
  }
  seq[destinationSeq].noteDur = seq[sourceSeq].noteDur; // NOTE DURATION
  seq[destinationSeq].divSelection = seq[sourceSeq].divSelection; // TEMPO DIVISION
  seq[destinationSeq].patternLength = seq[sourceSeq].patternLength; // PATTERN LENGTH
  seq[destinationSeq].transpose = seq[sourceSeq].transpose; // TRANSPOSE VALUE
  seq[destinationSeq].swing = seq[sourceSeq].swing; // SWING AMOUNT
  seq[destinationSeq].bpm = seq[sourceSeq].bpm; // TEMPO
  copied = true;
}

void clearSeq()
{
  noteRelease();
  for (byte j = 0; j < 4; j++) // for each of the 4 voices
  {
    for (byte k = 0; k < 16; k++) // NOTE VALUES for each of the 16 steps
      seq[currentSeq].voice[j][k] = 255;
  }
  for (byte m = 0; m < 16; m++) // TIES for each of the 16 steps
    seq[currentSeq].tie[m] = 0;
  for (byte n = 0; n < 16; n++) // MUTES for each of the 16 steps
    seq[currentSeq].mute[n] = 0;
  for (byte o = 0; o < 16; o++) // VELOCITY for each of the 16 steps
    seq[currentSeq].velocity[o] = 255;
  for (byte p = 0; p < 4; p++) // CC NUMBER for each of the 4 controllers
    seq[currentSeq].controlNum[p] = 255;
  for (byte q = 0; q < 4; q++) // for each of the 4 controllers
  {
    for (byte r = 0; r < 16; r++) // CC VALUE for each of the 16 steps
      seq[currentSeq].controlValue[q][r] = 255;
  }
  seq[currentSeq].noteDur = 1023; // NOTE DURATION
  seq[currentSeq].divSelection = 1; // TEMPO DIVISION
  seq[currentSeq].patternLength = 16; // PATTERN LENGTH
  seq[currentSeq].transpose = 0; // TRANSPOSE VALUE
  seq[currentSeq].swing = 0; // SWING AMOUNT
  seq[currentSeq].bpm = 120; // TEMPO
}
void seqPlayStop()
{
  seqRunning = !seqRunning;
  seqStep = 0; // rewind the sequence
  pulseCounter = 0;
  eighthCounter = 0;
  lfo8thSyncCounter = 0;
  longStep = true;
  if (seqRunning)
  {
    cueNextSeq();
    seqJustStarted = true;
    if (midiClockOut)
      midiA.sendRealTime(midi::Start); // send a midi clock start signal
  }
  if (!seqRunning)
  {
    seqSendMidiNoteOffs = true;
    noteRelease();
    if (midiClockOut)
      midiA.sendRealTime(midi::Stop); // send a midi clock start signal
  }
}

void cueNextSeq()
{
  switch (bankMode)
  {
    case 0: // do nothing
      break;
    case 1: // loop 2
      if ((currentSeq % 2) == 0) // for even numbered sequences
        selectedSeq = currentSeq + 1;
      else
        selectedSeq = currentSeq - 1;
      break;
    case 2: // loop 4
      if (currentSeq < 3)
        selectedSeq = currentSeq + 1;
      else if (currentSeq == 3)
        selectedSeq = 0;
      else if (currentSeq < 7)
        selectedSeq = currentSeq + 1;
      else if (currentSeq == 7)
        selectedSeq = 4;
      break;
    case 3:
      if (currentSeq < 7)
        selectedSeq = currentSeq + 1;
      else
        selectedSeq = 0;
      break;
    case 4:
      do
        selectedSeq = random(0, 8); // upper bound is not included in random()
      while (selectedSeq == currentSeq); // if the random function delivers the currentSeq we'll get stuck
      break;
  }
}


boolean updateSeqNotes()
{
  assignIncrementButtons(&seqEditStep, 0, 15, 1);
  if (!midiMode)
  {
    for (byte i = 0; i < 4; i++)
      seq[currentSeq].voice[i][seqEditStep] = 255;
    byte useVoice = 0;
    for (byte i = 0; i < 13; i++)
    {
      if (pressed[i] && useVoice < 4)
      {
        seq[currentSeq].voice[useVoice][seqEditStep] = i;
        useVoice++;
      }
    }
    seq[currentSeq].velocity[seqEditStep] = keyVelocity;
    seq[currentSeq].mute[seqEditStep] = 0;
    seq[currentSeq].tie[seqEditStep] = 0;
    valueChange = true;
    return true;
  }
  else // MIDI mode
  {
    byte voxCounter = 0;
    for (byte i = 0; i < 4; i++)
    {
      seq[currentSeq].voice[i][seqEditStep] = 255;

      if (voice[i] != 255)
      {
        seq[currentSeq].voice[voxCounter][seqEditStep] = voice[i];
        voxCounter++;
      }
    }
    if (voxCounter != 0)
    {
      seq[currentSeq].velocity[seqEditStep] = midiVelocity;
      seq[currentSeq].mute[seqEditStep] = 0;
      seq[currentSeq].tie[seqEditStep] = 0;
      valueChange = true;
      return true;
    }
    else
      return false;
  }
}

void clearStep()
{
  for (byte j = 0; j < 4; j++) // NOTE for each of the 4 voices
    seq[currentSeq].voice[j][seqEditStep] = 255;
  // TIE
  seq[currentSeq].tie[seqEditStep] = 0;
  // MUTE
  seq[currentSeq].mute[seqEditStep] = 0;
  // VELOCITY
  seq[currentSeq].velocity[seqEditStep] = 255;
  // CONTROLLERS
  for (byte q = 0; q < 4; q++) // for each of the 4 controllers
    seq[currentSeq].controlValue[q][seqEditStep] = 255;
}
