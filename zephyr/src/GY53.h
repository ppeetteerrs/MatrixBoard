#ifndef __GY53__
#define __GY53__
#include <drivers/gpio.h>
#include <zephyr.h>

/* ----------------------------- Logging Setting ---------------------------- */
#define GY53_LOG 1

/* ------------------------------- GPIO Config ------------------------------ */
#define GY53_DEV "GPIOB"
#define GY53_PIN 1

/* ------------------------------ ThreadConfig ------------------------------ */
#define GY53_THREAD_STACK_SIZE 512
#define GY53_THREAD_PRIORITY 5
#define GY53_THREAD_SLEEP_TIME 100
#define GY53_THREAD_DELAY_TIME 500

/* --------------------------------- Structs -------------------------------- */
struct gy53_data {
	void* fifo_reserved;
	u32_t distance;
};

extern struct k_fifo gy53_fifo;

/* -------------------------------- Functions ------------------------------- */
void gy53_entry();

#endif