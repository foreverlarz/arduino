# arduino
A collection of open-source Arduino projects.

## wifi-gardener
Collects data from the Internet (including the time and weather events) and operates a solenoid to water your garden.

**Feature status:**
* Wifi connectivity: working
* NTP client: working (with issues)
* Weather: unimplemented
* Manual watering: unimplemented
* Automatic watering: unimplemented

**Parts list:**
* Adafruit Huzzah ESP8266 module
* Adafruit 7-segment LCD module
* Solenoid valve (12 V)
* TIP120 transistor & 1N4001 diode (considering upgrade to MOSFET)
* (2) ~10k Ω resistors
* Button
* Barrel adapter
* Prototyping perforated board

More information on this project as it becomes more mature.

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

**Required libraries:**
* [Adafruit RGB LCD Shield Library](https://github.com/adafruit/Adafruit-RGB-LCD-Shield-Library)
* [Adafruit RTClib](https://github.com/adafruit/RTClib)
