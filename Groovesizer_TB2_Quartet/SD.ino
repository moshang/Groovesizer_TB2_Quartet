// *** WAVE FILE HEADER ***
typedef struct {
  char RIFF[4];
  int32_t chunk_size;
  char WAVE[4];
  char fmt_[4];
  int32_t subchunk1_size;
  int16_t audio_format;
  int16_t num_channels;
  int32_t sample_rate;
  int32_t byte_rate;
  int16_t block_align;
  int16_t bits_per_sample;
  char DATA[4];
  int32_t subchunk2_size;
} wave_header;

wave_header header;

void getWaveform()
{
  if (dirCount == 0 && dirChecked == false)
  {
    getDirCount();
  }

  else
  {
    while (tempCount < dirChoice)
    {
      file.openNext(sd.vwd(), O_READ);
      tempCount++;
      file.getFilename(fileName);
      if (file.isDir())
      {
        folder = true;
        file.getFilename(folderName);
      }
      else
        folder = false;
      file.close();
    }
    if (tempCount == dirChoice)
    {
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(fileName);
      if (folder)
        arrow(strlen(fileName) + 4, 1);
      //lcd.print(char(127));
      lcd.print("            "); // sloppy way to clear excess characters
    }
    if (!waveshapeLoaded)
    {
      // if (
      if (checkExtension(".WAV"))
        //);
        loadWaveshape();
    }
  }
}

void getDirCount()
{
  sd.vwd()->rewind();
  tempCount = 0;
  while (file.openNext(sd.vwd(), O_READ))
  {
    //if (!macFile())
    tempCount++;
    file.close();
  }
  dirCount = tempCount;
  dirChoice = 0;
  sd.vwd()->rewind();
  dirChecked = true;
  valueChange = true;
}

void loadWaveshape()
{
  if (!folder)
  {
    file.open(fileName);
    file.seekSet(44); // just after the header data
    file.read(waveShapeBuffer, 1200); // we know our waveeshape files are all 1200 bytes long (ie. 600 * 16 bit ints)
    file.close();
    switch (menu)
    {
      case 11:
        for (int i = 0; i < 600; i ++)
          nUserTable1[i] = (waveShapeBuffer[i] >> 4) + 2048;
        break;
      case 21:
        for (int i = 0; i < 600; i ++)
          nUserTable2[i] = (waveShapeBuffer[i] >> 4) + 2048;
        break;
      case 51:
        for (int i = 0; i < 600; i ++)
          nUserTable3[i] = (waveShapeBuffer[i] >> 4) + 2048;
        break;
    }
    file.close();
    waveshapeLoaded = true;
  }
}

boolean checkExtension(char ext[]) // checks the extension of the file, use argument like ".WAV", ".TB2"
{
  byte nameLength = strlen(fileName);
  if (nameLength < 6)
    return false;
  else
  {
    char lastFour[5];
    for (byte i = 0; i < 5; i++)
      lastFour[i] = fileName[nameLength - 4 + i]; // create a string with only the last four chars
    if (strcmp(ext, lastFour) == 0)
      return true;
    else
      return false;
  }
}

void savePatch()
{
  file.open(fileName, O_RDWR | O_CREAT); // create file if it doesn't exist and open the file for write
  // OSC1
  patchBuffer[0] = osc1WaveType;
  patchBuffer[1] = osc1Octave;
  patchBuffer[2] = osc1Volume;
  patchBuffer[3] = osc1Detune;
  patchBuffer[4] = pulseWidth;
  // OSC2
  patchBuffer[5] = osc2WaveType;
  patchBuffer[6] = osc2Octave;
  patchBuffer[7] = osc2Volume;
  patchBuffer[8] = osc2Detune;
  // FILTER
  patchBuffer[9] = filterCutoff;
  patchBuffer[10] = filterResonance;
  patchBuffer[11] = fType;
  patchBuffer[12] = filterBypass;
  // ENVELOPE
  patchBuffer[13] = attackTime;
  patchBuffer[14] = decayTime;
  patchBuffer[15] = sustainLevel;
  patchBuffer[16] = releaseTime;
  // LFO
  patchBuffer[17] = lfoShape;
  patchBuffer[18] = lfoRate;
  patchBuffer[19] = lfoLowRange;
  patchBuffer[20] = retrigger;
  // MODULATION
  patchBuffer[21] = lfoOsc1DetuneFactor;
  patchBuffer[22] = lfoOsc2DetuneFactor;
  patchBuffer[23] = osc1OctaveMod;
  patchBuffer[24] = osc2OctaveMod;
  patchBuffer[25] = lfoAmount;
  patchBuffer[26] = lfoAmpFactor;
  patchBuffer[27] = lfoPwFactor;
  patchBuffer[28] = envOsc1PitchFactor;
  patchBuffer[29] = envOsc2PitchFactor;
  patchBuffer[30] = envFilterCutoffFactor;
  patchBuffer[31] = envLfoRate;
  // SHAPER & GAIN
  patchBuffer[32] = shaperType;
  patchBuffer[33] = shaperType1PotVal;
  patchBuffer[34] = waveShapeAmount2;
  patchBuffer[35] = gainAmountPotVal;
  patchBuffer[36] = bitMuncher;
  // MONO & PORTA
  patchBuffer[37] = portamento;
  patchBuffer[38] = monoMode;
  patchBuffer[39] = unison;
  patchBuffer[40] = uniSpread;
  // write zeros to 100 for future use
  for (byte i = 41; i < 100; i++)
    patchBuffer[i] = 0;
  // USER 1 WAVESHAPE
  for (uint16_t i = 0; i < 600; i++)
    patchBuffer[i + 100] = nUserTable1[i];
  // USER 2 WAVESHAPE
  for (uint16_t i = 0; i < 600; i++)
    patchBuffer[i + 700] = nUserTable2[i];
  // USER 3 WAVESHAPE
  for (uint16_t i = 0; i < 600; i++)
    patchBuffer[i + 1300] = nUserTable3[i];
  // WRITE TO SD
  if (file.write(patchBuffer, 7600) != -1) // note - we are writing 1900 4 byte ints from the patch buffer to 7600 bytes on the SD
  {
    if (file.sync())
    {
      lcd.setCursor(4, 1);
      lcd.print("Saved!      ");
    }
  }
  file.close();
}

void loadPatch()
{
  loadRampUp = false;
  loadRampDown = true;
}

void loadProceed()
{
  file.open(fileName);
  if (file.read(patchBuffer, 7600) == 7600) // note - we are reading 7600 bytes to a buffer of 1900 4 byte ints
  {
    // OSC1
    osc1WaveType = patchBuffer[0];
    setOsc1WaveType(osc1WaveType);
    osc1Octave = patchBuffer[1];
    osc1Volume = patchBuffer[2];
    osc1Detune = patchBuffer[3];
    pulseWidth = patchBuffer[4];
    uiPulseWidth = pulseWidth;
    // OSC2
    osc2WaveType = patchBuffer[5];
    setOsc2WaveType(osc2WaveType);
    osc2Octave = patchBuffer[6];
    osc2Volume = patchBuffer[7];
    osc2Detune = patchBuffer[8];
    // FILTER
    filterCutoff = patchBuffer[9];
    filterResonance = patchBuffer[10];
    setFilterResonance(filterResonance);
    fType = patchBuffer[11];
    filterBypass = patchBuffer[12];
    // ENVELOPE
    attackTime = patchBuffer[13];
    decayTime = patchBuffer[14];
    sustainLevel = patchBuffer[15];
    releaseTime = patchBuffer[16];
    // LFO
    lfoShape  = patchBuffer[17];
    setLfoShape(lfoShape);
    lfoRate = patchBuffer[18];
    userLfoRate = lfoRate;
    lfoLowRange = patchBuffer[19];
    retrigger = patchBuffer[20];
    // MODULATION
    lfoOsc1DetuneFactor = patchBuffer[21];
    lfoOsc2DetuneFactor = patchBuffer[22];
    osc1OctaveMod = patchBuffer[23];
    osc2OctaveMod = patchBuffer[24];
    lfoAmount = patchBuffer[25];
    lfoAmpFactor = patchBuffer[26];
    lfoPwFactor = patchBuffer[27];
    envOsc1PitchFactor = patchBuffer[28];
    envOsc2PitchFactor = patchBuffer[29];
    envFilterCutoffFactor = patchBuffer[30];
    envLfoRate = patchBuffer[31];
    // USER 1 WAVESHAPE
    for (uint16_t i = 0; i < 600; i++)
      nUserTable1[i] = patchBuffer[i + 100];
    // USER 2 WAVESHAPE
    for (uint16_t i = 0; i < 600; i++)
      nUserTable2[i] = patchBuffer[i + 700];
    // USER 3 WAVESHAPE
    for (uint16_t i = 0; i < 600; i++)
      nUserTable3[i] = patchBuffer[i + 1300];
    // SHAPER & GAIN
    int changeCounter = 0;
    if (patchBuffer[32] != shaperType)
    {
      shaperType = patchBuffer[32];
      changeCounter++;
    }
    if (patchBuffer[33] != shaperType1PotVal)
    {
      shaperType1PotVal = patchBuffer[33];
      float tmp = (float)shaperType1PotVal / 1024;
      waveShapeAmount = tmp;
      changeCounter++;
    }
    if (patchBuffer[34] != waveShapeAmount2)
    {
      waveShapeAmount2  = patchBuffer[34];
      changeCounter++;
    }
    if (changeCounter) // ie. changeCounter is not 0
      createWaveShaper();
    if (patchBuffer[35] != gainAmountPotVal)
    {
      gainAmountPotVal = patchBuffer[35];
      float tmp = (float)gainAmountPotVal / 1024;
      tmp += 1.0;
      gainAmount = tmp;
      createGainTable();
    }
    bitMuncher = patchBuffer[36];
    // MONO & PORTA
    portamento = patchBuffer[37];
    monoMode = patchBuffer[38];
    unison = patchBuffer[39];
    uniSpread = patchBuffer[40];
    if (uniSpread == 0)
      uniSpread = 10000;
  }
  file.close();
  synthPatchLoaded = true;
  loadRampUp = true;
}

void setFolder()
{
  dirCount = 0;
  dirChecked = false;
  switch (inFolder)
  {
    case true:
      inFolder = false;
      sd.chdir(); // change current directory to root
      sd.vwd()->rewind();
      clearJust();
      valueChange = true;
      break;

    case false:
      inFolder = true;
      sd.chdir(folderName); // change current directory to folderName
      sd.vwd()->rewind();
      clearJust();
      valueChange = true;
      break;
  }
}

void getSynthPatch()
{
  while (tempCount < dirChoice && (file.openNext(sd.vwd(), O_READ)))
  {
    tempCount++;
    file.getFilename(fileName);
    if (file.isDir())
    {
      folder = true;
      file.getFilename(folderName);
    }
    else
      folder = false;
    file.close();
  }
  if (tempCount == dirChoice)
  {
    lcd.setCursor(0, 1);
    lcd.print("    ");
    lcd.print(fileName);
    if (folder)
      arrow(strlen(fileName) + 4, 1);
    lcd.print("            "); // sloppy way to clear excess characters
  }
  if (!synthPatchLoaded)
  {
    if (checkExtension(".TB2"))
      loadPatch();
  }
  //  }
}

void saveSynthPatch()
{
  if (dirCount == 0 && dirChecked == false)
  {
    static uint16_t tempCount = 0;
    while (file.openNext(sd.vwd(), O_READ))
    {
      file.close();
      dirCount++;
    }
    dirChoice = 0;
    sd.vwd()->rewind();
    dirChecked = true;
    valueChange = true;
    if (dirCount < 98)
    {
      numberName = dirCount + 1;
      if (numberName < 10)
      {
        sprintf(saveName, "0%d.TB2", numberName);
      }
      else
      {
        sprintf(saveName, "%d.TB2", numberName);
      }
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(saveName);
      lcd.print("      ");
      strcpy(fileName, saveName);
    }
    else
    {
      lcd.setCursor(0, 1);
      lcd.print("    Folder Full!");
    }
  }

  else
  {
    while (tempCount < dirChoice)
    {
      file.openNext(sd.vwd(), O_READ);
      tempCount++;
      file.getFilename(fileName);
      if (file.isDir())
      {
        folder = true;
        file.getFilename(folderName);
      }
      else
        folder = false;
      file.close();
    }
    if (tempCount == dirChoice)
    {
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(fileName);
      if (folder)
        arrow(strlen(fileName) + 4, 1);
      lcd.print("            "); // sloppy way to clear excess characters
    }
  }
}


void getFirstFile()
{
  sd.vwd()->rewind();
  file.openNext(sd.vwd(), O_READ);
  file.getFilename(fileName);
  if (file.isDir())
  {
    folder = true;
    file.getFilename(folderName);
  }
  file.close();
}

void prepNextChoice()
{
  sd.vwd()->rewind();
  tempCount = 0;
  waveshapeLoaded = false;
  synthPatchLoaded = false;
  seqBankLoaded = false;
}

void saveSeqBank()
{
  if (dirCount == 0 && dirChecked == false)
  {
    static uint16_t tempCount = 0;
    while (file.openNext(sd.vwd(), O_READ))
    {
      file.close();
      dirCount++;
    }
    dirChoice = 0;
    sd.vwd()->rewind();
    dirChecked = true;
    valueChange = true;
    if (dirCount < 98)
    {
      numberName = dirCount + 1;
      if (numberName < 10)
      {
        sprintf(saveName, "0%d.SEQ", numberName);
      }
      else
      {
        sprintf(saveName, "%d.SEQ", numberName);
      }
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(saveName);
      lcd.print("      ");
      strcpy(fileName, saveName);
    }
    else
    {
      lcd.setCursor(0, 1);
      lcd.print("    Folder Full!");
    }
  }

  else
  {
    while (tempCount < dirChoice)
    {
      file.openNext(sd.vwd(), O_READ);
      tempCount++;
      file.getFilename(fileName);
      if (file.isDir())
      {
        folder = true;
        file.getFilename(folderName);
      }
      else
        folder = false;
      file.close();
    }
    if (tempCount == dirChoice)
    {
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(fileName);
      if (folder)
        arrow(strlen(fileName) + 4, 1);
      lcd.print("            "); // sloppy way to clear excess characters
    }
  }
}

void saveBank()
{
  file.open(fileName, O_RDWR | O_CREAT); // create file if it doesn't exist and open the file for write
  // PACK THE BUFFER WITH CURRENT SEQUENCES
  for (byte i = 0; i < 8; i++) // for each of the 8 sequences
  {
    for (byte j = 0; j < 4; j++) // for each of the 4 voices
    {
      for (byte k = 0; k < 16; k++) // NOTE VALUES for each of the 16 steps
        seqBankBuffer[(i * 186) + (j * 16) + k] = seq[i].voice[j][k];
    }
    for (byte m = 0; m < 16; m++) // TIES for each of the 16 steps
      seqBankBuffer[(i * 186) + 64 + m] = seq[i].tie[m];
    for (byte n = 0; n < 16; n++) // MUTES for each of the 16 steps
      seqBankBuffer[(i * 186) + 80 + n] = seq[i].mute[n];
    for (byte o = 0; o < 16; o++) // VELOCITY for each of the 16 steps
      seqBankBuffer[(i * 186) + 96 + o] = seq[i].velocity[o];
    for (byte p = 0; p < 4; p++) // CC NUMBER for each of the 4 controllers
      seqBankBuffer[(i * 186) + 112 + p] = seq[i].controlNum[p];
    for (byte q = 0; q < 4; q++) // for each of the 4 controllers
    {
      for (byte r = 0; r < 16; r++) // CC VALUE for each of the 16 steps
        seqBankBuffer[(i * 186) + 116 + (q * 16) + r] = seq[i].controlValue[q][r];
    }
    seqBankBuffer[(i * 186) + 180] = seq[i].noteDur; // NOTE DURATION
    seqBankBuffer[(i * 186) + 181] = seq[i].divSelection; // TEMPO DIVISION
    seqBankBuffer[(i * 186) + 182] = seq[i].patternLength; // PATTERN LENGTH
    seqBankBuffer[(i * 186) + 183] = seq[i].transpose; // TRANSPOSE VALUE
    seqBankBuffer[(i * 186) + 184] = seq[i].swing; // SWING AMOUNT
    seqBankBuffer[(i * 186) + 185] = seq[i].bpm; // TEMPO
  }
  seqBankBuffer[1488] = bankMode;
  for (int s = 1489; s < 1600; s++)
    seqBankBuffer[s] = 0; // write zeros for padding in case we need it for future use
  // WRITE TO SD
  if (file.write(seqBankBuffer, 6400) != -1) // note - we are writing 1600 4 byte ints from the patch buffer to 6400 bytes on the SD
  {
    if (file.sync())
    {
      lcd.setCursor(4, 1);
      lcd.print("Saved!      ");
    }
  }
  file.close();
}

void loadBank()
{
  file.open(fileName);
  if (file.read(seqBankBuffer, 6400) == 6400) // note - we are reading 6400 bytes to a buffer of 1600 4 byte ints
  {
    // we update the buffer with the sequence
    // but we only want to unpack it if we hit the enter key with unpackSeqBankBuffer()
  }
  file.close();
  seqBankLoaded = true;
}

void getSeqBank()
{
  if (dirCount == 0 && dirChecked == false)
  {
    static uint16_t tempCount = 0;
    while (file.openNext(sd.vwd(), O_READ))
    {
      file.close();
      dirCount++;
    }
    dirChoice = 0;
    sd.vwd()->rewind();
    dirChecked = true;
    valueChange = true;
  }

  else
  {
    while (tempCount < dirChoice)
    {
      file.openNext(sd.vwd(), O_READ);
      tempCount++;
      file.getFilename(fileName);
      if (file.isDir())
      {
        folder = true;
        file.getFilename(folderName);
      }
      else
        folder = false;
      file.close();
    }
    if (tempCount == dirChoice)
    {
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(fileName);
      if (folder)
        arrow(strlen(fileName) + 4, 1);
      lcd.print("            "); // sloppy way to clear excess characters
    }
    if (!seqBankLoaded)
    {
      if (checkExtension(".SEQ"))
        loadBank();
    }
  }
}

void unpackSeqBankBuffer()
{
  // UNPACK THE BUFFER TO CURRENT SEQUENCES
  for (byte i = 0; i < 8; i++) // for each of the 8 sequences
  {
    for (byte j = 0; j < 4; j++) // for each of the 4 voices
    {
      for (byte k = 0; k < 16; k++) // NOTE VALUES for each of the 16 steps
        seq[i].voice[j][k] = seqBankBuffer[(i * 186) + (j * 16) + k];
    }
    for (byte m = 0; m < 16; m++) // TIES for each of the 16 steps
      seq[i].tie[m] = seqBankBuffer[(i * 186) + 64 + m];
    for (byte n = 0; n < 16; n++) // MUTES for each of the 16 steps
      seq[i].mute[n] = seqBankBuffer[(i * 186) + 80 + n];
    for (byte o = 0; o < 16; o++) // VELOCITY for each of the 16 steps
      seq[i].velocity[o] = seqBankBuffer[(i * 186) + 96 + o];
    for (byte p = 0; p < 4; p++) // CC NUMBER for each of the 4 controllers
      seq[i].controlNum[p] = seqBankBuffer[(i * 186) + 112 + p];
    for (byte q = 0; q < 4; q++) // for each of the 4 controllers
    {
      for (byte r = 0; r < 16; r++) // CC VALUE for each of the 16 steps
        seq[i].controlValue[q][r] = seqBankBuffer[(i * 186)  + 116 + (q * 16) + r];
    }
    seq[i].noteDur = seqBankBuffer[(i * 186) + 180]; // NOTE DURATION
    seq[i].divSelection = seqBankBuffer[(i * 186) + 181]; // TEMPO DIVISION
    seq[i].patternLength = seqBankBuffer[(i * 186) + 182]; // PATTERN LENGTH
    if (seqStep >= seq[i].patternLength)
      seqStep = 0;
    seq[i].transpose = seqBankBuffer[(i * 186) + 183]; // TRANSPOSE VALUE
    seq[i].swing = seqBankBuffer[(i * 186) + 184]; // SWING AMOUNT
    seq[i].bpm = seqBankBuffer[(i * 186) + 185]; // TEMPO
  }
  bankMode = seqBankBuffer[1488];
}

void loadSettings()
{
  sd.chdir(); // change current directory to root
  // if we're in the root and we can't open the file, make it with savePreferences()
  if (!file.open("TB2PREFS.set"))
  {
    saveSettings();
  }
  else // load the preferences
  {
    if (file.read(settingsBuffer, 400) == 400) // note - we are reading 400 bytes to a buffer of 100 4 byte ints
    {
      // UNPACK THE BUFFER TO SETTINGS
      midiOut = settingsBuffer[0];
      midiChannel = settingsBuffer[2];
      midiThruType = settingsBuffer[3];
      checkThru();
      midiSync = settingsBuffer[4];
      setSyncType();
      keysOut = settingsBuffer[5];
      keyVelocity = settingsBuffer[6];
      //settingsBuffer[7]
      //settingsBuffer[8]
      midiTriggerOut = settingsBuffer[9];
      midiTriggerChannel = settingsBuffer[10];
      if (midiTriggerChannel == 0)
        midiTriggerChannel = 1; // in case preferences have not yet been saved
      for (int i = 0; i < 8; i++)
        midiTrigger[i] = settingsBuffer[11 + i];
      volume = settingsBuffer[19];
      if (volume == 0)
        volume = 1023; // in case preferences have not yet been saved
      else if (volume == 1025)
        volume = 0;
    }
    file.close();
  }
}

void saveSettings()
{

  // PACK THE BUFFER WITH SEETTINGS
  for (int i = 0; i < 100; i++)
    settingsBuffer[i] = 0; // clear the buffer

  settingsBuffer[0] = midiOut;
  settingsBuffer[2] = midiChannel;
  settingsBuffer[3] = midiThruType;
  settingsBuffer[4] = midiSync;
  settingsBuffer[5] = keysOut;
  settingsBuffer[6] = keyVelocity;
  //settingsBuffer[7]
  //settingsBuffer[8]
  settingsBuffer[9] = midiTriggerOut;
  settingsBuffer[10] = midiTriggerChannel;
  for (int i = 0; i < 8; i++)
    settingsBuffer[11 + i] = midiTrigger[i];
  settingsBuffer[19] = (volume > 0) ? volume : 1025;
  file.open("TB2PREFS.set", O_RDWR | O_CREAT); // create file if it doesn't exist and open the file for write
  if (file.write(settingsBuffer, 400) != -1) // note - we are writing 100 4 byte ints from the patch buffer to 400 bytes on the SD
  {
    if (file.sync())
    {
      lcd.setCursor(0, 1);
      lcd.print("Settings Saved! ");
    }
  }
  file.close();
}

void gotoRootDir()
{
  inFolder = false;
  sd.chdir(); // change current directory to root
  sd.vwd()->rewind();
}
