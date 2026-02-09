/**
 * @brief GPIO driver example in C.
 */
#include "driver/gpio.h"

#define LED_PIN 8U     // LED pin.
#define BUTTON_PIN 13U // Button pin.

/**
 * @brief Use GPIO driver to control an LED and a button.
 * 
 * @return 0 on successful termination of the program (should never occur).
 */
int main(void)
{
    // Create and initialize GPIO instances.
    gpio_t* led    = gpio_new(LED_PIN, GPIO_DIRECTION_OUT);
    gpio_t* button = gpio_new(BUTTON_PIN, GPIO_DIRECTION_IN_PULLUP);

    while (1) 
    {
        // Enable the LED whenever the button is pressed.
        const bool value = gpio_read(button);
        gpio_write(led, value);
    }
    // Release allocated resources, then terminate the program.
    gpio_del(&led);
    gpio_del(&button);
    return 0;
}
