# AliExpress Order List

Last checked: 2026-05-11.

AliExpress listings move around, sellers stop shipping to some regions, and product IDs change. For that reason this list uses stable search links first, with a few example listing links where a matching listing was visible through search/price-index pages. Prefer sellers with many orders, recent reviews, combined shipping, and clear photos of the exact module pin labels.

## Core Cart

Order this if you want to build the current version and have spares.

| Qty | Part | Search link | Example link | Notes |
| ---: | --- | --- | --- | --- |
| 2 | Raspberry Pi Pico H / Pico with headers | [search](https://www.aliexpress.com/wholesale?SearchText=official+Raspberry+Pi+Pico+H+RP2040) | [example](https://www.aliexpress.com/item/1005002074105889.html) | Buy headers version if possible. One spare avoids waiting after a wiring mistake. |
| 2 | 0.96 inch 128x64 SSD1306 I2C OLED, 4-pin | [search](https://www.aliexpress.com/wholesale?SearchText=0.96+inch+OLED+I2C+SSD1306+128x64+module) | [example](https://www.aliexpress.com/item/1005003843092806.html) | Must be I2C with `GND VCC SCL SDA` or similar. Avoid SPI-only OLEDs for this firmware. |
| 5-10 | H11L1 / H11L1M DIP-6 opto-isolator | [search](https://www.aliexpress.com/wholesale?SearchText=H11L1+H11L1M+DIP6+optoisolator) | [example](https://www.aliexpress.com/item/32530234473.html) | Preferred MIDI IN opto for 3.3 V logic. Order spares. |
| 5 | 5-pin DIN female panel/PCB socket, 180 degree | [search](https://www.aliexpress.com/wholesale?SearchText=5+pin+DIN+female+socket+MIDI+panel+mount) | [example](https://www.aliexpress.com/item/32402564814.html) | Buy enough for MIDI IN plus future MIDI OUT/THRU. |
| 5 | 3.5 mm stereo panel jack with nut | [search](https://www.aliexpress.com/wholesale?SearchText=PJ392A+3.5mm+stereo+panel+mount+jack+nut) | [example](https://www.aliexpress.com/item/4001063855295.html) | Use tip as mono signal and sleeve as ground for the first build. |
| 10 | B10K 16 mm linear potentiometers, panel mount | [search](https://www.aliexpress.com/wholesale?SearchText=B10K+16mm+linear+potentiometer+6mm+shaft) | - | `B10K` is usually linear on AliExpress. Avoid `A10K` unless you want audio/log taper. |
| 10 | Knobs for 6 mm potentiometer shaft | [search](https://www.aliexpress.com/wholesale?SearchText=6mm+shaft+potentiometer+knob+black) | - | Match D-shaft, splined, or round shaft to the pots you buy. |
| 2 | 5-way navigation switch / 5D joystick switch module | [search](https://www.aliexpress.com/wholesale?SearchText=5D+joystick+switch+module+Arduino) | - | The current firmware expects digital up/down/left/right/click switches. |
| 1 pack | 1N4148 diodes | [search](https://www.aliexpress.com/wholesale?SearchText=1N4148+diode+100pcs) | - | Used for MIDI input protection. |
| 1 kit | 1/4 W resistor assortment | [search](https://www.aliexpress.com/wholesale?SearchText=1%2F4W+metal+film+resistor+assortment+kit) | - | Must include 220R, 470R, 1k, 2.2k, 10k, and 100k. |
| 1 kit | Ceramic capacitor assortment | [search](https://www.aliexpress.com/wholesale?SearchText=ceramic+capacitor+assortment+10nF+100nF) | - | Must include 10 nF and 100 nF. |
| 1 kit | Electrolytic capacitor assortment | [search](https://www.aliexpress.com/wholesale?SearchText=electrolytic+capacitor+assortment+10uF+100uF) | - | Must include 10 uF, 10 V or higher. |
| 5-10 | Prototype perfboard / stripboard | [search](https://www.aliexpress.com/wholesale?SearchText=double+side+prototype+perfboard+2.54mm) | - | 2.54 mm pitch. Get several sizes. |
| 1 kit | Male and female 2.54 mm pin headers | [search](https://www.aliexpress.com/wholesale?SearchText=2.54mm+male+female+pin+header+assortment) | - | Useful for removable Pico/modules. |
| 1 kit | Dupont jumper wires, M-M/M-F/F-F | [search](https://www.aliexpress.com/wholesale?SearchText=dupont+jumper+wire+kit+male+female) | [example](https://www.aliexpress.com/item/4001095716403.html) | Useful for bench testing before soldering. |
| 1 roll | 24 AWG or 26 AWG stranded hookup wire | [search](https://www.aliexpress.com/wholesale?SearchText=24AWG+silicone+hookup+wire+assortment) | - | Stranded wire is nicer for panel controls. |
| 1 kit | Heat-shrink tubing assortment | [search](https://www.aliexpress.com/wholesale?SearchText=heat+shrink+tube+assortment+kit) | - | Use on jack, DIN, and pot terminals. |
| 1 | Micro-USB data cable | [search](https://www.aliexpress.com/wholesale?SearchText=micro+USB+data+cable+Raspberry+Pi+Pico) | - | Must be a data cable, not charge-only. |

## Order These Upgrades Now

These are not required for the first PWM-audio build, but they are the parts most likely to save a second order.

| Qty | Part | Search link | Example link | Why order it now |
| ---: | --- | --- | --- | --- |
| 2 | PCM5102A I2S DAC module, often `GY-PCM5102` | [search](https://www.aliexpress.com/wholesale?SearchText=PCM5102A+I2S+DAC+module+GY-PCM5102) | [example](https://www.aliexpress.com/item/33000036886.html) | Best audio-quality upgrade over PWM. Line-level stereo output. Firmware needs an I2S audio change. |
| 2 | ADS1115 16-bit I2C ADC module | [search](https://www.aliexpress.com/wholesale?SearchText=ADS1115+16bit+I2C+ADC+module) | [example](https://www.aliexpress.com/item/1005001703504835.html) | Adds four analog inputs for more pots or analog joystick axes. |
| 2 | Analog XY joystick module with push button | [search](https://www.aliexpress.com/wholesale?SearchText=analog+joystick+module+push+button+Arduino) | - | Use with ADS1115 if you want real X/Y joystick control. |
| 5 | EC11 rotary encoders with push switch | [search](https://www.aliexpress.com/wholesale?SearchText=EC11+rotary+encoder+push+button+module) | - | Good future UI upgrade for menus and sequencer editing. |
| 2 | TDA1308 or TPA6132 headphone amp module | [search](https://www.aliexpress.com/wholesale?SearchText=TDA1308+headphone+amplifier+module+3.5mm) | - | Use after the DAC/filter if you want headphone output. Do not drive headphones directly from Pico GPIO. |
| 2 | PAM8403 5 V class-D speaker amplifier module | [search](https://www.aliexpress.com/wholesale?SearchText=PAM8403+5V+stereo+amplifier+module+volume) | [example](https://www.aliexpress.com/item/32417104445.html) | For a small built-in speaker. Not a headphone output; class-D speaker outputs are not ground-referenced. |
| 5 | 74HCT14 or 74HCT125 DIP IC | [search](https://www.aliexpress.com/wholesale?SearchText=74HCT14+DIP+IC) | - | Useful for a future 5 V-compliant MIDI OUT/THRU driver. |
| 2 | MicroSD card SPI module | [search](https://www.aliexpress.com/wholesale?SearchText=microSD+card+module+SPI+3.3V+Arduino) | - | Future preset/pattern storage. Not used by current firmware. |
| 2 | USB-C panel mount extension cable | [search](https://www.aliexpress.com/wholesale?SearchText=USB+C+panel+mount+extension+cable) | - | Cleaner enclosure power/programming access if you mount the Pico inside. |
| 2 | 5 V buck converter module | [search](https://www.aliexpress.com/wholesale?SearchText=MP1584+buck+converter+module+5V) | - | For later standalone power from a higher-voltage supply. |
| 1 | 8-channel USB logic analyzer clone | [search](https://www.aliexpress.com/wholesale?SearchText=24MHz+8+channel+USB+logic+analyzer) | - | Very useful for debugging MIDI RX, UART polarity, I2C, and clocks. |

## Enclosure and Panel Hardware

| Qty | Part | Search link | Notes |
| ---: | --- | --- | --- |
| 1-2 | ABS project box, around 100x60x25 mm or larger | [search](https://www.aliexpress.com/wholesale?SearchText=ABS+project+box+100x60x25mm) | Size depends on your panel layout. Bigger is easier for a first build. |
| 1 kit | M2.5/M3 nylon standoffs, screws, nuts | [search](https://www.aliexpress.com/wholesale?SearchText=M2.5+M3+nylon+standoff+screw+kit) | Mount Pico, OLED, perfboard. |
| 1 kit | Small toggle switches | [search](https://www.aliexpress.com/wholesale?SearchText=mini+toggle+switch+panel+mount) | Optional power/mode switches. |
| 1 kit | JST-XH or JST-PH connector kit | [search](https://www.aliexpress.com/wholesale?SearchText=JST+XH+connector+kit+2.54mm) | Makes panel wiring removable. |

## Optional Test/Use Items

| Qty | Part | Search link | Notes |
| ---: | --- | --- | --- |
| 1 | DIN MIDI cable | [search](https://www.aliexpress.com/wholesale?SearchText=5+pin+DIN+MIDI+cable) | Needed if you do not already have one. |
| 1 | USB-MIDI interface with DIN OUT | [search](https://www.aliexpress.com/wholesale?SearchText=USB+MIDI+interface+DIN+out) | Cheap cable-style interfaces are hit-or-miss. Fine for basic note testing, not always reliable for clock/SysEx. |
| 1 | Small powered speaker or amp board | [search](https://www.aliexpress.com/wholesale?SearchText=small+powered+speaker+amplifier+module+3.5mm+input) | Use with line output; do not connect passive speakers directly to Pico/DAC output. |

## Do Not Accidentally Buy

- SPI OLED modules if you want to use the current firmware unchanged. Buy I2C SSD1306, 4-pin.
- `A10K` pots for the three control pots unless you intentionally want log taper. Buy `B10K linear`.
- A 6N138-only MIDI input build unless you are comfortable powering the 6N138 logic side from 5 V and pulling its output to 3.3 V.
- PAM8403 modules as headphone amps. They are speaker amps; their outputs are bridge-tied and should not be wired to a normal grounded headphone jack.
- 5 V sensor/modules that drive Pico GPIO directly. Pico GPIO is not 5 V tolerant.

## Practical Cart Summary

If you want the safest one-shot order, put these in the cart:

- 2x Pico H
- 2x SSD1306 I2C OLED
- 10x H11L1M
- 5x DIN 5 female sockets
- 5x 3.5 mm panel jacks
- 10x B10K pots plus 10 matching knobs
- 2x 5-way digital joystick switch modules
- resistor, ceramic capacitor, electrolytic capacitor, diode, header, wire, heat-shrink, and perfboard kits
- 2x PCM5102A DAC modules
- 2x ADS1115 ADC modules
- 2x analog joystick modules
- 5x EC11 encoders
- 2x headphone amp modules
- 2x PAM8403 modules if you may add speakers
- 1x logic analyzer
- enclosure, standoffs, and connector kits
