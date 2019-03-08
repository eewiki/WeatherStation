#include "stdio.h"
#include <atmel_start.h>
#include "BME280_Driver.h"
#include "BME280_Defs.h"

volatile uint8_t data_out;
volatile uint8_t data_in;
volatile uint8_t reg;
struct io_descriptor *io;
struct io_descriptor *I2C_0_io;

float tmp_f;
volatile uint8_t counter;

void enable_uart(void)
{
	usart_sync_get_io_descriptor(&USART_0, &io);
	usart_sync_enable(&USART_0);
}

uint8_t i2c_read_register(uint8_t addr)
{
	uint8_t tmp_data = addr;
	io_write(I2C_0_io, &tmp_data, 1);
	io_read(I2C_0_io, &tmp_data, 1);
	return tmp_data;
}

void enable_i2c(void)
{
	i2c_m_sync_get_io_descriptor(&I2C_0, &I2C_0_io);
	i2c_m_sync_enable(&I2C_0);
	i2c_m_sync_set_slaveaddr(&I2C_0, BME280_I2C_ADDRESS1, I2C_M_SEVEN);
}

void enable_watchdog(void)
{
	uint32_t clk_rate;
	uint16_t timeout_period;

	clk_rate       = 1000;
	timeout_period = 4096;
	wdt_set_timeout_period(&WDT_0, clk_rate, timeout_period);
	wdt_enable(&WDT_0);	
}

void BME280_INIT() {
  BME280_SetStandbyTime(BME280_STANDBY_TIME_1_MS);                              // Standby time 1ms
  BME280_SetFilterCoefficient(BME280_FILTER_COEFF_16);                          // IIR Filter coefficient 16
  BME280_SetOversamplingPressure(BME280_OVERSAMP_16X);                          // Pressure x16 oversampling
  BME280_SetOversamplingTemperature(BME280_OVERSAMP_2X);                        // Temperature x2 oversampling
  BME280_SetOversamplingHumidity(BME280_OVERSAMP_1X);                           // Humidity x1 oversampling
  BME280_SetOversamplingMode(BME280_NORMAL_MODE);
}

void Display_Values(void) {
	volatile char buf_counter[5];	
	volatile char buf[10];
	
	counter++;

	if (counter == 100)
	{
		counter = 10;
	}
	
	itoa(counter, buf_counter, 10);
	
	tmp_f = BME280_GetTemperature();
	tmp_f = (float)(tmp_f * 9.0);
	tmp_f = (float)(tmp_f / 5.0);
	tmp_f = (float)(tmp_f + 32.0);
	gcvt(tmp_f, 5, buf);

	io_write(io, (uint8_t *)"$L11T:", 6);
	io_write(io, buf_counter, 2);
	io_write(io, (uint8_t *)":TempF:", 7);
	io_write(io, buf, 5);
	io_write(io, (uint8_t *)"F*\r\n", 4);

	tmp_f = BME280_GetPressure();
	gcvt(tmp_f, 6, buf);

	io_write(io, (uint8_t *)"$L11P:", 6);
	io_write(io, buf_counter, 2);	
	io_write(io, (uint8_t *)":Pres:", 6);	
	io_write(io, buf, 6);
	io_write(io, (uint8_t *)"hPa*\r\n", 6);

	tmp_f = BME280_GetHumidity();
	gcvt(tmp_f, 5, buf);

	io_write(io, (uint8_t *)"$L11H:", 6);
	io_write(io, buf_counter, 2);	
	io_write(io, (uint8_t *)":Humid:", 7);	
	io_write(io, buf, 5);
	io_write(io, (uint8_t *)"H*\r\n", 4);	
	
	io_write(io, (uint8_t *)"\r\n", 2);		
}

int main(void)
{
	uint8_t value;

	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	enable_watchdog();
	enable_uart();

	io_write(io, (uint8_t *)"SAML11 Starting Up\r\n", 20);
	
	enable_i2c();
	
	value = i2c_read_register(BME280_CHIP_ID_REG);

	if (value == BME280_CHIP_ID)
	{
		io_write(io, (uint8_t *)"BME280 Connected..\r\n", 20);
	}
	
	delay_ms(200);
	
	BME280_ReadCalibrationParams();                                               //Read calibration parameters
	BME280_SetOversamplingPressure(BME280_OVERSAMP_1X);
	BME280_SetOversamplingTemperature(BME280_OVERSAMP_1X);
	BME280_SetOversamplingHumidity(BME280_OVERSAMP_1X);
	BME280_SetOversamplingMode(BME280_FORCED_MODE);
	
	wdt_feed(&WDT_0);
	while(BME280_IsMeasuring());
	BME280_ReadMeasurements();
  
	BME280_INIT();
	
	counter = 10;

	/* Replace with your application code */
	while (1) {
		wdt_feed(&WDT_0);
		while(BME280_IsMeasuring());
		BME280_ReadMeasurements();
		Display_Values();
		BME280_INIT();

		wdt_feed(&WDT_0);
		delay_ms(3000);

		wdt_feed(&WDT_0);
		delay_ms(3000);

		wdt_feed(&WDT_0);
		delay_ms(3000);
	}
}
