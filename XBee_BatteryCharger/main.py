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

print("#Booting Up...")

# Instantiate an I2C peripheral.
i2c = I2C(1)
for address in i2c.scan():
    print("- I2C device found at address: %s" % hex(address))

while xbee.atcmd("AI") != 0:
    print("#Trying to Connect...")
    time.sleep(0.5)

print("#Online...")

TARGET_64BIT_ADDR = b'\x00\x13\xA2\x00\x41\xA7\xAD\xBC'

#ina219:
#A0:A1
# 1:1 = 0x45 Solar
# 0:1 = 0x44 Battery
# 1:0 = 0x41 5VoltRail
# 0:0 = 0x40 12VoltRail

SOLAR_INA219A_ADDR = 0x45
BATTERY_INA219A_ADDR = 0x44
FIVEVR_INA219A_ADDR = 0x41
TWELVEVR_INA219A_ADDR = 0x40

I2C_INTERFACE_NO = 1
SHUNT_OHMS = 0.1

def read_solar():
    try:
        solar_voltage = str(solar_ina.voltage())
        solar_current = str(solar_ina.current())
        print_solar = "Solar:" + time_snapshot + ":BusVolt:" + solar_voltage + "V:Current:" + solar_current + "mA:#"
    except:
        print_solar = "Solar:" + time_snapshot + ":BusVolt:INVALID:Current:INVALID:#"
        print("INA219:0x45: Solar read failed...")

    try:
        xbee.transmit(TARGET_64BIT_ADDR, print_solar)
    except:
        print("XBee: TX Solar Failed...")

def read_battery():
    try:
        battery_voltage = str(battery_ina.voltage())
        battery_current = str(battery_ina.current())
        print_battery = "Battery:" + time_snapshot + ":BusVolt:" + battery_voltage + "V:Current:" + battery_current + "mA:#"
    except:
        print_battery = "Battery:" + time_snapshot + ":BusVolt:INVALID:Current:INVALID:#"
        print("INA219:0x44: Battery read failed...")

    try:
        xbee.transmit(TARGET_64BIT_ADDR, print_battery)
    except:
        print("XBee: Battery TX Failed...")

def read_fivevr():
    try:
        fivevr_voltage = str(fivevr_ina.voltage())
        fivevr_current = str(fivevr_ina.current())
        print_fivevr = "5V_Rail:" + time_snapshot + ":BusVolt:" + fivevr_voltage + "V:Current:" + fivevr_current + "mA:#"
    except:
        print_fivevr = "5V_Rail:" + time_snapshot + ":BusVolt:INVALID:Current:INVALID:#"
        print("INA219:0x41: 5V Rail read failed...")

    try:
        xbee.transmit(TARGET_64BIT_ADDR, print_fivevr)
    except:
        print("XBee: 5V Rail TX Failed...")

def read_twelvevr():
    try:
        twelvevr_voltage = str(twelvevr_ina.voltage())
        twelvevr_current = str(twelvevr_ina.current())
        print_twelvevr = "12V_Rail:" + time_snapshot + ":BusVolt:" + twelvevr_voltage + "V:Current:" + twelvevr_current + "mA:#"
    except:
        print_twelvevr = "12V_Rail:" + time_snapshot + ":BusVolt:INVALID:Current:INVALID:#"
        print("INA219:0x41: 12V Rail read failed...")

    try:
        xbee.transmit(TARGET_64BIT_ADDR, print_twelvevr)
    except:
        print("XBee: 12V Rail TX Failed...")

solar_ina = INA219(SHUNT_OHMS, I2C(I2C_INTERFACE_NO), SOLAR_INA219A_ADDR)
try:
    print("INA219:0x45: Configuring solar...")
    solar_ina.configure_32v_2a()
except:
    print("INA219:0x45: Solar Missing...")

battery_ina = INA219(SHUNT_OHMS, I2C(I2C_INTERFACE_NO), BATTERY_INA219A_ADDR)
try:
    print("INA219:0x44: Configuring Battery...")
    battery_ina.configure_32v_2a()
except:
    print("INA219:0x44: Battery Missing...")

fivevr_ina = INA219(SHUNT_OHMS, I2C(I2C_INTERFACE_NO), FIVEVR_INA219A_ADDR)
try:
    print("INA219:0x41: Configuring 5V Rail...")
    fivevr_ina.configure_32v_2a()
except:
    print("INA219:0x41: 5V Rail Missing...")

twelvevr_ina = INA219(SHUNT_OHMS, I2C(I2C_INTERFACE_NO), TWELVEVR_INA219A_ADDR)
try:
    print("INA219:0x40: Configuring 12V Rail...")
    twelvevr_ina.configure_32v_2a()
except:
    print("INA219:0x40: 12V Rail Missing...")

while True:
    time_snapshot = str(time.ticks_cpu())
    read_solar()
    time.sleep(6)
    read_battery()
    time.sleep(6)
    read_fivevr()
    time.sleep(6)
    read_twelvevr()
    time.sleep(6)
