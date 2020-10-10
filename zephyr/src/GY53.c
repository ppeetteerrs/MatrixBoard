#include <GY53.h>
#include <drivers/gpio.h>
#include <zephyr.h>

/**
 * @brief GPIO driver
 * 
 */
struct device *pwm;
struct gpio_callback pwm_cb;

/**
 * @brief Volatile variables to store values from interrupt
 * 
 */
static volatile u32_t last_reading;
static volatile u32_t last_cycle;

/**
 * @brief Interrupt handler for GY53
 * 
 * @param dev GPIO device
 * @param cb GPIO callback struct
 * @param pins GPIO pin mask
 */
static void handler(struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins) {
	u64_t val = gpio_pin_get(dev, GY53_PIN);
	if (val > 0) {
		last_cycle = k_cycle_get_32();
	} else {
		last_reading = k_cyc_to_ns_near32(k_cycle_get_32() - last_cycle) / 10000;
	}
}

/**
 * @brief New FIFO for GY53 data
 * 
 */
K_FIFO_DEFINE(gy53_fifo);

/**
 * @brief Setup interrupt callbacks
 * 
 */
static inline void gy53_setup() {
#if GY53_LOG
	printk("[GY53] Started\n");
#endif

	pwm = device_get_binding(GY53_DEV);

	// Configure pin
	gpio_pin_configure(pwm, GY53_PIN, GPIO_INPUT);
	gpio_pin_interrupt_configure(pwm, GY53_PIN, GPIO_INT_EDGE_BOTH);

	// Setup interrupt callback
	gpio_init_callback(&pwm_cb, handler, BIT(GY53_PIN));
	gpio_add_callback(pwm, &pwm_cb);
}

/**
 * @brief Read latest distance reading and publish to FIFO
 * 
 */
static inline void gy53_loop() {
	struct gy53_data reading = {.distance = last_reading / 20};
	k_fifo_put(&gy53_fifo, &reading);
}

/**
 * @brief GY53 thread entry point
 * 
 */
void gy53_entry() {
	gy53_setup();
	while (1) {
		gy53_loop();
		k_msleep(GY53_THREAD_SLEEP_TIME);
	}
}