# Human Badge v0

The "HUMAN BADGE" is my attempt at a Defcon/tech electronic badge that does some neat things, especially when it's around other badges.

This is v0 because I'm still prototyping everything.

The overall idea is to make a badge (kind of like the Defcon 27 badge) that interacts with other badges. A kind of hackable Nintendo StreetPass. Your badge is either sending or listening for pulses, and those pulses can contain a little bit of info about you.

The basic features include right now:

- An Arduino Nano v3 powering everything.
- An nRF24L01+ RF module that talks over 2.4GHz to other badges within range.
- An RGB LED that visually displays the current status of the badge.
- A little piezo speaker that beeps when a pulse is sent and plays a series of tones when one is received.

Future plans:

- A small screen to display your status and icon and who you've recently been pinged by.
- Some switches to turn on/off the display and radio.
- A proper custom PCB for everything!

## Resources

- [Radio module tutorial](https://www.deviceplus.com/how-tos/arduino-guide/nrf24l01-rf-module-tutorial/)
- [RadioHead driver docs](http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF24.html)
- [Another tutorial for this radio](https://www.instructables.com/id/Wireless-Remote-Using-24-Ghz-NRF24L01-Simple-Tutor/)
