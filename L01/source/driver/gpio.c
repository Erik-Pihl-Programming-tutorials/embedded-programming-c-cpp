/**
 * @brief GPIO driver implementation details for ATmega328p.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>

#include "driver/gpio.h"

#define PIN_COUNT 20U // The number of available pins.

/** I/O port offsets (when mapping against Arduino pins). */
#define PIN_OFFSET_B 8U  // Pin offset for I/O port B (pin 9 = PORTB1).
#define PIN_OFFSET_C 14U // Pin offset for I/O port C (pin 14 = PORTC0).
#define PIN_OFFSET_D 0U  // Pin offset for I/O port D (pin 5 = PORTD5).

/**
 * @brief GPIO structure.
 */
typedef struct gpio
{
    /** Pointer to the data direction register. */
    volatile uint8_t* ddrx;

    /** Pointer to the port register. */
    volatile uint8_t* portx;

    /** Pointer to the pin register. */
    volatile uint8_t* pinx;

    /** Pin on the associated I/O port. */
    uint8_t pin;

    /** Pin ID (equal to the Arduino Uno pin). */
    uint8_t id;
} gpio_t;

/** Pin registry holding the state of each pin (1 = reserved, 0 = free). */
static uint32_t pin_registry = 0U;

// -----------------------------------------------------------------------------
static bool is_pin_free(const uint8_t id)
{
    // Return true if the corresponding bit in the pin registry is 0, false otherwise.
    return PIN_COUNT > id ? (0U == (pin_registry & (1U << id))) : false;
}

// -----------------------------------------------------------------------------
static void reserve_pin(const uint8_t id) 
{ 
    // Set the corresponding bit in the pin registry.
    pin_registry |= (1U << id); 
}

// -----------------------------------------------------------------------------
static void release_pin(const uint8_t id) 
{ 
    // Clear the corresponding bit in the pin registry.
    pin_registry &= ~(1U << id); 
}

// -----------------------------------------------------------------------------
static bool gpio_is_output(const gpio_t* self)
{
    return (bool)(*(self->ddrx) & (1U << self->pin));
}

// -----------------------------------------------------------------------------
static void gpio_init(gpio_t* self, const uint8_t pin, const gpio_direction_t direction)
{
    // Save the pin ID for reservation and cleanup.
    self->id = pin;

    // If 0 <= pin <= 7 => the GPIO is connected to I/O port D.
    if (PIN_OFFSET_B > pin)
    {
        self->ddrx  = &DDRD;
        self->portx = &PORTD;
        self->pinx  = &PIND;
        self->pin   = pin - PIN_OFFSET_D;
    }
    // Else if 8 <= pin <= 13 => the GPIO is connected to I/O port B.
    else if (PIN_OFFSET_C > pin)
    {
        self->ddrx  = &DDRB;
        self->portx = &PORTB;
        self->pinx  = &PINB;
        self->pin   = pin - PIN_OFFSET_B;
    }
    // Else if 14 <= pin <= 19 => the GPIO is connected to I/O port C.
    else
    {
        self->ddrx  = &DDRC;
        self->portx = &PORTC;
        self->pinx  = &PINC;
        self->pin   = pin - PIN_OFFSET_C;
    }

    // Set data direction.
    switch (direction)
    {
        case GPIO_DIRECTION_IN_PULLUP:
        {
            // Clear the associated bit in DDRx (configure as input).
            // Set the associated bit in PORTx (enable pull-up).
            *(self->ddrx) &= ~(1U << self->pin);
            *(self->portx) |= (1U << self->pin);
            break;

        }
        case GPIO_DIRECTION_OUT:
        {
            // Set the associated bit in DDRx (configure as output).
            // Clear the associated bit in PORTx (set the initial output value to 0).
            *(self->ddrx) |= (1U << self->pin);
            *(self->portx) &= ~(1U << self->pin);
            break;
        }
        default:
        {
            // Clear the associated bits in DDRx and PORTx as default.
            *(self->ddrx) &= ~(1U << self->pin);
            *(self->portx) &= ~(1U << self->pin);
            break;
        }
    }
    // Reserve the pin before terminating the function.
    reserve_pin(pin);
}

// -----------------------------------------------------------------------------
static void gpio_cleanup(gpio_t* self)
{
    // Clear the associated bits in the GPIO registers and the pin registry.
    *(self->ddrx) &= ~(1U << self->pin);
    *(self->portx) &= ~(1U << self->pin);
    release_pin(self->id);
}

// -----------------------------------------------------------------------------
gpio_t* gpio_new(uint8_t pin, const gpio_direction_t direction)
{
    // Return a nullptr if the pin is reserved.
    if (!is_pin_free(pin)) { return NULL; }

    // Create new GPIO instance, return nullptr on failure.
    gpio_t* self = (gpio_t*)(malloc(sizeof(gpio_t)));
    if (NULL == self) { return NULL; }

    // Initialize the GPIO instance.
    gpio_init(self, pin, direction);

    // Return the initialized GPIO instance.
    return self;
}

// -----------------------------------------------------------------------------
void gpio_del(gpio_t** self)
{
    // Terminate the function if self or the instance the self pointing at is a nullptr.
    if ((NULL == self) || (NULL == *self)) { return; }

    // Clean up GPIO registers and the pin registry.
    gpio_cleanup(*self);

    // Free allocated memory.
    free(*self);

    // Set the associated pointer to null.
    *self = NULL;
}

// -----------------------------------------------------------------------------
bool gpio_read(const gpio_t* self)
{
    // Return true if the input is high, false otherwise.
    return NULL != self ? (bool)(*(self->pinx) & (1U << self->pin)) : false; 
}

// -----------------------------------------------------------------------------
void gpio_write(gpio_t* self, const bool value)
{
    // Terminate the function if the GPIO is a nullptr or configured as input.
    if ((NULL == self) || !gpio_is_output(self)) { return; }

    // Set/clear the output in accordance with the given value.
    if (value) { *(self->portx) |= (1U << self->pin); }
    else { *(self->portx) &= ~(1U << self->pin); }
}

// -----------------------------------------------------------------------------
void gpio_toggle(gpio_t* self)
{
    // Terminate the function if the GPIO is a nullptr or configured as input.
    if ((NULL == self) || !gpio_is_output(self)) { return; }

    // Toggle the output by writing to PINx (the hardware will toggle the output of PORTx
    // if the pin is configured as output).
    *(self->pinx) |= (1U << self->pin);
}
