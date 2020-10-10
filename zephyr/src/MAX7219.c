#include <GY521.h>
#include <GY53.h>
#include <MAX7219.h>
#include <TTP223B.h>
#include <debug/thread_analyzer.h>
#include <drivers/pinmux.h>
#include <drivers/spi.h>
#include <zephyr.h>

/**
 * @brief LED patterns
 * 
 */

/**
 * @brief SPI drivers, config and buffers
 * 
 */
static struct device* spi;
static struct spi_config spi_cfg;
static struct spi_cs_control spi_cs;
static struct spi_buf_set spi_buf_set_tx;
static struct spi_buf spi_buf_tx;
static u8_t tx_buf[MAX7219_PACKET_SIZE];

/**
 * @brief MAX7219 data container
 * 
 */
struct max7219_data data = {
	.distance = 0,
	.accel_x = 0,
	.accel_y = 0,
	.accel_z = 0,
	.temp = 0,
	.gyro_x = 0,
	.gyro_y = 0,
	.gyro_z = 0,
	.touched = false};

/**
 * @brief Send a command packet to MAX7219
 * 
 * @param cmd Opcode
 * @param value Value
 */
static inline void
max7219_send_cmd(u8_t cmd, u8_t value) {
	tx_buf[0] = cmd;
	tx_buf[1] = value;
#if MAX7219_LOG
	printk("Sending data: %02x%02x", tx_buf[0], tx_buf[1]);
#endif
	k_sched_lock();
	int ret = spi_write(spi, &spi_cfg, &spi_buf_set_tx);
	k_sched_unlock();
#if MAX7219_LOG
	if (ret == 0) {
		printk(" - Successful.\n");
	} else {
		printk(" - Failed with code %d.\n", ret);
	}
#endif
}

/**
 * @brief Set up SPI driver
 * 
 */
static inline void max7219_setup() {
#if MAX7219_LOG
	printk("[MAX7219] Started\n");
#endif

	spi = device_get_binding(MAX7219_DEV);

	spi_cfg.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER;
	spi_cfg.frequency = 2000000;
	spi_cfg.slave = 1;

	spi_cs.gpio_dev = device_get_binding(MAX7219_CS_DEV);
	spi_cs.gpio_pin = MAX7219_CS_PIN;
	spi_cfg.cs = &spi_cs;

	spi_buf_tx.buf = tx_buf;
	spi_buf_tx.len = 2;

	spi_buf_set_tx.buffers = &spi_buf_tx;
	spi_buf_set_tx.count = 1;

	max7219_send_cmd(MAX7219_TEST, 0x0F);
	k_msleep(300);
	max7219_send_cmd(MAX7219_TEST, 0x00);
	max7219_send_cmd(MAX7219_DECODE, 0x00);
	max7219_send_cmd(MAX7219_INTENSITY, 0x00);
	max7219_send_cmd(MAX7219_SCAN_LIMIT, 0x0F);
	max7219_send_cmd(MAX7219_D0, 0x00);
	max7219_send_cmd(MAX7219_D1, 0x00);
	max7219_send_cmd(MAX7219_D2, 0x00);
	max7219_send_cmd(MAX7219_D3, 0x00);
	max7219_send_cmd(MAX7219_D4, 0x00);
	max7219_send_cmd(MAX7219_D5, 0x00);
	max7219_send_cmd(MAX7219_D6, 0x00);
	max7219_send_cmd(MAX7219_D7, 0x00);
	max7219_send_cmd(MAX7219_SHUTDOWN, 0x01);
}

/**
 * @brief Send a LED pattern to MAX7219
 * 
 * @param pattern LED pattern array
 */
static inline void max7219_send_pattern(u8_t* pattern) {
	for (int i = 0; i < MAX7219_N_ROWS; i++) {
		max7219_send_cmd(MAX7219_D0 + i, pattern[i]);
	}
}

/**
 * @brief Update LED pattern based on readings
 * 
 */
static inline void max7219_update_pattern() {
	u8_t pattern[MAX7219_N_ROWS];
	pattern[0] = data.accel_x;
	pattern[1] = data.accel_y;
	pattern[2] = data.accel_z;
	pattern[3] = data.temp;
	pattern[4] = data.gyro_x;
	pattern[5] = data.gyro_y;
	pattern[6] = data.gyro_z;
	pattern[7] = data.distance;
	if (data.touched) {
		max7219_send_cmd(MAX7219_INTENSITY, 0x02);
	} else {
		max7219_send_cmd(MAX7219_INTENSITY, 0x00);
	}
	max7219_send_pattern(pattern);
}

/**
 * @brief Blinks LED matrix
 * 
 */
static inline void max7219_loop() {
	struct gy53_data* gy53Data = (struct gy53_data*)k_fifo_get(&gy53_fifo, K_NO_WAIT);
	if (gy53Data != NULL) {
		data.distance = gy53Data->distance;
	}
	struct gy521_data* gy521Data = (struct gy521_data*)k_fifo_get(&gy521_fifo, K_NO_WAIT);
	if (gy521Data != NULL) {
		data.accel_x = gy521Data->accel_x;
		data.accel_y = gy521Data->accel_y;
		data.accel_z = gy521Data->accel_z;
		data.temp = gy521Data->temp;
		data.gyro_x = gy521Data->gyro_x;
		data.gyro_y = gy521Data->gyro_y;
		data.gyro_z = gy521Data->gyro_z;
	}
	struct ttp223b_data* ttp223bData = (struct ttp223b_data*)k_fifo_get(&ttp223b_fifo, K_NO_WAIT);
	if (ttp223bData != NULL) {
		data.touched = ttp223bData->touched;
	};
	max7219_update_pattern();
#if MAX7219_LOG
	printk("[GY521] Accel   : X - %05hd | Y - %05hd | Z - %05hd\r\n", data.accel_x, data.accel_y, data.accel_z);
	printk("[GY521] Temp    : %05hd\r\n", data.temp);
	printk("[GY521] Gyro    : X - %05hd | Y - %05hd | Z - %05hd\r\n", data.gyro_x, data.gyro_y, data.gyro_y);
	printk("[TTP223B] Touch : %s\n", data.touched ? "Yes" : "No");
	printk("[GY521] Distance: %05umm\n", data.distance);
#endif
}

/**
 * @brief MAX7219 thread entry
 * 
 */
void max7219_entry() {
#if MAX7219_LOG
	printk("[MAX7219] Started\n");
#endif
	max7219_setup();
	while (1) {
		max7219_loop();
		k_msleep(MAX7219_THREAD_SLEEP_TIME);
	}
}