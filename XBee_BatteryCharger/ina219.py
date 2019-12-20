"""Originally from: https://github.com/chrisb2/pyb_ina219 heavily tweaked.."""

"""MicroPython library for the INA219 sensor.

This library supports the INA219 sensor from Texas Instruments with
MicroPython using the I2C bus.
"""

import utime
from micropython import const

class INA219:
    """Provides all the functionality to interact with the INA219 sensor."""

    RANGE_16V = const(0)  # Range 0-16 volts
    RANGE_32V = const(1)  # Range 0-32 volts

    GAIN_1_40MV = const(0)  # Maximum shunt voltage 40mV
    GAIN_2_80MV = const(1)  # Maximum shunt voltage 80mV
    GAIN_4_160MV = const(2)  # Maximum shunt voltage 160mV
    GAIN_8_320MV = const(3)  # Maximum shunt voltage 320mV
    GAIN_AUTO = const(-1)  # Determine gain automatically

    ADC_9BIT = const(0)  # 9-bit conversion time  84us.
    ADC_10BIT = const(1)  # 10-bit conversion time 148us.
    ADC_11BIT = const(2)  # 11-bit conversion time 2766us.
    ADC_12BIT = const(3)  # 12-bit conversion time 532us.
    ADC_2SAMP = const(9)  # 2 samples at 12-bit, conversion time 1.06ms.
    ADC_4SAMP = const(10)  # 4 samples at 12-bit, conversion time 2.13ms.
    ADC_8SAMP = const(11)  # 8 samples at 12-bit, conversion time 4.26ms.
    ADC_16SAMP = const(12)  # 16 samples at 12-bit,conversion time 8.51ms
    ADC_32SAMP = const(13)  # 32 samples at 12-bit, conversion time 17.02ms.
    ADC_64SAMP = const(14)  # 64 samples at 12-bit, conversion time 34.05ms.
    ADC_128SAMP = const(15)  # 128 samples at 12-bit, conversion time 68.10ms.

    __ADC_CONVERSION = {
        ADC_9BIT: "9-bit",
        ADC_10BIT: "10-bit",
        ADC_11BIT: "11-bit",
        ADC_12BIT: "12-bit",
        ADC_2SAMP: "12-bit, 2 samples",
        ADC_4SAMP: "12-bit, 4 samples",
        ADC_8SAMP: "12-bit, 8 samples",
        ADC_16SAMP: "12-bit, 16 samples",
        ADC_32SAMP: "12-bit, 32 samples",
        ADC_64SAMP: "12-bit, 64 samples",
        ADC_128SAMP: "12-bit, 128 samples"
    }

    INA219_CONFIG_BVOLTAGERANGE_32V = const(1)
    INA219_CONFIG_GAIN_8_320MV = const(3)
    INA219_CONFIG_BADCRES_12BIT = const(1)
    INA219_CONFIG_SADCRES_12BIT_1S_532US = const(3)
    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS = const(7)

    #__INA219_CONFIG_BVOLTAGERANGE_32V = 0x2000 :  1 << 13
    #__INA219_CONFIG_GAIN_8_320MV = 0x1800 : 3 << 11
    #__INA219_CONFIG_BADCRES_12BIT = 0x0400 : 1 << 7
    #__INA219_CONFIG_SADCRES_12BIT_1S_532US = 0x0018 : 3 < 3
    #__INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS = 0x0007 7 << 1

    __ADDRESS = 0x40

    __REG_CONFIG = 0x00
    __REG_SHUNTVOLTAGE = 0x01
    __REG_BUSVOLTAGE = 0x02
    __REG_POWER = 0x03
    __REG_CURRENT = 0x04
    __REG_CALIBRATION = 0x05

    __RST = 15
    __BRNG = 13
    __PG1 = 12
    __PG0 = 11
    __BADC4 = 10
    __BADC3 = 9
    __BADC2 = 8
    __BADC1 = 7
    __SADC4 = 6
    __SADC3 = 5
    __SADC2 = 4
    __SADC1 = 3
    __MODE3 = 2
    __MODE2 = 1
    __MODE1 = 0

    __OVF = 1
    __CNVR = 2

    __BUS_RANGE = [16, 32]
    __GAIN_VOLTS = [0.04, 0.08, 0.16, 0.32]

    __CONT_SH_BUS = 7

    __AMP_ERR_MSG = ('Expected current %.3fA is greater '
                     'than max possible current %.3fA')
    __RNG_ERR_MSG = ('Expected amps %.2fA, out of range, use a lower '
                     'value shunt resistor')
    __VOLT_ERR_MSG = ('Invalid voltage range, must be one of: '
                      'RANGE_16V, RANGE_32V')

    __SHUNT_MILLIVOLTS_LSB = 0.01  # 10uV
    __BUS_MILLIVOLTS_LSB = 4  # 4mV
    __CALIBRATION_FACTOR = 0.04096
    # Max value supported value (65534 decimal) of the calibration register
    # (D0 bit is always zero, p31 of spec)
    __MAX_CALIBRATION_VALUE = 0xFFFE
    # In the spec (p17) the current LSB factor for the minimum LSB is
    # documented as 32767, but a larger value (100.1% of 32767) is used
    # to guarantee that current overflow can always be detected.
    __CURRENT_LSB_FACTOR = 32800

    def __init__(self, shunt_ohms, i2c, address=__ADDRESS):
        """Construct the class.

        At a minimum pass in the resistance of the shunt resistor and I2C
        interface to which the sensor is connected.

        Arguments:
        shunt_ohms -- value of shunt resistor in Ohms (mandatory).
        i2c -- an instance of the I2C class from the *machine* module, either
            I2C(1) or I2C(2) (mandatory).
        address -- the I2C address of the INA219, defaults to
            *0x40* (optional).
        """
        self._i2c = i2c
        self._address = address
        self._shunt_ohms = shunt_ohms
        self._gain = None
        self._auto_gain_enabled = False

    def configure_32v_2a(self):
        #ina219_currentDivider_mA = 10; // Current
        #ina219_powerDivider_mW = 2
        self._calibration_register(0x1000)
        self._configure(self.INA219_CONFIG_BVOLTAGERANGE_32V, self.INA219_CONFIG_GAIN_8_320MV, self.INA219_CONFIG_BADCRES_12BIT, self.INA219_CONFIG_SADCRES_12BIT_1S_532US, self.INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS)

    def voltage(self):
        """Return the bus voltage in volts."""
        value = self._voltage_register()
        return float(value) * self.__BUS_MILLIVOLTS_LSB / 1000

    def current(self):
        """Return the bus current in milliamps."""
        return self._current_register() * 1000 / 1000

    def sleep(self):
        """Put the INA219 into power down mode."""
        configuration = self._read_configuration()
        self._configuration_register(configuration & 0xFFF8)

    def wake(self):
        """Wake the INA219 from power down mode."""
        configuration = self._read_configuration()
        self._configuration_register(configuration | 0x0007)
        # 40us delay to recover from powerdown (p14 of spec)
        utime.sleep_us(40)

    def _configure(self, voltage_range, gain, bus_adc, shunt_adc, mode):
        configuration = (
            voltage_range << self.__BRNG | gain << self.__PG0 |
            bus_adc << self.__BADC1 | shunt_adc << self.__SADC1 |
            mode)
        self._configuration_register(configuration)

    def _configuration_register(self, register_value):
        self.__write_register(self.__REG_CONFIG, register_value)

    def _read_configuration(self):
        return self.__read_register(self.__REG_CONFIG)

    def _calibration_register(self, register_value):
        self.__write_register(self.__REG_CALIBRATION, register_value)

    def _voltage_register(self):
        register_value = self._read_voltage_register()
        return register_value >> 3

    def _read_voltage_register(self):
        return self.__read_register(self.__REG_BUSVOLTAGE)

    def _current_register(self):
        #Bug fix from Arduino...
        self._calibration_register(0x1000)
        return self.__read_register(self.__REG_CURRENT, True)

    def __write_register(self, register, register_value):
        register_bytes = self.__to_bytes(register_value)
        self._i2c.writeto_mem(self._address, register, register_bytes)

    def __to_bytes(self, register_value):
        return bytearray([(register_value >> 8) & 0xFF, register_value & 0xFF])

    def __read_register(self, register, negative_value_supported=False):
        register_bytes = self._i2c.readfrom_mem(self._address, register, 2)
        register_value = int.from_bytes(register_bytes, 'big')
        if negative_value_supported:
            # Two's compliment
            if register_value > 32767:
                register_value -= 65536

        return register_value
