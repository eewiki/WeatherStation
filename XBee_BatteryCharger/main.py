# Copyright (c) 2019 Robert Nelson <robert.nelson@digikey.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

from ina219 import INA219
from machine import I2C
import sys
import xbee
import time

TARGET_64BIT_ADDR = b'\x00\x13\xA2\x00\x41\xA7\xAD\xBC'

ina219a_addr= 0x45

I2C_INTERFACE_NO = 1
SHUNT_OHMS = 0.1
max_expected_amps=2

solar_ina = INA219(SHUNT_OHMS, I2C(I2C_INTERFACE_NO), max_expected_amps, ina219a_addr)
solar_ina.configure_32v_2a()

while True:
    solar_voltage = str(solar_ina.voltage())
    solar_current = str(solar_ina.current())
    print_solar_voltage = "$Solar:BusVolt:" + solar_voltage + "V*"
    print_solar_current = "$Solar:Current:" + solar_current + "mA*"
    #print("$Solar:BusVolt:%sV*" % (solar_voltage))
    #print("$Solar:Current:%smA*" % (solar_current))
    xbee.transmit(TARGET_64BIT_ADDR, print_solar_voltage)
    xbee.transmit(TARGET_64BIT_ADDR, print_solar_current)
    solar_ina.sleep()
    time.sleep(15)
    solar_ina.wake()
