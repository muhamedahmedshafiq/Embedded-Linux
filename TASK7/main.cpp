#include "gpio.h"
#include <unistd.h>
#include <iostream>

int main() {
    GPIO led(26); // BCM26

    std::cout << "LED ON\n";
    led.on();
    sleep(2);

    std::cout << "LED OFF\n";
    led.off();

    return 0;
}
