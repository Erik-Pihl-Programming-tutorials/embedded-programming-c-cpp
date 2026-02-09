/**
 * @brief GPIO driver for ATmega328p.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Enumeration of data directions.
 */
typedef enum gpio_direction
{
    GPIO_DIRECTION_IN,        ///< Input with internal pull-up disabled.
    GPIO_DIRECTION_IN_PULLUP, ///< Input with internal pull-up enabled.
    GPIO_DIRECTION_OUT,       ///< Output.
} gpio_direction_t;

/** GPIO structure. */
typedef struct gpio gpio_t;

/**
 * @brief Create new GPIO.
 * 
 * @param[in] pin Arduino pin number.
 * @param[in] direction Data direction.
 * 
 * @return The new GPIO instance, or a nullptr on failure.
 */
gpio_t* gpio_new(uint8_t pin, gpio_direction_t direction);

/**
 * @brief Delete given GPIO.
 * 
 *        Relase allocated resources and set the corresponding pointer to null.
 * 
 * @param[in] self Double pointer to the GPIO to delete.
 */
void gpio_del(gpio_t** self);

/**
 * @brief Read input of given GPIO.
 * 
 * @param[in] self The GPIO to read.
 * 
 * @return True if the GPIO is enabled, false otherwise.
 */
bool gpio_read(const gpio_t* self);

/**
 * @brief Write output of given GPIO.
 * 
 *        This operation is only supported for outputs.
 * 
 * @param[in] self The GPIO to write.
 * @param[in] value The value to write.
 */
void gpio_write(gpio_t* self, bool value);

/**
 * @brief Toggle output of given GPIO.
 * 
 *        This operation is only supported for outputs.
 * 
 * @param[in] self The GPIO to toggle.
 */
void gpio_toggle(gpio_t* self);
