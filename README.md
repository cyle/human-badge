# Human Badge v0

The "HUMAN BADGE" is my attempt at a Defcon/tech electronic badge that does some neat things, especially when it's around other badges.

This is v0 because I'm still prototyping everything.

The overall idea is to make a badge (kind of like [the amazing Defcon 27 badge](http://www.grandideastudio.com/defcon-27-badge/)) that interacts with other badges. A kind of hackable Nintendo StreetPass. Your badge is either sending or listening for pulses, and those pulses can contain a little bit of info about you, or whatever you want. Receivers can see these things on their badge as they get them and look em up later.

The basic features include right now:

- An Arduino Nano v3 powering everything.
- An NRF24L01+ RF module that talks over 2.4GHz to other badges within range.
- An RGB LED that visually displays the current status of the badge.
  - Purple means it's listening for pulses.
  - Red means it's received a pulse. (You can see the message when using Arduino IDE's Serial Monitor.)
  - Green means it's sent a pulse. (You can see the message being sent when using Arduino IDE's Serial Monitor.)
- A little piezo speaker that beeps when a pulse is sent and plays a series of tones when one is received.
- A 1.3" square 240x240 display.

Future plans:

- The screen should display your status and icon and who you've recently been pinged by.
- Some switches to turn on/off the display and radio to save battery.
- A battery of some kind, lol. Maybe a something that can be recharged via USB.
- Some non-volatile memory so you can access who you've connected with.
- A serial terminal connection to the badge itself so you can set up what you want to send, and download what you've got.
- A proper custom PCB for everything!

## Code Requirements

- Arduino IDE 1.8.9
- [RadioHead library](http://www.airspayce.com/mikem/arduino/RadioHead/)
- Adafruit GFX, Zero DMA, ST7735 and ST7789, SPIFlash, and SdFat - Adafruit Fork libraries, via Arduino IDE's package manager.

Debugging via serial is essential. It'll tell you what you wanna know at 9600 baud.

## Physical Setup

If you want to breadboard it:

- Strap in your Arduino Nano v3 to a breadboard, and I put 3v3 (not 5v) and ground into the breadboard bus.
- Connecting the radio to your Arduino digital pins and whatnot is in the below tutorials, I'm using digital pin 2 for the IRQ pin (which is usually absent from the tutorials but seems to be required?).
- Piezo speaker goes from digital pin 8 to a 100 ohm resistor and then to positive on the speaker, ground on the speaker goes to ground.
- I'm using common cathode RGB LED, so the longest pin goes to ground.
  - Red pin is the single one next to the longest pin, goes to a 330 ohm resistor then digital pin 6.
  - On the other side of the longest pin, next is green, goes to a 330 ohm resistor then digital pin 5.
  - Last is blue, goes to a 330 ohm resistor then digital pin 3.
- The display shares pins 11, 12, and 13 with the radio, and I'm using pin A0 for TCS, A1 for RST, and A2 for D/C.

You should be able to hook up your Arduino Nano to your computer via USB and upload the sketch.

You'll need at least two setups to get the actual functionality of the thing, of course, so there's someone else for your badge to talk to.

## Resources

- [Radio module tutorial](https://www.deviceplus.com/how-tos/arduino-guide/nrf24l01-rf-module-tutorial/)
- [RadioHead driver docs](http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF24.html)
- [Another tutorial for this radio](https://www.instructables.com/id/Wireless-Remote-Using-24-Ghz-NRF24L01-Simple-Tutor/)
- [Adafruit GFX library reference and guide](https://learn.adafruit.com/adafruit-gfx-graphics-library)
- [Adafruit 1.3" display guide](https://learn.adafruit.com/adafruit-1-3-and-1-54-240-x-240-wide-angle-tft-lcd-displays)
