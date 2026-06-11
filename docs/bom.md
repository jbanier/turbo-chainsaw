# Bill of Materials

Quantities are for one module.

## Core

| Qty | Part | Notes |
| ---: | --- | --- |
| 1 | Raspberry Pi Pico or Pico H | Pico H is easier if you do not want to solder headers. |
| 1 | USB cable | For power and upload. |
| 1 | Perfboard or small prototype PCB | Keep audio filter parts close to GP14. |
| 1 | Pin headers / sockets | Optional but useful while debugging. |

## MIDI Input

| Qty | Part | Notes |
| ---: | --- | --- |
| 1 | 5-pin DIN female panel connector | MIDI IN. |
| 1 | H11L1 opto-isolator | Preferred for this 3.3 V Pico build. |
| 1 | 220 ohm resistor | MIDI input current limiting. |
| 1 | 1k resistor | H11L1 output pull-up to 3V3. |
| 1 | 1N4148 diode | Reverse protection across opto LED. |
| 1 | 100 nF capacitor | Decoupling at opto VCC/GND. |

Fallback: a 6N138 can be used, but power its logic side from 5 V while pulling its output up to 3.3 V. Do not pull the MIDI RX output up to 5 V.

## User Interface

| Qty | Part | Notes |
| ---: | --- | --- |
| 1 | 128x64 SSD1306 I2C OLED | 3.3 V compatible, address usually `0x3C`. |
| 1 | 5-way navigation joystick switch | Up/down/left/right/click with common ground. |
| 3 | 10k linear potentiometer | Tone, oscillator thickness, tempo. |
| 3 | Knobs | Match pot shaft type. |
| 0-3 | 100 nF capacitor | Optional pot wiper filtering. |

## Audio Output

| Qty | Part | Notes |
| ---: | --- | --- |
| 1 | 3.5 mm mono or stereo jack | For stereo jack, wire tip as mono signal and sleeve as ground. |
| 2 | 1k resistor | Two-pole PWM low-pass filter. |
| 2 | 10 nF capacitor | Two-pole PWM low-pass filter. |
| 1 | 10 uF electrolytic capacitor | AC coupling, positive side toward Pico/filter. |
| 1 | 470 ohm resistor | Output isolation/protection. |
| 1 | 100k resistor | Jack tip pulldown. |

## Recommended Tools

- Soldering iron with fine tip
- Multimeter with continuity mode
- Small side cutters
- Breadboard jumpers for first test
- MIDI source with a real DIN MIDI OUT
- Powered speaker, mixer, or audio interface line input
