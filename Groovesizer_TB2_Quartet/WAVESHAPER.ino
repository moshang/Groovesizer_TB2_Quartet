// *** WAVESHAPER***
// the floating point math executes too slowly to use in the audio interrupt directly
// so we pre-calculate a waveshaper lookup table outside of the audio loop and just plug in the current sample value in the audio interrupt


void createWaveShaper()
{
  if (shaperType == 0) // off - no change
  {
    for (uint16_t sampleInput = 0; sampleInput < 4096; sampleInput++)
      waveShaper[sampleInput] = sampleInput;
  }

  else if (shaperType == 1) // http://www.musicdsp.org/showArchiveComment.php?ArchiveID=46
  {
    float k = 2 * waveShapeAmount / (1 - waveShapeAmount);
    for (uint16_t sampleInput = 0; sampleInput < 4096; sampleInput++)
    {
      float x = (float)(sampleInput - 2048) / 2048;
      float y = (1 + k) * x / (1 + k * abs(x));
      float tmpOutput = (y * 2048) + 2048;
      uint16_t sampleOutput = (uint16_t)tmpOutput;
      waveShaper[sampleInput] = sampleOutput;
    }
  }

  else if (shaperType == 2) // http://www.musicdsp.org/showArchiveComment.php?ArchiveID=41 
  {
    float a = (float)waveShapeAmount2;
    for (uint16_t sampleInput = 0; sampleInput < 4096; sampleInput++)
    {
      float x = (float)(sampleInput - 2048) / 2048;
      float y = x * (abs(x) + a) / ( pow(x, 2) + (a - 1) * abs(x) + 1);
      float tmpOutput = (y * 2048) + 2048;
      uint16_t sampleOutput = (uint16_t)tmpOutput;
      waveShaper[sampleInput] = sampleOutput;
    }
  }
}

