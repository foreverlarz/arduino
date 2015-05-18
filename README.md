# arduino
A collection of open-source Arduino projects.

## desk-gardener
This project will culminate in a desktop alarm clock that operates a peristaltic pump to water your plant.

**Feature status:**
* LCD: fully implemented ([menu demo](https://www.youtube.com/watch?v=RMsWIaFMqAU))
* RTC: partially implemented
* Pump: unimplemented

**Parts list:**
* Arduino Uno R3
* Adafruit RGB LCD shield
* Adafruit RTC1307 breakout (although I'm considering more accurate alternatives)
* Arduino prototyping shield
* Peristaltic pump (12 V)
* L293D H-Bridge driver IC
* Power supply (wall wart, barrel jack, 12 V)

I'm primarily using Adafruit parts---their pricing is decent and their website is helpful, so I like to support them. You can easily find the same (or similar) parts elsewhere (e.g., [SparkFun](https://www.sparkfun.com/) or [Digi-Key](http://www.digikey.com/)).
