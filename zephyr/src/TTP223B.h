#ifndef __TTP223B__
#define __TTP223B__
#include <drivers/gpio.h>
#include <zephyr.h>

/* ----------------------------- Logging Setting ---------------------------- */
#define TTP223B_LOG 1

/* ------------------------------- GPIO Config ------------------------------ */
#define TTP223B_DEV "GPIOB"
#define TTP223B_PIN 0

/* ------------------------------ ThreadConfig ------------------------------ */
#define TTP223B_THREAD_STACK_SIZE 512
#define TTP223B_THREAD_PRIORITY 5
#define TTP223B_THREAD_SLEEP_TIME 100
#define TTP223B_THREAD_DELAY_TIME 500

/* --------------------------------- Structs -------------------------------- */
struct ttp223b_data {
	void* fifo_reserved;
	bool touched;
};

extern struct k_fifo ttp223b_fifo;

/* -------------------------------- Functions ------------------------------- */
void ttp223b_entry();

#endif