#ifndef __MAX7219__
#define __MAX7219__
#include <GY521.h>
#include <GY53.h>
#include <TTP223B.h>
#include <drivers/spi.h>
#include <zephyr.h>

/* ------------------------------- Log Setting ------------------------------ */
#define MAX7219_LOG 1

/* ------------------------------- SPI Config ------------------------------- */
#define MAX7219_DEV "SPI_1"
#define MAX7219_CS_DEV "GPIOA"
#define MAX7219_CS_PIN 4

/* ------------------------------ Thread Config ----------------------------- */
#define MAX7219_THREAD_STACK_SIZE 256
#define MAX7219_THREAD_PRIORITY 4
#define MAX7219_THREAD_SLEEP_TIME 100
#define MAX7219_THREAD_DELAY_TIME 500

/* ----------------------------- Device Setting ----------------------------- */
#define MAX7219_N_ROWS 8
#define MAX7219_PACKET_SIZE 2

/* ----------------------------- Opcodes ---------------------------- */
#define MAX7219_NOOP 0x00
#define MAX7219_D0 0x01
#define MAX7219_D1 0x02
#define MAX7219_D2 0x03
#define MAX7219_D3 0x04
#define MAX7219_D4 0x05
#define MAX7219_D5 0x06
#define MAX7219_D6 0x07
#define MAX7219_D7 0x08
#define MAX7219_DECODE 0x09
#define MAX7219_INTENSITY 0x0A
#define MAX7219_SCAN_LIMIT 0x0B
#define MAX7219_SHUTDOWN 0x0C
#define MAX7219_TEST 0x0F

/* --------------------------------- Structs -------------------------------- */
struct max7219_data {
	s16_t accel_x;
	s16_t accel_y;
	s16_t accel_z;
	s16_t temp;
	s16_t gyro_x;
	s16_t gyro_y;
	s16_t gyro_z;
	u32_t distance;
	bool touched;
};

/* -------------------------------- Functions ------------------------------- */
void max7219_entry();

#endif