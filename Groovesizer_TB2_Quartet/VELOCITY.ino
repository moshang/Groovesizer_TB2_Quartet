// *** VELOCITY ***
// we calculate velocity modulation variables as and when we receive velocity data
// ie. we don't have to calculate it in the LFO interrupt, we just pass the value there

void setVeloModulation(byte velocity)
{
  if (velOsc1DetuneFactor != 0)
    velOsc1Detune = (velocity * velOsc1DetuneFactor) << 6;
  else
    velOsc1Detune = 0;

  if (velOsc2DetuneFactor != 0)
    velOsc2Detune = (velocity * velOsc2DetuneFactor) << 6;
  else
    velOsc2Detune = 0;

  if (velCutoffFactor != 0)
    velCutoff = map(velocity, 0, 127, 0, velCutoffFactor >> 2);
  else
    velCutoff = 0;

  if (velAmpFactor != 0)
    tempVelAmp = map(velocity, 0, 127, velAmpFactor, 0);
  else
    tempVelAmp = 0; // this gets added to lfoAmp

  if (velPwFactor != 0)
    velPw = map(velocity, 0, 127, 0, velPwFactor);
  else
    velPw = 0;

  if (velLfoRateFactor != 0)
  velLfoRate = constrain(lfoRate - map(velLfoRateFactor, 0, 1023, 0, velocity), 0, lfoRate);
  else
    velLfoRate = lfoRate;
}


