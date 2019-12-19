# Default template for XBee MicroPython projects

from ina219 import INA219
from machine import I2C
import time

ina219a_addr= 0x45

I2C_INTERFACE_NO = 1
SHUNT_OHMS = 0.1
max_expected_amps=2

ina = INA219(SHUNT_OHMS, I2C(I2C_INTERFACE_NO), max_expected_amps, ina219a_addr)
ina.configure_32v_2a()
while True:
    print("LOAD:BusVolt:%.3fV" % ina.voltage())
    print("LOAD:Current:%.3fmA" % ina.current())
    ina.sleep()
    time.sleep(60)
    ina.wake()
