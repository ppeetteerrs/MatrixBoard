#ifndef __GY521__
#define __GY521__
#include <zephyr.h>

/* ------------------------------- Log Setting ------------------------------ */
#define GY521_LOG 1

/* ------------------------------- I2C Config ------------------------------- */
#define GY521_DEV "I2C_1"

/* ------------------------------ Thread Config ----------------------------- */
#define GY521_THREAD_STACK_SIZE 1024
#define GY521_THREAD_PRIORITY 5
#define GY521_THREAD_SLEEP_TIME 300
#define GY521_THREAD_DELAY_TIME 500

/* ------------------------ Register Address Mappings ----------------------- */
#define GY521_ADDR 0x68
#define GY521_ACCEL_X 0x3B
#define GY521_ACCEL_Y 0x3D
#define GY521_ACCEL_Z 0x3F
#define GY521_TEMP 0x41
#define GY521_GYRO_X 0x43
#define GY521_GYRO_Y 0x45
#define GY521_GYRO_Z 0x47

/* ----------------------------- Reading Offsets ---------------------------- */
#define GY521_ACCEL_X_OFFSET 0
#define GY521_ACCEL_Y_OFFSET 0
#define GY521_ACCEL_Z_OFFSET 0
#define GY521_TEMP_OFFSET 36.53
#define GY521_GYRO_X_OFFSET 0
#define GY521_GYRO_Y_OFFSET 0
#define GY521_GYRO_Z_OFFSET 0
#define GY521_ACCEL_X_SCALE 64
#define GY521_ACCEL_Y_SCALE 64
#define GY521_ACCEL_Z_SCALE 64
#define GY521_TEMP_SCALE 340
#define GY521_GYRO_X_SCALE 131
#define GY521_GYRO_Y_SCALE 131
#define GY521_GYRO_Z_SCALE 131

/* --------------------------------- Structs -------------------------------- */
struct gy521_data {
	void* fifo_reserved;
	s16_t accel_x;
	s16_t accel_y;
	s16_t accel_z;
	s16_t temp;
	s16_t gyro_x;
	s16_t gyro_y;
	s16_t gyro_z;
	s16_t accel_x_raw;
	s16_t accel_y_raw;
	s16_t accel_z_raw;
	s16_t temp_raw;
	s16_t gyro_x_raw;
	s16_t gyro_y_raw;
	s16_t gyro_z_raw;
};

extern struct k_fifo gy521_fifo;

/* -------------------------------- Functions ------------------------------- */
void gy521_entry();

#endif