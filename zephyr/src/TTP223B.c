#include <TTP223B.h>
#include <drivers/gpio.h>
#include <zephyr.h>
/**
 * @brief GPIO driver
 * 
 */
struct device *pin;
struct gpio_callback pin_cb;

/**
 * @brief Volatile variables to store values from interrupt
 * 
 */
static volatile bool touched;

/**
 * @brief Interrupt handler for TTP223B
 * 
 * @param dev GPIO device
 * @param cb GPIO callback struct
 * @param pins GPIO pin mask
 */
static void handler(struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins) {
	u64_t val = gpio_pin_get(dev, TTP223B_PIN);
	if (val > 0) {
		touched = true;
	} else {
		touched = false;
	}
}

/**
 * @brief New FIFO for TTP223B data
 * 
 */
K_FIFO_DEFINE(ttp223b_fifo);

/**
 * @brief Setup interrupt callbacks
 * 
 */
static inline void ttp223b_setup() {
#if TTP223B_LOG
	printk("[TTP223B] Started\n");
#endif

	pin = device_get_binding(TTP223B_DEV);

	// Configure pin
	gpio_pin_configure(pin, TTP223B_PIN, GPIO_INPUT);
	gpio_pin_interrupt_configure(pin, TTP223B_PIN, GPIO_INT_EDGE_BOTH);

	// Setup interrupt callback
	gpio_init_callback(&pin_cb, handler, BIT(TTP223B_PIN));
	gpio_add_callback(pin, &pin_cb);
}

/**
 * @brief Read latest distance reading and publish to FIFO
 * 
 */
static inline void ttp223b_loop() {
	struct ttp223b_data data = {.touched = touched};
	k_fifo_put(&ttp223b_fifo, &data);
}

/**
 * @brief TTP223B thread entry point
 * 
 */
void ttp223b_entry() {
	ttp223b_setup();
	while (1) {
		ttp223b_loop();
		k_msleep(TTP223B_THREAD_SLEEP_TIME);
	}
}