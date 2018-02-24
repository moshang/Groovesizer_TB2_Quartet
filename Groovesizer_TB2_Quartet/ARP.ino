void arpNextStep()
{
  if (arpLength != 0)
  {
    if (arpPosition == 0)
      arpOctaveCounter = ((arpOctaveCounter + 1) < arpOctaves) ? arpOctaveCounter + 1 : 0;
    if (arpIncrement > 0 && arpIncrement < 4)
      arpPosition = (arpPosition + arpIncrement) % arpLength;
    else
    {
      switch (arpIncrement)
      {
        case 0: // random
          arpPosition = random(0, arpLength);
          break;

        case 4: // 2 forward, 1 back

          static boolean toggle = 0;
          static boolean first = true;
          if (lastArpLength != arpLength)
          {
            first = true;
            lastArpLength = arpLength;
          }
          if (first == true)
          {
            arpPosition = 0;
            first = false;
            toggle = 0;
          }
          else
          {
            if (!toggle)
            {
              if ((arpPosition + 2) <= arpLength - 1)
                arpPosition += 2;
              else
              {
                arpPosition = arpLength - 1;
                first = true;
              }
              toggle = true;
            }
            else if (toggle) // toggle is true
            {
              if (arpPosition == arpLength - 1)
                first = true;
              arpPosition -= 1;
              toggle = false;
            }
          }
          break;
      }
    }
    if (!monoMode)
    {
      voice[0] = sortedArpList[arpPosition] + (arpOctaveCounter * 12);
      for (int i = 0; i < 3; i++)
        voice[i + 1] = 255;
    }
    else
    {
      for (byte j = 0; j < 4; j++)
      {
        if (j < unison + 1)
          voice[0 + j] = sortedArpList[arpPosition] + (arpOctaveCounter * 12);
        else
          voice[0 + j] = 255;
      }
    }
    noteTrigger();
    arpReleasePulse = (pulseCounter + arpNoteDur) % (currentDivision * 2);
    arpReleased = false;
    arpSendMidiNoteOn = true; // send a MIDI note
  }
  else
    lastArpLength = 0;
}

void sortArp()
{
  if (!midiMode)
  {
    if (arpForward)
    {
      for (byte j = 0; j < arpLength; j++)
        sortedArpList[j] = rawArpList[j];
    }
    else // backward
    {
      for (byte j = 0; j < arpLength; j++)
        sortedArpList[j] = rawArpList[(arpLength - 1) - j];
    }
  }
  else // we're in MIDI mode
  {
    arpLength = 0;
    for (int i = 0; i < 10; i++)
    {
      if (rawArpList[i] != 255)
        arpLength++;
    }
    if (arpLength) // ie arpLength != 0 - don't bother to sort if arpLength is 0
    {
      if (arpForward)
      {
        int tmpArpList[10];
        for (byte m = 0; m < 10; m++)
        {
          tmpArpList[m] = rawArpList[m];
        }
        int lowest = 255;
        for (int j = 0; j < arpLength; j++)
        {
          for (int k = 0; k < 10; k++)
          {
            if (tmpArpList[k] < 255 && tmpArpList[k] < lowest)
            {
              lowest = (rawArpList[k]);
            }
          }
          sortedArpList[j] = lowest;
          for (byte n = 0; n < 10; n++)
          {
            if (tmpArpList[n] == lowest)
              tmpArpList[n] = 255;
          }
          lowest = 255;
        }
      }
      else // backwards
      {
      }
    }
  }
}
