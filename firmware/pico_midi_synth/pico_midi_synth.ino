/*
  Pico MIDI Synth Module

  Board package:
    Earle Philhower Arduino-Pico core for Raspberry Pi Pico / RP2040.

  Required libraries:
    - MIDI Library by Francois Best / lathoub
    - Adafruit SSD1306
    - Adafruit GFX Library

  Hardware summary:
    MIDI IN opto output -> GP1 (UART0 RX, called Serial1 in Arduino)
    OLED I2C SDA/SCL   -> GP4 / GP5 (I2C0, 100kHz default or 400kHz recommended)
    Audio PWM output   -> GP14 through external RC filter (two-pole 1k-10nF-1k-10nF)
                          PWMAudio library sets PWM freq automatically (~125kHz typical)
                          Output: 3.3V logic level, needs 470R + 10uF AC coupling for line out
                          Max load: 10k ohm or higher recommended
    Pots               -> GP26/A0, GP27/A1, GP28/A2
                          CRITICAL: Pots MUST be wired between 3.3V and GND (NOT 5V!)
                          RP2040 ADC is 3.3V max - 5V will permanently damage the chip
                          Use 10k linear potentiometers
    Joystick switches  -> GP6-GP10 to GND, internal pullups
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>
#include <PWMAudio.h>
#include <math.h>

constexpr uint32_t SAMPLE_RATE = 22050;  // 22.05kHz audio, Nyquist @ 11.025kHz
constexpr uint8_t MAX_VOICES = 4;
constexpr uint8_t SEQ_STEPS = 8;

constexpr uint8_t PIN_MIDI_TX = 0;
constexpr uint8_t PIN_MIDI_RX = 1;
constexpr uint8_t PIN_OLED_SDA = 4;
constexpr uint8_t PIN_OLED_SCL = 5;
constexpr uint8_t PIN_JOY_UP = 6;
constexpr uint8_t PIN_JOY_DOWN = 7;
constexpr uint8_t PIN_JOY_LEFT = 8;
constexpr uint8_t PIN_JOY_RIGHT = 9;
constexpr uint8_t PIN_JOY_CLICK = 10;
constexpr uint8_t PIN_AUDIO_PWM = 14;

constexpr uint8_t PIN_POT_TONE = A0;  // Pico GP26
constexpr uint8_t PIN_POT_OSC = A1;   // Pico GP27
constexpr uint8_t PIN_POT_TEMPO = A2; // Pico GP28

constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t OLED_ADDR = 0x3C;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
PWMAudio audio(PIN_AUDIO_PWM);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

enum Waveform : uint8_t {
  WAVE_SINE,
  WAVE_SAW,
  WAVE_SQUARE,
  WAVE_TRIANGLE,
  WAVE_NOISE,
  WAVE_COUNT
};

enum EnvStage : uint8_t {
  ENV_IDLE,
  ENV_ATTACK,
  ENV_DECAY,
  ENV_SUSTAIN,
  ENV_RELEASE
};

enum UiPage : uint8_t {
  PAGE_PLAY,
  PAGE_OSC,
  PAGE_SEQ,
  PAGE_COUNT
};

struct Voice {
  bool active = false;
  uint8_t note = 0;
  uint8_t velocity = 0;
  uint32_t phaseA = 0;
  uint32_t phaseB = 0;
  uint32_t phaseInc = 0;
  float env = 0.0f;
  EnvStage stage = ENV_IDLE;
  uint32_t age = 0;
};

struct ButtonState {
  bool pressed = false;
  bool lastReading = false;
  bool fell = false;
  bool rose = false;
  bool longPress = false;
  bool longConsumed = false;
  uint32_t changedAt = 0;
  uint32_t pressedAt = 0;
};

Voice voices[MAX_VOICES];
ButtonState btnUp, btnDown, btnLeft, btnRight, btnClick;

int16_t sineTable[256];
uint32_t noiseState = 0x12345678;
uint32_t voiceAgeCounter = 0;

Waveform waveform = WAVE_SAW;
UiPage uiPage = PAGE_PLAY;

float masterVolume = 0.45f;  // Default volume ~45% to prevent clipping with 4-voice polyphony
float filterState = 0.0f;
float filterAlpha = 0.2f;
float oscAmount = 0.0f;
float oscDetune = 1.002f;
float pitchBendSemis = 0.0f;
float midiToneMod = 0.0f;
float midiOscMod = 0.0f;

uint16_t potToneRaw = 0;
uint16_t potOscRaw = 0;
uint16_t potTempoRaw = 0;
uint16_t bpm = 110;

bool sequencerRunning = false;
uint8_t selectedStep = 0;
uint8_t currentStep = 0;
int8_t seqIntervals[SEQ_STEPS] = {0, 3, 7, 10, 12, 10, 7, 3};
uint8_t seqRootNote = 60;
int16_t seqPlayingNote = -1;
uint32_t lastSeqAdvanceMs = 0;
uint32_t seqGateOffMs = 0;

uint32_t lastControlMs = 0;
uint32_t lastDisplayMs = 0;

void allNotesOff();

const char *waveName(Waveform wave) {
  switch (wave) {
    case WAVE_SINE: return "SINE";
    case WAVE_SAW: return "SAW";
    case WAVE_SQUARE: return "SQUARE";
    case WAVE_TRIANGLE: return "TRI";
    case WAVE_NOISE: return "NOISE";
    default: return "?";
  }
}

float clampf(float value, float low, float high) {
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

float potNorm(uint16_t raw) {
  return clampf(static_cast<float>(raw) / 4095.0f, 0.0f, 1.0f);
}

int16_t clampi16(int16_t value, int16_t low, int16_t high) {
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

uint32_t midiNoteToPhaseInc(float midiNote) {
  const float frequency = 440.0f * powf(2.0f, (midiNote - 69.0f) / 12.0f);
  const double inc = (static_cast<double>(frequency) * 4294967296.0) / SAMPLE_RATE;
  return static_cast<uint32_t>(inc);
}

void refreshVoicePitch(Voice &voice) {
  voice.phaseInc = midiNoteToPhaseInc(static_cast<float>(voice.note) + pitchBendSemis);
}

uint32_t xorshift32() {
  uint32_t x = noiseState;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  noiseState = x;
  return x;
}

int16_t oscillatorSample(Waveform wave, uint32_t phase) {
  switch (wave) {
    case WAVE_SINE:
      return sineTable[phase >> 24];

    case WAVE_SAW:
      return static_cast<int16_t>((phase >> 16) - 32768);

    case WAVE_SQUARE:
      return (phase < 0x80000000UL) ? 32767 : -32767;

    case WAVE_TRIANGLE: {
      const uint16_t p = phase >> 16;
      const int32_t tri = (p < 32768) ? (static_cast<int32_t>(p) * 2 - 32768)
                                      : (98304 - static_cast<int32_t>(p) * 2);
      return static_cast<int16_t>(tri);
    }

    case WAVE_NOISE:
      return static_cast<int16_t>(xorshift32() >> 16);

    default:
      return 0;
  }
}

void advanceEnvelope(Voice &voice) {
  constexpr float attackDelta = 1.0f / (0.006f * SAMPLE_RATE);
  constexpr float decayDelta = 1.0f / (0.080f * SAMPLE_RATE);
  constexpr float releaseDelta = 1.0f / (0.180f * SAMPLE_RATE);
  constexpr float sustain = 0.72f;

  switch (voice.stage) {
    case ENV_ATTACK:
      voice.env += attackDelta;
      if (voice.env >= 1.0f) {
        voice.env = 1.0f;
        voice.stage = ENV_DECAY;
      }
      break;

    case ENV_DECAY:
      voice.env -= decayDelta;
      if (voice.env <= sustain) {
        voice.env = sustain;
        voice.stage = ENV_SUSTAIN;
      }
      break;

    case ENV_SUSTAIN:
      voice.env = sustain;
      break;

    case ENV_RELEASE:
      voice.env -= releaseDelta;
      if (voice.env <= 0.0f) {
        voice.env = 0.0f;
        voice.stage = ENV_IDLE;
        voice.active = false;
      }
      break;

    case ENV_IDLE:
    default:
      voice.env = 0.0f;
      voice.active = false;
      break;
  }
}

int16_t renderSample() {
  float mixed = 0.0f;
  uint8_t activeCount = 0;

  for (uint8_t i = 0; i < MAX_VOICES; ++i) {
    Voice &voice = voices[i];
    if (!voice.active) continue;

    advanceEnvelope(voice);
    if (!voice.active) continue;

    voice.phaseA += voice.phaseInc;
    voice.phaseB += static_cast<uint32_t>(static_cast<float>(voice.phaseInc) * oscDetune);

    const float primary = static_cast<float>(oscillatorSample(waveform, voice.phaseA));
    const float secondary = static_cast<float>(oscillatorSample(waveform, voice.phaseB));
    const float oscMix = primary * (1.0f - oscAmount) + secondary * oscAmount;
    const float velocityGain = static_cast<float>(voice.velocity) / 127.0f;

    mixed += oscMix * voice.env * velocityGain;
    activeCount++;
  }

  if (activeCount > 0) {
    mixed /= static_cast<float>(MAX_VOICES);
  }

  filterState += filterAlpha * (mixed - filterState);
  const float out = clampf(filterState * masterVolume, -32767.0f, 32767.0f);
  return static_cast<int16_t>(out);
}

void fillAudio() {
  while (audio.availableForWrite() > 0) {
    audio.write(renderSample(), false);
  }
}

void pollMidi() {
  for (uint8_t i = 0; i < 16; ++i) {
    MIDI.read();
  }
}

Voice *allocateVoice() {
  Voice *oldest = &voices[0];
  for (uint8_t i = 0; i < MAX_VOICES; ++i) {
    if (!voices[i].active) return &voices[i];
    if (voices[i].age < oldest->age) oldest = &voices[i];
  }
  return oldest;
}

void startNote(uint8_t note, uint8_t velocity) {
  Voice *voice = allocateVoice();
  voice->active = true;
  voice->note = note;
  voice->velocity = velocity;
  voice->phaseA = 0;
  voice->phaseB = 0x40000000UL;
  voice->env = 0.0f;
  voice->stage = ENV_ATTACK;
  voice->age = ++voiceAgeCounter;
  refreshVoicePitch(*voice);
}

void releaseNote(uint8_t note) {
  for (uint8_t i = 0; i < MAX_VOICES; ++i) {
    if (voices[i].active && voices[i].note == note && voices[i].stage != ENV_RELEASE) {
      voices[i].stage = ENV_RELEASE;
    }
  }
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  (void)channel;
  if (velocity == 0) {
    releaseNote(note);
    return;
  }

  seqRootNote = note;
  startNote(note, velocity);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  (void)channel;
  (void)velocity;
  releaseNote(note);
}

void handleControlChange(byte channel, byte number, byte value) {
  (void)channel;

  if (number == 74) {
    midiToneMod = static_cast<float>(value) / 127.0f;
  } else if (number == 1) {
    midiOscMod = static_cast<float>(value) / 127.0f;
  } else if (number == 120 || number == 123) {
    allNotesOff();
  }
}

void handlePitchBend(byte channel, int bend) {
  (void)channel;
  pitchBendSemis = (static_cast<float>(bend) / 8192.0f) * 2.0f;

  for (uint8_t i = 0; i < MAX_VOICES; ++i) {
    if (voices[i].active) refreshVoicePitch(voices[i]);
  }
}

void allNotesOff() {
  for (uint8_t i = 0; i < MAX_VOICES; ++i) {
    voices[i].stage = ENV_RELEASE;
  }
  seqPlayingNote = -1;
}

void updateButton(ButtonState &button, uint8_t pin, uint32_t now) {
  const bool reading = (digitalRead(pin) == LOW);
  button.fell = false;
  button.rose = false;
  button.longPress = false;

  if (reading != button.lastReading) {
    button.changedAt = now;
    button.lastReading = reading;
  }

  if ((now - button.changedAt) > 25 && reading != button.pressed) {
    button.pressed = reading;
    if (button.pressed) {
      button.fell = true;
      button.longConsumed = false;
      button.pressedAt = now;
    } else {
      button.rose = true;
    }
  }

  if (button.pressed && !button.longConsumed && (now - button.pressedAt) > 700) {
    button.longPress = true;
    button.longConsumed = true;
  }
}

void updateControls() {
  const uint32_t now = millis();

  updateButton(btnUp, PIN_JOY_UP, now);
  updateButton(btnDown, PIN_JOY_DOWN, now);
  updateButton(btnLeft, PIN_JOY_LEFT, now);
  updateButton(btnRight, PIN_JOY_RIGHT, now);
  updateButton(btnClick, PIN_JOY_CLICK, now);

  if (btnClick.longPress) {
    sequencerRunning = !sequencerRunning;
    if (sequencerRunning) {
      currentStep = SEQ_STEPS - 1;
      lastSeqAdvanceMs = now;  // Start timing from now, not from boot
    }
    if (!sequencerRunning && seqPlayingNote >= 0) {
      releaseNote(static_cast<uint8_t>(seqPlayingNote));
      seqPlayingNote = -1;
    }
  } else if (btnClick.rose && !btnClick.longConsumed) {
    uiPage = static_cast<UiPage>((uiPage + 1) % PAGE_COUNT);
  }

  if (btnLeft.fell) {
    selectedStep = (selectedStep + SEQ_STEPS - 1) % SEQ_STEPS;
  }
  if (btnRight.fell) {
    selectedStep = (selectedStep + 1) % SEQ_STEPS;
  }

  if (uiPage == PAGE_PLAY) {
    if (btnUp.fell) masterVolume = clampf(masterVolume + 0.05f, 0.0f, 0.95f);
    if (btnDown.fell) masterVolume = clampf(masterVolume - 0.05f, 0.0f, 0.95f);
  } else if (uiPage == PAGE_OSC) {
    if (btnUp.fell) waveform = static_cast<Waveform>((waveform + WAVE_COUNT - 1) % WAVE_COUNT);
    if (btnDown.fell) waveform = static_cast<Waveform>((waveform + 1) % WAVE_COUNT);
  } else if (uiPage == PAGE_SEQ) {
    if (btnUp.fell) {
      seqIntervals[selectedStep] = static_cast<int8_t>(clampi16(seqIntervals[selectedStep] + 1, -24, 24));
    }
    if (btnDown.fell) {
      seqIntervals[selectedStep] = static_cast<int8_t>(clampi16(seqIntervals[selectedStep] - 1, -24, 24));
    }
  }

  // Read pots every 12ms (83Hz rate) - fast enough for responsive UI without excessive ADC overhead
  // Sequential reads take ~180-240μs total on RP2040, acceptable for control rate
  if (now - lastControlMs < 12) return;
  lastControlMs = now;

  potToneRaw = analogRead(PIN_POT_TONE);
  potOscRaw = analogRead(PIN_POT_OSC);
  potTempoRaw = analogRead(PIN_POT_TEMPO);

  const float tone = clampf(potNorm(potToneRaw) * 0.80f + midiToneMod * 0.20f, 0.0f, 1.0f);
  const float cutoff = 120.0f * powf(70.0f, tone);
  filterAlpha = clampf(1.0f - expf(-2.0f * PI * cutoff / SAMPLE_RATE), 0.01f, 0.95f);

  const float osc = clampf(potNorm(potOscRaw) * 0.85f + midiOscMod * 0.15f, 0.0f, 1.0f);
  oscAmount = osc * 0.45f;
  oscDetune = 1.0f + ((osc - 0.5f) * 0.018f);

  bpm = 50 + static_cast<uint16_t>(potNorm(potTempoRaw) * 190.0f);
}

void updateSequencer() {
  const uint32_t now = millis();
  if (!sequencerRunning) return;

  uint32_t stepMs = 15000UL / (bpm == 0 ? 1 : bpm);
  if (stepMs < 40) stepMs = 40;

  if (seqPlayingNote >= 0 && now >= seqGateOffMs) {
    releaseNote(static_cast<uint8_t>(seqPlayingNote));
    seqPlayingNote = -1;
  }

  if (now - lastSeqAdvanceMs < stepMs) return;
  lastSeqAdvanceMs = now;

  if (seqPlayingNote >= 0) {
    releaseNote(static_cast<uint8_t>(seqPlayingNote));
    seqPlayingNote = -1;
  }

  currentStep = (currentStep + 1) % SEQ_STEPS;
  const int16_t nextNote = clampi16(seqRootNote + seqIntervals[currentStep], 0, 127);
  seqPlayingNote = nextNote;
  seqGateOffMs = now + ((stepMs * 7) / 10);
  startNote(static_cast<uint8_t>(nextNote), 92);
}

void drawBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, float value) {
  display.drawRect(x, y, width, height, SSD1306_WHITE);
  const uint8_t fill = static_cast<uint8_t>(clampf(value, 0.0f, 1.0f) * (width - 2));
  display.fillRect(x + 1, y + 1, fill, height - 2, SSD1306_WHITE);
}

void updateDisplay() {
  const uint32_t now = millis();
  if (now - lastDisplayMs < 120) return;
  lastDisplayMs = now;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("Pico MIDI Synth");
  display.setCursor(98, 0);
  display.print(sequencerRunning ? "SEQ" : "LIVE");

  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  display.setCursor(0, 15);
  display.print("Page ");
  if (uiPage == PAGE_PLAY) display.print("PLAY");
  if (uiPage == PAGE_OSC) display.print("OSC");
  if (uiPage == PAGE_SEQ) display.print("SEQ");

  display.setCursor(75, 15);
  display.print("Wave ");
  display.print(waveName(waveform));

  display.setCursor(0, 27);
  display.print("Vol");
  drawBar(25, 26, 36, 8, masterVolume);

  display.setCursor(70, 27);
  display.print("BPM ");
  display.print(bpm);

  display.setCursor(0, 39);
  display.print("Tone");
  drawBar(32, 38, 32, 8, potNorm(potToneRaw));

  display.setCursor(71, 39);
  display.print("Osc");
  drawBar(95, 38, 32, 8, potNorm(potOscRaw));

  display.setCursor(0, 52);
  display.print("Step ");
  display.print(selectedStep + 1);
  display.print(" ");
  if (seqIntervals[selectedStep] >= 0) display.print("+");
  display.print(static_cast<int>(seqIntervals[selectedStep]));

  for (uint8_t i = 0; i < SEQ_STEPS; ++i) {
    const uint8_t x = 72 + i * 7;
    const bool active = (i == currentStep && sequencerRunning);
    const bool selected = (i == selectedStep);
    display.drawRect(x, 54, 6, 9, SSD1306_WHITE);
    if (active) display.fillRect(x + 1, 55, 4, 7, SSD1306_WHITE);
    if (selected) display.drawFastHLine(x, 52, 6, SSD1306_WHITE);
  }

  display.display();
}

void initSineTable() {
  for (uint16_t i = 0; i < 256; ++i) {
    const float phase = (static_cast<float>(i) / 256.0f) * 2.0f * PI;
    sineTable[i] = static_cast<int16_t>(sinf(phase) * 32767.0f);
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(PIN_JOY_UP, INPUT_PULLUP);
  pinMode(PIN_JOY_DOWN, INPUT_PULLUP);
  pinMode(PIN_JOY_LEFT, INPUT_PULLUP);
  pinMode(PIN_JOY_RIGHT, INPUT_PULLUP);
  pinMode(PIN_JOY_CLICK, INPUT_PULLUP);

  analogReadResolution(12);

  initSineTable();

  Wire.setSDA(PIN_OLED_SDA);
  Wire.setSCL(PIN_OLED_SCL);
  Wire.begin();
  Wire.setClock(400000);  // 400kHz I2C for faster display updates (~2.5ms vs 10ms at 100kHz)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    // Display init failed - blink LED rapidly to indicate error
    // Synth will still work for audio, just no display
    while (true) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }
  display.clearDisplay();
  display.display();

  Serial1.setRX(PIN_MIDI_RX);
  Serial1.setTX(PIN_MIDI_TX);
  Serial1.setFIFOSize(128);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  // Audio buffer config: 6 buffers × 128 samples = 768 samples total (~35ms @ 22kHz)
  // Memory: 768 × 2 bytes = 1.5KB RAM
  // Latency: ~35ms total buffering provides safety margin for display I2C blocking
  audio.setBuffers(6, 128);
  audio.begin(SAMPLE_RATE);

  lastSeqAdvanceMs = millis();
}

void loop() {
  pollMidi();
  updateControls();
  updateSequencer();
  fillAudio();
  updateDisplay();
  fillAudio();

  digitalWrite(LED_BUILTIN, sequencerRunning ? HIGH : LOW);
}
