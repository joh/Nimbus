ARDUINO_DIR    = /usr/share/arduino

AVR_TOOLS_PATH         = /usr/bin
AVRDUDE_CONF           = /etc/avrdude.conf
AVRDUDE_ARD_PROGRAMMER = arduino
AVRDUDE_ARD_BAUDRATE   = 57600

TARGET       = robot
MCU          = atmega328p
F_CPU        = 16000000
ARDUINO_PORT = /dev/ttyUSB*

include ../Arduino.mk

