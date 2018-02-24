void currentEnvelope()
{
  static uint16_t attackVolume = 0;
  static unsigned int lastVolume = 0;
  static unsigned int releaseVolume = 0;
  if (envelopeTrigger)
  {
    portaStartTime = millis();
    portaEndTime = portaStartTime + portamento;
    envelopeProgress = 255;
    if (envelopeVolume > 10)
      envelopeVolume -= 9; // we need to do this to prevent clicking
    else
    {
      attackStartTime = millis();
      envelopeProgress = 0;
      attackVolume = lastVolume;
      for (byte i = 0; i < 4; i++)
      {
        if (muteVoice[i])
        {
          voiceSounding[i] = false;
          muteVoice[i] = false;
        }
      }
      velAmp = tempVelAmp;
      assignVoices();
      envelopeTrigger = false;
    }
  }
  switch (envelopeProgress)
  {
    case 0: // attack
      if ((millis() - attackStartTime) > attackTime)
      {
        envelopeProgress = 1;
        decayStartTime = millis();
      }
      else
      {
        envelopeVolume = map(millis(), attackStartTime, attackStartTime + attackTime, 0, 1023);
        lastVolume = envelopeVolume;
        releaseVolume = envelopeVolume;
        envOsc1Pitch = (envelopeVolume * envOsc1PitchFactor) << 4;
        envOsc2Pitch = (envelopeVolume * envOsc2PitchFactor) << 4;
        envFilterCutoff = (envelopeVolume * envFilterCutoffFactor) >> 12;
        if (envLfoRate > 10)
        {
          lfoRate = map(envelopeVolume, 0, 1023, envLfoRate, userLfoRate);
          velLfoRate = lfoRate;
        }
      }
      break;
    case 1: // decay
      if ((millis() - decayStartTime) > decayTime)
      {
        envelopeProgress = 2;
      }
      else
      {
        envelopeVolume = map(millis(), decayStartTime, decayStartTime + decayTime, 1023, sustainLevel);
        lastVolume = envelopeVolume;
        releaseVolume = envelopeVolume;
        envOsc1Pitch = (envelopeVolume * envOsc1PitchFactor) << 4;
        envOsc2Pitch = (envelopeVolume * envOsc2PitchFactor) << 4;
        envFilterCutoff = (envelopeVolume * envFilterCutoffFactor) >> 12;
        if (envLfoRate > 10)
        {
          lfoRate = map(envelopeVolume, 0, 1023, envLfoRate, userLfoRate);
          velLfoRate = lfoRate;
        }
      }
      break;
    case 2: // sustain
      envelopeVolume = sustainLevel;
      lastVolume = sustainLevel;
      releaseVolume = envelopeVolume;
      envOsc1Pitch = (envelopeVolume * envOsc1PitchFactor) << 4;
      envOsc2Pitch = (envelopeVolume * envOsc2PitchFactor) << 4;
      envFilterCutoff = (envelopeVolume * envFilterCutoffFactor) >> 12;
      if (envLfoRate > 10)
      {
        lfoRate = map(envelopeVolume, 0, 1023, envLfoRate, userLfoRate);
        velLfoRate = lfoRate;
      }
      break;
    case 3: // release
      if ((millis() - releaseStartTime) > releaseTime)
      {
        envelopeProgress = 255;
        lastVolume = 0; // just make sure it's actually at zero
        for (byte i = 0; i < 4; i++)
          voiceSounding[i] = false; // none of the voices are sounding
        clearHeld();
      }
      else
      {
        if (envelopeVolume > 0)
        {
          envelopeVolume = map(millis(), releaseStartTime, releaseStartTime + releaseTime, releaseVolume, 0);
          envOsc1Pitch = (envelopeVolume * envOsc1PitchFactor) << 4;
          envOsc2Pitch = (envelopeVolume * envOsc2PitchFactor) << 4;
          envFilterCutoff = (envelopeVolume * envFilterCutoffFactor) >> 12;
          if (envLfoRate > 10)
          {
            lfoRate = map(envelopeVolume, 0, 1023, envLfoRate, userLfoRate);
            velLfoRate = lfoRate;
          }
        }
        else
        {
          envelopeVolume = 0;
          envOsc1Pitch = 0;
          envOsc2Pitch = 0;
          lfoRate = userLfoRate;
          velLfoRate = lfoRate;
        }
        lastVolume = envelopeVolume;
      }
      break;
    case 255: // the envelope is idle
      break;
  }

  if (loadRampDown)
  {
    loadRampFactor = (loadRampFactor > 5) ? loadRampFactor - 5 : 0;
    if (loadRampFactor == 0)
    {
      loadRampDown = false;
      loadProceed();
    }
  }
  else if (loadRampUp)
  {
    loadRampFactor = (loadRampFactor < 1018) ? loadRampFactor + 5 : 1023;
    if (loadRampFactor == 1023)
      loadRampUp = false;
  }
}

void noteTrigger()
{
  envelopeTrigger = true;
  if (retrigger) // should we retrigger the LFO?
    lfoIndex = 0;
}

void noteRelease()
{
  if (envelopeProgress != 3 && envelopeProgress != 255)
  {
    envelopeProgress = 3; // we're in release phase
    releaseStartTime = millis();
  }
}

