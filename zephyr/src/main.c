#include <GY521.h>
#include <GY53.h>
#include <MAX7219.h>
#include <TTP223B.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <drivers/pinmux.h>
#include <drivers/spi.h>

K_THREAD_DEFINE(gy521_thread, GY521_THREAD_STACK_SIZE,
				gy521_entry, NULL, NULL, NULL,
				GY521_THREAD_PRIORITY, 0, GY521_THREAD_DELAY_TIME);
K_THREAD_DEFINE(gy53_thread, GY53_THREAD_STACK_SIZE,
				gy53_entry, NULL, NULL, NULL,
				GY53_THREAD_PRIORITY, 0, GY53_THREAD_DELAY_TIME);
K_THREAD_DEFINE(ttp223b_thread, TTP223B_THREAD_STACK_SIZE,
				ttp223b_entry, NULL, NULL, NULL,
				TTP223B_THREAD_PRIORITY, 0, TTP223B_THREAD_DELAY_TIME);
K_THREAD_DEFINE(max7219_thread, MAX7219_THREAD_STACK_SIZE,
				max7219_entry, NULL, NULL, NULL,
				MAX7219_THREAD_PRIORITY, 0, MAX7219_THREAD_DELAY_TIME);