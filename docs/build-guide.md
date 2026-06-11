# Build Guide

## 1. Install Software

In Arduino IDE:

1. Add the Earle Philhower Arduino-Pico board package.
2. Select your Pico board, for example `Raspberry Pi Pico`.
3. Install these libraries from Library Manager:
   - `MIDI Library` by Francois Best and lathoub
   - `Adafruit SSD1306`
   - `Adafruit GFX Library`

The sketch uses Arduino-Pico's built-in `PWMAudio` support.

## 2. Upload a Minimal Blink First

Before adding hardware, confirm the Pico uploads correctly. Hold `BOOTSEL` while plugging in USB if the board is not detected.

## 3. Bring Up the OLED

Wire only the OLED first:

- VCC to 3V3
- GND to GND
- SDA to GP4
- SCL to GP5

Upload the synth sketch. If the display stays blank:

- Try OLED address `0x3D` in the sketch.
- Check that VCC is 3.3 V, not 5 V.
- Check whether your OLED labels SDA/SCL from the front or back of the board.

## 4. Add Pots

Wire the pots to GP26, GP27, and GP28. The OLED should show changing values:

- `Tone`: low-pass filter cutoff
- `Osc`: second oscillator mix/detune amount
- `BPM`: sequencer tempo

If a pot works backward, swap its two outer lugs.

## 5. Add Audio Output

Build the RC filter and AC coupling circuit from [wiring.md](wiring.md). Connect the jack to a powered speaker, mixer, or audio interface line input at low volume.

Expected result before MIDI:

- The module should be mostly quiet.
- When the sequencer is enabled, it should produce notes from the internal pattern.

If the audio is harsh or noisy:

- Confirm both 10 nF capacitors go to ground.
- Confirm the 10 uF capacitor polarity.
- Reduce gain on the receiving amplifier.
- Keep GP14 and the filter physically away from I2C and DIN wiring.

## 6. Add MIDI IN

Build the H11L1 opto-isolated MIDI input. Test with a keyboard, groovebox, or USB-MIDI interface that has a real DIN MIDI OUT.

The sketch listens on all MIDI channels. It responds to:

- Note On
- Note Off
- Control Change 74 for tone/cutoff
- Control Change 1 for osc thickness/mod amount
- Pitch Bend, +/- 2 semitones

If MIDI does not work:

- Check DIN pins 4 and 5; mirrored DIN pinouts are the most common mistake.
- Check that the opto output pin idles near 3.3 V.
- Check that the opto output pin pulses low during incoming MIDI data.
- Confirm GP1 is used, not GP0.

## 7. Add Joystick

Wire the joystick common pin to ground. Each direction line should read low when pressed.

The OLED page changes with the click button:

- Play page
- Oscillator page
- Sequencer page

Hold click for about 0.7 s to start or stop the sequencer.

## 8. Enclosure Notes

For a first module, leave USB accessible for firmware updates. Use insulated panel jacks if your enclosure is metal, especially around the audio jack and MIDI DIN. Label the jack as `LINE OUT`, not headphone out.

## 9. Good Upgrade Path

Once the PWM version works:

1. Replace PWM audio with a PCM5102A I2S DAC.
2. Add a MIDI OUT/THRU jack if you need chaining.
3. Add an external ADC for more pots or an analog joystick.
4. Move from perfboard to a small PCB.
5. Add preset storage in flash.
