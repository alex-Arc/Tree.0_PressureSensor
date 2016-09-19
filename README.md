# Tree.0_PressureSensor
Pressure sensor for the Tree.0

## OSC setup
the arduino will always listen to UDP port `49160` for OSC formatted messages. from default it will respond on UDP port `49155` to IP `192.168.1.2` this can be changed with:
`/settings/outIP/[0-3] [0-255]`
`/settings/outPort [0-65535]` recommended reading [UDP port numbers](https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers)

the arduino will report pressure data through the OSC address: `/pressureSens/[0-3] [0-1023]`

it will only send new data when the the difference between the current and last reading is larger than the threshold, that can be set with: `/settings/threshold/[0-3] [0-255]`

the data will first be filtered using the fixed-point method described [here](https://kiritchatterjee.wordpress.com/2014/11/10/a-simple-digital-low-pass-filter-in-c/)
and beta can be set through: `/settings/beta/[0-3] [0-255]`

## Software and hardware
- [Arduino 1.6.11 or newer](https://www.arduino.cc/en/Main/Software)
- [CNMAT/OSC library](https://github.com/CNMAT/OSC)
- [adafruit/Ethernet2 library](https://github.com/adafruit/Ethernet2)
- [Arduino Leonardo ETH](http://www.arduino.org/products/boards/arduino-leonardo-eth)
