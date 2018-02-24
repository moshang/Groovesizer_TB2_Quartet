/***********************************************************************************************************
***   GROOVESIZER TB2 Quartet - 4 Voice Paraphonic Synth and Step Sequencer
***   for the Arduino DUE with TB2 shield http://groovesizer.com
***   August 2014
***   by MoShang (Jean Marais) - moshang@groovesizer.com
***   Adapted from tutorials by DuaneB at http://rcarduino.blogspot.com
***   And the Groovuino library by Gaétan Ro at http://groovuino.blogspot.com
***   Licensed under a Creative Commons Attribution-ShareAlike 3.0
***   AKWF Waveforms by Adventure Kid is licensed under a Creative Commons Attribution 3.0 Unported License.
***   Based on a work at http://www.adventurekid.se
***   Tillstånd utöver denna licens kan vara tillgängligt från http://www.adventurekid.se.
************************************************************************************************************/
int versionNumber = 132;

#include <DueTimer.h>
#include <MIDI.h>
#include <TB2_LCD.h>
#include <SdFat.h>

// *** SD CARD ***
// SD chip select pin
const uint8_t chipSelect = SS;
// file system object
SdFat sd;
SdFile file;
char fileName[13];
char folderName[13];
int16_t waveShapeBuffer[600];
uint16_t dirCount = 0; // count how many files are in a directory
int dirChoice = 999; // nothing is selected yet
uint16_t tempCount = 0;
uint16_t lastDirChoice = 0;
boolean folder = false;
boolean inFolder = false; // are we in a folder?
boolean justEnteredFolder = false;
boolean dirChecked = false; // have we run through the directory to see how many files are inside?
boolean waveshapeLoaded = false;
boolean synthPatchLoaded = false;
boolean seqBankLoaded = false;
int patchBuffer[1900];
int seqBankBuffer[1600];
int settingsBuffer[100];
boolean saveConfirm = false;
int numberName = 0;
char saveName[13] = {" "};
boolean loadRampDown = false;
boolean loadRampUp = false;
uint32_t loadRampFactor = 1023;

// *** LCD ***
// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// *** BUTTONS ***
#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty
// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
byte buttons[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36}; // the analog 0-5 pins are also known as 14-19
// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
// we will track if a button is just pressed, just released, or 'currently pressed'
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

byte lastPressed = 0;

boolean shiftL = false;
boolean shiftR = false;


int *adjustValue;
int increment = 0;
int upperLimit = 0;
int lowerLimit = 0;

// *** LED ***
#define LED 13
boolean LEDon; // a boolean to keep track of whether the LED should be turned on or off at the end of the loop with updateLED()

// *** POTS ***
int pot[5]; // to store the values of out 5 pots
int potLock[5]; // to lock the pots when they're not being adjusted
unsigned long lockTimer = 0; // keep track oof millis() to see if it's time to lock the pots
int lastPotValue[5]; // keep track of the last value so hte pot doesn't lock while you're adjusting it

// *** SYNTH ***
// the phase accumulator points to the current sample in our wavetable
uint32_t ulPhaseAccumulator[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// the phase increment controls the rate at which we move through the wave table
// higher values = higher frequencies
volatile uint32_t ulPhaseIncrement[8] = {0, 0, 0, 0, 0, 0, 0, 0};   // 32 bit phase increment, see below

// full waveform = 0 to SAMPLES_PER_CYCLE
// Phase Increment for 1 Hz =(SAMPLES_PER_CYCLE_FIXEDPOINT/SAMPLE_RATE) = 1Hz
// Phase Increment for frequency F = (SAMPLES_PER_CYCLE/SAMPLE_RATE)*F
#define SAMPLE_RATE 44100.0
#define SAMPLES_PER_CYCLE 600
#define SAMPLES_PER_CYCLE_FIXEDPOINT (SAMPLES_PER_CYCLE<<20)
#define TICKS_PER_CYCLE (float)((float)SAMPLES_PER_CYCLE_FIXEDPOINT/(float)SAMPLE_RATE)

// to represent 600 we need 10 bits
// Our fixed point format will be 10P22 = 32 bits


// We have 521K flash and 96K ram to play with

// Create a table to hold the phase increments we need to generate midi note frequencies at our 44.1Khz sample rate
#define MIDI_NOTES 128
uint32_t nMidiPhaseIncrement[MIDI_NOTES];

// Create a table to hold pre computed sinewave, the table has a resolution of 600 samples
#define WAVE_SAMPLES 600
// default int is 32 bit, in most cases its best to use uint32_t but for large arrays its better to use smaller
// data types if possible, here we are storing 12 bit samples in 16 bit ints
uint16_t nSineTable[WAVE_SAMPLES];
uint16_t nSquareTable[WAVE_SAMPLES];
uint16_t nSawTable[WAVE_SAMPLES];
uint16_t nTriangleTable[WAVE_SAMPLES];
uint16_t nUserTable1[WAVE_SAMPLES];
uint16_t nUserTable2[WAVE_SAMPLES];
uint16_t nUserTable3[WAVE_SAMPLES];

int pulseWidth = 0;
int uiPulseWidth = 0; // the value set by the user

uint16_t *wavePointer[8];
int osc1WaveType = 1;
int osc2WaveType = 1;

byte waveshapes = 8; // how many waveshapes are there? sine, triangle, saw, square, user1, user2, user3, noise

uint32_t ulInput[8];

uint32_t lfoAccumulator = 0;
// the phase increment controls the rate at which we move through the wave table
// higher values = higher frequencies
volatile uint32_t lfoIncrement = 1;   // 32 bit phase increment, see below

int osc1Detune = 0; // -24 - +24 semitones
int osc2Detune = 0; // range +- 32767
int lastOsc1Detune = 0; //
int lastOsc2Detune = 0;

int osc1Volume = 1023;
int osc2Volume = 1023;

/*
int16_t osc1PanR = 0; // 0 = full left, 1023 = full right
int16_t osc1PanL = 1023 - osc1PanR;
int16_t osc2PanR = 1023;
int16_t osc2PanL = 1023 - osc2PanR;
*/

boolean doOscSpread[8] = {false, false, false, false, false, false, false, false};

uint16_t osc1VolTable[4096]; // a lookup table to store volume values for osc1
uint16_t osc2VolTable[4096]; // a lookup table to store volume values for osc2
uint16_t gainTable[4096]; // a lookup table to store post gain values

float gainAmount = 1.2; // 1.0 - 2.0

// *** FILTER ***

#define FX_SHIFT 8
#define SHIFTED_1 256

int f;
long fb;
int q;
int bufL0, bufL1, hp, bp; // bufR0, bufR1,
unsigned char fType;

int filterBypass = 1;
int filterCutoff; // filter cutoff
int lastFilterCutoff = 0; // needed so we can switch back to the front-panel keybaord after wind controller
int filterResonance = 0;
int filterType = 0; // 0 = LP, 1 = BP, 2 = HP

// *** UI ***
boolean splash = true; // show the splash on bootup
boolean uiRefresh = false;
int mainMenu = 0; // which main mode are we in?
/*
0 = SYNTH
1 = ARPEGGIATOR
2 = SEQUENCER
3 = SETTINGS
*/
int lastMainMenu = 0;

int menu = 0;
boolean valueChange = false;
int synthMenu[9] = {0, 10, 20, 30, 40, 50, 60, 65, 68};
int arpMenu[3] = {0, 100, 110};
int seqMenu[5] = {0, 200, 210, 220, 230};
int menuChoice = 0;
int menuPages = 0;
int lastMenu = 0;
unsigned long longPress = 0; // to keep track of millis() for long presses
/*
0 = SPLASH/MAIN
SYNTH
10 = OSC1
11 = OSC1 - select user waveshape
12 = SQUARE - PULSE WIDTH
20 = OSC2
21 = OSC2 - select user waveshape
22 = SQUARE - PULSE WIDTH
30 = FILTER
40 = ENVELOPE
50 = LFO
60 = MODULATION
65 = SHAPER & GAIN
68 = MONO & PORTA
70 = SYNTH LOAD
80 = SYNTH SAVE
100 = ARP Page 1
110 = ARP Page 2
200 = SEQ Trigger
210 = SEQ Edit
211 = SEQ Edit Pattern
220 = SEQ Settings
230 = SEQ Bank Settings
250 = SEQ BANK LOAD
260 = SEQ BANK SAVE
300 = SETTINGS MIDI
310 = SETTINGS KEYBOARD
320 = SETTINGS TRIGGER MIDI
330 = SETTINGS GENERAL

*/

char* noteName[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}; // to display the names of notes

int synPatchLoadSave = 0; // 0 or false = load, 1 or true = save
int seqBankLoadSave = 0; // 0 or false = load, 1 or true = save

boolean doSeqBlink = false; // are we blinking the currently selected step in the edit mode of the sequencer?

// we need these, beacause the in/dec function can't work with floats
int shaperType1PotVal = 205;
int gainAmountPotVal = 205;

// *** MIDI ***
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, midiA);
boolean midiMode = false; // are we listening for notes from the onboard keyboard or from external MIDI?
int monoMode = 0; // 0 = off, 1 = highest note priority, 2 = lowest note priority, 3 = last note priority
int unison = 0; // 0 = off, 1 = 2 voices, 2 = 3 voices, 3 = 4 voices
int uniSpread = 10000;
byte midiCC[128]; // an array to store the latest incoming MIDI cc values

byte arpMidiOut = 255;
int seqMidiOn[4] = {255, 255, 255, 255};
int seqMidiOff[4] = {255, 255, 255, 255};
// the following booleans are used to trigger MIDI send messages
boolean arpSendMidiNoteOn = false;
boolean arpSendMidiNoteOff = false;
boolean seqSendMidiNoteOns = false;
boolean seqSendMidiNoteOffs = false;

int midiClockOut = 0; // are we sending 24ppq clock pulses (clock master)
int clockOutCounter = 0;
boolean receivingClock = false; // are we receiving MIDI clock?
unsigned long lastClock = 0;
boolean seqJustStarted = false;

unsigned long inPulseCounter = 0;
unsigned long clockInMicros = 0; // the time in microseconds when the pulse arrived
int tmpBpm[24];
int pulseMicros = 0; // the duration of a pulse in microseconds
int clockInBpm = 0; // the average of the last 4 BPM measurements
byte midiVelocity = 0; // the last received MIDI velocity
int keysOut = true; // are we sending MIDI notes from the frontpanel keyboard?
byte keyboardOut[4]; // the MIDI notes sent out by the keyboard corresponds to the 4 voices
int midiTrigger[8] = {60, 61, 62, 63, 64, 65, 66, 67};
int midiTriggerOut = 1;
int midiTriggerChannel = 10;
int editTrigger = 0;
boolean syncIn = true;
int midiSync = 0; // 0 = none, 1 = master, 2 = slave

// *** KEYBOARD ***

int voice[4] = {255, 255, 255, 255}; // 255 means no note is assigned (255 is an impossible MIDI note value)
int osc1Octave = 4; // can be set by user
int osc2Octave = 5;
byte osc1OctaveOut; // after modulation
byte osc2OctaveOut;
int osc1OctaveMod = 0;
int osc2OctaveMod = 0;
byte lastOsc1Octave = 4;
byte lastOsc2Octave = 5;
boolean keyAssigned[13] = {
  false, false, false, false,
  false, false, false, false,
  false, false, false, false,
  false
};
byte voiceCounter; // how many voices are currently used?
byte lastVoiceCount; // we need to be able to know when the last key is released
boolean voiceSounding[4] = {false, false, false, false}; // can we hear this voice?
boolean muteVoice[4] = {false, false, false, false}; // to prevent clicking - is this voice marked to go from true to false for voiceSounding?
boolean soundKeys = true; // are we playing sound when the front panel keys are pressed?

// *** ENVELOPE ***
boolean envelopeTrigger = false;
int attackTime = 50;
int decayTime = 100;
int sustainLevel = 800;
int releaseTime = 500;
int32_t envelopeVolume = 0; // the current volume according to the envelope on a scale from 0 to 1023 (10 bits) - needs to be unsigned so we can multiply with it for modulation
unsigned long attackStartTime = 0;
unsigned long decayStartTime = 0;
unsigned long releaseStartTime = 0;
unsigned char envelopeProgress = 255; // 255 = the envelope is idle
int envOsc1Pitch = 0;
int envOsc2Pitch = 0;
int envOsc1PitchFactor = 0; // value between -1023 and 1023
int envOsc2PitchFactor = 0; // value between -1023 and 1023
int envFilterCutoff = 0;
int envFilterCutoffFactor = 0; // value between -1023 and 1023
int envLfoRate = 0;

// *** LFO ***
uint16_t *lfoShapePointer = &nSineTable[0]; // pointer to the array for the waveshape we're using for the LFO
int16_t tmpLFO = 0;
int lfoShape = 0;
int tmpLfoRate = 0; // directly set by user
int lfoRate = 50;
int userLfoRate = 50;
boolean lfoAdvance = false;
int lfoAmount = 0; // value between 0 and 1023
int16_t tmpCutoff = 0;
uint16_t lfoIndex = 0;
int lfoLowRange = 1;
boolean retrigger = true;
int lfoOsc1Detune = 0;
int lfoOsc2Detune = 0;
int lfoOsc1DetuneFactor = 0;
int lfoOsc2DetuneFactor = 0;
int lfoAmp = 0;
int lfoAmpFactor = 0; // 0 to 1023
int lfoPwFactor = 0;
byte dest = 0; // how many destinatiosn for a given source?
boolean lfoLED = false; // do we want to send the current LFO value to the LED?
boolean lfoSync = false;
int lfoSyncCounter = 0;
int lfoSyncTarget = 18;
int syncTicks[5] = {48, 96, 192, 384, 768};
int syncSelector = 0;
int lfoSyncClockCounter = 0;
int lfo8thSync = 1;
int lfo8thSyncCounter = 0;

// *** MODULATION ***
int source = 0; // 0 = LFO, 1 = ENVELOPE, 2 = VELOCITY, 3 = USER CC ()
int destination = 0;
/*
LFO
0 = Osc1 pitch
1 = Osc2 pitch
2 = Osc1 octave
3 = Osc2 octave
4 = Filter Cutoff
5 = AMP
6 = Pulse Width

ENVELOPE
7 = Osc1 pitch
8 = Osc2 pitch
9 = Filter Cutoff
10 = LFO rate

VELOCITY
11 = Osc1 pitch
12 = Osc2 pitch
13 = Osc1 octave
14 = Osc2 octave
15 = Filter Cutoff
16 = AMP
17 = Pulse Width

*/

// *** CLOCK ***
int bpm = 120;
int lastBpm = 120;
int pulseCounter = 0;
unsigned int eighthCounter = 0;
int bpmPeriod = 5208; // for 120bpm

// *** ARP ***
int arp = 0; // is the arpeggiator active? 0 = false, 1 = true
int rawArpList[10]; // the list of notes currently held down from lowest to highest
int sortedArpList[10]; // the list of notes sorted from lowest to highest, or highest to lowest
int arpNoteDur = 60; // the duration of an arp note from attack to release in PPQ pulses - maximum is 96 (1 quarter note)
int arpLength; // how many notes in the pattern?
int lastArpLength = 0; // needed for some of the arp styles
int arpOctaves = 1; // how many actaves are we playing the arp pattern in?
int arpOctaveCounter = 0;
int arpForward = 1; // arp direction: 1 is true, 0 is false ie. backward
int arpPosition = 0; // where are we in the arp pattern?
int arpIncrement = 1; // by how many steps are we advancing through the arp pattern?
/*
0 = random
1 - 3 = 1 - 3 steps
4 = 2 steps forward, 1 back
*/
int arpReleasePulse; // on which pulse should we release the note?
boolean arpReleased = true;
int arpDivSelection = 4; // 16th is default (24)
int arpDivision[7] = {96, 64, 48, 32, 24, 16, 12}; // in clock pulses
/*
96 1/4
64  1/4 tr
48 1/8
32 1/8 tr
24 1/16
16 1/16 tr
12 1/32
*/

int currentDivision = 0;
// *** SEQUENCER ***
typedef struct Sequences
{
  int voice[4][16] = // voice number, step number - note number 0 - 127, 200 for tie, 255 for off
  {
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}
  };
  boolean tie[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // is this step tied?
  boolean mute[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // is this step muted?
  byte velocity[16] = // velocity at this step
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
  byte controlNum[4] = // controller number
  {255, 255, 255, 255};
  byte controlValue[4][16] = // the controller value
  {
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}
  };
  int noteDur = 1023; // range 0 - 1023 (will be mapped from 4 pulses to the division)
  int divSelection = 1; // 16th is default (24 pulses) - selects from seqDivision[3] = {12, 24, 28}
  int patternLength = 16; // how many steps in the sequence?
  int transpose = 0; // number of semitones to transpose by (up or down ie. pos or neg)
  int swing = 0;
  int bpm = 120; // set it to the default bpm
} sequence;

int bankMode = 0; // 0 = loop 1, 1 = loop 2, 2 = loop 4, 3 = loop 8, 4 = random,

sequence seq[8]; // create 8 sequence structs
int currentSeq = 0; // the currently selected sequence

int seqDivision[3] = {12, 24, 28}; // in clock pulses
int seqStep = 0; // which step of the sequence are we on?
int seqReleasePulse = 0; // when do we go into the release phase of the note
boolean seqReleased = 0; // have we already triggered the release phase?
int seqIncrement = 1; // by how many steps are we advancing through the sequence?
int seqVoiceCount = 0;
int seqLastVoiceCount = 0;
int seqRunning = 0; // is the sequencer running?

byte whiteButtons[8] = {0, 2, 4, 5, 7, 9, 11, 12};
byte blackButtons[5] = {1, 3, 6, 8, 10};
int seqEditStep = 0; // which step of the sequence are we editing?
boolean seqBlink = false; // to blink the selected step in edit mode
int seqBlinkCounter = 0;
int selectedSeq = 0; // which sequence did the user select?
int loadBpm = true; // do we want to allow sequences to change the tempo?
int seqPlayMode = 0;
byte sourceSeq = 0; // used to copy sequences
byte destinationSeq = 0; // used to copy sequences
boolean copied = false; // used to copy sequences
boolean longStep = true; // is this the longer or shorter step of a swing pair
int swingFactor = 0; // a positive or negative value (in pulses) depending on if it's the longer or shorter step of a swing pair
/*
0 = forward
1 = reverse
2 = pendulum
3 = random interval
4 = drunk
5 = random step
*/
boolean seqUpdateDisplay = false; // updating the display causes issues with audio playback - save it for non critical pulses

// *** VELOCITY ***
// the variables with factor at the end are set by the user on the modulation page
int velOsc1DetuneFactor = 0;
int velOsc2DetuneFactor = 0;
int velCutoffFactor = 0;
int velAmpFactor = 1023; // by default, the synth responds full scale to incoming velocity data
int velPwFactor = 0;
int velLfoRateFactor = 0;
// the variables without factor are the ones used in LFO
int velOsc1Detune = 0;
int velOsc2Detune = 0;
int velCutoff = 0;
int velPw = 0;
int velLfoRate = 0;

int tempVelAmp = 0; // we need to delay updating velamp until the envelope is triggered to avoid clicks
int velAmp = 0;
int outVelocity = 0; // the velocity that's sent out from the sequencer

// *** SETTINGS ***
// MIDI
int midiOut = 1; // are we sending MIDI data?
int midiThruType = 2; // 0 = off, 1 = clock only, 2 = all
int midiChannel = 1; // which channel are we sending data on? 0 is off
int keyVelocity = 127; // the fixed velocity of the front-panel keyboard

boolean settingsConfirm = false;
int settingsMenu[5] = {0, 300, 310, 320, 330};

int volume = 1023; // the output volume

// *** WAVESHAPER ***
uint16_t waveShaper[4096]; // a lookup table to store waveshaped values
float waveShapeAmount = 0.2;
int waveShapeAmount2 = 2;
int shaperType = 2; // 0 = off, 1 = type 1, 2 = type 2

int bitMuncher = 0; // an effect where we lose accuracy by bitshifting right and left again - this variable is how many bits to shift by

// *** PORTAMENTO ***
uint32_t portaStartTime = 0;
uint32_t portaEndTime = 0;
int portamento = 0; // (0 - 1000ms)

