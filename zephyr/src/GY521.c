#include <GY521.h>
#include <debug/thread_analyzer.h>
#include <drivers/i2c.h>
#include <zephyr.h>

/**
 * @brief I2C driver
 * 
 */
static struct device* i2c;

/**
 * @brief New FIFO for GY521 data
 * 
 */
K_FIFO_DEFINE(gy521_fifo);

/**
 * @brief Setup a new GY521 object
 * 
 */
static inline void gy521_setup() {
#if GY521_LOG
	printk("[GY521] Started\n");
#endif
	// Get I2C Device Binding
	i2c = device_get_binding(GY521_DEV);
	k_sched_lock();

	// Turn on GY5216050
	i2c_reg_write_byte(i2c, GY521_ADDR, 0x6B, 0x00);

	// Set Full Scale to 250 deg per sec and 2g
	i2c_reg_write_byte(i2c, GY521_ADDR, 0x1B, 0x00);
	i2c_reg_write_byte(i2c, GY521_ADDR, 0x1C, 0x00);
	k_sched_unlock();
}

/**
 * @brief Read a 16-bit reading from GY521
 * 
 * @param reg_addr Register address
 * @return s16_t Reading
 */
static inline s16_t gy521_read_val(u8_t reg_addr) {
	u8_t value[2];
	k_sched_lock();
	i2c_reg_read_byte(i2c, GY521_ADDR, reg_addr, value);
	i2c_reg_read_byte(i2c, GY521_ADDR, reg_addr + 1U, value + 1U);
	k_sched_unlock();
	return (value[0] << 8) | value[1];
}

/**
 * @brief Read all sensor readings from GY521
 * 
 */
static inline void gy521_loop() {
	struct gy521_data data;
	data.accel_x_raw = gy521_read_val(GY521_ACCEL_X);
	data.accel_y_raw = gy521_read_val(GY521_ACCEL_Y);
	data.accel_z_raw = gy521_read_val(GY521_ACCEL_Z);
	data.temp_raw = gy521_read_val(GY521_TEMP);
	data.gyro_x_raw = gy521_read_val(GY521_GYRO_X);
	data.gyro_y_raw = gy521_read_val(GY521_GYRO_Y);
	data.gyro_z_raw = gy521_read_val(GY521_GYRO_Z);
	data.accel_x = data.accel_x_raw / GY521_ACCEL_X_SCALE + GY521_ACCEL_X_OFFSET;
	data.accel_y = data.accel_y_raw / GY521_ACCEL_Y_SCALE + GY521_ACCEL_Y_OFFSET;
	data.accel_z = data.accel_z_raw / GY521_ACCEL_Z_SCALE + GY521_ACCEL_Z_OFFSET;
	data.temp = (data.temp_raw * 200 / GY521_TEMP_SCALE + GY521_TEMP_OFFSET * 200) - 30 * 200;
	data.gyro_x = data.gyro_x_raw / GY521_GYRO_X_SCALE + GY521_GYRO_X_OFFSET;
	data.gyro_y = data.gyro_y_raw / GY521_GYRO_Y_SCALE + GY521_GYRO_Y_OFFSET;
	data.gyro_z = data.gyro_z_raw / GY521_GYRO_Z_SCALE + GY521_GYRO_Z_OFFSET;

	k_fifo_put(&gy521_fifo, &data);
}

/**
 * @brief GY521 Thread
 * 
 */
void gy521_entry() {
	gy521_setup();
	while (1) {
		gy521_loop();
		k_msleep(GY521_THREAD_SLEEP_TIME);
	}
}