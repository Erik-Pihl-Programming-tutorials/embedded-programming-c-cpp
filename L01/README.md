# Struktar i C: Implementation av en GPIO-driver.

## Beskrivning
En GPIO-driver implementerad i C med hjälp av en struktur kallad `gpio`:
* GPIO-instanser kontrolleras enkelt med ett fåtal associerade funktioner, såsom `gpio_write()`
och `gpio_read()`.
* Struktens implementation hålls privat genom att strukten enbart fördeklareras i det publika
API:et. Därmed kan instansattribut, såsom pinnummer och associerade register, inte ändras – varken medvetet eller av misstag – under programexekvering.

## Video tutorial
Implementationen demonstrerades i följande [video tutorial](https://youtu.be/T7o8Vj8ZAUc).

## Filer
* [driver/gpio.h](./include/driver/gpio.h) utgör GPIO-driverns publika API. Här deklareras associerade funktioner såsom `gpio_new()`, `gpio_write()` och `gpio_read()` för att skapa,
skriva och läsa till GPIO-pinnar.
* [driver/gpio.c](./source/driver/gpio.c) innehåller GPIO-driverns implementationsdetaljer.
* [main.c](./main.c) innehåller ett testprogram innehållande instanser av GPIO-drivern.

---