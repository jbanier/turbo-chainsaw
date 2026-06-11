# Controls and Firmware Behavior

## Pots

| Pot | Firmware name | Behavior |
| --- | --- | --- |
| GP26 / A0 | Tone | Low-pass filter cutoff. Left is darker, right is brighter. |
| GP27 / A1 | Osc | Adds a second detuned oscillator. Left is cleaner, right is thicker. |
| GP28 / A2 | Tempo | Internal sequencer tempo, about 50-240 BPM. |

## Joystick

The joystick is page-based.

| Action | Play page | Osc page | Seq page |
| --- | --- | --- | --- |
| Click | Next page | Next page | Next page |
| Hold click | Start/stop sequencer | Start/stop sequencer | Start/stop sequencer |
| Up | Increase volume | Previous waveform | Raise selected step |
| Down | Decrease volume | Next waveform | Lower selected step |
| Left | Select previous step | Select previous step | Select previous step |
| Right | Select next step | Select next step | Select next step |

The selected sequencer step is shown on the OLED. The sequencer stores eight intervals relative to the current root note.

## MIDI Behavior

The synth listens to all channels.

| MIDI message | Behavior |
| --- | --- |
| Note On | Starts a voice. Velocity controls level. A zero-velocity Note On is treated as Note Off by the MIDI library defaults. |
| Note Off | Releases matching active voices. |
| Pitch Bend | Bends active and future voices by +/- 2 semitones. |
| CC 74 | Adds a cutoff/tone offset. |
| CC 1 | Adds a modulation/thickness offset. |
| CC 120 / CC 123 | Releases all active voices. |

The last MIDI note received becomes the sequencer root. If no MIDI note has been received, the default root is middle C (`60`).

## Oscillators

Available oscillator pages:

- `SINE`
- `SAW`
- `SQUARE`
- `TRI`
- `NOISE`

Each voice has two oscillator phases. Pot 2 brings in the second oscillator and detunes it slightly for a wider sound.

## Sequencer

The internal sequencer has eight steps. Each step is a semitone interval from the root note.

Default pattern:

```text
0, 3, 7, 10, 12, 10, 7, 3
```

At each step the firmware triggers a note with a short gate. MIDI notes can still be played while the sequencer is running.

## Current Firmware Limits

- Mono audio output.
- Four-voice polyphony.
- No preset storage yet.
- No MIDI clock sync yet.
- PWM audio is good for a prototype, but it is not as clean as an external DAC.
